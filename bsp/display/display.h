#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "esp_err.h"
#include "esp_lcd_types.h"

/* ============ 显示屏硬件配置 ============ */
#define DISPLAY_WIDTH       240
#define DISPLAY_HEIGHT      320

/* 根据提供的引脚图更新引脚 */
#define DISPLAY_SCL_PIN     12  // SCK
#define DISPLAY_SDA_PIN     11  // MOSI
#define DISPLAY_MISO_PIN    -1  // MISO 
#define DISPLAY_CS_PIN      10  // CS
#define DISPLAY_DC_PIN      9   // DC
#define DISPLAY_RESET_PIN   8   // RST
#define DISPLAY_BACKLIGHT_PIN 48 // BL

/* SPI 配置 */
#define DISPLAY_SPI_HOST    SPI2_HOST
// ESP32-S3 配合 ST7789 可以跑到 40MHz 甚至 80MHz，布线好可以直接上 80*1000*1000
#define DISPLAY_CLOCK_HZ    (5 * 1000 * 1000) 

/* ============ API接口 ============ */

/**
 * @brief 初始化LCD显示屏和SPI总线
 */
esp_err_t display_init(void);

/**
 * @brief 初始化背光(PWM)
 */
esp_err_t backlight_init(void);

/**
 * @brief 设置背光亮度
 * @param brightness 亮度值 0-100
 */
void backlight_set(uint8_t brightness);

/**
 * @brief 获取屏幕句柄 (后续给 LVGL 挂载使用)
 */
esp_lcd_panel_handle_t display_get_panel_handle(void);

/**
 * @brief 阻塞等待DMA传输完成 (LVGL flush_cb 会用到)
 */
void display_wait_flush_done(void);

/**
 * @brief 屏幕驱动测试函数，显示彩色块
 */
void display_test(void);

#endif // _DISPLAY_H_