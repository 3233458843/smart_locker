#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_system.h"
#include "esp_timer.h"  
#include "esp_log.h"

#include "lvgl.h"
#include "demos/lv_demos.h"

#include "../bsp/ui/generated/gui_guider.h"
#include "../bsp/ui/generated/events_init.h"

// #include "xst.h"
// #include "xst_pack_t.h"

#include "../bsp/lvgl_port/lv_port_disp.h"
#include "../bsp/lvgl_port/lv_port_indev.h"

#define TAG "MAIN"

static esp_timer_handle_t lvgl_tick_timer = NULL;

lv_ui guider_ui;  // 全局 UI 结构体实例

// 定时回调函数，每 1ms 触发
static void lv_tick_task(void *arg) {
    lv_tick_inc(1);
}
 
// 初始化 LVGL Tick 定时器
void lvgl_tick_timer_init(void) {
    const esp_timer_create_args_t timer_args = {
        .callback = &lv_tick_task,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "lv_tick_timer"
    };
 
    esp_timer_create(&timer_args, &lvgl_tick_timer);
    esp_timer_start_periodic(lvgl_tick_timer, 1000); // 1ms 触发
}

/* LVGL demo 任务 - 运行在独立任务中，避免主任务栈溢出 */
static void lvgl_demo_task(void *arg)
{
    (void)arg;

    setup_ui(&guider_ui);
    events_init(&guider_ui);   

    ESP_LOGI(TAG, "LVGL demo started");
    
    while (1) {
        lv_task_handler();  // LVGL 任务管理
        vTaskDelay(pdMS_TO_TICKS(5));  // 延迟 10ms
    }
    
    vTaskDelete(NULL);
}

void app_main(void){
    /* 初始化 LVGL Tick 定时器 */
    lvgl_tick_timer_init();
    lv_init();  // 初始化 LVGL 库

    /* 初始化显示和触摸 */
    lv_port_disp_init();
    lv_port_indev_init();   

    /* 创建LVGL demo任务 */
    xTaskCreatePinnedToCore(
        lvgl_demo_task,
        "lvgl_demo_task",
        24000,                 
        NULL,
        3,                     /* 优先级 */
        NULL,
        0                      /* 运行在核心0上 */
    );
    ESP_LOGI(TAG, "App initialized successfully");
}