#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

#include "display.h"
#include "touch.h"
#include "xst.h"

#define TAG "MAIN"

void my_xst_callback(uint8_t nid, uint8_t *data, uint16_t len) {
    switch (nid) {
        case NID_READY:
            ESP_LOGI("APP", "Module is Ready!");
            break;
        case NID_FACE_STATE:
            ESP_LOGI("APP", "Face detected during interaction");
            break;
        default:
            ESP_LOGI("APP", "Received Note ID: %d", nid);
            break;
    }
}

void app_main(void){
    xst_init(my_xst_callback);

    ESP_LOGI("APP", "Resetting module...");

    xst_result_t reset_result = xst_cmd_reset();
    if (reset_result == MR_SUCCESS) {
        ESP_LOGI("APP", "Reset Success");
    } else {
        ESP_LOGW("APP", "Reset Failed, error code: %d", reset_result);  // 用 %d 打印枚举
    }
    vTaskDelay(pdMS_TO_TICKS(1000));

        // 4. 获取状态
    uint8_t status = 0;
    xst_cmd_get_status(&status);
    ESP_LOGI("APP", "Status: %d", status);
    vTaskDelay(pdMS_TO_TICKS(5000));

    reset_result = xst_cmd_del_all();
    if (reset_result == MR_SUCCESS) {
        ESP_LOGI("APP", "Delete All Success");
    } else {
        ESP_LOGW("APP", "Delete All Failed, error code: %d", reset_result);
    }
    vTaskDelay(pdMS_TO_TICKS(5000));

    xst_cmd_verify(10,NULL , "abc");
    vTaskDelay(pdMS_TO_TICKS(5000));
    
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}

