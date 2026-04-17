# 智能储物柜控制系统（ESP32-S3）

## 项目简介
这是一个基于 `ESP32-S3` + `ESP-IDF` + `LVGL` 的智能储物柜控制项目。  
当前代码已完成显示、触摸、柜门 GPIO 控制、XST 掌静脉模组通信，以及 SoftAP + TCP 指令转发调试链路。

## 当前进度（2026-04-17）
### 本次修订点
- 将 README 内容从泛化项目介绍，修正为与当前代码实现一致的状态说明。
- 重新梳理 `已完成`、`进行中`、`已知问题`、`下一步计划`，避免把未接通功能写成已完成。
- 明确 Setting 页、Take 页、Save 页、XST 协议栈、WiFi 调试链路的实际落地情况。
- 补充当前硬编码项、未实现接口、UI 刷新缺口、`crumble_password()` 缺陷等真实问题。
- 标注 `bsp/ui/generated/events_init.c` 中存在手写业务逻辑，提示后续重新生成 UI 时有覆盖风险。

### 已完成
- **基础框架**：`ESP-IDF 5.3.3`，目标芯片 `esp32s3`。
- **UI**：Gui Guider 生成 5 个页面（`main/save_page/take_page/help_page/setting_page`），各页面在 `setup_scr_xxx()` 内完成事件绑定；主页面可切换到存件、取件、帮助，长按帮助按钮可进入设置页。
- **显示**：`ST7789` SPI DMA 驱动，`lv_port_disp.c` 渲染缓冲区加倍（`RENDER_BUF_LINES` 40→80），LVGL 已接入，背光 PWM 可调（0-100）。
- **触摸**：`FT6336U` I2C 轮询，已接入 LVGL；`lv_port_indev.c` 修正了触摸坐标映射逻辑（`*x = touch_point.x; *y = touch_point.y`），解决坐标轴方向问题。
- **LVGL 配置优化**（`lv_conf.h`）：
  - 堆内存 54KB → 60KB，渲染线程栈 8KB → 10KB
  - 刷新周期 5ms → 15ms（降低 CPU 负载）
  - DPI 100 → 130
  - 启用 FreeRTOS 调度（`LV_USE_OS = LV_OS_FREERTOS`）
  - 启用 `LV_USE_GIF`/`LV_USE_QRCODE`，关闭 `LV_USE_GRID`
  - `LV_USE_ST7789` 已使能
- **XST 掌静脉协议栈**：
  - UART 双任务收发（`xst_uart_rx_task` 优先级15 + `xst_parse_task` 优先级10）
  - 滑动窗口分帧、异或校验、错误处理
  - 二值信号量异步唤醒解析任务（高效解耦）
  - 链表 + 队列双缓存
  - 主动通知回调机制（`xst_note_callback_t`）
  - 命令 API：`reset`/`get_status`/`enroll_single`/`verify`/`del_user`/`del_all`/`get_user_count`
  - 详细日志（MsgID/Result/NID 字符串翻译）
  - WiFi TCP 透传（`g_vofa_client_fd`），UART 收/发均实时转发
- **柜门控制**：`locker_t` 结构封装 4 路 GPIO，支持：
  - `locker_on()` / `locker_all_on()` — 脉冲开门
  - `Detection_locker_on_off()` — 门磁检测（高电平=开启）
  - `get_locker_by_id()` — 根据用户 ID 查找柜门
  - `crumble_password()` — 随机密码生成
  - `gpio_dump_io_configuration()` — GPIO 配置转储
- **Setting 页面部分按钮已打通**：
  - `RES` → `xst_cmd_reset()`
  - `locker1/2/3/4` → `locker_on(&lockers[0/1/2/3])`
  - `ALL` → `locker_all_on()`
- **主页面柜门状态显示**：启动时会读取 4 路门磁输入，并在首页用红/绿块显示一次初始状态。
- **Take 页面**：扫脉界面（`cont_1`）与密码界面（`cont_2`）可切换；密码当前硬编码为 `0000`，验证后只更新提示文案，无实际开门。
- **Setting 页面密码门禁**：设置页默认先显示 4 位密码键盘，管理员密码当前硬编码为 `1234`，输入正确后才显示设置菜单。
- **Save 页面**：显示引导文案 + 40 帧动画 + 静态进度条（固定 50）+ 静态标签（当前显示为 `2`），无实际录入逻辑。
- **调试链路**：板端 SoftAP（`ESP32_DEBUG_WIFI`），TCP(`8080`) 双向 HEX 透传。

### 进行中
- **Setting 页面 XST 命令**：`READ`/`VERI`/`DELL` 按钮当前仅更新 `NOTE` 标签，未真正调用 `xst_cmd_get_user_count()` / `xst_cmd_verify()` / `xst_cmd_del_all()`。
- **Take 页面取件流程**：
  - 扫脉界面目前仅显示提示文案和静态图片，未触发 `xst_cmd_verify()`。
  - 密码验证成功后仅有 UI 提示，未调用 `locker_on()` 开门。
- **Save 页面存件流程**：仅有返回主菜单交互，无掌静脉录入、用户分配、柜门绑定逻辑。
- **XST UI 集成**：识别/录入结果、模组状态（`READY`/`PALM_STATE`/`ERROR`）当前只写日志，未在页面上实时呈现。
- **Setting 页面 spangroup**：当前内容仍为 `hello`，未显示用户数或模组状态。
- **数据持久化**：管理员密码、取件密码、用户-柜门绑定关系目前均为硬编码或仅驻留 RAM，未接入 NVS。
- `xst_cmd_get_user_info()` 已声明但未实现（TODO）。

## 主要模块
| 文件 | 说明 |
|------|------|
| `main/main.c` | 系统入口：NVS、LVGL Tick、显示/触摸、柜门、XST、WiFi AP、TCP |
| `bsp/display/` | `ST7789` SPI LCD 驱动 + 背光 PWM，DMA 双缓冲 |
| `bsp/lvgl_port/lv_port_disp.c` | LVGL 显示适配层，渲染缓冲加倍至 80 行 |
| `bsp/lvgl_port/lv_port_indev.c` | LVGL 触摸适配层，坐标修正 |
| `bsp/lvgl/src/lv_conf.h` | LVGL 配置文件，内存/调度/刷新/DPI 优化 |
| `bsp/XST/xst.c` | 掌静脉模组协议栈（双任务异步解析，583行） |
| `bsp/XST/xst_pack_t.h` | 协议数据类型定义 |
| `bsp/locker/locker.c` | 柜门 GPIO 控制与门磁检测（162行） |
| `bsp/locker/locker.h` | 柜门 API 声明 |
| `bsp/ui/generated/` | Gui Guider 生成的 LVGL UI 代码；当前页面事件和部分业务逻辑也写在 `events_init.c` |
| `bsp/ui/custom/` | 自定义扩展区（当前基本未使用） |

## XST 模组架构
### 设计亮点
- **双任务解耦**：`xst_uart_rx_task`（优先级15）负责接收切帧；`xst_parse_task`（优先级10）负责解析分发
- **二值信号量异步唤醒**：解析任务仅在有数据时唤醒，不空转耗 CPU
- **链表 + 队列双缓存**：链表存待解析帧，队列用于 API 阻塞等待回复
- **完整日志调试**：每个 MsgID/Result/NID 都有字符串翻译
- **通知回调机制**：上层注册 `xst_note_callback_t` 处理 NOTE 消息
- **TCP 透传**：`g_vofa_client_fd` 全局 socket，UART 双向数据实时转发手机

### 已实现的命令 API
| 函数 | 功能 | 状态 |
|-----|------|------|
| `xst_cmd_reset()` | 模组硬件复位（上电掉电） | ✅ 已实现，已打通 Setting 页面 RES 按钮 |
| `xst_cmd_get_status(status)` | 获取模组状态 | ✅ 已实现 |
| `xst_cmd_enroll_single(name,admin,timeout,out_user_id)` | 单次活体录入 | ✅ 已实现 |
| `xst_cmd_verify(timeout,out_user_id,out_name)` | 活体识别验证 | ✅ 已实现 |
| `xst_cmd_del_user(user_id)` | 删除指定用户 | ✅ 已实现 |
| `xst_cmd_del_all()` | 清空所有用户 | ✅ 已实现 |
| `xst_cmd_get_user_count(count)` | 获取已注册用户数 | ✅ 已实现 |
| `xst_cmd_get_user_info(query_id,out_info)` | 查询用户信息 | ⚠️ **已声明但未实现**（TODO） |

## 硬件配置
### 显示屏 ST7789（SPI2 / DMA）
- `SCK=12`，`MOSI=11`，`CS=10`，`DC=9`，`RST=8`，`BL=48`
- 分辨率：`240x320`，SPI 时钟：40 MHz
- 渲染缓冲：80 行（DMA 传输优化）

### 触摸 FT6336U（I2C）
- `SDA=1`，`SCL=2`，`INT=14`
- I2C 地址：`0x38`
- 坐标已修正（`lv_port_indev.c`）

### 柜门控制/检测
- 锁控：`GPIO38/GPIO39/GPIO40/GPIO41`
- 检测：`GPIO15/GPIO16/GPIO17/GPIO18`（高电平=门开启）

### XST 掌静脉模组
- 电源：`GPIO43`（高电平供电）
- 串口：`UART1`，`TX=GPIO5`，`RX=GPIO4`，`115200`

## 快速开始
```bash
idf.py set-target esp32s3
idf.py menuconfig
idf.py build
idf.py -p PORT flash monitor
```

## 调试链路
- WiFi AP：`ESP32_DEBUG_WIFI` / 密码 `12345678`
- TCP 端口：`8080`
- 连接 AP 后，TCP 客户端连接 `8080` 可与 XST 串口做双向 HEX 透传

## 下一步计划
- [ ] **实现 `xst_cmd_get_user_info()`**
- [ ] **Setting 页面 XST 命令打通**：`READ` → `xst_cmd_get_user_count()`，`VERI` → `xst_cmd_verify()`，`DELL` → `xst_cmd_del_all()`
- [ ] **Take 页面**：密码验证成功后调用 `locker_on()` 开门
- [ ] **Take 页面**：扫脉模式接入 `xst_cmd_verify()`
- [ ] **Save 页面**：整合 `xst_cmd_enroll_single()` 录入流程，分配柜号、绑定密码
- [ ] **XST UI 集成**：识别结果/模组状态在屏幕上实时显示
- [ ] **Setting 页面 spangroup**：填充为用户状态/模组状态显示
- [ ] **NVS 持久化**：管理员密码、用户-柜门绑定关系、存取记录
- [ ] **OTA 升级支持**

## 已知问题 & 优化空间
- `xst_cmd_get_user_info()` 已在头文件声明但未实现
- Setting 页面 `READ`/`VERI`/`DELL` 按钮仅打印 NOTE，未调用 XST 命令
- Take 页面密码"0000"硬编码，无 NVS 持久化
- Setting 页面管理员密码 `1234` 硬编码，无 NVS 持久化
- Take 页面扫脉模式当前没有接入真实识别流程
- PALM_STATE / FACE_STATE / NID_UNKNOWNERROR 通知仅打印日志，未触发 UI 更新
- Setting 页面 spangroup 当前内容为 "hello"
- Save 页面进度条（`bar_1`）和标签（`label_2`）当前为静态值，无动态业务逻辑
- `crumble_password()` 中 `sizeof(password)` 在实参传递时退化为指针大小（4字节），密码覆盖逻辑有 bug
- 主页面 4 个柜门状态指示灯仅在启动时更新一次，后续开关门不会自动刷新 UI
- TCP 链路在 WiFi 断开时无自动重连机制

## 注意事项
- `bsp/ui/generated/` 下文件理论上是 Gui Guider 生成区，但当前项目的页面事件和部分业务逻辑直接写在 `bsp/ui/generated/events_init.c` 中；如果后续用 Gui Guider 重新生成，需要先迁移或备份这些手写逻辑。
- `bsp/ui/custom/` 是自定义扩展区，不会被 Gui Guider 覆盖，但当前尚未承接主要业务逻辑。
- 若改动分区/组件配置，建议执行 `idf.py fullclean` 后重新构建。

## 许可证与反馈
License：待定。  
问题或建议请提交 issue：<https://github.com/3233458843/smart_locker/issues>
