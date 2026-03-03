#include "touch.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "FT6336U";

static i2c_master_dev_handle_t touch_i2c_dev;

// 内部读取寄存器函数
static esp_err_t ft6336u_read_registers(uint8_t reg, uint8_t *data, size_t len)
{
    return i2c_master_transmit_receive(touch_i2c_dev, &reg, 1, data, len, -1);
}

esp_err_t touch_init(void)
{
    // 1. 初始化 V5 版本的 I2C Master 总线
    i2c_master_bus_config_t bus_config = {
        .i2c_port = -1, // 自动分配可用端口
        .sda_io_num = TOUCH_I2C_SDA_PIN,
        .scl_io_num = TOUCH_I2C_SCL_PIN,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));

    // 2. 将 FT6336U 设备挂载到总线
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = FT6336U_I2C_ADDR,
        .scl_speed_hz = TOUCH_I2C_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config, &touch_i2c_dev));

    // 3. 配置中断引脚 (LVGL通常使用轮询，但配置好INT引脚有助于后续做休眠唤醒)
    gpio_config_t int_io_conf = {
        .pin_bit_mask = (1ULL << TOUCH_INT_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&int_io_conf);

    // 4. 验证芯片 (读取 FT6336U 的 Chip ID, 寄存器地址 0xA8)
    uint8_t chip_id = 0;
    esp_err_t ret = ft6336u_read_registers(0xA8, &chip_id, 1);
    if (ret == ESP_OK && (chip_id == 0x11 || chip_id == 0x51)) {
        ESP_LOGI(TAG, "FT6336U Found! Chip ID: 0x%02x", chip_id);
    } else {
        ESP_LOGW(TAG, "FT6336U Not Found or wrong ID (0x%02x)", chip_id);
    }

    return ESP_OK;
}

void touch_read_point(touch_point_t *point)
{
    if (!point) return;
    point->is_pressed = false; // 默认未按下

    uint8_t data[4];
    // 读取 0x02 寄存器：获取触摸点数量
    uint8_t touch_pnt_cnt;
    if (ft6336u_read_registers(0x02, &touch_pnt_cnt, 1) != ESP_OK) return;
    
    touch_pnt_cnt &= 0x0F; // 低4位有效

    if (touch_pnt_cnt > 0) {
        // FT6336U 坐标寄存器：0x03(XH), 0x04(XL), 0x05(YH), 0x06(YL)
        if (ft6336u_read_registers(0x03, data, 4) == ESP_OK) {
            uint16_t x = ((data[0] & 0x0F) << 8) | data[1];
            uint16_t y = ((data[2] & 0x0F) << 8) | data[3];
            
            // 注意：具体需不需要交换X/Y或反转，取决于你外屏贴合的方向
            // 如果LVGL发现触摸方向反了，在这里修改即可：
            point->x = x; 
            point->y = y; 
            point->is_pressed = true;
        }
    }
}