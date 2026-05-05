/**
 * @file      serve.c
 * @brief     ${USER_PROMPT}
 * @author    Your Name (you@yourdomain.com)
 * @version   1.0
 * @date      2026-04-21
 * 
 * @copyright Copyright (c) 2026 All rights reserved.
 * 
 * @note      
 */

/* Includes ------------------------------------------------------------------*/
#include "serve.h"

#include "freertos/FREERTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include <string.h>

#include "xst.h"
#include "locker.h"
#include "buzzer.h"
#include "lwip/sockets.h"

/* Private macros ------------------------------------------------------------*/
#define SERVE_TAG "serve"
/* Private types -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
SemaphoreHandle_t ready_save = NULL; // 通知能够进行存件
SemaphoreHandle_t ready_take = NULL; // 通知能够进行取件
SemaphoreHandle_t verify_debug = NULL; // debug通知能够进行取件

QueueHandle_t save_verify_process = NULL; // 存件录取进度队列

uint16_t user_num[128] = {0};

static serve_save_status_t g_save_status = {
    .state = SERVE_FLOW_IDLE,
    .locker_id = 0xFF,
    .password = {0, 0, 0, 0},
    .has_password = false,
    .user_id = 0,
    .err = ESP_OK,
    .message = "idle",
};

static serve_take_status_t g_take_status = {
    .state = SERVE_FLOW_IDLE,
    .locker_id = 0xFF,
    .user_id = 0,
    .err = ESP_OK,
    .message = "idle",
};



buzzer_handle_t g_buzzer_handle = NULL; // 蜂鸣器句柄，由 main.c 初始化后赋值

static void serve_save_status_reset(const char* message){
    g_save_status.state = SERVE_FLOW_IDLE;
    g_save_status.locker_id = 0xFF;
    memset(g_save_status.password, 0, sizeof(g_save_status.password));
    g_save_status.has_password = false;
    g_save_status.user_id = 0;
    g_save_status.err = ESP_OK;
    strncpy(g_save_status.message, message, sizeof(g_save_status.message) - 1);
    g_save_status.message[sizeof(g_save_status.message) - 1] = '\0';
}

static void serve_save_status_fail(esp_err_t err, const char* message){
    g_save_status.state = SERVE_FLOW_FAILED;
    g_save_status.err = err;
    strncpy(g_save_status.message, message, sizeof(g_save_status.message) - 1);
    g_save_status.message[sizeof(g_save_status.message) - 1] = '\0';
}

static void serve_take_status_set(serve_flow_state_t state, uint8_t locker_id, uint16_t user_id, esp_err_t err,
                                  const char* message){
    g_take_status.state = state;
    g_take_status.locker_id = locker_id;
    g_take_status.user_id = user_id;
    g_take_status.err = err;
    strncpy(g_take_status.message, message, sizeof(g_take_status.message) - 1);
    g_take_status.message[sizeof(g_take_status.message) - 1] = '\0';
}

/* Private function prototypes -----------------------------------------------*/
void ready_take_task(void* param); // 等待取件任务
void ready_save_task(void* param); // 等待存件任务
void verify_debug_task(void* param); // 等待存件任务
/* Exported functions --------------------------------------------------------*/
void serve_init(void){
    //业务通知信号量
    ready_save = xSemaphoreCreateBinary();
    ready_take = xSemaphoreCreateBinary();
    verify_debug = xSemaphoreCreateBinary();

    // 录取进度队列创建
    save_verify_process = xQueueCreate(4, sizeof(uint16_t));

    if (ready_save == NULL || ready_take == NULL || verify_debug == NULL){
        ESP_LOGE(SERVE_TAG, "Failed to create semaphores");
        return;
    }

    xTaskCreatePinnedToCore(ready_take_task, "ready_take_task", 4096, NULL, 10, NULL, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(ready_save_task, "ready_save_task", 4096, NULL, 10, NULL, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(verify_debug_task, "verify_debug_task", 4096, NULL, 10, NULL, tskNO_AFFINITY);
}

void serve_main(void){
}

bool serve_request_save(void){
    if (ready_save == NULL){
        return false;
    }
    if (g_save_status.state == SERVE_FLOW_PENDING || g_save_status.state == SERVE_FLOW_RUNNING){
        return false;
    }

    serve_save_status_reset("save requested");
    g_save_status.state = SERVE_FLOW_PENDING;


    return xSemaphoreGive(ready_save) == pdTRUE;
}

bool serve_request_take_by_palm(void){
    if (ready_take == NULL){
        return false;
    }
    serve_take_status_set(SERVE_FLOW_PENDING, 0xFF, 0, ESP_OK, "palm take requested");
    return xSemaphoreGive(ready_take) == pdTRUE;
}

bool serve_request_take_by_password(const uint8_t password[4]){
    if (password == NULL){
        serve_take_status_set(SERVE_FLOW_FAILED, 0xFF, 0, ESP_ERR_INVALID_ARG, "Invalid password input");
        return false;
    }

    user_locker_entry_t locker_entry = {0};
    serve_take_status_set(SERVE_FLOW_RUNNING, 0xFF, 0, ESP_OK, "password take running");

    esp_err_t db_err = locker_db_get_entry_by_password(password, &locker_entry);
    if (db_err != ESP_OK){
        ESP_LOGW(SERVE_TAG, "No locker binding found for input password");
        if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_ERROR);
        serve_take_status_set(SERVE_FLOW_FAILED, 0xFF, 0, db_err, "Password not found");
        return false;
    }

    if (locker_entry.locker_id >= 4){
        ESP_LOGE(SERVE_TAG, "Invalid locker_id=%u for password take", locker_entry.locker_id);
        if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_ERROR);
        serve_take_status_set(SERVE_FLOW_FAILED, locker_entry.locker_id, locker_entry.user_id, ESP_ERR_INVALID_STATE,
                              "Invalid locker id");
        return false;
    }

    ESP_LOGI(SERVE_TAG, "Password take success for user_id=%u, opening locker %u",
             locker_entry.user_id,
             locker_entry.locker_id + 1);
    locker_on(&lockers[locker_entry.locker_id]);
    if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_SUCCESS);

    xst_result_t del_res = xst_cmd_del_user(locker_entry.user_id);
    if (del_res != MR_SUCCESS){
        ESP_LOGW(SERVE_TAG, "Locker opened but failed to delete user_id=%u from XST, res=%d",
                 locker_entry.user_id,
                 del_res);
    }

    db_err = locker_db_remove_entry_by_locker(locker_entry.locker_id);
    if (db_err != ESP_OK){
        ESP_LOGE(SERVE_TAG, "Locker opened but failed to clear locker %u binding: %s",
                 locker_entry.locker_id + 1,
                 esp_err_to_name(db_err));
        if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_ERROR);
        serve_take_status_set(SERVE_FLOW_FAILED, locker_entry.locker_id, locker_entry.user_id, db_err,
                              "Failed to clear binding");
        return false;
    }

    serve_take_status_set(SERVE_FLOW_SUCCESS, locker_entry.locker_id, locker_entry.user_id, ESP_OK,
                          "password take completed");
    return true;
}

bool serve_request_debug_verify(void){
    if (verify_debug == NULL){
        return false;
    }
    return xSemaphoreGive(verify_debug) == pdTRUE;
}

void serve_get_save_status(serve_save_status_t *out){
    if (out == NULL){
        return;
    }
    memcpy(out, &g_save_status, sizeof(*out));
}

void serve_get_take_status(serve_take_status_t *out){
    if (out == NULL){
        return;
    }
    memcpy(out, &g_take_status, sizeof(*out));
}

/* Private functions ---------------------------------------------------------*/
void ready_take_task(void* param){
    while (1){
        if (xSemaphoreTake(ready_take, portMAX_DELAY) == pdTRUE){
            ESP_LOGI(SERVE_TAG, "Received ready_take signal, can start take process...");
            serve_take_status_set(SERVE_FLOW_RUNNING, 0xFF, 0, ESP_OK, "palm take running");

            uint16_t verified_user_id = 0;
            char verified_name[32] = {0};
            user_locker_entry_t locker_entry = {0};

            xst_result_t verify_res = xst_cmd_verify(10, &verified_user_id, verified_name);
            if (verify_res != MR_SUCCESS){
                ESP_LOGE(SERVE_TAG, "Failed to verify user, res=%d", verify_res);
                if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_ERROR);
                serve_take_status_set(SERVE_FLOW_FAILED, 0xFF, 0, ESP_FAIL, "Palm verify failed");
                continue;
            }

            esp_err_t db_err = locker_db_get_entry_by_user(verified_user_id, &locker_entry);
            if (db_err != ESP_OK){
                ESP_LOGW(SERVE_TAG, "No locker binding found for user_id=%u", verified_user_id);
                if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_ERROR);
                serve_take_status_set(SERVE_FLOW_FAILED, 0xFF, verified_user_id, db_err, "No locker binding found");
                continue;
            }

            if (locker_entry.locker_id >= 4){
                ESP_LOGE(SERVE_TAG, "Invalid locker_id=%u for user_id=%u", locker_entry.locker_id, verified_user_id);
                if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_ERROR);
                serve_take_status_set(SERVE_FLOW_FAILED, locker_entry.locker_id, verified_user_id,
                                      ESP_ERR_INVALID_STATE, "Invalid locker id");
                continue;
            }

            ESP_LOGI(SERVE_TAG, "Verify success for user_id=%u name=%s, opening locker %u",
                     verified_user_id,
                     verified_name,
                     locker_entry.locker_id + 1);
            locker_on(&lockers[locker_entry.locker_id]);
            if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_SUCCESS);

            xst_result_t del_res = xst_cmd_del_user(verified_user_id);
            if (del_res != MR_SUCCESS){
                ESP_LOGW(SERVE_TAG, "Locker opened but failed to delete user_id=%u from XST, res=%d; local locker binding will still be cleared",
                         verified_user_id,
                         del_res);
            }

            db_err = locker_db_remove_entry_by_locker(locker_entry.locker_id);
            if (db_err != ESP_OK){
                ESP_LOGE(SERVE_TAG, "Locker opened and XST user deleted, but failed to clear locker %u binding: %s",
                         locker_entry.locker_id + 1,
                         esp_err_to_name(db_err));
                if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_ERROR);
                serve_take_status_set(SERVE_FLOW_FAILED, locker_entry.locker_id, verified_user_id, db_err,
                                      "Failed to clear binding");
                continue;
            }

            serve_take_status_set(SERVE_FLOW_SUCCESS,
                                  locker_entry.locker_id,
                                  verified_user_id,
                                  (del_res == MR_SUCCESS) ? ESP_OK : ESP_FAIL,
                                  (del_res == MR_SUCCESS) ? "palm take completed" : "palm take completed, XST delete warning");
            ESP_LOGI(SERVE_TAG, "Take flow completed for user_id=%u, locker %u released",
                     verified_user_id,
                     locker_entry.locker_id + 1);
        }
        vTaskDelay(pdMS_TO_TICKS(200)); // 模拟取件处理时间
    }
}

void ready_save_task(void* param){
    while (1){
        if (xSemaphoreTake(ready_save, portMAX_DELAY) == pdTRUE){
            ESP_LOGI(SERVE_TAG, "Received ready_save signal, can start save process...");
            g_save_status.state = SERVE_FLOW_RUNNING;
            g_save_status.err = ESP_OK;
            strncpy(g_save_status.message, "saving in progress", sizeof(g_save_status.message) - 1);
            g_save_status.message[sizeof(g_save_status.message) - 1] = '\0';

            uint8_t locker_id = locker_db_find_free_locker();
            if (locker_id >= 4){
                ESP_LOGW(SERVE_TAG, "No free locker available for save flow");
                if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_ERROR);
                serve_save_status_fail(ESP_ERR_NOT_FOUND, "No free locker available");
                continue;
            }

            uint16_t new_user_id = 0;
            locker_t* target_locker = &lockers[locker_id];

            // 重试机制：最多重试 3 次
            uint8_t retry_count = 0;
            xst_result_t enroll_res = MR_REJECTED;

            while (retry_count < 3 && enroll_res != MR_SUCCESS){
                ESP_LOGI(SERVE_TAG, "Enroll attempt %u/3 for locker %d",
                         retry_count + 1,
                         target_locker->locker_id + 1);

                enroll_res = xst_cmd_enroll_single((const char*)target_locker->locker_info.locker_user_info,
                                                   1,
                                                   10,
                                                   &new_user_id);

                if (enroll_res == MR_SUCCESS){
                    ESP_LOGI(SERVE_TAG, "Enroll success for locker %d with user_id=%u",
                             target_locker->locker_id + 1,
                             new_user_id);
                    target_locker->have_saved = true;
                    if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_SHORT);
                    break;
                }

                retry_count++;
                if (retry_count < 3){
                    // 重试前等待 1 秒
                    vTaskDelay(pdMS_TO_TICKS(1000));
                }
            }

            if (enroll_res != MR_SUCCESS){
                ESP_LOGE(SERVE_TAG, "Failed to enroll user for locker %d after 3 attempts, res=%d",
                         target_locker->locker_id + 1,
                         enroll_res);
                if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_ERROR);
                serve_save_status_fail(ESP_FAIL, "Failed to enroll user (timeout)");
                target_locker->have_saved = false;
                continue;
            }

            g_save_status.user_id = new_user_id;
            target_locker->locker_info.locker_user_info_id[0] = (new_user_id >> 8) & 0xFF;
            target_locker->locker_info.locker_user_info_id[1] = new_user_id & 0xFF;
            crumble_password(target_locker->password);

            user_locker_entry_t locker_entry = {
                .user_id = new_user_id,
                .locker_id = target_locker->locker_id,
                .timestamp = (uint32_t)xTaskGetTickCount(),
                .is_valid = true,
            };
            memcpy(locker_entry.password, target_locker->password, sizeof(locker_entry.password));

            esp_err_t db_err = locker_db_add_entry(&locker_entry);
            if (db_err != ESP_OK){
                ESP_LOGE(SERVE_TAG, "Failed to save locker binding for locker %d: %s",
                         target_locker->locker_id + 1,
                         esp_err_to_name(db_err));
                if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_ERROR);
                serve_save_status_fail(db_err, "Failed to persist binding");
                xst_cmd_del_user(new_user_id);
                continue;
            }

            g_save_status.locker_id = target_locker->locker_id;
            memcpy(g_save_status.password, locker_entry.password, sizeof(g_save_status.password));
            g_save_status.has_password = true;

            ESP_LOGI(SERVE_TAG,
                     "Save flow success: user_id=%u locker=%u password=%d%d%d%d",
                     new_user_id,
                     target_locker->locker_id + 1,
                     locker_entry.password[0],
                     locker_entry.password[1],
                     locker_entry.password[2],
                     locker_entry.password[3]);

            vTaskDelay(pdMS_TO_TICKS(1000));
            locker_on(target_locker);
            if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_SUCCESS);
            g_save_status.state = SERVE_FLOW_SUCCESS;
            g_save_status.err = ESP_OK;
            strncpy(g_save_status.message, "save flow completed", sizeof(g_save_status.message) - 1);
            g_save_status.message[sizeof(g_save_status.message) - 1] = '\0';
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void verify_debug_task(void* param){
    while (1){
        if (xSemaphoreTake(verify_debug, portMAX_DELAY) == pdTRUE){
            ESP_LOGI(SERVE_TAG, "已收到测试识别信号量");
            uint16_t* out_user_id = malloc(sizeof(uint16_t));
            if (out_user_id == NULL){
                ESP_LOGE(SERVE_TAG, "Failed to allocate memory for verify output");
                if (out_user_id) free(out_user_id);
                continue;
            }
            xst_cmd_enroll_single("debuger", 0, 10, out_user_id);
            if (out_user_id[0] == 0){
                ESP_LOGI(SERVE_TAG, "User id is 0");
            }
            else{
                ESP_LOGI(SERVE_TAG, "User id is %d", *out_user_id);
            }
            free(out_user_id);
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // 模拟取件处理时间
    }
}
