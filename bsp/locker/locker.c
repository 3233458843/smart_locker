#include "locker.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_random.h"
#include <errno.h>
#include "freertos/FreeRTOS.h"
#include <freertos/projdefs.h>
#include <freertos/task.h>

#define TAG "LOCKER"

locker_t lockers[4]; // 定义 4 个锁实例

/// @brief 初始化锁的状态和 GPIO 引脚
/// @param None
void locker_init(void){
    error_t err;
    // 初始化锁的状态和 GPIO 引脚
    lockers[0] = (locker_t){
        .locker_info = {.locker_user_info = {"locker1"}, .locker_user_info_id = {0, 0}}, .locker_id = 0 , .is_locked = false,.have_saved = false,
        .locker_pin = LOCKER1_GPIO_PIN, .locker_detection_pin = LOCKER1_Detection_GPIO_PIN, .password = {0, 0, 0, 0}
    };
    lockers[1] = (locker_t){
        .locker_info = {.locker_user_info = {"locker2"}, .locker_user_info_id = {0, 0}}, .locker_id = 1 ,.is_locked = false,.have_saved = false,
        .locker_pin = LOCKER2_GPIO_PIN, .locker_detection_pin = LOCKER2_Detection_GPIO_PIN, .password = {0, 0, 0, 0}
    };
    lockers[2] = (locker_t){
        .locker_info = {.locker_user_info = {"locker3"}, .locker_user_info_id = {0, 0}}, .locker_id = 2 ,.is_locked = false,.have_saved = false,
        .locker_pin = LOCKER3_GPIO_PIN, .locker_detection_pin = LOCKER3_Detection_GPIO_PIN, .password = {0, 0, 0, 0}
    };
    lockers[3] = (locker_t){
        .locker_info = {.locker_user_info = {"locker4"}, .locker_user_info_id = {0, 0}}, .locker_id = 3 ,.is_locked = false,.have_saved = false,
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

/** 打开指定锁
 *
 * @param locker
 */
void locker_on(locker_t* locker){
    if (locker == NULL){
        ESP_LOGE(TAG, "Locker pointer is NULL");
        return;
    }
    esp_err_t err = gpio_set_level(locker->locker_pin, 1);
    if (err != ESP_OK){
        ESP_LOGE(TAG, "Failed to set GPIO level for locker: %s", esp_err_to_name(err));
    }
    else if (err == ESP_OK){
        locker->is_locked = true;
        ESP_LOGI(TAG, "Locker %d opened", locker->locker_id + 1);
    }
    vTaskDelay(pdMS_TO_TICKS(500)); // 延时 50ms，确保机械动作完成

    err = gpio_set_level(locker->locker_pin, 0);

    if (err != ESP_OK){
        ESP_LOGE(TAG, "Failed to set GPIO level for locker: %s", esp_err_to_name(err));
    }
    else if (err == ESP_OK){
        locker->is_locked = false;
        ESP_LOGI(TAG, "Locker %d closed", locker->locker_id + 1);
    }
}

/** 打开所有锁
 *
 */
void locker_all_on(void){
    locker_on(&lockers[0]);
    vTaskDelay(pdMS_TO_TICKS(50));
    locker_on(&lockers[1]);
    vTaskDelay(pdMS_TO_TICKS(50));
    locker_on(&lockers[2]);
    vTaskDelay(pdMS_TO_TICKS(50));
    locker_on(&lockers[3]);
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
    if (password == NULL) {
        ESP_LOGE(TAG, "Password buffer is NULL");
        return;
    }

    for (uint8_t i = 0; i < 4; i++) {
        password[i] = esp_random() % 10;
    }
    // 确保生成的密码不全为0，避免过于简单
    while (password[0] == 0 && password[1] == 0 && password[2] == 0 && password[3] == 0){
        for (uint8_t i = 0; i < 4; i++){
            password[i] = esp_random() % 10;
        }
        continue;
    }

    ESP_LOGI(TAG, "4位数字密码已生成: %d%d%d%d",
             password[0], password[1], password[2], password[3]);
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
        return false;
    }
    return true;
}

/**
 * 判断柜子是否存放物品
 * @param locker 要检测的柜号
 * @return true:柜子内存放有物品  false:柜子内无物品
 * @note 物品存放状态独立于门开关状态，仅由 have_saved 标志决定
 */
bool has_item_in_locker(const locker_t* locker){
    if (locker == NULL){
        ESP_LOGE(TAG, "Locker pointer is NULL");
        return false;
    }
    // 物品存放判断：仅由 have_saved 标志决定
    return locker->have_saved;
}

/**
 * 判断柜子是否处于安全状态（已存物且门已关闭）
 * @param locker 要检测的柜号
 * @return true:柜子内有物品且门已关闭  false:其他情况
 * @note 用于验证用户完成存物流程
 */
bool is_locker_secured(const locker_t* locker){
    if (locker == NULL){
        ESP_LOGE(TAG, "Locker pointer is NULL");
        return false;
    }
    // 安全状态判断：有物品且门关闭
    return locker->have_saved && (!Detection_locker_on_off(locker));
}

// ============================================================================
// 【用户-柜号数据库实现】
// ============================================================================

#include "nvs.h"
#include "nvs_flash.h"
#include <time.h>

static user_locker_entry_t user_locker_db[4] = {0};

/**
 * 初始化数据库，从 NVS 恢复数据
 */
esp_err_t locker_db_init(void){
    nvs_handle_t nvs_h;
    esp_err_t err = nvs_open("locker", NVS_READONLY, &nvs_h);

    memset(user_locker_db, 0, sizeof(user_locker_db));
    for (int i = 0; i < 4; i++){
        lockers[i].have_saved = false;
        memset(lockers[i].password, 0, sizeof(lockers[i].password));
        lockers[i].locker_info.locker_user_info_id[0] = 0;
        lockers[i].locker_info.locker_user_info_id[1] = 0;
    }

    if (err == ESP_OK){
        for(int i = 0; i < 4; i++){
            char key[32];
            snprintf(key, sizeof(key), "entry_%d", i);
            size_t len = sizeof(user_locker_entry_t);
            esp_err_t get_err = nvs_get_blob(nvs_h, key, &user_locker_db[i], &len);
            if (get_err != ESP_OK){
                memset(&user_locker_db[i], 0, sizeof(user_locker_entry_t));
                continue;
            }

            if (user_locker_db[i].is_valid){
                lockers[i].have_saved = true;
                memcpy(lockers[i].password, user_locker_db[i].password, sizeof(lockers[i].password));
                lockers[i].locker_info.locker_user_info_id[0] = (user_locker_db[i].user_id >> 8) & 0xFF;
                lockers[i].locker_info.locker_user_info_id[1] = user_locker_db[i].user_id & 0xFF;
            }
        }
        nvs_close(nvs_h);
        ESP_LOGI(TAG, "Locker database loaded from NVS");
    } else {
        ESP_LOGI(TAG, "NVS not found, using fresh database");
    }
    return ESP_OK;
}

/**
 * 添加用户-柜号绑定条目
 */
esp_err_t locker_db_add_entry(user_locker_entry_t* entry){
    if(entry == NULL || entry->locker_id >= 4){
        return ESP_ERR_INVALID_ARG;
    }

    // 添加到内存数据库
    memcpy(&user_locker_db[entry->locker_id], entry, sizeof(user_locker_entry_t));

    // 更新柜子状态
    lockers[entry->locker_id].have_saved = true;
    memcpy(lockers[entry->locker_id].password, entry->password, 4);
    lockers[entry->locker_id].locker_info.locker_user_info_id[0] = (entry->user_id >> 8) & 0xFF;
    lockers[entry->locker_id].locker_info.locker_user_info_id[1] = entry->user_id & 0xFF;

    // 保存到 NVS
    return locker_db_save_to_nvs();
}

/**
 * 按用户ID查找绑定条目
 */
esp_err_t locker_db_get_entry_by_user(uint16_t user_id, user_locker_entry_t* entry){
    if(entry == NULL){
        return ESP_ERR_INVALID_ARG;
    }

    for(int i = 0; i < 4; i++){
        if(user_locker_db[i].user_id == user_id && user_locker_db[i].is_valid){
            memcpy(entry, &user_locker_db[i], sizeof(user_locker_entry_t));
            return ESP_OK;
        }
    }
    return ESP_ERR_NOT_FOUND;
}

/**
 * 按柜号查找绑定条目
 */
esp_err_t locker_db_get_entry_by_locker(uint8_t locker_id, user_locker_entry_t* entry){
    if(entry == NULL || locker_id >= 4){
        return ESP_ERR_INVALID_ARG;
    }

    if(user_locker_db[locker_id].is_valid){
        memcpy(entry, &user_locker_db[locker_id], sizeof(user_locker_entry_t));
        return ESP_OK;
    }
    return ESP_ERR_NOT_FOUND;
}

/**
 * 按 4 位密码查找有效绑定条目
 */
esp_err_t locker_db_get_entry_by_password(const uint8_t password[4], user_locker_entry_t* entry){
    if(password == NULL || entry == NULL){
        return ESP_ERR_INVALID_ARG;
    }

    for(int i = 0; i < 4; i++){
        if(user_locker_db[i].is_valid && memcmp(user_locker_db[i].password, password, 4) == 0){
            memcpy(entry, &user_locker_db[i], sizeof(user_locker_entry_t));
            return ESP_OK;
        }
    }
    return ESP_ERR_NOT_FOUND;
}

/**
 * 按 4 位手机号查找有效绑定条目
 */
esp_err_t locker_db_get_entry_by_phone(const uint8_t phone[4], user_locker_entry_t* entry){
    if(phone == NULL || entry == NULL){
        return ESP_ERR_INVALID_ARG;
    }

    for(int i = 0; i < 4; i++){
        if(user_locker_db[i].is_valid && memcmp(user_locker_db[i].phone, phone, 4) == 0){
            memcpy(entry, &user_locker_db[i], sizeof(user_locker_entry_t));
            return ESP_OK;
        }
    }
    return ESP_ERR_NOT_FOUND;
}

/**
 * 按柜号清除绑定条目
 */
esp_err_t locker_db_remove_entry_by_locker(uint8_t locker_id){
    if (locker_id >= 4){
        return ESP_ERR_INVALID_ARG;
    }

    memset(&user_locker_db[locker_id], 0, sizeof(user_locker_entry_t));
    lockers[locker_id].have_saved = false;
    memset(lockers[locker_id].password, 0, sizeof(lockers[locker_id].password));
    lockers[locker_id].locker_info.locker_user_info_id[0] = 0;
    lockers[locker_id].locker_info.locker_user_info_id[1] = 0;

    return locker_db_save_to_nvs();
}

/**
 * 清空全部本地柜号绑定，用于 XST 用户库为空时的上电同步
 */
esp_err_t locker_db_clear_all(void){
    memset(user_locker_db, 0, sizeof(user_locker_db));
    for (int i = 0; i < 4; i++){
        lockers[i].have_saved = false;
        memset(lockers[i].password, 0, sizeof(lockers[i].password));
        lockers[i].locker_info.locker_user_info_id[0] = 0;
        lockers[i].locker_info.locker_user_info_id[1] = 0;
    }

    nvs_handle_t nvs_h;
    esp_err_t err = nvs_open("locker", NVS_READWRITE, &nvs_h);
    if (err == ESP_OK){
        err = nvs_erase_all(nvs_h);
        if (err == ESP_OK){
            err = nvs_commit(nvs_h);
        }
        nvs_close(nvs_h);
    }

    ESP_LOGI(TAG, "Local locker database cleared");
    return err;
}

/**
 * 上电同步本地储物信息：本地绑定只保留仍存在于 XST 模组用户库中的用户
 */
esp_err_t locker_db_sync_with_xst_users(const uint16_t* xst_user_ids, uint16_t xst_user_count){
    if (xst_user_count == 0){
        ESP_LOGI(TAG, "Power-on sync: XST user list is empty, clearing all local locker bindings");
        return locker_db_clear_all();
    }
    if (xst_user_ids == NULL){
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t first_err = ESP_OK;
    for (int i = 0; i < 4; i++){
        if (!user_locker_db[i].is_valid){
            lockers[i].have_saved = false;
            continue;
        }

        bool user_exists_in_xst = false;
        for (uint16_t j = 0; j < xst_user_count; j++){
            if (user_locker_db[i].user_id == xst_user_ids[j]){
                user_exists_in_xst = true;
                break;
            }
        }

        if (!user_exists_in_xst){
            ESP_LOGW(TAG, "Power-on sync: clearing stale local binding locker=%d user_id=%u",
                     i + 1,
                     (unsigned int)user_locker_db[i].user_id);
            esp_err_t err = locker_db_remove_entry_by_locker((uint8_t)i);
            if (err != ESP_OK && first_err == ESP_OK){
                first_err = err;
            }
        }
        else{
            lockers[i].have_saved = true;
        }
    }

    return first_err;
}

/**
 * 查找空闲柜号（have_saved=false）
 */
uint8_t locker_db_find_free_locker(void){
    for(int i = 0; i < 4; i++){
        if(!lockers[i].have_saved){
            return i;
        }
    }
    return 0xFF;  // 无空闲柜
}

/**
 * 保存数据库到 NVS
 */
esp_err_t locker_db_save_to_nvs(void){
    nvs_handle_t nvs_h;
    esp_err_t err = nvs_open("locker", NVS_READWRITE, &nvs_h);

    if(err == ESP_OK){
        for(int i = 0; i < 4; i++){
            char key[32];
            snprintf(key, sizeof(key), "entry_%d", i);
            nvs_set_blob(nvs_h, key, &user_locker_db[i], sizeof(user_locker_entry_t));
        }
        err = nvs_commit(nvs_h);
        nvs_close(nvs_h);
        ESP_LOGI(TAG, "Locker database saved to NVS");
    }
    return err;
}
