# Buzzer 驱动使用示例

## 概述
本文档提供 Buzzer 驱动的详细使用示例，展示如何在 ESP32-S3 智能储物柜项目中集成蜂鸣器驱动。

## 1. 基本集成步骤

### 1.1 在 main.c 中添加头文件

```c
#include "buzzer.h"
```

### 1.2 创建并注册蜂鸣器设备

在 `app_main()` 函数中，初始化蜂鸣器驱动：

```c
void app_main(void)
{
    // ... 其他初始化代码 ...
    
    // 创建 GPIO 蜂鸣器设备
    // GPIO46 作为蜂鸣器控制脚 (高电平激活)
    buzzer_device_t *buzzer_dev = buzzer_create_gpio_device(
        0,              // device_id
        GPIO_NUM_46,    // GPIO pin
        "main_buzzer"   // device name
    );
    
    if (!buzzer_dev) {
        ESP_LOGE(TAG, "Failed to create buzzer device");
        return;
    }
    
    // 注册设备到驱动框架
    ESP_ERROR_CHECK(buzzer_register_device(buzzer_dev));
    
    // 初始化所有已注册的蜂鸣器设备
    ESP_ERROR_CHECK(buzzer_init());
    
    // ... 继续其他初始化 ...
}
```

## 2. 使用场景示例

### 2.1 取件成功蜂鸣

用户通过掌静脉识别或密码验证成功后，播放成功音：

```c
void on_unlock_success(void)
{
    buzzer_handle_t handle = buzzer_get_handle(0);
    if (handle) {
        buzzer_beep_pattern(handle, BUZZER_BEEP_SUCCESS);
        ESP_LOGI(TAG, "Playing success beep");
    }
}
```

### 2.2 取件失败蜂鸣

用户识别失败或密码错误时，播放错误音：

```c
void on_unlock_failed(void)
{
    buzzer_handle_t handle = buzzer_get_handle(0);
    if (handle) {
        buzzer_beep_pattern(handle, BUZZER_BEEP_ERROR);
        ESP_LOGI(TAG, "Playing error beep");
    }
}
```

### 2.3 存件完成蜂鸣

用户成功存入物品后，播放双响提示：

```c
void on_item_saved(void)
{
    buzzer_handle_t handle = buzzer_get_handle(0);
    if (handle) {
        buzzer_beep_pattern(handle, BUZZER_BEEP_DOUBLE);
        ESP_LOGI(TAG, "Playing save confirmation beep");
    }
}
```

### 2.4 单次自定义蜂鸣

播放自定义时长的蜂鸣声（例如 200ms）：

```c
void custom_beep(uint32_t duration_ms)
{
    buzzer_handle_t handle = buzzer_get_handle(0);
    if (handle) {
        buzzer_beep_once(handle, duration_ms);
        ESP_LOGI(TAG, "Playing custom beep: %lu ms", duration_ms);
    }
}
```

### 2.5 连续开启/关闭蜂鸣

需要连续蜂鸣时：

```c
void start_continuous_buzz(void)
{
    buzzer_handle_t handle = buzzer_get_handle(0);
    if (handle) {
        buzzer_on(handle);
        ESP_LOGI(TAG, "Buzzer ON");
    }
}

void stop_continuous_buzz(void)
{
    buzzer_handle_t handle = buzzer_get_handle(0);
    if (handle) {
        buzzer_off(handle);
        ESP_LOGI(TAG, "Buzzer OFF");
    }
}
```

## 3. 回调通知示例

### 3.1 注册蜂鸣完成回调

可以在蜂鸣模式完成时触发回调函数：

```c
// 定义回调函数
static void buzzer_complete_callback(void *user_data)
{
    const char *event = (const char *)user_data;
    ESP_LOGI(TAG, "Buzzer pattern completed: %s", event);
}

// 在应用中注册回调
void setup_buzzer_callback(void)
{
    buzzer_handle_t handle = buzzer_get_handle(0);
    if (handle) {
        buzzer_set_callback(handle, buzzer_complete_callback, (void *)"user_unlock");
    }
}
```

### 3.2 使用回调执行后续操作

```c
static void on_beep_complete(void *user_data)
{
    uint32_t event_id = (uintptr_t)user_data;
    
    switch (event_id) {
    case EVENT_UNLOCK_COMPLETE:
        // 开门已完成声音提示，执行开门动作
        locker_on(&lockers[0]);
        break;
    case EVENT_SAVE_COMPLETE:
        // 存件完成声音提示，更新 UI
        update_save_status();
        break;
    default:
        break;
    }
}
```

## 4. 状态查询示例

### 4.1 获取蜂鸣器状态

```c
void check_buzzer_status(void)
{
    buzzer_handle_t handle = buzzer_get_handle(0);
    if (handle) {
        buzzer_state_t state = buzzer_get_state(handle);
        
        switch (state) {
        case BUZZER_STATE_OFF:
            ESP_LOGI(TAG, "Buzzer state: OFF");
            break;
        case BUZZER_STATE_ON:
            ESP_LOGI(TAG, "Buzzer state: ON (continuous)");
            break;
        case BUZZER_STATE_BEEPING:
            ESP_LOGI(TAG, "Buzzer state: BEEPING (pattern)");
            break;
        case BUZZER_STATE_IDLE:
            ESP_LOGI(TAG, "Buzzer state: IDLE (ready)");
            break;
        case BUZZER_STATE_ERROR:
            ESP_LOGI(TAG, "Buzzer state: ERROR");
            break;
        }
    }
}
```

### 4.2 获取设备详细信息

```c
void dump_buzzer_info(void)
{
    buzzer_handle_t handle = buzzer_get_handle(0);
    if (handle) {
        const buzzer_device_t *dev_info = buzzer_get_device_info(handle);
        if (dev_info) {
            ESP_LOGI(TAG, "Device name: %s", dev_info->name);
            ESP_LOGI(TAG, "Device ID: %d", dev_info->device_id);
            ESP_LOGI(TAG, "GPIO pin: %d", dev_info->gpio_pin);
            ESP_LOGI(TAG, "Volume: %u%%", dev_info->current_volume);
        }
    }
}

// 或使用调试工具输出所有设备
void dump_all_devices(void)
{
    buzzer_dump_devices();  // 输出所有已注册的蜂鸣器设备信息
}
```

## 5. 在 LVGL 事件中集成蜂鸣器

### 5.1 按钮按下时播放蜂鸣音

在 `bsp/ui/generated/events_init.c` 中添加蜂鸣：

```c
#include "buzzer.h"

static void btn_click_event_cb(lv_event_t * e)
{
    buzzer_handle_t handle = buzzer_get_handle(0);
    if (handle) {
        buzzer_beep_pattern(handle, BUZZER_BEEP_SHORT);  // 按钮点击声
    }
    
    // ... 处理按钮点击逻辑 ...
}
```

### 5.2 验证成功时播放蜂鸣

```c
static void on_verification_success(void)
{
    buzzer_handle_t handle = buzzer_get_handle(0);
    if (handle) {
        buzzer_beep_pattern(handle, BUZZER_BEEP_SUCCESS);
    }
    
    // 更新 UI ...
}
```

### 5.3 错误提示时播放蜂鸣

```c
static void on_verification_failed(void)
{
    buzzer_handle_t handle = buzzer_get_handle(0);
    if (handle) {
        buzzer_beep_pattern(handle, BUZZER_BEEP_ERROR);
    }
    
    // 显示错误提示 ...
}
```

## 6. 多设备管理示例

### 6.1 创建多个蜂鸣器

如果系统中有多个蜂鸣器（例如主蜂鸣和副蜂鸣）：

```c
void init_multiple_buzzers(void)
{
    // 主蜂鸣器
    buzzer_device_t *main_buzzer = buzzer_create_gpio_device(
        0, GPIO_NUM_46, "main_buzzer"
    );
    buzzer_register_device(main_buzzer);
    
    // 副蜂鸣器（可选）
    buzzer_device_t *sub_buzzer = buzzer_create_gpio_device(
        1, GPIO_NUM_47, "sub_buzzer"
    );
    buzzer_register_device(sub_buzzer);
    
    // 初始化驱动
    buzzer_init();
}
```

### 6.2 控制特定蜂鸣器

```c
void control_specific_buzzer(uint8_t device_id, buzzer_beep_type_t beep_type)
{
    buzzer_handle_t handle = buzzer_get_handle(device_id);
    if (handle) {
        buzzer_beep_pattern(handle, beep_type);
    } else {
        ESP_LOGE(TAG, "Buzzer device %d not found", device_id);
    }
}
```

## 7. 资源清理示例

### 7.1 优雅关闭驱动

```c
void shutdown_buzzer(void)
{
    // 关闭所有设备
    buzzer_deinit();
    
    // 释放设备资源
    // ... 需要保存设备指针进行释放 ...
}
```

## 8. 推荐的集成点

在智能储物柜应用中，以下位置适合集成蜂鸣器提示：

1. **取件流程**
   - 掌静脉识别成功：`BUZZER_BEEP_SUCCESS`
   - 掌静脉识别失败：`BUZZER_BEEP_ERROR`
   - 密码验证成功：`BUZZER_BEEP_DOUBLE`
   - 密码验证失败：`BUZZER_BEEP_ERROR`

2. **存件流程**
   - 掌静脉录入成功：`BUZZER_BEEP_SUCCESS`
   - 掌静脉录入失败：`BUZZER_BEEP_ERROR`
   - 分配柜号成功：`BUZZER_BEEP_DOUBLE`

3. **系统事件**
   - 系统启动完成：`BUZZER_BEEP_SHORT`
   - 异常发生：`BUZZER_BEEP_TRIPLE`
   - 定时提醒：自定义模式

## 9. 调试技巧

### 9.1 使用日志跟踪蜂鸣状态

将以下宏定义在 `idf.py menuconfig` 中启用详细日志：

```bash
idf.py menuconfig
# 路径: Component config → Log output
# 设置 Default log verbosity 为 DEBUG
```

### 9.2 查看蜂鸣器设备注册信息

```c
void print_buzzer_debug_info(void)
{
    ESP_LOGI(TAG, "========== Buzzer Debug Info ==========");
    buzzer_dump_devices();
    
    for (int i = 0; i < 4; i++) {
        buzzer_handle_t h = buzzer_get_handle(i);
        if (h) {
            buzzer_state_t state = buzzer_get_state(h);
            ESP_LOGI(TAG, "Device %d state: %d", i, state);
        }
    }
    ESP_LOGI(TAG, "=======================================");
}
```

## 10. 注意事项

1. **GPIO 冲突检查**
   - 确保蜂鸣器 GPIO 不与其他设备冲突
   - 推荐使用 GPIO46（RTC 域，更稳定）

2. **电源管理**
   - 蜂鸣器在连续开启时会消耗电流
   - 长时间使用建议添加超时自动关闭

3. **线程安全**
   - 所有 buzzer API 都是线程安全的（内部使用 mutex）
   - 可以在多个任务中并发调用

4. **内存管理**
   - 调用 `buzzer_get_handle()` 返回的 handle 需要在不再使用时释放
   - 调用 `free()` 释放 opaque handle

5. **音量考虑**
   - GPIO 蜂鸣器不支持动态音量调整
   - PWM 蜂鸣器（后续实现）将支持 0-100% 音量控制

## 相关文件

- **头文件**: `bsp/buzzer/buzzer.h` - API 声明和类型定义
- **实现**: `bsp/buzzer/buzzer.c` - 驱动实现
- **CMakeLists**: `bsp/buzzer/CMakeLists.txt` - 构建配置
- **本示例**: `bsp/buzzer/BUZZER_USAGE_EXAMPLE.md` - 使用示例（本文件）

