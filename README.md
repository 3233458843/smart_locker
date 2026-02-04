# Smart Locker 智能门锁系统

## 项目简介

Smart Locker 是一个基于 ESP32-S3 微控制器的智能门锁系统。通过指纹识别（XST）、显示屏和触摸屏交互，提供安全可靠的门锁解决方案。

## 支持的目标芯片

| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |

## 主要功能（初版）

- **指纹识别**：通过 XST 指纹模块进行用户身份验证
- **显示输出**：LCD 显示屏实时反馈门锁状态
- **触摸交互**：触摸屏提供用户交互界面
- **用户管理**：支持指纹注册、删除和用户查询

## 项目结构

```
smart_locker/
├── CMakeLists.txt              # 项目构建配置
├── sdkconfig                   # ESP-IDF 配置文件
├── main/                       # 主应用程序
│   ├── main.c                  # 主程序入口
│   ├── main.h                  # 主程序头文件
│   └── CMakeLists.txt
├── bsp/                        # 硬件抽象层（Board Support Package）
│   ├── display/                # 显示屏驱动模块
│   │   ├── display.c
│   │   ├── display.h
│   │   ├── touch.c             # 触摸屏驱动
│   │   ├── touch.h
│   │   └── CMakeLists.txt
│   └── XST/                    # 指纹识别模块（XST 协议）
│       ├── xst.c
│       ├── xst.h
│       ├── xst_pack_t.h        # 数据包定义
│       └── CMakeLists.txt
└── README.md                   # 项目说明文档
```

## 硬件配置

### XST 指纹模块接口
- **通信协议**：UART2
- **TX 引脚**：GPIO 4
- **RX 引脚**：GPIO 6
- **波特率**：115200 bps

### 其他模块
- 显示屏驱动（LCD）
- 触摸屏驱动

## 快速开始

### 前置要求
- ESP-IDF v5.0 或更高版本
- ESP32-S3 开发板
- 配置好的工具链

### 构建项目

```bash
# 配置项目
idf.py menuconfig

# 构建项目
idf.py build

# 烧录到设备
idf.py -p <PORT> flash

# 监控串口输出
idf.py -p <PORT> monitor
```

## API 概览

### XST 指纹模块 API

```c
// 初始化指纹模块
void xst_init(xst_note_callback_t callback);

// 重置模块
xst_result_t xst_cmd_reset(void);

// 获取模块状态
xst_result_t xst_cmd_get_status(uint8_t *status);

// 指纹注册
xst_result_t xst_cmd_enroll_single(const char *name, uint8_t admin, 
                                   uint8_t timeout, uint16_t *out_user_id);

// 删除用户指纹
xst_result_t xst_cmd_del_user(uint16_t user_id);

// 删除所有用户
xst_result_t xst_cmd_del_all(void);

// 获取用户数量
xst_result_t xst_cmd_get_user_count(uint16_t *count);

// 获取用户信息
xst_result_t xst_cmd_get_user_info(uint16_t query_id, xst_user_info_t *out_info);

// 指纹验证
xst_result_t xst_cmd_verify(uint8_t timeout, uint16_t *out_user_id, char *out_name);
```

## 使用 FreeRTOS

项目基于 FreeRTOS 实时操作系统，支持多任务并发运行。

## 开发指南

### 添加新功能

1. 如果是硬件驱动，在 `bsp/` 目录下创建新的模块
2. 在 `main/` 目录下实现应用逻辑
3. 更新对应的 `CMakeLists.txt` 配置文件

### 编译命令

```bash
# 全量构建
idf.py build

# 清理编译产物
idf.py fullclean

# 仅编译应用
idf.py build app
```

## 注意事项

- 本项目为初版实现，功能仍在开发中
- 请确保 ESP-IDF 路径配置正确
- 首次使用需配置 menuconfig 以适应硬件差异

## 许可证

[待定]

## 联系方式

[待定]
