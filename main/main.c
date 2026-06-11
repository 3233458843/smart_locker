#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "driver/uart.h"
#include "esp_timer.h"
#include "esp_log.h"

#include "lvgl.h"
#include "../bsp/ui/generated/gui_guider.h"
#include "../bsp/ui/generated/events_init.h"

#include "xst.h"

#include "locker.h"

#include "buzzer/buzzer.h"

#include "../bsp/lvgl_port/lv_port_disp.h"
#include "../bsp/lvgl_port/lv_port_indev.h"

#include "serve.h"
#include "network.h"
#include "palm_progress.h"

#undef TAG
#define TAG "MAIN"
// ------------------------------------------------------------------------------------
static esp_timer_handle_t lvgl_tick_timer = NULL;
static bool g_locker_status_shown[4] = {false, false, false, false};
static lv_obj_t* g_locker_status_bound_main = NULL;

static void main_locker_status_cache_reset(void){
    for (uint8_t i = 0; i < 4; i++){
        g_locker_status_shown[i] = false;
    }
    g_locker_status_bound_main = NULL;
}


static bool main_locker_status_widget_is_valid(lv_obj_t* obj){
    return obj != NULL && lv_obj_is_valid(obj);
}

static bool main_locker_status_need_refresh(void){
    lv_obj_t* locker_widgets[4] = {
        guider_ui.main_locker1,
        guider_ui.main_locker2,
        guider_ui.main_locker3,
        guider_ui.main_locker4,
    };

    if (!main_locker_status_widget_is_valid(guider_ui.main)){
        return false;
    }

    if (g_locker_status_bound_main != guider_ui.main){
        return true;
    }

    for (uint8_t i = 0; i < 4; i++){
        if (!main_locker_status_widget_is_valid(locker_widgets[i])){
            return false;
        }
    }

    return false;
}

static void main_locker_status_apply(lv_obj_t* widget, bool occupied){
    if (!main_locker_status_widget_is_valid(widget)){
        return;
    }

    lv_obj_set_style_radius(widget, LV_RADIUS_CIRCLE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(widget, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(widget, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(widget, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(widget,
                              occupied ? lv_color_hex(0xFF0000) : lv_color_hex(0x2FDA64),
                              LV_PART_MAIN | LV_STATE_DEFAULT);
}

lv_ui guider_ui;

static void lv_tick_task(void* arg){
    lv_tick_inc(1);
}

void lvgl_tick_timer_init(void){
    const esp_timer_create_args_t timer_args = {
        .callback = &lv_tick_task,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "lv_tick_timer"
    };

    esp_timer_create(&timer_args, &lvgl_tick_timer);
    esp_timer_start_periodic(lvgl_tick_timer, 1000);
}

static void main_locker_status_styles_init(void){
    lv_obj_t* locker_widgets[4] = {
        guider_ui.main_locker1,
        guider_ui.main_locker2,
        guider_ui.main_locker3,
        guider_ui.main_locker4,
    };

    main_locker_status_cache_reset();

    if (!main_locker_status_widget_is_valid(guider_ui.main)){
        return;
    }

    g_locker_status_bound_main = guider_ui.main;

    for (uint8_t i = 0; i < 4; i++){
        lv_obj_t* widget = locker_widgets[i];
        if (!main_locker_status_widget_is_valid(widget)){
            continue;
        }
        g_locker_status_shown[i] = has_item_in_locker(&lockers[i]);
        main_locker_status_apply(widget, g_locker_status_shown[i]);
    }
}

static void main_locker_status_update_task(void){
    if (lv_screen_active() != guider_ui.main){
        main_locker_status_cache_reset();
        return;
    }

    if (main_locker_status_need_refresh()){
        main_locker_status_styles_init();
    }

    lv_obj_t* locker_widgets[4] = {
        guider_ui.main_locker1,
        guider_ui.main_locker2,
        guider_ui.main_locker3,
        guider_ui.main_locker4,
    };

    for (uint8_t i = 0; i < 4; i++){
        if (!main_locker_status_widget_is_valid(locker_widgets[i])){
            continue;
        }

        bool occupied = has_item_in_locker(&lockers[i]);
        if (occupied == g_locker_status_shown[i]){
            continue;
        }

        main_locker_status_apply(locker_widgets[i], occupied);
        g_locker_status_shown[i] = occupied;
    }
}

static void lvgl_task(void* arg){
    (void)arg;

    setup_ui(&guider_ui);
    events_init(&guider_ui);
    main_locker_status_styles_init();
    main_locker_status_update_task();

    ESP_LOGI(TAG, "LVGL started");

    TickType_t last_status_refresh = xTaskGetTickCount();
    while (1){
        TickType_t now = xTaskGetTickCount();
        if ((now - last_status_refresh) >= pdMS_TO_TICKS(1000)){
            main_locker_status_update_task();
            last_status_refresh = now;
        }

        palm_progress_update(now);

        lv_task_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void xst_note_cb(uint8_t nid, uint8_t* data, uint16_t len){
    ESP_LOGI(TAG, "==== XST note (NID: %d) ====", nid);
    switch (nid){
    case NID_READY:
        ESP_LOGI(TAG, "Palm module ready");
        break;
    case NID_PALM_STATE:
        if (len > 0){
            g_xst_palm_progress = data[0];
            g_palm_progress_updated = true;
            ESP_LOGI(TAG, "Palm state progress: %d", g_xst_palm_progress);
        }
        break;
    case NID_UNKNOWNERROR:
        ESP_LOGE(TAG, "Palm hardware error");
        break;
    default:
        ESP_LOGI(TAG, "Other note, len=%d", len);
        break;
    }
}

static void power_on_locker_xst_sync(void){
    uint16_t xst_user_ids[64] = {0};
    uint16_t xst_user_count = 0;
    xst_result_t res = MR_FAILED4_TIME_OUT;

    ESP_LOGI(TAG, "Power-on locker/XST sync start");
    for (uint8_t attempt = 0; attempt < 3; attempt++){
        res = xst_cmd_get_all_user_ids(xst_user_ids,
                                       (uint16_t)(sizeof(xst_user_ids) / sizeof(xst_user_ids[0])),
                                       &xst_user_count);
        if (res == MR_SUCCESS){
            break;
        }
        ESP_LOGW(TAG, "Sync attempt %u/3 failed, res=%d", attempt + 1, res);
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    if (res != MR_SUCCESS){
        ESP_LOGW(TAG, "Sync skipped: failed to read XST users, res=%d", res);
        return;
    }

    esp_err_t err = locker_db_sync_with_xst_users(xst_user_ids, xst_user_count);
    if (err != ESP_OK){
        ESP_LOGE(TAG, "Sync failed: %s", esp_err_to_name(err));
    }

    // 反向同步：删除 XST 中不存在于 NVS 绑定的孤立用户
    for (uint16_t i = 0; i < xst_user_count; i++){
        user_locker_entry_t entry;
        if (locker_db_get_entry_by_user(xst_user_ids[i], &entry) != ESP_OK){
            ESP_LOGW(TAG, "Power-on sync: deleting orphaned XST user_id=%u", xst_user_ids[i]);
            xst_cmd_del_user(xst_user_ids[i]);
        }
    }

    ESP_LOGI(TAG, "Sync done, xst_user_count=%u", (unsigned int)xst_user_count);
}

void main_serve(void* arg){
    (void)arg;
    while (1){
        for (uint8_t i = 0; i < 4; i++){
            bool current_state = Detection_locker_on_off(&lockers[i]);
            if (current_state != lockers[i].is_locked){
                lockers[i].is_locked = current_state;
                ESP_LOGI(TAG, "Locker %d state: %s",
                         i + 1,
                         current_state ? "locked" : "unlocked");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void app_main(void){
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    serve_init();

    lvgl_tick_timer_init();
    lv_init();

    lv_port_disp_init();
    lv_port_indev_init();

    locker_init();
    locker_db_init();

    for (uint8_t i = 0; i < 4; i++){
        lockers[i].is_locked = Detection_locker_on_off(&lockers[i]);
    }
    for (uint8_t i = 0; i < 4; i++){
        ESP_LOGI(TAG, "Locker %d initial: %s",
                 lockers[i].locker_id + 1,
                 lockers[i].is_locked ? "locked" : "unlocked");
    }

    xst_init(xst_note_cb);
    xst_set_progress_callback(palm_progress_on_xst_progress);
    vTaskDelay(pdMS_TO_TICKS(500));
    power_on_locker_xst_sync();

    buzzer_device_t* buzzer_dev = buzzer_create_gpio_device(
        0, GPIO_NUM_13, "main_buzzer"
    );
    if (!buzzer_dev){
        ESP_LOGE(TAG, "Failed to create buzzer device");
        return;
    }
    ESP_ERROR_CHECK(buzzer_register_device(buzzer_dev));
    ESP_ERROR_CHECK(buzzer_init());

    g_buzzer_handle = buzzer_get_handle(0);
    if (g_buzzer_handle != NULL){
        ESP_LOGI(TAG, "Buzzer ready");
        buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_SHORT);
    }

    wifi_init_softap();
    xTaskCreatePinnedToCore(tcp_server_task, "tcp_server", 4096, NULL, 5, NULL, 1);

    xTaskCreatePinnedToCore(
        lvgl_task, "lvgl_task", 20480, NULL, 5, NULL, tskNO_AFFINITY
    );
    ESP_LOGI(TAG, "App initialized successfully");

    xTaskCreatePinnedToCore(main_serve, "main_serve", 4096, NULL, 5, NULL, 1);
}
