#include "locker.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_random.h"
#include <errno.h>

#define TAG "LOCKER"

locker_t lockers[4]; // 定义 4 个锁实例

/// @brief 初始化锁的状态和 GPIO 引脚
/// @param None
void locker_init(void){
    error_t err;
    // 初始化锁的状态和 GPIO 引脚
    lockers[0] = (locker_t){
        .locker_info = {.locker_user_info = {0}, .locker_user_info_id = {0, 0}}, .locker_id = 0 , .is_locked = false,
        .locker_pin = LOCKER1_GPIO_PIN, .locker_detection_pin = LOCKER1_Detection_GPIO_PIN, .password = {0, 0, 0, 0}
    };
    lockers[1] = (locker_t){
        .locker_info = {.locker_user_info = {0}, .locker_user_info_id = {0, 0}}, .locker_id = 1 ,.is_locked = false,
        .locker_pin = LOCKER2_GPIO_PIN, .locker_detection_pin = LOCKER2_Detection_GPIO_PIN, .password = {0, 0, 0, 0}
    };
    lockers[2] = (locker_t){
        .locker_info = {.locker_user_info = {0}, .locker_user_info_id = {0, 0}}, .locker_id = 2 ,.is_locked = false,
        .locker_pin = LOCKER3_GPIO_PIN, .locker_detection_pin = LOCKER3_Detection_GPIO_PIN, .password = {0, 0, 0, 0}
    };
    lockers[3] = (locker_t){
        .locker_info = {.locker_user_info = {0}, .locker_user_info_id = {0, 0}}, .locker_id = 3 ,.is_locked = false,
        .locker_pin = LOCKER4_GPIO_PIN, .locker_detection_pin = LOCKER4_Detection_GPIO_PIN, .password = {0, 0, 0, 0}
    };

    // 配置柜门开合 GPIO 引脚为输出模式
    for (int i = 0; i < 4; i++){
        gpio_config_t locker_io_conf = {
            .pin_bit_mask = (1ULL << lockers[i].locker_pin), // 选择要配置的 GPIO
            .mode = GPIO_MODE_OUTPUT, // 设置为输出模式
            .pull_up_en = GPIO_PULLUP_DISABLE, // 禁用上拉
            .pull_down_en = GPIO_PULLDOWN_DISABLE, // 禁用下拉
            .intr_type = GPIO_INTR_DISABLE // 禁用中断
        };
        err = gpio_config(&locker_io_conf);
        if (err != ESP_OK){
            ESP_LOGE(TAG, "Failed to configure GPIO for locker %d: %s", i + 1, esp_err_to_name(err));
            continue; // 继续配置下一个锁
        }
        err = gpio_set_level(lockers[i].locker_pin, lockers[i].is_locked ? 1 : 0); // 初始状态为锁定
        if (err != ESP_OK){
            ESP_LOGE(TAG, "Failed to set GPIO level for locker %d: %s", i + 1, esp_err_to_name(err));
        }
    }

    for (uint8_t i = 0; i < 4; i++){
        gpio_config_t locker_detection_io_conf = {
            .pin_bit_mask = (1ULL << lockers[i].locker_detection_pin), // 选择要配置的 GPIO
            .mode = GPIO_MODE_INPUT, // 设置为输入模式
            .pull_up_en = GPIO_PULLUP_DISABLE, // 禁用上拉
            .pull_down_en = GPIO_PULLDOWN_DISABLE, // 禁用下拉
            .intr_type = GPIO_INTR_DISABLE // 禁用中断
        };
        err = gpio_config(&locker_detection_io_conf);
        if (err != ESP_OK){
            ESP_LOGE(TAG, "Failed to configure GPIO for locker_detection %d: %s", i + 1, esp_err_to_name(err));
            continue; // 继续配置下一个锁
        }
    }
    ESP_LOGI(TAG, "Lockers initialized");
    gpio_dump_io_configuration(stdout, (1ULL << lockers[0].locker_pin) | (1ULL << lockers[1].locker_pin) | (1ULL << lockers[2].locker_pin) | (1ULL << lockers[3].locker_pin));
    gpio_dump_io_configuration(stdout, (1ULL << lockers[0].locker_detection_pin) | (1ULL << lockers[1].locker_detection_pin) | (1ULL << lockers[2].locker_detection_pin) | (1ULL << lockers[3].locker_detection_pin));
}

/// @brief 打开或关闭锁
/// @param locker 需打开或关闭的锁实例
/// @param lock 锁定状态，true 表示锁定，false 表示解锁
void locker_on_off(locker_t* locker, bool lock){
    if (locker == NULL){
        ESP_LOGE(TAG, "Locker pointer is NULL");
        return;
    }
    locker->is_locked = lock;
    esp_err_t err = gpio_set_level(locker->locker_pin, lock ? 1 : 0);
    if (err != ESP_OK){
        ESP_LOGE(TAG, "Failed to set GPIO level for locker: %s", esp_err_to_name(err));
    }
    else{
        ESP_LOGI(TAG, "Locker%d %s",locker->locker_id, lock ? "locked" : "unlocked");
    }
}

/// @brief 根据用户信息 ID 获取对应的锁实例
/// @param id 用户信息 ID
/// @return  对应的锁实例指针，如果未找到则返回 NULL
locker_t* get_locker_by_id(uint8_t* id) {
    for (int i = 0; i < 4; i++) {
        if (lockers[i].locker_info.locker_user_info_id[0] == id[0] && lockers[i].locker_info.locker_user_info_id[1] == id[1]) {
            return &lockers[i];
        }
    }
    ESP_LOGW(TAG, "Locker with ID %d not found", (id[0] << 8) | id[1]);
    return NULL; // 未找到对应的锁
}

/**
 *
 * @param password 随机4位密码生成
 *
 */
void crumble_password(uint8_t* password) {
    for (uint8_t i = 0; i < sizeof(password); i++) {
        password[i] = 0; // 将密码数据清零
    }
    ESP_LOGI(TAG, "柜号，密码已清0");
    for (uint8_t i = 0; i < sizeof(password); i++) {
        password[i] = esp_random() & 0xFF; // 用随机数据覆盖密码
    }
    ESP_LOGI(TAG, "柜号，密码已覆盖随机数据");
    ESP_LOGI(TAG, "柜号密码是：%02X %02X %02X %02X", password[0], password[1], password[2], password[3]);
}

/**
 *检测柜门是否关闭
 * @param locker 要检测的柜号
 * @return ture:柜门开启  false:柜门关闭
 */
bool Detection_locker_on_off(const locker_t* locker){
    if (locker == NULL){
        ESP_LOGE(TAG, "Locker pointer is NULL");
        return 0;
    }
    if (gpio_get_level(locker->locker_detection_pin) == 0){
        return true;
    }
    return false;
}
