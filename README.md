# 智能储物柜控制系统（ESP32-S3）

## 项目概述
基于 **ESP32-S3** 的智能储物柜控制方案，整合触控 LCD、XST 掌静脉识别模组和 LVGL 界面，在 FreeRTOS 上实现交互、安全与多任务并行。项目依托 ESP-IDF 5.3.3，采用分层架构设计，支持物品存放检测、完整的存/取业务流程、实时 UI 状态更新和蜂鸣器反馈。

## 主要特性
- **多核任务架构**：主要逻辑运行在两个核心，支持并行处理 LVGL UI、掌静脉识别、柜门控制等耗时操作。
- **LVGL 实时界面**：Gui Guider 生成，代码位于 `bsp/ui/generated`；动态显示 4 个柜位的实时开/关状态。
- **掌静脉识别**：`bsp/XST` 提供完整 UART 协议接口（初始化、录入、删除、验证、状态查询）。
- **储物柜物品检测**：独立的 `have_saved` 标志与门开关状态解耦，准确判断物品存放情况。
- **存/取业务流程**：通过信号量驱动的异步任务，支持掌静脉识别、自动柜号分配、密码生成、NVS 持久化存储。
- **蜂鸣器驱动与反馈**：支持外部设备注册、状态机管理、GPIO/PWM 实现切换的抽象设计；业务全流程接入成功/错误提示音。
- **实时状态同步**：专用任务每秒轮询柜门传感器，自动更新 UI 显示。
- **NVS 数据持久化**：用户-柜号-密码绑定数据跨电源周期保持，支持快速恢复。

## 核心架构
### 应用层（`main/`）
- `main.c`：系统入口，初始化 NVS、serve 业务层、LVGL、XST、蜂鸣器、WiFi/TCP 等组件。
- `serve_init()`：启动 3 个异步任务（`ready_save_task`、`ready_take_task`、`verify_debug_task`）处理业务逻辑。
- `main_serve` 任务：每 1 秒自动刷新 4 个柜位的 UI 显示状态。

### 业务服务层（`serve/`）
- `serve.c`：中间层解耦 UI 与硬件控制，通过信号量接收 UI 事件并驱动业务流程。
- **状态管理**：`serve_save_status_t` / `serve_take_status_t` 记录完整流程状态，UI 通过 `serve_get_save_status()` / `serve_get_take_status()` 查询。
- **请求接口**：`serve_request_save()` / `serve_request_take_by_palm()` / `serve_request_take_by_password()` — UI 通过统一 API 发起业务，不直接操作信号量。
- **蜂鸣器反馈**：所有业务节点（成功/失败/错误）均接入 `buzzer_beep_pattern()` 提示音。
- `ready_save_task`：
  - 遍历 4 个柜位查找空闲柜
  - 调用 `xst_cmd_enroll_single()` 录入新用户（失败自动重试 3 次）
  - 自动分配柜号、生成随机 4 位密码
  - 打开柜门让用户放入物品
  - 数据保存到 NVS
- `ready_take_task`：
  - 调用 `xst_cmd_verify()` 验证用户身份
  - 查询 NVS 获取用户对应的柜号
  - 打开对应柜门
  - 删除 XST 中的用户记录
  - 清除 NVS 中的绑定信息

### BSP 层
#### 柜门控制（`bsp/locker/`）
- `locker.c/h`：
  - 4 个柜位的 GPIO 驱动（开/关/检测）
  - `locker_init()`：初始化 GPIO 输出（控制）和输入（门状态检测）
  - `locker_on()`：打开柜门（输出高电平 500ms，触发机械锁定，然后返回低电平）
  - `Detection_locker_on_off()`：检测门是否打开（读取检测引脚）
  - `has_item_in_locker()`：判断物品存放状态（读取 `have_saved` 标志）
  - `is_locker_secured()`：判断柜子是否安全（物品存放 + 门关闭）
  - **NVS 数据库**：
    - `locker_db_init()`：启动时从 NVS 恢复用户绑定数据
    - `locker_db_add_entry()`：添加用户-柜号-密码绑定
    - `locker_db_get_entry_by_user()`/`locker_db_get_entry_by_locker()`：查询绑定
    - `locker_db_remove_entry_by_locker()`：删除绑定（取件完成时调用）
    - `locker_db_save_to_nvs()`：持久化存储

#### 蜂鸣器驱动（`bsp/buzzer/`）
- `buzzer.c/h`：
  - **设备外部注册模式**：通过 `buzzer_create_gpio_device()` 创建设备，然后 `buzzer_register_device()` 注册。
  - **状态机管理**：OFF/IDLE/ON/BEEPING/ERROR 状态转换。
  - **Ops 多态结构**：定义 `buzzer_ops_t` 包含 init/deinit/turn_on/turn_off/beep_once/beep_pattern/get_state 函数指针。
  - **GPIO 实现**：`gpio_buzzer_ops` 提供 GPIO-based 蜂鸣器实现。
  - **异步控制**：支持 FreeRTOS 定时器驱动的鸣叫模式（短鸣、长鸣、连续鸣）。
  - **线程安全**：使用 mutex 保护设备状态。

#### XST 掌静脉模组（`bsp/XST/`）
- `xst.c/h`：UART 驱动与协议解析。
- 支持接口：
  - `xst_init()`：UART 初始化 + 回调注册
  - `xst_cmd_enroll_single()`：录入新用户，返回用户 ID
  - `xst_cmd_verify()`：验证用户身份，返回用户 ID
  - `xst_cmd_del_user()`：删除用户记录
  - 状态回调：`NID_READY`、`NID_PALM_STATE`、`NID_UNKNOWNERROR` 等

#### LVGL & 触摸（`bsp/display/` 、`bsp/lvgl_port/`）
- `display.c/h`：屏幕初始化与显示驱动
- `touch.c/h`：触摸输入驱动
- `lv_port_disp_init()`：注册显示设备给 LVGL
- `lv_port_indev_init()`：注册输入设备给 LVGL

#### UI 与事件（`bsp/ui/generated/`）
- `setup_scr_main.c`：Gui Guider 生成的界面定义。
- `events_init.c`：按钮、屏幕等事件回调：
  - **存件**：`save_page_enroll_task()` 在后台运行，获取掌纹后发送 `ready_save` 信号量。
  - **取件**：`take_page_verify_task()` 在后台运行，验证掌纹后发送 `ready_take` 信号量。
  - 屏幕切换事件、按钮回调等。

### 信号流程
```
┌─────────────────┐
│   LVGL UI       │  (保存/取件按钮)
└────────┬────────┘
         │ 
         ▼ (create task)
┌─────────────────┐
│ enroll/verify   │  (异步识别，10秒超时)
│ task (events    │
│  _init.c)       │
└────────┬────────┘
         │ (xSemaphoreGive)
         ▼
┌─────────────────┐
│ serve layer     │  (接收信号量)
│ (serve.c)       │
└────────┬────────┘
         │
      ┌──┴───────────────────┐
      ▼                      ▼
┌──────────────┐      ┌──────────────┐
│  Save Flow   │      │  Take Flow   │
│  - XST注册   │      │  - XST验证   │
│  - 柜号分配  │      │  - 查询NVS   │
│  - 密码生成  │      │  - 开柜门    │
│  - NVS保存   │      │  - 删除用户  │
│  - 开柜门    │      │  - NVS清除   │
└──────────────┘      └──────────────┘
      │                      │
      └──────────┬───────────┘
                 ▼
         ┌──────────────────┐
         │  GPIO 柜门控制   │
         │ (locker.c)       │
         └──────────────────┘
```

### GPIO 管理
| 功能 | 引脚 | 方向 | 说明 |
|------|------|------|------|
| 柜门1控制 | GPIO 38 | 输出 | 高电平触发开锁 |
| 柜门2控制 | GPIO 39 | 输出 | 高电平触发开锁 |
| 柜门3控制 | GPIO 40 | 输出 | 高电平触发开锁 |
| 柜门4控制 | GPIO 41 | 输出 | 高电平触发开锁 |
| 柜门1检测 | GPIO 15 | 输入 | 高电平=门打开，低电平=门关闭 |
| 柜门2检测 | GPIO 16 | 输入 | 高电平=门打开，低电平=门关闭 |
| 柜门3检测 | GPIO 17 | 输入 | 高电平=门打开，低电平=门关闭 |
| 柜门4检测 | GPIO 18 | 输入 | 高电平=门打开，低电平=门关闭 |
| 蜂鸣器 | GPIO 13 | 输出 | 高电平驱动蜂鸣 |
| XST UART | TX: GPIO 42<br/>RX: GPIO 41 | 双向 | 掌静脉模组通信 |

## 目录结构
```
smart_locker/
├── CMakeLists.txt              # ESP-IDF 项目根配置
├── partitions.csv              # Flash 分区表（NVS/OTA/APP）
├── dependencies.lock           # 依赖锁定文件
├── sdkconfig                   # IDF 编译配置快照
├── README.md                   # 项目文档
├── main/
│   ├── CMakeLists.txt         # 应用层编译配置
│   └── main.c                 # 系统入口：初始化所有子系统
├── serve/
│   ├── CMakeLists.txt         # 业务层编译配置
│   ├── serve.c                # 异步业务流程处理（存/取）
│   └── serve.h                # 业务层头文件与信号量声明
├── bsp/
│   ├── buzzer/                # 蜂鸣器驱动（外部注册+状态机）
│   │   ├── buzzer.c
│   │   ├── buzzer.h
│   │   └── CMakeLists.txt
│   ├── locker/                # 柜门控制与 NVS 数据库
│   │   ├── locker.c           # GPIO 驱动 + 数据库实现
│   │   ├── locker.h           # 公共接口与结构定义
│   │   └── CMakeLists.txt
│   ├── display/               # 显示屏驱动与触摸驱动
│   │   ├── display.c
│   │   ├── touch.c
│   │   ├── *.h
│   │   └── CMakeLists.txt
│   ├── lvgl_port/             # LVGL 适配层（显示 + 输入）
│   │   ├── lv_port_disp.c     # 显示设备注册
│   │   ├── lv_port_indev.c    # 输入设备注册
│   │   └── CMakeLists.txt
│   ├── ui/                    # Gui Guider 生成的界面
│   │   └── generated/
│   │       ├── setup_scr_main.c       # 界面定义
│   │       ├── events_init.c          # 事件回调（存/取流程）
│   │       ├── gui_guider.h
│   │       └── *.h
│   ├── XST/                   # 掌静脉识别模块驱动
│   │   ├── xst.c              # UART 协议 + 命令处理
│   │   ├── xst.h              # 公共 API
│   │   ├── xst_pack_t.h       # 数据包定义
│   │   └── CMakeLists.txt
│   └── lvgl/                  # LVGL 配置文件
│       └── src/lv_conf.h      # LVGL 功能开关
└── build/                     # 编译输出目录（自动生成）
```

## 快速开始

### 环境准备
1. 安装 [ESP-IDF v5.3.3](https://docs.espressif.com/projects/esp-idf/zh_CN/v5.3.3/esp32s3/get-started/)
   ```bash
   # 激活 IDF 环境
   source ~/esp-idf/export.sh    # Linux/Mac
   # 或
   %USERPROFILE%\esp-idf\export.bat  # Windows CMD
   # 或
   & "${env:USERPROFILE}\esp-idf\export.ps1"  # Windows PowerShell
   ```
2. 确认 `idf.py` 可用：`idf.py --version`
3. 连接 ESP32-S3 开发板，确认串口（Windows: `COM3` 等；Linux: `/dev/ttyUSB0` 等）

### 构建与烧录

#### 方法1：使用 IDF 脚本
```bash
# 设置目标芯片
idf.py set-target esp32s3

# 配置项目（可选，通常用默认值即可）
idf.py menuconfig

# 清理旧构建（首次或更换分区表时必做）
idf.py fullclean

# 编译
idf.py build

# 烧录并查看日志
idf.py -p COM3 flash monitor
# 或分开执行：
idf.py -p COM3 flash
idf.py -p COM3 monitor
```

#### 方法2：Windows PowerShell（推荐用于此项目）
```powershell
# 进入项目目录
cd E:\esp_code\smart_locker

# 构建并烧录
& "D:\ESP_IDF\Espressif\tools\idf-exe\1.0.3\idf.py.exe" -B ".\build" build
& "D:\ESP_IDF\Espressif\tools\idf-exe\1.0.3\idf.py.exe" -p COM3 -B ".\build" flash
& "D:\ESP_IDF\Espressif\tools\idf-exe\1.0.3\idf.py.exe" -p COM3 monitor
```

### 首次启动检查清单
- [ ] 串口日志显示所有子系统初始化成功（NVS、LVGL、XST、蜂鸣器、WiFi）
- [ ] LVGL 主页面显示 4 个柜位，初始状态应为绿色（已关闭）
- [ ] WiFi AP "ESP32_DEBUG_WIFI" 可被手机搜到（密码: 12345678）
- [ ] 按下存件按钮后，屏幕进入掌纹录入界面并倒计时 10 秒
- [ ] 掌纹识别完成后，某个柜位自动打开（听到蜂鸣器鸣叫）
- [ ] 取件按钮功能类似，但验证已存储用户并打开对应柜号

## 核心功能详解

### 存件流程（Save Flow）
1. 用户在 LVGL 主页点击"存件"按钮
2. 屏幕切换到掌纹录入页，显示 10 秒倒计时
3. `save_page_enroll_task()` 异步调用 `xst_cmd_enroll_single()`，获取掌纹数据
4. 录入成功后，发送 `ready_save` 信号量到 `ready_save_task()`
5. `ready_save_task()` 执行：
   - 遍历 4 个柜位，找空闲柜（`have_saved == false`）
   - 将新用户 ID、密码、时间戳保存到 NVS（`locker_db_add_entry()`）
   - 更新 `locker_t.have_saved = true` 和密码
   - 打开对应柜门（`locker_on()`）
   - 返回 UI，显示密码和柜号
6. 用户放入物品后关闭柜门，系统自动检测门关闭（GPIO 检测引脚）

### 取件流程（Take Flow - 掌纹）
1. 用户在 LVGL 主页点击"取件"按钮
2. 屏幕切换到掌纹验证页，显示 10 秒倒计时
3. `take_page_verify_task()` 异步调用 `xst_cmd_verify()`，验证掌纹
4. 验证成功后，发送 `ready_take` 信号量到 `ready_take_task()`
5. `ready_take_task()` 执行：
   - 根据返回的 `user_id` 在 NVS 中查找绑定记录（`locker_db_get_entry_by_user()`）
   - 获取用户对应的柜号
   - 打开对应柜门（`locker_on()`）
   - 调用 `xst_cmd_del_user()` 删除 XST 中的用户记录
   - 调用 `locker_db_remove_entry_by_locker()` 清除 NVS 绑定
   - 柜位恢复为空闲状态

### 取件流程（Take Flow - 密码）
1. 用户在取件页面点击"密码"按钮切换到密码输入模式
2. 通过4位密码键盘输入4位密码
3. `serve_request_take_by_password()` 在 NVS 中查找对应柜号
4. 执行开柜、清除绑定（同掌纹取件流程）

### 实时状态监控（`main_serve` 任务）
- 每 2 秒循环执行
- 读取 4 个柜位的门传感器 GPIO
- 仅监测硬件状态，UI 更新由 `lvgl_demo_task` 处理：

### 蜂鸣器反馈
- 系统启动时：短鸣 1 次（`BUZZER_BEEP_SHORT`）
- 存件/取件流程各节点均已接入音频提示：
  - 掌纹录入成功 → `BUZZER_BEEP_SHORT`
  - 掌纹验证成功/密码正确开柜 → `BUZZER_BEEP_SUCCESS`（连响 3 次）
  - 流程失败/错误 → `BUZZER_BEEP_ERROR`
- 蜂鸣器句柄全局共享，serve 层通过 `g_buzzer_handle` 直接调用

### NVS 数据持久化
**命名空间**：`"locker"`  
**存储结构**：
```c
user_locker_entry_t {
  uint16_t user_id;      // XST 返回的用户ID
  uint8_t locker_id;     // 0-3
  uint8_t password[4];   // 4 位随机密码
  uint32_t timestamp;    // 存件时间
  bool is_valid;         // 是否有效
}
```
存储方式：4 个槽位对应 4 个柜位，键名为 `entry_0` 到 `entry_3`

## 开发指南

### 修改 UI 界面
1. 在 Gui Guider 中更新设计
2. 导出代码到 `bsp/ui/generated/`
3. 项目会自动重新编译
4. **注意**：不要直接编辑生成文件，事件处理逻辑写在 `events_init.c` 中

### 扩展 BSP 模块
1. 在 `bsp/` 下新建目录（如 `bsp/my_device/`）
2. 编写 `my_device.c/h` 与 `CMakeLists.txt`
3. 在 `CMakeLists.txt` 中指定 `PRIV_REQUIRES` 依赖（如 `nvs_flash`、`driver`）
4. 在 `main/main.c` 中调用初始化函数

### 添加新的异步任务
建议在 `serve.c` 中创建，遵循以下模式：
```c
void my_async_task(void* param) {
    while (1) {
        if (xSemaphoreTake(my_signal, portMAX_DELAY) == pdTRUE) {
            // 处理业务逻辑
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
```

### 调试常见问题

| 问题 | 原因 | 解决方案 |
|------|------|---------|
| 无法识别掌纹 | XST 模块通信失败 | 检查 UART 接线、波特率设置、GPIO 40/41 |
| 柜门不开 | GPIO 控制失败或引脚配置错误 | 检查 GPIO 38-41 是否正确配置为输出 |
| NVS 读取失败 | 分区表不匹配或 NVS 初始化未完成 | 运行 `idf.py fullclean` 后重新烧录 |
| UI 显示卡顿 | LVGL 任务优先级过低或内存不足 | 增加 LVGL 任务栈大小（主.c 中 10240 字节） |
| 掌纹识别超时 | 用户操作不当或光线不足 | 增加 timeout 参数或改进录入指引 |

## 项目进度跟踪

### ✅ 已完成功能
- [x] **硬件驱动**
  - [x] GPIO 柜门控制（打开/检测）
  - [x] XST 掌静脉识别（UART 通信、录入、验证、删除）
  - [x] 蜂鸣器驱动（GPIO、状态机、外部注册）
  - [x] LVGL 显示与触摸输入

- [x] **核心业务逻辑**
  - [x] 存件流程：掌纹录入 → 自动分配柜号 → 密码生成 → 开柜
  - [x] 取件流程（掌纹）：掌纹验证 → 查询绑定 → 开柜 → 数据清除
  - [x] 取件流程（密码）：4位密码输入 → NVS查询 → 开柜 → 数据清除
  - [x] 物品存放检测（独立于门状态）
  - [x] 柜门状态实时监控
  - [x] 存/取流程蜂鸣器提示音（成功/失败/错误）
  - [x] 掌纹实时进度显示（取件页识别进度 + 存件页录取进度条与百分比）
  - [x] 统一业务请求接口（serve_request_*），UI 不直接操作信号量
  - [x] ENROLL_PROGRESS 帧解析与进度值实时提取

- [x] **XST 管理员接口**
  - [x] 模组复位（RES 按钮）
  - [x] 用户数读取（READ 按钮）
  - [x] 用户删除（DELL 按钮）
  - [x] 识别测试（VERI 按钮）

- [x] **数据持久化**
  - [x] NVS 用户-柜号-密码绑定存储
  - [x] 启动时自动恢复数据
  - [x] 取件完成后自动清除

- [x] **系统集成**
  - [x] FreeRTOS 多任务架构（LVGL、业务、状态监控）
  - [x] 信号量驱动的异步业务流程
  - [x] WiFi AP + TCP 调试透传
  - [x] 实时 UI 状态更新（主页柜体颜色 + 取件/存件进度条）
  - [x] 密码取件界面（4位密码输入键盘）
  - [x] 管理员 Setting 页面（密码保护）
  - [x] 业务层状态管理（serve_save_status_t / serve_take_status_t）
  - [x] 蜂鸣器全流程音频反馈

### 🔄 进行中功能
- [ ] **系统稳定性优化**：LVGL 控件悬空指针保护、看门狗超时排查
- [ ] **XST 数据库运行时同步**：检测 XST 与本地 NVS 不一致时自动修复
- [ ] **OTA 升级**：预留分区，实现固件远程更新

### ⏳ 计划中功能
- [ ] **远程控制**：手机端完整存/取件控制与密码推送
- [ ] **多语言支持**：中英文切换
- [ ] **管理员认证**：密码/指纹保护 Setting 页面
- [ ] **日志系统**：记录存/取操作时间戳与用户信息
- [ ] **故障诊断**：传感器/执行器自检

## 技术亮点

### 设计模式
1. **外部设备注册模式**（蜂鸣器驱动）：将设备实例化与框架解耦，支持动态添加/移除
2. **状态机模式**（蜂鸣器、柜门）：通过明确的状态转换避免竞态条件
3. **信号量驱动架构**（serve 层）：UI 与业务逻辑完全解耦，支持并行执行
4. **NVS 槽位固定映射**（数据库）：使用固定 4 槽设计与 4 柜一一对应，简化查询逻辑

### 并发控制
- **核心分工**：Core 0 运行 LVGL 任务，Core 1 运行业务任务，避免竞争
- **互斥保护**：关键资源（如 buzzer 设备状态）使用 FreeRTOS mutex
- **事件驱动**：信号量替代轮询，降低 CPU 占用率

### 响应延迟
- **掌纹识别**：10 秒超时（异步任务，不阻塞 UI）
- **柜门开启**：< 1 秒（GPIO 高电平脉冲 500ms）
- **UI 状态刷新**：1 秒周期（dedicated task）
- **数据存储**：50-100 ms（NVS 写入时间）

## 许可证与反馈
**License**：MIT（开源）  
**作者**：Smart Locker Project Team  
**联系方式**：如有问题或建议，欢迎提交 Issue 或 Pull Request。

## 参考资源
- [ESP-IDF 官方文档](https://docs.espressif.com/projects/esp-idf/zh_CN/v5.3.3/esp32s3/)
- [LVGL 官方文档](https://docs.lvgl.io/)
- [FreeRTOS 教程](https://www.freertos.org/index.html)
- [ESP32-S3 芯片规格书](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_cn.pdf)

## 🐛 修复历史

### 2026-05-05 版本：蜂鸣器反馈 + 录取进度 + 稳定性修复
**新增功能**：
1. 蜂鸣器全流程提示音
   - 存件/取件/密码取件各节点接入成功/错误蜂鸣
   - 全局 `g_buzzer_handle` 共享，serve 层直接调用

2. 存件页掌纹录取实时进度
   - 实时进度条 (`save_page_bar_1`) + 百分比标签 (`save_page_label_2`)
   - 修复 `xst_exec_cmd()` 丢弃 `ENROLL_PROGRESS` (0x14) 帧的问题，提取进度值供 UI 显示

3. 统一业务请求接口重构
   - `serve_request_save()` / `serve_request_take_by_palm()` / `serve_request_take_by_password()`
   - UI 不再直接操作信号量
   - 完整状态机：`SERVE_FLOW_IDLE → PENDING → RUNNING → SUCCESS/FAILED`

**问题修复**：
1. Guru Meditation (LoadProhibited) 崩溃
   - 原因：`init_scr_del_flag` 使每次导航重建控件，旧屏幕删除后 `guider_ui` 指针悬空
   - 修复：所有 LVGL 控件访问加 `lv_obj_is_valid()` 保护

2. 存件页中文乱码
   - 原因：自定义字体 `Lemi_Little_Milk_Foam_Font_16` 不包含"正、在、录、入"等字符
   - 修复：只更新 `save_page_bar_1` 和 `save_page_label_2`（标准字体），不动主文案

**编译结果**：✅ 成功  
**文件修改**：
- `bsp/XST/xst.c`：ENROLL_PROGRESS 帧解析、进度全局变量 extern
- `serve/serve.c` / `serve/serve.h`：buzzer 集成、请求 API、状态管理
- `serve/CMakeLists.txt`：添加 buzzer 依赖
- `main/main.c`：lvgl_task 实时进度 + 控件有效保护
- `bsp/ui/generated/events_init.c`：存件页 SCREEN_LOADED 重置进度 UI

### 2026-04-26 版本：关键性能修复
**问题**：
1. 看门狗持续超时 (Task Watchdog Triggered)
   - 原因：`main_serve` 任务在 CPU 1 上不断调用 LVGL UI 函数 (`lv_obj_set_style_bg_color`)
   - 影响：LVGL 不是线程安全的，多任务并发调用导致长时间阻塞
   
2. XST 掌纹识别超时错误 (res=13: TIME_OUT)
   - 原因：单次失败后无重试机制
   - 影响：任何网络抖动或设备响应延迟都会导致流程失败

**修复**：
1. 移除 `main_serve` 中的 LVGL 直接调用
   - 改为仅监测柜门硬件状态（GPIO 输入）
   - UI 更新统一由 `lvgl_demo_task`（LVGL 核心任务）处理
   - 增加检测周期至 2 秒，进一步降低 CPU 占用
   
2. 添加 XST 掌纹识别重试机制
   - `ready_save_task` 中录入时最多重试 3 次
   - 重试间隔 1 秒，避免设备还未就绪
   - 完整的错误状态跟踪和日志记录
   
3. 改进状态跟踪与错误处理
   - 新增 `serve_save_status_t` 结构体记录完整流程状态
   - 添加 `serve_get_save_status()` 接口供 UI 查询
   - 详细的错误信息反馈

**编译结果**：✅ 成功  
**文件修改**：
- `main/main.c`：重构 `main_serve()` 任务
- `serve/serve.c`：添加重试机制和状态管理


