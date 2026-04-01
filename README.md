# 智能储物柜控制系统（ESP32-S3）

## 项目概述
基于 **ESP32-S3** 的智能储物柜控制方案，整合触控 LCD、XST 指纹模组和 LVGL 界面，在 FreeRTOS 上实现交互、安全与多任务并行。项目依托 ESP-IDF 5.0+，BSP 统一封装各外设，便于直接构建、演示和扩展。

## 主要特性
- ESP32-S3 + FreeRTOS：`main/main.c` 启动 LVGL 任务、1 ms Tick 定时器，并完成显示与触摸初始化。
- LVGL 界面：Gui Guider 生成，代码位于 `bsp/ui/generated`，`setup_scr_main.c` 负责绑定界面元素与事件。
- 指纹识别：`bsp/XST` 提供初始化、录入、删除、验证、状态查询等 UART 命令接口。
- 显示与触摸 BSP：`bsp/display` 和 `bsp/lvgl_port` 封装屏幕与触控驱动，确保 UI 可直接跑在实机。
- 可重复构建：`partitions.csv`、`dependencies.lock` 记录分区与依赖锁定，跨机器保持一致。

## 架构简述
- `app_main`：完成 LVGL、显示、触摸初始化后创建 `lvgl_demo_task`，每 5 ms 处理 GUI 循环。
- UI 胶水层：`bsp/ui/generated/gui_guider.h` 与 `events_init.h` 暴露 `setup_ui()` / `events_init()` 供 LVGL 任务一次性调用。
- LVGL 端口：`lv_port_disp_init()`、`lv_port_indev_init()` 位于 `bsp/lvgl_port`，注册显示与输入设备。
- 指纹接口：`bsp/XST` 的 C API 将 GUI 命令转换为 UART 协议，供各屏幕调用。

## 仓库结构
- `.devcontainer/`、`.vscode/`、`.clangd`：可选的 VS Code/Dev Container 配置。
- `CMakeLists.txt`：ESP-IDF 根配置。
- `dependencies.lock`：`idf.py` 使用的依赖锁定文件。
- `partitions.csv`：Flash 分区表，如需 OTA/NVS 调整请同步更新。
- `main/`：应用层，包含入口 `main.c`。
- `bsp/`：BSP 与 UI 代码  
  - `bsp/display`：LCD 与触摸驱动。  
  - `bsp/lvgl_port`：LVGL 显示/输入端口。  
  - `bsp/XST`：指纹模组协议与 API。  
  - `bsp/ui/generated`：Gui Guider 生成的界面及事件钩子。  
  - `bsp/ui/src`（若存在）：自定义界面辅助代码。

## 快速开始
### 环境准备
1. 安装 [ESP-IDF v5.0+](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32s3/get-started/) 及 Python 工具链。
2. 确认 `idf.py` 已在 PATH 中。
3. 连接 ESP32-S3 开发板，记录串口号（如 `/dev/ttyUSB0` 或 `COM3`）。

### 构建与烧录
```bash
git clone https://github.com/3233458843/smart_locker.git
cd smart_locker
idf.py set-target esp32s3
idf.py menuconfig          # 校验串口、引脚等配置
idf.py build
idf.py -p PORT flash
idf.py -p PORT monitor
```
推荐使用 `idf.py -p PORT flash monitor` 一步完成烧录并查看日志；若切换目标板或分区表，请先 `idf.py fullclean`。

## UI 与显示说明
- `bsp/lvgl/src/lv_conf.h` 控制 LVGL 功能开关，保持默认缓冲/显示设置能确保界面正常。
- `bsp/ui/generated/setup_scr_main.c` 为自动生成文件，修改界面请在 Gui Guider 中更新后重新生成再编译；触摸/点击事件绑定在同目录生成代码并由 `events_init.c` 引用。
- 动画与控件引用 `esp-idf/generated/images/` 中的资源，新增素材时记得同步头文件与资源列表。

## BSP 与外设指引
- `bsp/display`：屏幕驱动（`display.c`）与触摸辅助（`touch.c`）；新增屏幕在此封装并在 `lv_port_disp_init()` 注册。
- `bsp/XST`：界面可直接调用 C API（如 `xst_cmd_verify`、`xst_cmd_enroll_single`）与指纹模组通信。
- GUI 与外设交互建议通过 `events_init()` 中的回调流转，保持界面逻辑解耦。

## 扩展路线
1. 新增 BSP 模块：创建 `bsp/<module>`，补充代码/CMakeLists，并在 `bsp/` 暴露头文件。
2. 在 `main/main.c` 调用新 API；若涉及实时性，考虑单独的 FreeRTOS 任务。
3. 在 Gui Guider 添加新界面/控件，重新生成后 `events_init.c` 与 `setup_scr_main.c` 会自动更新。
4. 调整分区请同步修改 `partitions.csv`，并在 `idf.py menuconfig` 校验。

## 常见提示
- 每次拉取更新后跑一遍 `idf.py menuconfig`，防止新组件或配置缺失。
- 烧录异常时检查 `ESPPORT`、线缆与板卡是否进入下载模式，可用 `idf.py -p PORT monitor` 查看日志。
- 依赖升级后执行 `idf.py update-dependencies` 并提交更新后的 `dependencies.lock`。

## 许可证与反馈
License：待定。  
如有问题或建议，欢迎在仓库提交 [issue](https://github.com/3233458843/smart_locker/issues)。
