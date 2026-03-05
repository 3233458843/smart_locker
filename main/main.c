#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

#include "display.h"
#include "touch.h"
#include "xst.h"

#include "esp_lcd_panel_ops.h"

#define TAG "MAIN"

#define COLOR_RED   0xF800
#define COLOR_GREEN 0x07E0
#define COLOR_BLUE  0x001F
#define COLOR_WHITE 0xFFFF
#define COLOR_BLACK 0x0000

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

static void test_draw_background(esp_lcd_panel_handle_t panel_handle, uint16_t color)
{
    ESP_LOGI(TAG, "Drawing background color: 0x%04X", color);
    
    // ESP32 的 SPI DMA 要求内存必须具有 DMA 属性，这样传输最快
    uint16_t *bg_buf = (uint16_t *)heap_caps_malloc(DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(uint16_t), MALLOC_CAP_DMA);
    if (bg_buf == NULL) {
        ESP_LOGE(TAG, "No memory for background buffer!");
        return;
    }

    // 填充颜色
    for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
        bg_buf[i] = color; 
    }

    // 调用 DMA 异步刷屏
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, bg_buf);
    
    // 阻塞等待 DMA 传输完成（调用我们在 display.c 写的信号量等待）
    display_wait_flush_done();
    
    // 传输完成后才能释放内存，否则会导致系统崩溃
    free(bg_buf);
}

void app_main(void){
    // xst_init(my_xst_callback);

    // ESP_LOGI("APP", "Resetting module...");

    // xst_result_t reset_result = xst_cmd_reset();
    // if (reset_result == MR_SUCCESS) {
    //     ESP_LOGI("APP", "Reset Success");
    // } else {
    //     ESP_LOGW("APP", "Reset Failed, error code: %d", reset_result);  // 用 %d 打印枚举
    // }
    // vTaskDelay(pdMS_TO_TICKS(1000));

    //     // 4. 获取状态
    // uint8_t status = 0;
    // xst_cmd_get_status(&status);
    // ESP_LOGI("APP", "Status: %d", status);
    // vTaskDelay(pdMS_TO_TICKS(5000));

    // reset_result = xst_cmd_del_all();
    // if (reset_result == MR_SUCCESS) {
    //     ESP_LOGI("APP", "Delete All Success");
    // } else {
    //     ESP_LOGW("APP", "Delete All Failed, error code: %d", reset_result);
    // }
    // vTaskDelay(pdMS_TO_TICKS(5000));

    // xst_cmd_verify(10,NULL , "abc");
    // vTaskDelay(pdMS_TO_TICKS(5000));
    
    // while (1) {
    //     vTaskDelay(pdMS_TO_TICKS(1000));
    // }


    ESP_LOGI(TAG, "--- System Initialization Start ---");

    /* 1. 初始化显示屏 */
    ESP_ERROR_CHECK(display_init());

    /* 2. 初始化背光并设置为 80% 亮度 */
    ESP_ERROR_CHECK(backlight_init());
    backlight_set(100);

    /* 3. 初始化触摸屏 */
    ESP_ERROR_CHECK(touch_init());

    ESP_LOGI(TAG, "--- System Initialization Done ---");

    /* 获取屏幕句柄，准备画图 */
    esp_lcd_panel_handle_t panel = display_get_panel_handle();

    /* 4. 测试屏幕：刷成纯蓝色背景 */
    test_draw_background(panel, COLOR_RED);

    /* 5. 准备触摸反馈：申请一个 10x10 像素的小红块内存 */
    #define POINT_SIZE 10
    uint16_t *point_buf = (uint16_t *)heap_caps_malloc(POINT_SIZE * POINT_SIZE * sizeof(uint16_t), MALLOC_CAP_DMA);
    if (point_buf != NULL) {
        for (int i = 0; i < POINT_SIZE * POINT_SIZE; i++) {
            point_buf[i] = COLOR_BLACK; // 红色
        }
    }

    /* 6. 进入主循环：读取触摸并给出反馈 */
    touch_point_t tp;
    
    ESP_LOGI(TAG, "Please touch the screen...");
    while (1) {
        // 读取触摸坐标
        touch_read_point(&tp);

        if (tp.is_pressed) {
            // 打印坐标到串口终端
            ESP_LOGI(TAG, "Touch Detected! X: %03d, Y: %03d", tp.x, tp.y);

            // 在屏幕上画一个 10x10 的小红块跟随手指
            if (point_buf != NULL) {
                // 计算红块的起始和结束坐标，并防止超出屏幕边界
                uint16_t x_start = (tp.x > POINT_SIZE / 2) ? (tp.x - POINT_SIZE / 2) : 0;
                uint16_t y_start = (tp.y > POINT_SIZE / 2) ? (tp.y - POINT_SIZE / 2) : 0;
                uint16_t x_end = x_start + POINT_SIZE;
                uint16_t y_end = y_start + POINT_SIZE;

                if (x_end > DISPLAY_WIDTH) x_end = DISPLAY_WIDTH;
                if (y_end > DISPLAY_HEIGHT) y_end = DISPLAY_HEIGHT;

                // 局部刷新小红块
                esp_lcd_panel_draw_bitmap(panel, x_start, y_start, x_end, y_end, point_buf);
                display_wait_flush_done();
            }
        }

        // 延时 20ms，相当于约 50Hz 的触摸采样率，释放 CPU 给其他任务
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
