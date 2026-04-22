#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "nvs_flash.h"
#include "freertos/timers.h"
#include "esp_system.h"
#include "driver/uart.h"
#include "esp_timer.h"
#include "esp_log.h"

#include "esp_wifi.h"
#include "lwip/sockets.h"

#include "lvgl.h"
// #include "demos/lv_demos.h"

#include "../bsp/ui/generated/gui_guider.h"
#include "../bsp/ui/generated/events_init.h"

#include "xst.h"
#include "xst_pack_t.h"

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

/* LVGL demo 任务 - 运行在独立任务中，避免主任务栈溢出 */
static void lvgl_demo_task(void* arg){
    (void)arg;

    setup_ui(&guider_ui);
    events_init(&guider_ui);
    // 柜门状态LED
    if (lockers[0].is_locked == true){
        lv_obj_set_style_bg_color(guider_ui.main_locker1, lv_color_hex(0x00FF00), 0);
    }
    else{
        lv_obj_set_style_bg_color(guider_ui.main_locker1, lv_color_hex(0xFF0000), 0);
    }

    if (lockers[1].is_locked == true){
        lv_obj_set_style_bg_color(guider_ui.main_locker2, lv_color_hex(0x00FF00), 0);
    }
    else{
        lv_obj_set_style_bg_color(guider_ui.main_locker2, lv_color_hex(0xFF0000), 0);
    }

    if (lockers[2].is_locked == true){
        lv_obj_set_style_bg_color(guider_ui.main_locker3, lv_color_hex(0x00FF00), 0);
    }
    else{
        lv_obj_set_style_bg_color(guider_ui.main_locker3, lv_color_hex(0xFF0000), 0);
    }

    if (lockers[3].is_locked == true){
        lv_obj_set_style_bg_color(guider_ui.main_locker4, lv_color_hex(0x00FF00), 0);
    }
    else{
        lv_obj_set_style_bg_color(guider_ui.main_locker4, lv_color_hex(0xFF0000), 0);
    }

    ESP_LOGI(TAG, "LVGL started");

    while (1){
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
        ESP_LOGI(TAG, "掌静脉状态更新 (靠近/移开等)...");
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
 */
void main_serve(void *arg){
    (void)arg;
    while (1){
        // 实时更新主页柜子状态显示
        if (lockers[0].is_locked == true){
            lv_obj_set_style_bg_color(guider_ui.main_locker1, lv_color_hex(0xFF0000), 0);
        }
        else{
            lv_obj_set_style_bg_color(guider_ui.main_locker1, lv_color_hex(0x00FF00), 0);
        }

        if (lockers[1].is_locked == true){
            lv_obj_set_style_bg_color(guider_ui.main_locker2, lv_color_hex(0xFF0000), 0);
        }
        else{
            lv_obj_set_style_bg_color(guider_ui.main_locker2, lv_color_hex(0x00FF00), 0);
        }

        if (lockers[2].is_locked == true){
            lv_obj_set_style_bg_color(guider_ui.main_locker3, lv_color_hex(0xFF0000), 0);
        }
        else{
            lv_obj_set_style_bg_color(guider_ui.main_locker3, lv_color_hex(0x00FF00), 0);
        }

        if (lockers[3].is_locked == true){
            lv_obj_set_style_bg_color(guider_ui.main_locker4, lv_color_hex(0xFF0000), 0);
        }
        else{
            lv_obj_set_style_bg_color(guider_ui.main_locker4, lv_color_hex(0x00FF00), 0);
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // 每秒检查一次
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

    buzzer_handle_t buzzer_handle = buzzer_get_handle(0);

    if (buzzer_handle != NULL){
        ESP_LOGI(TAG, "Buzzer device initialized successfully");
        buzzer_beep_pattern(buzzer_handle, BUZZER_BEEP_SHORT);
    }

    // 初始化 WiFi
    wifi_init_softap();
    // 初始化 TCP 服务
    xTaskCreatePinnedToCore(tcp_server_task, "tcp_server", 4096, NULL, 5, NULL, 1);

    /* 创建 LVGL 任务 */
    xTaskCreatePinnedToCore(
        lvgl_demo_task,
        "lvgl_demo_task",
        10240,
        NULL,
        5, /* 优先级 */
        NULL,
        tskNO_AFFINITY /* 运行在核心0上 */
    );
    ESP_LOGI(TAG, "App initialized successfully");

    xTaskCreatePinnedToCore(main_serve, "main_serve", 4096, NULL, 5, NULL, 1);
}
