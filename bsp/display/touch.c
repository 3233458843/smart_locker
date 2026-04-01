#include "touch.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "FT6336U";

#define TOUCH_PROBE_TIMEOUT_MS           20
#define TOUCH_RW_TIMEOUT_MS              20
#define TOUCH_MAX_CONSECUTIVE_RW_ERRORS 8

static i2c_master_bus_handle_t s_touch_i2c_bus = NULL;
static i2c_master_dev_handle_t s_touch_i2c_dev = NULL;
static bool s_touch_ready = false;
static uint8_t s_consecutive_rw_errors = 0;

static void touch_cleanup_resources(void)
{
    s_touch_ready = false;
    s_consecutive_rw_errors = 0;

    if (s_touch_i2c_dev) {
        i2c_master_bus_rm_device(s_touch_i2c_dev);
        s_touch_i2c_dev = NULL;
    }

    if (s_touch_i2c_bus) {
        i2c_del_master_bus(s_touch_i2c_bus);
        s_touch_i2c_bus = NULL;
    }
}

static void touch_disable(const char *reason)
{
    s_touch_ready = false;
    if (reason) {
        ESP_LOGW(TAG, "%s", reason);
    }
}

// 内部读取寄存器函数
static esp_err_t ft6336u_read_registers(uint8_t reg, uint8_t *data, size_t len)
{
    if (!s_touch_ready || s_touch_i2c_dev == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret = i2c_master_transmit_receive(s_touch_i2c_dev, &reg, 1, data, len, TOUCH_RW_TIMEOUT_MS);
    if (ret == ESP_OK) {
        s_consecutive_rw_errors = 0;
        return ESP_OK;
    }

    if (s_consecutive_rw_errors < 0xFF) {
        s_consecutive_rw_errors++;
    }
    if (s_consecutive_rw_errors >= TOUCH_MAX_CONSECUTIVE_RW_ERRORS) {
        touch_disable("Too many I2C errors while polling touch, touch input disabled");
    }
    return ret;
}

esp_err_t touch_init(void)
{
    if (s_touch_ready) {
        return ESP_OK;
    }

    // 1. 初始化 V5 版本的 I2C Master 总线
    i2c_master_bus_config_t bus_config = {
        .i2c_port = -1, // 自动分配可用端口
        .sda_io_num = TOUCH_I2C_SDA_PIN,
        .scl_io_num = TOUCH_I2C_SCL_PIN,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    esp_err_t ret = i2c_new_master_bus(&bus_config, &s_touch_i2c_bus);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create I2C bus: %s", esp_err_to_name(ret));
        touch_cleanup_resources();
        return ret;
    }

    ret = i2c_master_probe(s_touch_i2c_bus, FT6336U_I2C_ADDR, TOUCH_PROBE_TIMEOUT_MS);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Touch controller not responding at I2C address 0x%02X", FT6336U_I2C_ADDR);
        touch_cleanup_resources();
        return ESP_ERR_NOT_FOUND;
    }

    // 2. 将 FT6336U 设备挂载到总线
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = FT6336U_I2C_ADDR,
        .scl_speed_hz = TOUCH_I2C_FREQ_HZ,
    };
    ret = i2c_master_bus_add_device(s_touch_i2c_bus, &dev_config, &s_touch_i2c_dev);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add touch device: %s", esp_err_to_name(ret));
        touch_cleanup_resources();
        return ret;
    }

    // 3. 配置中断引脚 (LVGL通常使用轮询，但配置好INT引脚有助于后续做休眠唤醒)
    gpio_config_t int_io_conf = {
        .pin_bit_mask = (1ULL << TOUCH_INT_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&int_io_conf);

    // 4. 验证芯片 (读取 FT6336U 的 Chip ID, 寄存器地址 0xA8)
    uint8_t chip_id = 0;
    s_touch_ready = true;
    s_consecutive_rw_errors = 0;

    ret = ft6336u_read_registers(0xA8, &chip_id, 1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read touch chip ID: %s", esp_err_to_name(ret));
        touch_cleanup_resources();
        return ESP_ERR_NOT_FOUND;
    }

    if (chip_id == 0x11 || chip_id == 0x51) {
        ESP_LOGI(TAG, "FT6336U Found! Chip ID: 0x%02x", chip_id);
    } else {
        ESP_LOGW(TAG, "Unexpected chip ID 0x%02x at 0x%02x, continue with FT6336U-compatible polling",
                 chip_id, FT6336U_I2C_ADDR);
    }

    return ESP_OK;
}

void touch_read_point(touch_point_t *point)
{
    if (!point) return;
    point->is_pressed = false; // 默认未按下

    if (!s_touch_ready) {
        return;
    }

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
