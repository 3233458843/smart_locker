#include "network.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "lwip/sockets.h"
#include <string.h>

#include "driver/uart.h"
#include "xst.h"

#define TAG "NETWORK"

void wifi_init_softap(void){
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = "ESP32_DEBUG_WIFI",
            .ssid_len = strlen("ESP32_DEBUG_WIFI"),
            .password = "12345678",
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "WiFi AP Started. SSID:%s password:%s", "ESP32_DEBUG_WIFI", "12345678");
}

#define TCP_PORT 8080

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
        g_vofa_client_fd = sock;

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 50000;
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

        while (1){
            int len = recv(sock, rx_buffer, sizeof(rx_buffer), 0);
            if (len <= 0) break;

            uart_write_bytes(XST_UART_NUM, rx_buffer, len);
            ESP_LOGI(TAG, "Forwarded %d bytes from phone to UART", len);
        }

        ESP_LOGW(TAG, "Phone Disconnected");
        close(sock);
        g_vofa_client_fd = -1;
    }
}
