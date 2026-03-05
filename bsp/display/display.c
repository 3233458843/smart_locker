#include "display.h"
#include "esp_lcd_io_spi.h"
#include "esp_lcd_panel_dev.h"
#include "esp_lcd_panel_st7789.h"
#include "esp_lcd_panel_ops.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static const char *TAG = "DISPLAY";

static esp_lcd_panel_io_handle_t io_handle = NULL;
static esp_lcd_panel_handle_t panel_handle = NULL;
static ledc_channel_config_t ledc_channel;

static SemaphoreHandle_t lcd_flush_sem = NULL;

/* LCD DMA 传输完成回调函数 */
static bool notify_lcd_flush_ready(esp_lcd_panel_io_handle_t panel_io, 
                                   esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{   
    BaseType_t need_yield = pdFALSE;
    if (lcd_flush_sem) {
        xSemaphoreGiveFromISR(lcd_flush_sem, &need_yield);
    }
    return (need_yield == pdTRUE);
}

esp_err_t display_init(void){
    vTaskDelay(pdMS_TO_TICKS(100));
    esp_err_t ret = ESP_OK;

    // 1. 创建同步信号量
    lcd_flush_sem = xSemaphoreCreateBinary();
    
    // 2. 初始化SPI总线
    spi_bus_config_t buscfg = {
        .sclk_io_num = DISPLAY_SCL_PIN,
        .mosi_io_num = DISPLAY_SDA_PIN,
        .miso_io_num = DISPLAY_MISO_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        // S3 有很大的SRAM，分配较大的DMA一次性传输大小以提高帧率
        .max_transfer_sz = DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(uint16_t), 
    };
    ret = spi_bus_initialize(DISPLAY_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) return ret;

    // 3. 创建LCD面板IO句柄
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = DISPLAY_DC_PIN,
        .cs_gpio_num = DISPLAY_CS_PIN,
        .pclk_hz = DISPLAY_CLOCK_HZ,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .spi_mode = 0,
        .trans_queue_depth = 10,
        .on_color_trans_done = notify_lcd_flush_ready, // 绑定回调
    };
    ret = esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)DISPLAY_SPI_HOST, &io_config, &io_handle);
    if (ret != ESP_OK) return ret;

    // 4. 创建LCD面板句柄(ST7789)
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = DISPLAY_RESET_PIN,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB, // 视屏幕偏光片而定，如果偏色改为 BGR
        .bits_per_pixel = 16,
        .data_endian = LCD_RGB_DATA_ENDIAN_LITTLE,     // ST7789 默认通常是大端模式
    };
    ret = esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle);
    if (ret != ESP_OK) return ret;

    // 5. 屏幕硬件初始化序列
    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);
    
    // ST7789 的 IPS 屏通常需要反色处理
    esp_lcd_panel_invert_color(panel_handle,false );
    
    // 打开显示
    esp_lcd_panel_disp_on_off(panel_handle, true);

    ESP_LOGI(TAG, "ST7789 Display initialized successfully on SPI2");
    return ESP_OK;
}

esp_err_t backlight_init(void)
{
    // 配置LEDC定时器
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_13_BIT,
        .freq_hz = 5000,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&ledc_timer);

    // 配置LEDC通道
    ledc_channel.channel = LEDC_CHANNEL_0;
    ledc_channel.duty = 0;
    ledc_channel.gpio_num = DISPLAY_BACKLIGHT_PIN;
    ledc_channel.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_channel.timer_sel = LEDC_TIMER_0;
    ledc_channel_config(&ledc_channel);

    ESP_LOGI(TAG, "Backlight initialized on GPIO%d", DISPLAY_BACKLIGHT_PIN);
    return ESP_OK;

    // // 退回到最简单的 GPIO 控制，不使用 LEDC(PWM)
    // gpio_reset_pin(DISPLAY_BACKLIGHT_PIN);
    // gpio_set_direction(DISPLAY_BACKLIGHT_PIN, GPIO_MODE_OUTPUT);
    
    // // 尝试拉高点亮背光。如果还是死黑，请把这里的 1 改成 0 再试一次！
    // gpio_set_level(DISPLAY_BACKLIGHT_PIN, 1); 
    
    // ESP_LOGI("DISPLAY", "Backlight initialized on GPIO%d (Simple GPIO Mode)", DISPLAY_BACKLIGHT_PIN);
    // return ESP_OK;
}

void backlight_set(uint8_t brightness)
{
    if (brightness > 100) brightness = 100;
    uint32_t duty = ((1 << LEDC_TIMER_13_BIT) - 1) * brightness / 100;
    ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, duty);
    ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
}

esp_lcd_panel_handle_t display_get_panel_handle(void)
{
    return panel_handle;
}

void display_wait_flush_done(void)
{
    if (lcd_flush_sem) {
        xSemaphoreTake(lcd_flush_sem, portMAX_DELAY);
    }
}

// 屏幕驱动测试函数
void display_test(void)
{
    // 初始化屏幕和背光
    if (display_init() != ESP_OK) {
        ESP_LOGE(TAG, "Display init failed!");
        return;
    }
    if (backlight_init() != ESP_OK) {
        ESP_LOGE(TAG, "Backlight init failed!");
        return;
    }
    backlight_set(100); // 亮度最大

    esp_lcd_panel_handle_t panel = display_get_panel_handle();
    if (!panel) {
        ESP_LOGE(TAG, "Panel handle is NULL!");
        return;
    }

    // 定义颜色
    uint16_t colors[4] = {0xF800, 0x07E0, 0x001F, 0xFFFF}; // 红、绿、蓝、白
    const char *color_names[4] = {"RED", "GREEN", "BLUE", "WHITE"};

    for (int i = 0; i < 4; ++i) {
        ESP_LOGI(TAG, "Fill color: %s", color_names[i]);
        // 填充整个屏幕
        size_t buf_size = DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(uint16_t);
        uint16_t *buf = heap_caps_malloc(buf_size, MALLOC_CAP_DMA);
        if (!buf) {
            ESP_LOGE(TAG, "DMA buffer alloc failed!");
            return;
        }
        for (int p = 0; p < DISPLAY_WIDTH * DISPLAY_HEIGHT; ++p) {
            buf[p] = colors[i];
        }
        esp_lcd_panel_draw_bitmap(panel, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, buf);
        display_wait_flush_done();
        vTaskDelay(pdMS_TO_TICKS(800));
        heap_caps_free(buf);
    }
    ESP_LOGI(TAG, "Display test finished.");
}