# Buzzer 驱动架构设计文档

## 目录
1. [概述](#概述)
2. [架构设计](#架构设计)
3. [核心组件](#核心组件)
4. [数据流](#数据流)
5. [接口设计](#接口设计)
6. [状态机](#状态机)
7. [线程安全](#线程安全)
8. [扩展指南](#扩展指南)

---

## 概述

### 目标
设计一个灵活、可扩展、线程安全的蜂鸣器驱动框架，支持：
- 多个蜂鸣器设备同时管理
- 不同硬件实现的抽象（GPIO、PWM、压电等）
- 异步蜂鸣模式播放
- 完整的生命周期管理

### 设计原则
1. **设备抽象**：通过 ops interface 隐藏硬件细节
2. **句柄管理**：使用 opaque handle 进行资源封装
3. **内部状态机**：每个设备独立管理状态转移
4. **线程安全**：FreeRTOS 互斥锁保护共享资源
5. **易于扩展**：支持新的蜂鸣器硬件实现

---

## 架构设计

### 分层架构

```
┌─────────────────────────────────────┐
│   应用层 (Application Layer)         │
│  (main.c, events_init.c等)          │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│  设备抽象层 (Device Abstraction)    │
│  ┌──────────────────────────────┐  │
│  │ GPIO Buzzer Ops Interface    │  │
│  │ PWM Buzzer Ops Interface     │  │
│  │ 其他硬件实现...              │  │
│  └──────────────────────────────┘  │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│  驱动核心层 (Driver Core)           │
│  ┌──────────────────────────────┐  │
│  │ 设备注册管理                 │  │
│  │ 句柄管理                     │  │
│  │ 生命周期控制                 │  │
│  │ 状态机引擎                   │  │
│  │ 定时器控制                   │  │
│  └──────────────────────────────┘  │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│  系统适配层 (System Adaptation)     │
│  ┌──────────────────────────────┐  │
│  │ FreeRTOS (Timer, Mutex)      │  │
│  │ ESP-IDF Drivers (GPIO, PWM)  │  │
│  │ Standard C Library           │  │
│  └──────────────────────────────┘  │
└─────────────────────────────────────┘
```

### 模块组成

#### 1. 驱动核心 (`buzzer_driver_core`)
- **职责**：管理全局设备注册表、分配句柄、协调生命周期
- **关键函数**：
  - `buzzer_register_device()` - 注册设备
  - `buzzer_get_handle()` - 获取句柄
  - `buzzer_init()` - 初始化驱动
  - `buzzer_deinit()` - 反初始化驱动

#### 2. 设备抽象层 (`buzzer_device_abstraction`)
- **职责**：定义操作接口、统一硬件访问
- **关键结构**：
  - `buzzer_ops_t` - 操作接口集合
  - `buzzer_device_t` - 设备基类

#### 3. GPIO 实现 (`gpio_buzzer_implementation`)
- **职责**：GPIO 蜂鸣器的具体实现
- **特点**：
  - 简单的高/低电平控制
  - 通过 FreeRTOS Timer 实现异步模式播放
  - 支持 6 种预定义蜂鸣模式

#### 4. 资源管理 (`resource_management`)
- **职责**：内存、互斥锁、定时器的生命周期管理
- **关键资源**：
  - 全局设备注册表 (最多 4 个)
  - 注册表保护互斥锁
  - 每个设备的私有互斥锁
  - FreeRTOS 定时器

---

## 核心组件

### 1. 数据结构设计

#### buzzer_ops_t - 操作接口
```c
typedef struct {
    esp_err_t (*init)(buzzer_device_t *dev);
    esp_err_t (*deinit)(buzzer_device_t *dev);
    esp_err_t (*turn_on)(buzzer_device_t *dev);
    esp_err_t (*turn_off)(buzzer_device_t *dev);
    esp_err_t (*beep_once)(buzzer_device_t *dev, uint32_t duration_ms);
    esp_err_t (*beep_pattern)(buzzer_device_t *dev, buzzer_beep_type_t beep_type);
    esp_err_t (*set_frequency)(buzzer_device_t *dev, uint32_t frequency_hz);
    esp_err_t (*set_volume)(buzzer_device_t *dev, uint8_t volume_percent);
    buzzer_state_t (*get_state)(buzzer_device_t *dev);
} buzzer_ops_t;
```

**设计说明**：
- 通过函数指针实现多态
- 每个硬件实现可以覆盖或保留默认行为
- 支持可选操作（如 set_frequency 可能不支持）

#### buzzer_device_t - 设备基类
```c
typedef struct buzzer_device {
    // 基础信息
    const char *name;
    uint8_t device_id;
    gpio_num_t gpio_pin;
    
    // 状态管理
    buzzer_state_t state;
    const buzzer_ops_t *ops;
    
    // 内部状态
    uint32_t current_frequency;
    uint8_t current_volume;
    
    // 蜂鸣模式
    uint32_t beep_pattern[BUZZER_BEEP_PATTERN_SIZE];
    uint8_t beep_pattern_len;
    uint8_t beep_pattern_idx;
    
    // 异步控制
    TimerHandle_t beep_timer;
    
    // 回调
    buzzer_callback_t on_complete_cb;
    void *callback_user_data;
    
    // 私有数据和互斥锁
    void *private_data;
    void *mutex;
} buzzer_device_t;
```

**设计说明**：
- 既是实际使用的结构，也是继承基类
- `private_data` 允许子类存储额外数据
- `mutex` 保护设备的并发访问
- `beep_timer` 实现异步蜂鸣模式

#### buzzer_handle_t_impl - 不透明句柄
```c
typedef struct buzzer_handle_s {
    buzzer_device_t *device;
} buzzer_handle_t_impl;
```

**设计说明**：
- 通过 opaque 指针 (`buzzer_handle_t`) 隐藏实现
- 应用程序无法直接访问设备结构
- 只能通过 API 与驱动交互

### 2. 全局资源

```c
// 设备注册表
static buzzer_device_t *g_buzzer_devices[BUZZER_MAX_DEVICES] = {NULL};

// 注册表保护互斥锁
static SemaphoreHandle_t g_registry_mutex = NULL;
```

**设计说明**：
- 固定大小数组 (最多 4 个设备)
- 互斥锁保护并发注册/注销
- 简单高效的 O(1) 查找

### 3. 私有数据结构

#### gpio_buzzer_private_t
```c
typedef struct {
    bool is_initialized;
    uint32_t on_duration_ms;
    bool pattern_running;
} gpio_buzzer_private_t;
```

**设计说明**：
- 存储在 `device->private_data` 中
- GPIO 实现特定的内部状态
- 允许不同实现具有不同的私有数据

---

## 数据流

### 初始化流程

```
应用程序
   │
   ├─→ buzzer_create_gpio_device(0, GPIO46, "buzzer0")
   │        └─→ malloc(buzzer_device_t)
   │        └─→ malloc(gpio_buzzer_private_t)
   │        └─→ xSemaphoreCreateMutex() [设备互斥锁]
   │        └─→ 返回 buzzer_device_t*
   │
   ├─→ buzzer_register_device(dev)
   │        ├─→ buzzer_init_registry() [创建注册表互斥锁]
   │        ├─→ buzzer_registry_lock()
   │        ├─→ g_buzzer_devices[0] = dev
   │        └─→ buzzer_registry_unlock()
   │
   └─→ buzzer_init()
            ├─→ buzzer_registry_lock()
            ├─→ for(i=0; i<BUZZER_MAX_DEVICES; i++)
            │    └─→ dev->ops->init(dev) [调用 GPIO 初始化]
            │         ├─→ gpio_config()
            │         ├─→ xTimerCreate() [创建 FreeRTOS 定时器]
            │         └─→ 设置 state = IDLE
            └─→ buzzer_registry_unlock()
```

### 蜂鸣播放流程

```
应用程序
   │
   ├─→ buzzer_get_handle(0)
   │    └─→ buzzer_registry_lock()
   │    └─→ dev = g_buzzer_devices[0]
   │    └─→ buzzer_registry_unlock()
   │    └─→ handle = malloc(buzzer_handle_t_impl)
   │    └─→ handle->device = dev
   │    └─→ 返回 handle
   │
   └─→ buzzer_beep_pattern(handle, BUZZER_BEEP_SUCCESS)
        │
        └─→ buzzer_setup_beep_pattern(dev, pattern, len)
             │
             ├─→ buzzer_lock_device(dev) [获取设备互斥锁]
             │
             ├─→ memcpy(dev->beep_pattern, pattern, ...)
             ├─→ dev->beep_pattern_idx = 0
             ├─→ dev->state = BEEPING
             │
             ├─→ gpio_set_level(GPIO46, 1) [打开蜂鸣器]
             │
             ├─→ xTimerChangePeriod(timer, first_duration)
             │    └─→ 定时器启动，duration 后触发回调
             │
             └─→ buzzer_unlock_device(dev)

[定时器中断到来...]
   │
   └─→ buzzer_beep_timer_callback(timer)
        │
        ├─→ buzzer_lock_device(dev)
        │
        ├─→ dev->beep_pattern_idx++ [移动到下一步]
        │
        ├─→ if (idx >= len) 
        │    ├─→ gpio_set_level(GPIO46, 0) [关闭蜂鸣器]
        │    ├─→ dev->state = IDLE
        │    └─→ on_complete_cb() [回调应用程序]
        │   else
        │    ├─→ gpio_level = idx % 2 [切换 ON/OFF]
        │    ├─→ gpio_set_level(GPIO46, gpio_level)
        │    └─→ xTimerChangePeriod(timer, next_duration)
        │
        └─→ buzzer_unlock_device(dev)
```

---

## 接口设计

### 1. 公共 API

#### 生命周期 API
```c
esp_err_t buzzer_init(void);
esp_err_t buzzer_deinit(void);
```

#### 设备管理 API
```c
esp_err_t buzzer_register_device(buzzer_device_t *dev);
esp_err_t buzzer_unregister_device(uint8_t device_id);
buzzer_handle_t buzzer_get_handle(uint8_t device_id);
```

#### 控制 API
```c
esp_err_t buzzer_on(buzzer_handle_t handle);
esp_err_t buzzer_off(buzzer_handle_t handle);
esp_err_t buzzer_beep_once(buzzer_handle_t handle, uint32_t duration_ms);
esp_err_t buzzer_beep_pattern(buzzer_handle_t handle, buzzer_beep_type_t beep_type);
```

#### 配置 API
```c
esp_err_t buzzer_set_frequency(buzzer_handle_t handle, uint32_t frequency_hz);
esp_err_t buzzer_set_volume(buzzer_handle_t handle, uint8_t volume_percent);
esp_err_t buzzer_set_callback(buzzer_handle_t handle, buzzer_callback_t callback, void *user_data);
```

#### 查询 API
```c
buzzer_state_t buzzer_get_state(buzzer_handle_t handle);
const buzzer_device_t *buzzer_get_device_info(buzzer_handle_t handle);
void buzzer_dump_devices(void);
```

### 2. 设备工厂函数

```c
buzzer_device_t *buzzer_create_gpio_device(uint8_t device_id, gpio_num_t gpio_pin, const char *device_name);
esp_err_t buzzer_destroy_gpio_device(buzzer_device_t *dev);
```

### 3. ops 接口

所有蜂鸣器实现都必须实现以下操作：

| 操作 | 说明 | 必需 |
|------|------|------|
| `init` | 初始化设备 | ✅ |
| `deinit` | 反初始化设备 | ✅ |
| `turn_on` | 连续开启 | ✅ |
| `turn_off` | 关闭 | ✅ |
| `beep_once` | 单次蜂鸣 | ✅ |
| `beep_pattern` | 蜂鸣模式 | ✅ |
| `set_frequency` | 设置频率 | ❌ (可选) |
| `set_volume` | 设置音量 | ❌ (可选) |
| `get_state` | 查询状态 | ✅ |

---

## 状态机

### 设备状态转移图

```
                    ┌─────────────────┐
                    │  BUZZER_STATE   │
                    │    ERROR        │
                    └────────┬────────┘
                             ▲
                             │
                  ┌──────────┐│┌──────────┐
                  │          ││          │
                  ▼          ││          ▼
        ┌───────────────┐    ││   ┌──────────────┐
        │ BUZZER_STATE  │    ││   │ BUZZER_STATE │
        │   OFF         │    ││   │    ON        │
        └───────────────┘    ││   └──────────────┘
                  ▲          ││          ▲
                  │          ││          │
                  │          │└──┐   ┌───┤
                  │          │   │   │   │
            ┌─────┴──────────┘   │   │   │
            │                    │   │   │
            │  ┌─────────────────┴───┴───┴────┐
            │  │ BUZZER_STATE_BEEPING          │
            │  │ (模式播放中，自动转移)       │
            │  └─────────────────┬──────────────┘
            │                    │
            │                    │[模式完成]
            │  ┌─────────────────▼──────────────┐
            └──┤ BUZZER_STATE_IDLE              │
               │ (准备就绪)                     │
               └────────────────────────────────┘
```

### 状态说明

| 状态 | 说明 | 转移 |
|------|------|------|
| `OFF` | 蜂鸣器关闭，GPIO 低电平 | 初始状态 |
| `IDLE` | 蜂鸣器就绪，准备接收命令 | 初始化后、模式播放完成后 |
| `ON` | 连续蜂鸣，GPIO 持续高电平 | 调用 `buzzer_on()` |
| `BEEPING` | 蜂鸣模式播放中 | 调用 `buzzer_beep_*()` |
| `ERROR` | 错误状态 | 初始化失败、操作异常 |

### 转移规则

```
OFF
 ├─→ [buzzer_init()] → IDLE
 └─→ [error] → ERROR

IDLE
 ├─→ [buzzer_on()] → ON
 ├─→ [buzzer_beep_*] → BEEPING
 └─→ [error] → ERROR

ON
 ├─→ [buzzer_off()] → IDLE
 └─→ [error] → ERROR

BEEPING
 ├─→ [模式完成] → IDLE
 ├─→ [buzzer_off()] → IDLE
 └─→ [error] → ERROR

ERROR
 └─→ [buzzer_deinit()] → OFF
```

---

## 线程安全

### 保护策略

#### 1. 注册表保护
```c
static SemaphoreHandle_t g_registry_mutex = NULL;

static void buzzer_registry_lock(void) {
    xSemaphoreTake(g_registry_mutex, portMAX_DELAY);
}

static void buzzer_registry_unlock(void) {
    xSemaphoreGive(g_registry_mutex);
}
```

**保护范围**：
- 所有设备注册/注销操作
- 设备表查询操作
- 保证原子性

#### 2. 设备资源保护
```c
typedef struct buzzer_device {
    // ...
    void *mutex;  // 每个设备都有自己的互斥锁
} buzzer_device_t;

static void buzzer_lock_device(buzzer_device_t *dev) {
    if (dev && dev->mutex) {
        xSemaphoreTake((SemaphoreHandle_t)dev->mutex, portMAX_DELAY);
    }
}

static void buzzer_unlock_device(buzzer_device_t *dev) {
    if (dev && dev->mutex) {
        xSemaphoreGive((SemaphoreHandle_t)dev->mutex);
    }
}
```

**保护范围**：
- 设备状态修改
- GPIO 操作
- 定时器操作
- 模式播放状态

#### 3. 保护矩阵

| 资源 | 保护机制 | 说明 |
|------|---------|------|
| `g_buzzer_devices[]` | 注册表互斥锁 | 全局设备表 |
| `device->state` | 设备互斥锁 | 每个设备的状态 |
| `device->beep_pattern*` | 设备互斥锁 | 蜂鸣模式数据 |
| `GPIO` 操作 | 设备互斥锁 | 硬件访问 |
| `device->beep_timer` | 设备互斥锁 | 定时器状态 |

### 并发场景分析

#### 场景 1：多任务并发控制
```
Task A                  Task B
   │                      │
   ├─ buzzer_on()         │
   │  ├─ get_handle()     ├─ buzzer_beep_pattern()
   │  │  ├─ lock_registry │
   │  │  └─ unlock_registry
   │  └─ turn_on()        │
   │     ├─ lock_device   ├─ get_handle()
   │     ├─ gpio_set()    │  ├─ lock_registry [等待]
   │     └─ unlock_device │
   │                      └─ [继续等待]
   │
   │ [Task A 完成]
   │
   │ Task B 继续...
   └─ [OK]
```

**结论**：✅ 安全 - 互斥锁确保串行化

#### 场景 2：定时器回调与主任务并发
```
Main Task                Timer Interrupt
    │                         │
    ├─ buzzer_off()           │
    │  ├─ lock_device         │
    │  ├─ xTimerStop()        │
    │  └─ unlock_device       │
    │                         │
    │                   [Timer 回调触发]
    │                         │
    │                         ├─ lock_device [等待]
    │                         │
    │                   [等待锁释放]
    │
    │ [Main Task 完成]
    │
    │                         └─ [获得锁，但 pattern_running=false]
    │                            └─ [快速返回]
```

**结论**：✅ 安全 - 设备互斥锁保护

### 死锁预防

**互斥锁顺序**：
1. 注册表互斥锁 (全局)
2. 设备互斥锁 (局部)

**规则**：永远不要反向获取锁！

```c
// ❌ 错误：可能导致死锁
buzzer_registry_lock();
  device = g_buzzer_devices[i];
  buzzer_lock_device(device);  // 嵌套锁
buzzer_registry_unlock();

// ✅ 正确：先释放外层锁
buzzer_registry_lock();
  device = g_buzzer_devices[i];
buzzer_registry_unlock();
buzzer_lock_device(device);  // 单独锁定
```

---

## 扩展指南

### 1. 添加新的蜂鸣器硬件实现

#### 步骤 1：定义 ops 接口

```c
// 在 buzzer.c 中添加 PWM 蜂鸣器 ops
static const buzzer_ops_t pwm_buzzer_ops = {
    .init = pwm_buzzer_init,
    .deinit = pwm_buzzer_deinit,
    .turn_on = pwm_buzzer_turn_on,
    .turn_off = pwm_buzzer_turn_off,
    .beep_once = pwm_buzzer_beep_once,
    .beep_pattern = pwm_buzzer_beep_pattern,
    .set_frequency = pwm_buzzer_set_frequency,
    .set_volume = pwm_buzzer_set_volume,
    .get_state = pwm_buzzer_get_state,
};
```

#### 步骤 2：实现具体操作

```c
static esp_err_t pwm_buzzer_init(buzzer_device_t *dev)
{
    // PWM 初始化逻辑
    return ESP_OK;
}

static esp_err_t pwm_buzzer_set_frequency(buzzer_device_t *dev, uint32_t frequency_hz)
{
    // 配置 PWM 频率
    return ESP_OK;
}

// ... 其他操作 ...
```

#### 步骤 3：添加工厂函数

```c
buzzer_device_t *buzzer_create_pwm_device(
    uint8_t device_id,
    ledc_channel_t ledc_channel,
    const char *device_name)
{
    // 类似于 buzzer_create_gpio_device
    buzzer_device_t *dev = malloc(sizeof(buzzer_device_t));
    
    // ... 初始化 ...
    
    dev->ops = &pwm_buzzer_ops;
    return dev;
}
```

#### 步骤 4：应用中使用

```c
// 创建 PWM 蜂鸣器
buzzer_device_t *pwm_buzzer = buzzer_create_pwm_device(1, LEDC_CHANNEL_0, "pwm_buzzer");
buzzer_register_device(pwm_buzzer);

// 使用相同的 API 控制
buzzer_handle_t handle = buzzer_get_handle(1);
buzzer_set_frequency(handle, 4000);  // 4kHz
buzzer_set_volume(handle, 80);       // 80% 音量
buzzer_beep_pattern(handle, BUZZER_BEEP_SUCCESS);
```

### 2. 添加新的蜂鸣模式

#### 步骤 1：扩展 enum

```c
typedef enum {
    BUZZER_BEEP_SHORT = 0,
    // ... 现有模式 ...
    BUZZER_BEEP_SUCCESS,
    
    // 新模式
    BUZZER_BEEP_ALARM,           // 警报
    BUZZER_BEEP_WARNING,         // 警告
    BUZZER_BEEP_NOTIFICATION,    // 通知
} buzzer_beep_type_t;
```

#### 步骤 2：定义模式数据

```c
const uint32_t alarm_pattern[] = {200, 100, 200, 100, 200, 200};
const uint32_t warning_pattern[] = {150, 50, 150, 50};
```

#### 步骤 3：添加模式处理

```c
static esp_err_t gpio_buzzer_beep_pattern(buzzer_device_t *dev, buzzer_beep_type_t beep_type)
{
    // ... 现有 switch 中添加 ...
    
    case BUZZER_BEEP_ALARM:
        pattern = alarm_pattern;
        pattern_len = sizeof(alarm_pattern) / sizeof(alarm_pattern[0]);
        break;
    case BUZZER_BEEP_WARNING:
        pattern = warning_pattern;
        pattern_len = sizeof(warning_pattern) / sizeof(warning_pattern[0]);
        break;
    
    // ...
}
```

### 3. 添加高级功能

#### 功能：自动关闭超时

```c
esp_err_t buzzer_set_auto_off_timeout(buzzer_handle_t handle, uint32_t timeout_ms)
{
    // 在模式播放后自动关闭
}
```

#### 功能：蜂鸣序列播放

```c
esp_err_t buzzer_play_sequence(buzzer_handle_t handle, const buzzer_beep_type_t *sequence, uint8_t count)
{
    // 顺序播放多个蜂鸣模式
}
```

#### 功能：动态模式生成

```c
esp_err_t buzzer_play_custom_pattern(buzzer_handle_t handle, const uint32_t *pattern, uint8_t pattern_len)
{
    // 播放自定义 on/off 时序
}
```

---

## 总结

Buzzer 驱动框架通过以下设计实现了灵活性和可靠性：

1. **分层架构**：清晰的模块划分，易于维护和扩展
2. **设备抽象**：ops interface 支持多种硬件实现
3. **句柄管理**：隐藏实现细节，提供安全的 API
4. **状态机**：完整的状态管理，防止非法操作
5. **线程安全**：FreeRTOS 互斥锁保护共享资源
6. **异步控制**：利用 FreeRTOS Timer 实现高效蜂鸣
7. **可扩展性**：易于添加新硬件和新功能

这个设计可以满足智能储物柜项目的需求，并为未来的扩展预留了空间。

