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

#include "xst.h"
#include "locker.h"

/* Private macros ------------------------------------------------------------*/
#define SERVE_TAG "serve"
/* Private types -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
SemaphoreHandle_t ready_save = NULL; // 通知能够进行存件
SemaphoreHandle_t ready_take = NULL; // 通知能够进行取件
SemaphoreHandle_t verify_debug = NULL; // 通知能够进行取件

QueueHandle_t save_verify_process = NULL; // 存件录取进度队列

uint16_t user_num[128] = {0};
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
    save_verify_process = xQueueCreate(4,sizeof(uint16_t));

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

/* Private functions ---------------------------------------------------------*/
void ready_take_task(void* param){
    while (1){
        if (xSemaphoreTake(ready_take, portMAX_DELAY) == pdTRUE){
            ESP_LOGI(SERVE_TAG, "Received ready_take signal, can start take process...");

            uint16_t verified_user_id = 0;
            char verified_name[32] = {0};
            user_locker_entry_t locker_entry = {0};

            xst_result_t verify_res = xst_cmd_verify(10, &verified_user_id, verified_name);
            if (verify_res != MR_SUCCESS){
                ESP_LOGE(SERVE_TAG, "Failed to verify user, res=%d", verify_res);
                continue;
            }

            esp_err_t db_err = locker_db_get_entry_by_user(verified_user_id, &locker_entry);
            if (db_err != ESP_OK){
                ESP_LOGW(SERVE_TAG, "No locker binding found for user_id=%u", verified_user_id);
                continue;
            }

            if (locker_entry.locker_id >= 4){
                ESP_LOGE(SERVE_TAG, "Invalid locker_id=%u for user_id=%u", locker_entry.locker_id, verified_user_id);
                continue;
            }

            ESP_LOGI(SERVE_TAG, "Verify success for user_id=%u name=%s, opening locker %u",
                     verified_user_id,
                     verified_name,
                     locker_entry.locker_id + 1);
            locker_on(&lockers[locker_entry.locker_id]);

            xst_result_t del_res = xst_cmd_del_user(verified_user_id);
            if (del_res != MR_SUCCESS){
                ESP_LOGW(SERVE_TAG, "Locker opened but failed to delete user_id=%u from XST, res=%d", verified_user_id, del_res);
                continue;
            }

            db_err = locker_db_remove_entry_by_locker(locker_entry.locker_id);
            if (db_err != ESP_OK){
                ESP_LOGE(SERVE_TAG, "Locker opened and XST user deleted, but failed to clear locker %u binding: %s",
                         locker_entry.locker_id + 1,
                         esp_err_to_name(db_err));
                continue;
            }

            ESP_LOGI(SERVE_TAG, "Take flow completed for user_id=%u, locker %u released",
                     verified_user_id,
                     locker_entry.locker_id + 1);
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // 模拟取件处理时间
    }
}

void ready_save_task(void* param){
    while (1){
        if (xSemaphoreTake(ready_save, portMAX_DELAY) == pdTRUE){
            ESP_LOGI(SERVE_TAG, "Received ready_save signal, can start save process...");

            uint8_t locker_id = locker_db_find_free_locker();
            if (locker_id >= 4){
                ESP_LOGW(SERVE_TAG, "No free locker available for save flow");
                continue;
            }

            uint16_t new_user_id = 0;
            locker_t *target_locker = &lockers[locker_id];
            xst_result_t enroll_res = xst_cmd_enroll_single((const char *)target_locker->locker_info.locker_user_info,
                                                            1,
                                                            10,
                                                            &new_user_id);
            if (enroll_res != MR_SUCCESS){
                ESP_LOGE(SERVE_TAG, "Failed to enroll user for locker %d, res=%d",
                         target_locker->locker_id + 1,
                         enroll_res);
                continue;
            }

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
                xst_cmd_del_user(new_user_id);
                continue;
            }

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
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // 模拟存件处理时间
    }
}

void verify_debug_task(void* param){
    while (1){
        if (xSemaphoreTake(verify_debug, portMAX_DELAY) == pdTRUE){
            ESP_LOGI(SERVE_TAG, "已收到测试识别信号量");
            uint16_t *out_user_id = malloc(sizeof(uint16_t));
            if (out_user_id == NULL ){
                ESP_LOGE(SERVE_TAG, "Failed to allocate memory for verify output");
                if (out_user_id) free(out_user_id);
                continue;
            }
            xst_cmd_enroll_single("debuger" ,0 ,10, out_user_id);
            if (out_user_id[0] == 0){
                ESP_LOGI(SERVE_TAG, "User id is 0");
            }else{
                ESP_LOGI(SERVE_TAG, "User id is %d", *out_user_id);
            }
            free(out_user_id);
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // 模拟取件处理时间
    }
}
