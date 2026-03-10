#include "display.h"
#include "esp_lcd_io_spi.h"
#include "esp_lcd_panel_dev.h"
#include "esp_lcd_panel_st7789.h"
#include "esp_lcd_panel_ops.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static const char *TAG = "DISPLAY";

static esp_lcd_panel_io_handle_t io_handle  = NULL;
static esp_lcd_panel_handle_t   panel_handle = NULL;
static ledc_channel_config_t    ledc_channel;

/* ── 信号量：仅供 display_test() 等非 LVGL 路径阻塞等待使用 ── */
static SemaphoreHandle_t lcd_flush_sem = NULL;

/* ── LVGL 异步通知回调：由 lv_port_disp 注册 ── */
static void (*s_flush_ready_cb)(void *arg) = NULL;
static void  *s_flush_ready_arg            = NULL;

void display_set_flush_ready_cb(void (*flush_ready_cb)(void *arg), void *arg)
{
    s_flush_ready_cb  = flush_ready_cb;
    s_flush_ready_arg = arg;
}

/*
 * DMA 传输完成中断回调。
 *
 * 优先路径（LVGL 已注册回调）：直接调用 flush_ready_cb，让 LVGL
 * 立即开始渲染下一块区域，DMA 传输与 CPU 渲染真正并行。
 *
 * 备用路径（display_test 等）：释放信号量，供阻塞等待方使用。
 */
static bool notify_lcd_flush_ready(esp_lcd_panel_io_handle_t panel_io,
                                   esp_lcd_panel_io_event_data_t *edata,
                                   void *user_ctx)
{
    if (s_flush_ready_cb) {
        /* 直接在 ISR 上下文中通知 LVGL —— lv_display_flush_ready() 内部
         * 仅设置一个标志位，在 FreeRTOS 下是 ISR 安全的。          */
        s_flush_ready_cb(s_flush_ready_arg);
        return false; /* 不需要任务切换 */
    }

    /* 备用路径：释放信号量给 display_wait_flush_done() */
    BaseType_t need_yield = pdFALSE;
    if (lcd_flush_sem) {
        xSemaphoreGiveFromISR(lcd_flush_sem, &need_yield);
    }
    return (need_yield == pdTRUE);
}

esp_err_t display_init(void)
{
    vTaskDelay(pdMS_TO_TICKS(100));
    esp_err_t ret = ESP_OK;

    /* 1. 创建信号量（备用路径使用） */
    lcd_flush_sem = xSemaphoreCreateBinary();

    /* 2. 初始化 SPI 总线 */
    spi_bus_config_t buscfg = {
        .sclk_io_num     = DISPLAY_SCL_PIN,
        .mosi_io_num     = DISPLAY_SDA_PIN,
        .miso_io_num     = DISPLAY_MISO_PIN,
        .quadwp_io_num   = -1,
        .quadhd_io_num   = -1,
        /* max_transfer_sz 需足够容纳一次最大 DMA 传输。
         * 全屏一次性传输可减少 SPI 事务开销，提高帧率。         */
        .max_transfer_sz = DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(uint16_t),
    };
    ret = spi_bus_initialize(DISPLAY_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) return ret;

    /* 3. 创建 LCD 面板 IO 句柄 */
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num          = DISPLAY_DC_PIN,
        .cs_gpio_num          = DISPLAY_CS_PIN,
        .pclk_hz              = DISPLAY_CLOCK_HZ,
        .lcd_cmd_bits         = 8,
        .lcd_param_bits       = 8,
        .spi_mode             = 0,
        /* trans_queue_depth > 1 让驱动可以排队多个 DMA 事务，
         * 避免 CPU 等待 SPI 总线空闲，进一步降低延迟。           */
        .trans_queue_depth    = 10,
        .on_color_trans_done  = notify_lcd_flush_ready,
    };
    ret = esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)DISPLAY_SPI_HOST,
                                   &io_config, &io_handle);
    if (ret != ESP_OK) return ret;

    /* 4. 创建 LCD 面板句柄 (ST7789) */
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num  = DISPLAY_RESET_PIN,
        .rgb_ele_order   = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel  = 16,
        .data_endian     = LCD_RGB_DATA_ENDIAN_LITTLE,
    };
    ret = esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle);
    if (ret != ESP_OK) return ret;

    /* 5. 硬件初始化序列 */
    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);
    esp_lcd_panel_invert_color(panel_handle, false);
    esp_lcd_panel_disp_on_off(panel_handle, true);

    ESP_LOGI(TAG, "ST7789 initialized on SPI2 @ %d MHz",
             DISPLAY_CLOCK_HZ / 1000000);
    return ESP_OK;
}

esp_err_t backlight_init(void)
{
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_13_BIT,
        .freq_hz         = 5000,
        .speed_mode      = LEDC_LOW_SPEED_MODE,
        .timer_num       = LEDC_TIMER_0,
        .clk_cfg         = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel.channel    = LEDC_CHANNEL_0;
    ledc_channel.duty       = 0;
    ledc_channel.gpio_num   = DISPLAY_BACKLIGHT_PIN;
    ledc_channel.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_channel.timer_sel  = LEDC_TIMER_0;
    ledc_channel_config(&ledc_channel);

    ESP_LOGI(TAG, "Backlight initialized on GPIO%d", DISPLAY_BACKLIGHT_PIN);
    return ESP_OK;
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
    /* 仅供非 LVGL 路径（如 display_test）使用 */
    if (lcd_flush_sem) {
        xSemaphoreTake(lcd_flush_sem, portMAX_DELAY);
    }
}

/* 屏幕驱动测试函数（不依赖 LVGL） */
void display_test(void)
{
    if (display_init()    != ESP_OK) { ESP_LOGE(TAG, "Display init failed!");   return; }
    if (backlight_init()  != ESP_OK) { ESP_LOGE(TAG, "Backlight init failed!"); return; }
    backlight_set(100);

    esp_lcd_panel_handle_t panel = display_get_panel_handle();
    if (!panel) { ESP_LOGE(TAG, "Panel handle is NULL!"); return; }

    uint16_t    colors[4]      = {0xF800, 0x07E0, 0x001F, 0xFFFF};
    const char *color_names[4] = {"RED", "GREEN", "BLUE", "WHITE"};

    for (int i = 0; i < 4; ++i) {
        ESP_LOGI(TAG, "Fill color: %s", color_names[i]);
        size_t    buf_size = DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(uint16_t);
        uint16_t *buf = heap_caps_malloc(buf_size, MALLOC_CAP_DMA);
        if (!buf) { ESP_LOGE(TAG, "DMA buffer alloc failed!"); return; }

        for (int p = 0; p < DISPLAY_WIDTH * DISPLAY_HEIGHT; ++p) buf[p] = colors[i];

        /* display_test 不走 LVGL，需要阻塞等待 DMA 完成 */
        esp_lcd_panel_draw_bitmap(panel, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, buf);
        display_wait_flush_done();

        vTaskDelay(pdMS_TO_TICKS(800));
        heap_caps_free(buf);
    }
    ESP_LOGI(TAG, "Display test finished.");
}
