#ifndef _TOUCH_H_
#define _TOUCH_H_

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

/* ============ 触摸屏引脚配置 ============ */
#define TOUCH_I2C_SDA_PIN   1
#define TOUCH_I2C_SCL_PIN   2
#define TOUCH_INT_PIN       14


#define TOUCH_I2C_FREQ_HZ   (400 * 1000) // FT6336支持400kHz高速I2C
#define FT6336U_I2C_ADDR    0x38

/* 触摸数据结构 (专为对接 LVGL 设计) */
typedef struct {
    uint16_t x;          
    uint16_t y;          
    bool is_pressed;     // 是否被按下
} touch_point_t;

/**
 * @brief 初始化 FT6336U 触摸屏驱动 (基于 V5 I2C Master API)
 */
esp_err_t touch_init(void);

/**
 * @brief 读取触摸坐标 (供 LVGL indev_read_cb 调用)
 * @param point 用于带回 X, Y 和状态
 */
void touch_read_point(touch_point_t *point);

#endif // _TOUCH_H_