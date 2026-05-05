#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "driver/uart.h"
#include "esp_timer.h"
#include "esp_log.h"

#include "esp_wifi.h"
#include "lwip/sockets.h"

#include "lvgl.h"
#include "../bsp/ui/generated/gui_guider.h"
#include "../bsp/ui/generated/events_init.h"

#include "xst.h"

#include "locker.h"

#include "buzzer/buzzer.h"

#include "../bsp/lvgl_port/lv_port_disp.h"
#include "../bsp/lvgl_port/lv_port_indev.h"

#include "serve.h"

#undef TAG
#define TAG "MAIN"
#define TCP_PORT 8080
// ------------------------------------------------------------------------------------
static esp_timer_handle_t lvgl_tick_timer = NULL;
static bool g_locker_status_shown[4] = {false, false, false, false};
static lv_obj_t* g_locker_status_bound_main = NULL;
uint8_t g_xst_palm_progress = 0;
bool g_palm_progress_updated = false;

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

lv_ui guider_ui; // 全局 UI 结构体实例

// 定时回调函数，每 1ms 触发
static void lv_tick_task(void* arg){
    lv_tick_inc(1);
}

// 初始化 LVGL Tick 定时器
void lvgl_tick_timer_init(void){
    const esp_timer_create_args_t timer_args = {
        .callback = &lv_tick_task,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "lv_tick_timer"
    };

    esp_timer_create(&timer_args, &lvgl_tick_timer);
    esp_timer_start_periodic(lvgl_tick_timer, 1000); // 1ms 触发
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

/* LVGL demo 任务 - 运行在独立任务中，避免主任务栈溢出 */
static void lvgl_task(void* arg){
    (void)arg;

    setup_ui(&guider_ui);
    events_init(&guider_ui);
    main_locker_status_styles_init();
    main_locker_status_update_task();

    ESP_LOGI(TAG, "LVGL started");

    TickType_t last_status_refresh = xTaskGetTickCount();
    uint8_t last_palm_progress = 0;
    uint8_t last_save_progress = 0;
    while (1){
        TickType_t now = xTaskGetTickCount();
        if ((now - last_status_refresh) >= pdMS_TO_TICKS(1000)){
            main_locker_status_update_task();
            last_status_refresh = now;
        }

        // 实时更新识别/录取进度显示
        if (lv_screen_active() == guider_ui.take_page) {
            if (main_locker_status_widget_is_valid(guider_ui.take_page_cont_1) &&
                !lv_obj_has_flag(guider_ui.take_page_cont_1, LV_OBJ_FLAG_HIDDEN)) {
                if (g_xst_palm_progress != last_palm_progress || g_palm_progress_updated) {
                    if (main_locker_status_widget_is_valid(guider_ui.take_page_label_1)){
                        char buf[100];
                        snprintf(buf, sizeof(buf), "请将手掌置于传感器前方10cm左右\n正在识别中... %d%%", g_xst_palm_progress);
                        lv_label_set_text(guider_ui.take_page_label_1, buf);
                    }
                    last_palm_progress = g_xst_palm_progress;
                    g_palm_progress_updated = false;
                }
            }
        } else if (lv_screen_active() == guider_ui.save_page) {
            // 存件页面：掌纹录取进度
            serve_save_status_t save_st;
            serve_get_save_status(&save_st);
            if (save_st.state == SERVE_FLOW_RUNNING || save_st.state == SERVE_FLOW_PENDING) {
                if (g_xst_palm_progress != last_save_progress || g_palm_progress_updated) {
                    if (main_locker_status_widget_is_valid(guider_ui.save_page_bar_1)){
                        lv_bar_set_value(guider_ui.save_page_bar_1, g_xst_palm_progress, LV_ANIM_ON);
                    }
                    char pct[8];
                    snprintf(pct, sizeof(pct), "%d%%", g_xst_palm_progress);
                    if (main_locker_status_widget_is_valid(guider_ui.save_page_label_2)){
                        lv_label_set_text(guider_ui.save_page_label_2, pct);
                    }
                    last_save_progress = g_xst_palm_progress;
                    g_palm_progress_updated = false;
                }
            }
        } else {
            // 离开取件/存件页面时重置进度
            last_palm_progress = 0;
            last_save_progress = 0;
        }

        lv_task_handler(); // LVGL 任务管理
        vTaskDelay(pdMS_TO_TICKS(10)); // 延迟 10ms
    }
}

// ------------------------------------------------------------------------------------
void xst_note_cb(uint8_t nid, uint8_t* data, uint16_t len){
    ESP_LOGI(TAG, "==== 收到掌静脉模块主动通知 (NID: %d) ====", nid);
    switch (nid){
    case NID_READY:
        ESP_LOGI(TAG, "掌静脉模块已准备就绪！可以开始识别了。");
        break;
    case NID_PALM_STATE:
        if (len > 0) {
            g_xst_palm_progress = data[0];
            g_palm_progress_updated = true;
            ESP_LOGI(TAG, "掌静脉状态更新: 进度 = %d", g_xst_palm_progress);
        } else {
            ESP_LOGI(TAG, "掌静脉状态更新 (无数据)...");
        }
        break;
    case NID_UNKNOWNERROR:
        ESP_LOGE(TAG, "掌静脉硬件发生异常！");
        break;
    default:
        ESP_LOGI(TAG, "收到其他通知，长度: %d", len);
        break;
    }
}

// ------------------------------------------------------------------------------------
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
        ESP_LOGW(TAG, "Power-on locker/XST sync read attempt %u/3 failed, res=%d",
                 attempt + 1,
                 res);
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    if (res != MR_SUCCESS){
        ESP_LOGW(TAG, "Power-on locker/XST sync skipped: failed to read XST users, res=%d", res);
        return;
    }

    esp_err_t err = locker_db_sync_with_xst_users(xst_user_ids, xst_user_count);
    if (err != ESP_OK){
        ESP_LOGE(TAG, "Power-on locker/XST sync failed: %s", esp_err_to_name(err));
        return;
    }

    ESP_LOGI(TAG, "Power-on locker/XST sync done, xst_user_count=%u", (unsigned int)xst_user_count);
}

// ------------------------------------------------------------------------------------
// WiFi AP 初始化
void wifi_init_softap(void){
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = "ESP32_DEBUG_WIFI", // 手机搜到的名字
            .ssid_len = strlen("ESP32_DEBUG_WIFI"),
            .password = "12345678", // 密码（至少8位）
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "WiFi AP Started. SSID:%s password:%s", "ESP32_DEBUG_WIFI", "12345678");
}

// TCP 服务任务：处理手机发来的指令
void tcp_server_task(void* pvParameters){
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(TCP_PORT);

    int listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    bind(listen_sock, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
    listen(listen_sock, 1);

    uint8_t rx_buffer[1024];
    while (1){
        ESP_LOGI(TAG, "Socket listening on port %d", TCP_PORT);
        struct sockaddr_storage source_addr;
        socklen_t addr_len = sizeof(source_addr);
        int sock = accept(listen_sock, (struct sockaddr*)&source_addr, &addr_len);
        if (sock < 0) continue;

        ESP_LOGI(TAG, "Phone Connected!");
        g_vofa_client_fd = sock; // 开启转发

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 50000;
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

        while (1){
            int len = recv(sock, rx_buffer, sizeof(rx_buffer), 0);
            if (len <= 0) break; // 手机断开连接

            // 将手机发来的 HEX 指令转发给串口硬件
            uart_write_bytes(XST_UART_NUM, rx_buffer, len);
            ESP_LOGI(TAG, "Forwarded %d bytes from phone to UART", len);
        }

        ESP_LOGW(TAG, "Phone Disconnected");
        close(sock);
        g_vofa_client_fd = -1; // 停止转发
    }
}

// ------------------------------------------------------------------------------------
/**main 主要逻辑编辑
 * LVGL UI 更新已集成到 lvgl_demo_task 中，此任务仅用于监测
 */
void main_serve(void* arg){
    (void)arg;
    while (1){
        // 定期检查柜子状态（非 UI 操作）
        for (uint8_t i = 0; i < 4; i++){
            // 检测柜门物理状态
            bool current_state = Detection_locker_on_off(&lockers[i]);
            if (current_state != lockers[i].is_locked){
                lockers[i].is_locked = current_state;
                ESP_LOGI(TAG, "Locker %d state changed to: %s",
                         i + 1,
                         current_state ? "locked" : "unlocked");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(2000)); // 每 2 秒检查一次，减少 CPU 占用
    }
}

// ------------------------------------------------------------------------------------
void app_main(void){
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
        // 如果 NVS 分区被占满或者版本不匹配，需要擦除后重新初始化
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    //----------------------------------------------------------------------------------------------------------
    serve_init();
    //---------------------------------------------------------------------------------------------------------
    /* 初始化 LVGL Tick 定时器 */
    lvgl_tick_timer_init();
    lv_init(); // 初始化 LVGL 库

    /* 初始化显示和触摸 */
    lv_port_disp_init();
    lv_port_indev_init();

    locker_init();
    // 初始化 locker 数据库（从 NVS 恢复数据）
    locker_db_init();
    // 检查柜门状态
    for (uint8_t i = 0; i < 4; i++){
        if (Detection_locker_on_off(&lockers[i])){
            lockers[i].is_locked = true;
        }
        else{
            lockers[i].is_locked = false;
        }
    }
    // 打印柜门状态
    for (uint8_t i = 0; i < 4; i++){
        ESP_LOGI(TAG, "Locker %d initial state: %s", lockers[i].locker_id + 1,
                 lockers[i].is_locked ? "未上锁" : "已上锁");
    }
    // 初始化 XST
    xst_init(xst_note_cb);
    vTaskDelay(pdMS_TO_TICKS(500));
    power_on_locker_xst_sync();

    buzzer_device_t* buzzer_dev = buzzer_create_gpio_device(
        0, // device_id
        GPIO_NUM_13, // GPIO pin
        "main_buzzer" // device name
    );

    if (!buzzer_dev){
        ESP_LOGE(TAG, "Failed to create buzzer device");
        return;
    }

    ESP_ERROR_CHECK(buzzer_register_device(buzzer_dev));

    ESP_ERROR_CHECK(buzzer_init());

    g_buzzer_handle = buzzer_get_handle(0);

    if (g_buzzer_handle != NULL){
        ESP_LOGI(TAG, "Buzzer device initialized successfully");
        buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_SHORT);
    }

    // 初始化 WiFi
    wifi_init_softap();
    // 初始化 TCP 服务
    xTaskCreatePinnedToCore(tcp_server_task, "tcp_server", 4096, NULL, 5, NULL, 1);

    /* 创建 LVGL 任务 */
    xTaskCreatePinnedToCore(
        lvgl_task,
        "lvgl_task",
        20480,
        NULL,
        5, /* 优先级 */
        NULL,
        tskNO_AFFINITY /* 运行在核心0上 */
    );
    ESP_LOGI(TAG, "App initialized successfully");

    xTaskCreatePinnedToCore(main_serve, "main_serve", 4096, NULL, 5, NULL, 1);
}
