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

        }
        vTaskDelay(pdMS_TO_TICKS(100)); // 模拟取件处理时间
    }
}

void ready_save_task(void* param){
    while (1){
        if (xSemaphoreTake(ready_save, portMAX_DELAY) == pdTRUE){
            ESP_LOGI(SERVE_TAG, "Received ready_save signal, can start save process...");
            // 先判断四个柜子有没有登记信息
            for (uint8_t i = 0 ; i < 4 ; i ++ ){
                if (lockers [i] . locker_info .locker_user_info_id [0] == 0 && lockers [i] . locker_info .locker_user_info_id [1] == 0){
                    ESP_LOGI(SERVE_TAG, "Locker %d is free", lockers[i].locker_id + 1);
                    lockers[i].have_saved = false ;
                }
                else{
                    ESP_LOGI(SERVE_TAG, "Locker %d is occupied by user ID: %02X%02X", lockers[i].locker_id + 1,
                             lockers[i].locker_info.locker_user_info_id[0], lockers[i].locker_info.locker_user_info_id[1]);
                    lockers[i].have_saved = true ;
                }
            }

            // 从头开始找一个没有登记信息的柜子来进行存件
            for ( uint8_t i = 0 ; i < 4 ; i ++ ){
                //确认该柜没有存件
                if (lockers[i].have_saved == false){
                    uint16_t new_user_id = 0 ;
                    // 发送注册序列并储存好生成的用户ID
                    if (MR_SUCCESS == xst_cmd_enroll_single((const char *)lockers[i].locker_info.locker_user_info, 1, 10, &new_user_id))
                    {
                        ESP_LOGI(SERVE_TAG, "Enrolled new user with ID: %d for locker %d", new_user_id, lockers[i].locker_id + 1);
                        // 将生成好的用户ID存放到储物柜单元结构体
                        lockers[i].locker_info.locker_user_info_id[0] = (new_user_id >> 8) & 0xFF;
                        lockers[i].locker_info.locker_user_info_id[1] = new_user_id & 0xFF;
                        lockers[i].have_saved = true ;

                        // 生成一下随机的4位密码，覆盖掉之前的默认密码
                        crumble_password(lockers[i].password);

                        // 这里要将密码推送到手机还要显示到屏幕上

                        //打开柜门
                        vTaskDelay(pdMS_TO_TICKS(1000));
                        locker_on(&lockers[i]);
                    }
                    else{
                        ESP_LOGE(SERVE_TAG, "Failed to enroll user for locker %d", lockers[i].locker_id + 1);
                    }
                    break;
                }
            }

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
