/**
 * @file      buzzer.c
 * @brief     Buzzer Driver Implementation with Device Abstraction, State Machine,
 *            Handle Management, External Registration, and OPS Interface
 * @author    Locker Team
 * @version   1.0
 * @date      2026-04-17
 * 
 * @copyright Copyright (c) 2026 All rights reserved.
 * 
 * @note      Implements:
 *            - Multi-device registry and management
 *            - GPIO-based buzzer implementation
 *            - Beep pattern state machine
 *            - Async beep control via FreeRTOS timers
 *            - Thread-safe operations with mutexes
 */

/* Includes ------------------------------------------------------------------*/
#include "buzzer.h"
#include <string.h>
#include <esp_log.h>
#include <freertos/semphr.h>

/* Private macros ------------------------------------------------------------*/
#define BUZZER_TIMER_NAME_PREFIX "buzzer_"
#define BUZZER_PATTERN_CHECK_INTERVAL_MS 50

/* Private types -------------------------------------------------------------*/

/**
 * @brief GPIO buzzer device private data
 */
typedef struct {
    bool is_initialized;
    uint32_t on_duration_ms;   /* Current beep on duration */
    bool pattern_running;      /* Is beep pattern currently running */
} gpio_buzzer_private_t;

/* Private variables ---------------------------------------------------------*/

/**
 * @brief Global device registry
 */
static buzzer_device_t *g_buzzer_devices[BUZZER_MAX_DEVICES] = {NULL};
static SemaphoreHandle_t g_registry_mutex = NULL;

/* Private function prototypes -----------------------------------------------*/

/* GPIO Buzzer OPS Implementation */
static esp_err_t gpio_buzzer_init(buzzer_device_t *dev);
static esp_err_t gpio_buzzer_deinit(buzzer_device_t *dev);
static esp_err_t gpio_buzzer_turn_on(buzzer_device_t *dev);
static esp_err_t gpio_buzzer_turn_off(buzzer_device_t *dev);
static esp_err_t gpio_buzzer_beep_once(buzzer_device_t *dev, uint32_t duration_ms);
static esp_err_t gpio_buzzer_beep_pattern(buzzer_device_t *dev, buzzer_beep_type_t beep_type);
static esp_err_t gpio_buzzer_set_frequency(buzzer_device_t *dev, uint32_t frequency_hz);
static esp_err_t gpio_buzzer_set_volume(buzzer_device_t *dev, uint8_t volume_percent);
static buzzer_state_t gpio_buzzer_get_state(buzzer_device_t *dev);

/* Helper functions */
static void buzzer_beep_timer_callback(TimerHandle_t timer);
static esp_err_t buzzer_setup_beep_pattern(buzzer_device_t *dev, const uint32_t *pattern, uint8_t pattern_len);
static void buzzer_lock_device(buzzer_device_t *dev);
static void buzzer_unlock_device(buzzer_device_t *dev);

/* ========================================================================= */
/* GPIO BUZZER OPS IMPLEMENTATION */
/* ========================================================================= */

/**
 * @brief GPIO buzzer operations interface
 */
static const buzzer_ops_t gpio_buzzer_ops = {
    .init = gpio_buzzer_init,
    .deinit = gpio_buzzer_deinit,
    .turn_on = gpio_buzzer_turn_on,
    .turn_off = gpio_buzzer_turn_off,
    .beep_once = gpio_buzzer_beep_once,
    .beep_pattern = gpio_buzzer_beep_pattern,
    .set_frequency = gpio_buzzer_set_frequency,
    .set_volume = gpio_buzzer_set_volume,
    .get_state = gpio_buzzer_get_state,
};

/* ========================================================================= */
/* GPIO BUZZER OPS - IMPLEMENTATION */
/* ========================================================================= */

static esp_err_t gpio_buzzer_init(buzzer_device_t *dev)
{
    if (!dev || !dev->private_data) {
        ESP_LOGE(TAG, "Invalid device pointer in gpio_buzzer_init");
        return ESP_ERR_INVALID_ARG;
    }

    gpio_buzzer_private_t *priv = (gpio_buzzer_private_t *)dev->private_data;

    /* Configure GPIO */
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << dev->gpio_pin,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    esp_err_t ret = gpio_config(&io_conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "GPIO config failed for device %d: %s", dev->device_id, esp_err_to_name(ret));
        return ret;
    }

    /* Initialize GPIO to OFF */
    gpio_set_level(dev->gpio_pin, 0);

    /* Initialize device state */
    dev->state = BUZZER_STATE_IDLE;
    priv->is_initialized = true;
    priv->pattern_running = false;
    dev->current_volume = 100;
    dev->current_frequency = 0;

    /* Create FreeRTOS timer for beep control */
    char timer_name[32];
    snprintf(timer_name, sizeof(timer_name), "%s%d", BUZZER_TIMER_NAME_PREFIX, dev->device_id);

    dev->beep_timer = xTimerCreate(
        timer_name,
        pdMS_TO_TICKS(BUZZER_PATTERN_CHECK_INTERVAL_MS),
        pdFALSE,  /* Single shot */
        (void *)dev,
        buzzer_beep_timer_callback
    );

    if (!dev->beep_timer) {
        ESP_LOGE(TAG, "Failed to create beep timer for device %d", dev->device_id);
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(TAG, "GPIO buzzer device %d initialized (GPIO %d)", dev->device_id, dev->gpio_pin);
    return ESP_OK;
}

static esp_err_t gpio_buzzer_deinit(buzzer_device_t *dev)
{
    if (!dev || !dev->private_data) {
        return ESP_ERR_INVALID_ARG;
    }

    gpio_buzzer_private_t *priv = (gpio_buzzer_private_t *)dev->private_data;

    /* Stop timer if running */
    if (dev->beep_timer) {
        xTimerStop(dev->beep_timer, pdMS_TO_TICKS(100));
        xTimerDelete(dev->beep_timer, pdMS_TO_TICKS(100));
        dev->beep_timer = NULL;
    }

    /* Turn off GPIO */
    gpio_set_level(dev->gpio_pin, 0);
    dev->state = BUZZER_STATE_OFF;
    priv->is_initialized = false;

    ESP_LOGI(TAG, "GPIO buzzer device %d deinitialized", dev->device_id);
    return ESP_OK;
}

static esp_err_t gpio_buzzer_turn_on(buzzer_device_t *dev)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }

    buzzer_lock_device(dev);
    gpio_set_level(dev->gpio_pin, 1);
    dev->state = BUZZER_STATE_ON;
    buzzer_unlock_device(dev);

    ESP_LOGD(TAG, "Device %d turned ON", dev->device_id);
    return ESP_OK;
}

static esp_err_t gpio_buzzer_turn_off(buzzer_device_t *dev)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }

    buzzer_lock_device(dev);

    /* Stop any running timer */
    if (dev->beep_timer) {
        xTimerStop(dev->beep_timer, 0);
    }

    gpio_set_level(dev->gpio_pin, 0);
    dev->state = BUZZER_STATE_IDLE;
    buzzer_unlock_device(dev);

    ESP_LOGD(TAG, "Device %d turned OFF", dev->device_id);
    return ESP_OK;
}

static esp_err_t gpio_buzzer_beep_once(buzzer_device_t *dev, uint32_t duration_ms)
{
    if (!dev || !dev->private_data) {
        return ESP_ERR_INVALID_ARG;
    }

    gpio_buzzer_private_t *priv = (gpio_buzzer_private_t *)dev->private_data;

    buzzer_lock_device(dev);

    priv->on_duration_ms = duration_ms;
    priv->pattern_running = true;
    dev->state = BUZZER_STATE_BEEPING;

    /* Turn on immediately */
    gpio_set_level(dev->gpio_pin, 1);

    /* Schedule turn-off after duration */
    if (dev->beep_timer) {
        xTimerChangePeriod(dev->beep_timer, pdMS_TO_TICKS(duration_ms), 0);
    }

    buzzer_unlock_device(dev);

    ESP_LOGD(TAG, "Device %d beep_once: %lu ms", dev->device_id, duration_ms);
    return ESP_OK;
}

static esp_err_t gpio_buzzer_beep_pattern(buzzer_device_t *dev, buzzer_beep_type_t beep_type)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Define beep patterns (on/off times in ms) */
    const uint32_t short_pattern[] = {100, 50};
    const uint32_t long_pattern[] = {500};
    const uint32_t double_pattern[] = {100, 50, 100, 50};
    const uint32_t triple_pattern[] = {100, 50, 100, 50, 100, 50};
    const uint32_t error_pattern[] = {150, 100, 150, 50};
    const uint32_t success_pattern[] = {100, 50, 100, 50, 100, 50, 100, 50};

    const uint32_t *pattern = NULL;
    uint8_t pattern_len = 0;

    switch (beep_type) {
    case BUZZER_BEEP_SHORT:
        pattern = short_pattern;
        pattern_len = sizeof(short_pattern) / sizeof(short_pattern[0]);
        break;
    case BUZZER_BEEP_LONG:
        pattern = long_pattern;
        pattern_len = sizeof(long_pattern) / sizeof(long_pattern[0]);
        break;
    case BUZZER_BEEP_DOUBLE:
        pattern = double_pattern;
        pattern_len = sizeof(double_pattern) / sizeof(double_pattern[0]);
        break;
    case BUZZER_BEEP_TRIPLE:
        pattern = triple_pattern;
        pattern_len = sizeof(triple_pattern) / sizeof(triple_pattern[0]);
        break;
    case BUZZER_BEEP_ERROR:
        pattern = error_pattern;
        pattern_len = sizeof(error_pattern) / sizeof(error_pattern[0]);
        break;
    case BUZZER_BEEP_SUCCESS:
        pattern = success_pattern;
        pattern_len = sizeof(success_pattern) / sizeof(success_pattern[0]);
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }

    return buzzer_setup_beep_pattern(dev, pattern, pattern_len);
}

static esp_err_t gpio_buzzer_set_frequency(buzzer_device_t *dev, uint32_t frequency_hz)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }

    /* GPIO buzzer doesn't support frequency control */
    dev->current_frequency = frequency_hz;
    ESP_LOGW(TAG, "Device %d: GPIO buzzer does not support frequency control", dev->device_id);
    return ESP_OK;
}

static esp_err_t gpio_buzzer_set_volume(buzzer_device_t *dev, uint8_t volume_percent)
{
    if (!dev || volume_percent > 100) {
        return ESP_ERR_INVALID_ARG;
    }

    /* GPIO buzzer doesn't support volume control (binary on/off) */
    dev->current_volume = volume_percent;
    ESP_LOGW(TAG, "Device %d: GPIO buzzer does not support volume control", dev->device_id);
    return ESP_OK;
}

static buzzer_state_t gpio_buzzer_get_state(buzzer_device_t *dev)
{
    if (!dev) {
        return BUZZER_STATE_ERROR;
    }
    return dev->state;
}

/* ========================================================================= */
/* HELPER FUNCTIONS */
/* ========================================================================= */

static void buzzer_lock_device(buzzer_device_t *dev)
{
    if (dev && dev->mutex) {
        xSemaphoreTake((SemaphoreHandle_t)dev->mutex, portMAX_DELAY);
    }
}

static void buzzer_unlock_device(buzzer_device_t *dev)
{
    if (dev && dev->mutex) {
        xSemaphoreGive((SemaphoreHandle_t)dev->mutex);
    }
}

/**
 * @brief Timer callback for beep pattern management
 */
static void buzzer_beep_timer_callback(TimerHandle_t timer)
{
    buzzer_device_t *dev = (buzzer_device_t *)pvTimerGetTimerID(timer);
    if (!dev || !dev->private_data) {
        return;
    }

    gpio_buzzer_private_t *priv = (gpio_buzzer_private_t *)dev->private_data;

    buzzer_lock_device(dev);

    if (!priv->pattern_running) {
        buzzer_unlock_device(dev);
        return;
    }

    /* Move to next pattern step */
    dev->beep_pattern_idx++;

    if (dev->beep_pattern_idx >= dev->beep_pattern_len) {
        /* Pattern completed */
        gpio_set_level(dev->gpio_pin, 0);
        dev->state = BUZZER_STATE_IDLE;
        priv->pattern_running = false;

        /* Call completion callback if registered */
        if (dev->on_complete_cb) {
            buzzer_unlock_device(dev);
            dev->on_complete_cb(dev->callback_user_data);
            buzzer_lock_device(dev);
        }
    } else {
        /* Toggle GPIO based on pattern */
        uint32_t duration = dev->beep_pattern[dev->beep_pattern_idx];
        uint8_t gpio_level = dev->beep_pattern_idx % 2;  /* Even=ON, Odd=OFF */

        gpio_set_level(dev->gpio_pin, gpio_level);

        /* Schedule next step */
        xTimerChangePeriod(dev->beep_timer, pdMS_TO_TICKS(duration), 0);
    }

    buzzer_unlock_device(dev);
}

/**
 * @brief Setup and start beep pattern
 */
static esp_err_t buzzer_setup_beep_pattern(buzzer_device_t *dev, const uint32_t *pattern, uint8_t pattern_len)
{
    if (!dev || !dev->private_data || !pattern || pattern_len == 0 || pattern_len > BUZZER_BEEP_PATTERN_SIZE) {
        return ESP_ERR_INVALID_ARG;
    }

    gpio_buzzer_private_t *priv = (gpio_buzzer_private_t *)dev->private_data;

    buzzer_lock_device(dev);

    /* Copy pattern into device */
    memcpy(dev->beep_pattern, pattern, pattern_len * sizeof(uint32_t));
    dev->beep_pattern_len = pattern_len;
    dev->beep_pattern_idx = 0;
    priv->pattern_running = true;
    dev->state = BUZZER_STATE_BEEPING;

    /* Start with first duration (ON time) */
    uint32_t first_duration = pattern[0];
    gpio_set_level(dev->gpio_pin, 1);  /* Turn ON */

    if (dev->beep_timer) {
        xTimerChangePeriod(dev->beep_timer, pdMS_TO_TICKS(first_duration), 0);
    }

    buzzer_unlock_device(dev);

    ESP_LOGD(TAG, "Device %d beep_pattern: %d steps, first=%lu ms", dev->device_id, pattern_len, first_duration);
    return ESP_OK;
}

/* ========================================================================= */
/* REGISTRY & LIFECYCLE MANAGEMENT */
/* ========================================================================= */

static esp_err_t buzzer_init_registry(void)
{
    if (!g_registry_mutex) {
        g_registry_mutex = xSemaphoreCreateMutex();
        if (!g_registry_mutex) {
            ESP_LOGE(TAG, "Failed to create registry mutex");
            return ESP_ERR_NO_MEM;
        }
    }
    return ESP_OK;
}

/**
 * @brief Lock registry (thread-safe)
 */
static void buzzer_registry_lock(void)
{
    if (g_registry_mutex) {
        xSemaphoreTake(g_registry_mutex, portMAX_DELAY);
    }
}

/**
 * @brief Unlock registry
 */
static void buzzer_registry_unlock(void)
{
    if (g_registry_mutex) {
        xSemaphoreGive(g_registry_mutex);
    }
}

/* ========================================================================= */
/* PUBLIC API IMPLEMENTATION */
/* ========================================================================= */

esp_err_t buzzer_register_device(buzzer_device_t *dev)
{
    if (!dev || dev->device_id >= BUZZER_MAX_DEVICES) {
        ESP_LOGE(TAG, "Invalid device in register_device");
        return ESP_ERR_INVALID_ARG;
    }

    buzzer_init_registry();
    buzzer_registry_lock();

    if (g_buzzer_devices[dev->device_id] != NULL) {
        ESP_LOGW(TAG, "Device slot %d already occupied", dev->device_id);
        buzzer_registry_unlock();
        return ESP_ERR_INVALID_STATE;
    }

    g_buzzer_devices[dev->device_id] = dev;
    buzzer_registry_unlock();

    ESP_LOGI(TAG, "Device %d registered: %s", dev->device_id, dev->name);
    return ESP_OK;
}

esp_err_t buzzer_unregister_device(uint8_t device_id)
{
    if (device_id >= BUZZER_MAX_DEVICES) {
        return ESP_ERR_INVALID_ARG;
    }

    buzzer_registry_lock();

    if (g_buzzer_devices[device_id] == NULL) {
        buzzer_registry_unlock();
        return ESP_ERR_NOT_FOUND;
    }

    g_buzzer_devices[device_id] = NULL;
    buzzer_registry_unlock();

    ESP_LOGI(TAG, "Device %d unregistered", device_id);
    return ESP_OK;
}

buzzer_handle_t buzzer_get_handle(uint8_t device_id)
{
    if (device_id >= BUZZER_MAX_DEVICES) {
        return NULL;
    }

    buzzer_registry_lock();
    buzzer_device_t *dev = g_buzzer_devices[device_id];
    buzzer_registry_unlock();

    if (!dev) {
        return NULL;
    }

    /* Create and return opaque handle */
    buzzer_handle_t_impl *handle = malloc(sizeof(buzzer_handle_t_impl));
    if (!handle) {
        return NULL;
    }
    handle->device = dev;
    return handle;
}

esp_err_t buzzer_init(void)
{
    ESP_LOGI(TAG, "Initializing buzzer driver");

    buzzer_init_registry();
    buzzer_registry_lock();

    esp_err_t ret = ESP_OK;
    for (int i = 0; i < BUZZER_MAX_DEVICES; i++) {
        if (g_buzzer_devices[i] && g_buzzer_devices[i]->ops && g_buzzer_devices[i]->ops->init) {
            esp_err_t device_ret = g_buzzer_devices[i]->ops->init(g_buzzer_devices[i]);
            if (device_ret != ESP_OK) {
                ESP_LOGE(TAG, "Device %d init failed: %s", i, esp_err_to_name(device_ret));
                ret = ESP_FAIL;
            }
        }
    }

    buzzer_registry_unlock();
    return ret;
}

esp_err_t buzzer_deinit(void)
{
    ESP_LOGI(TAG, "Deinitializing buzzer driver");

    buzzer_registry_lock();

    esp_err_t ret = ESP_OK;
    for (int i = 0; i < BUZZER_MAX_DEVICES; i++) {
        if (g_buzzer_devices[i] && g_buzzer_devices[i]->ops && g_buzzer_devices[i]->ops->deinit) {
            esp_err_t device_ret = g_buzzer_devices[i]->ops->deinit(g_buzzer_devices[i]);
            if (device_ret != ESP_OK) {
                ESP_LOGE(TAG, "Device %d deinit failed: %s", i, esp_err_to_name(device_ret));
                ret = ESP_FAIL;
            }
        }
    }

    buzzer_registry_unlock();
    return ret;
}

/* ========================================================================= */
/* DEVICE CONTROL API */
/* ========================================================================= */

esp_err_t buzzer_on(buzzer_handle_t handle)
{
    if (!handle) {
        return ESP_ERR_INVALID_ARG;
    }

    buzzer_handle_t_impl *h = (buzzer_handle_t_impl *)handle;
    if (!h->device || !h->device->ops || !h->device->ops->turn_on) {
        return ESP_ERR_INVALID_STATE;
    }

    return h->device->ops->turn_on(h->device);
}

esp_err_t buzzer_off(buzzer_handle_t handle)
{
    if (!handle) {
        return ESP_ERR_INVALID_ARG;
    }

    buzzer_handle_t_impl *h = (buzzer_handle_t_impl *)handle;
    if (!h->device || !h->device->ops || !h->device->ops->turn_off) {
        return ESP_ERR_INVALID_STATE;
    }

    return h->device->ops->turn_off(h->device);
}

esp_err_t buzzer_beep_once(buzzer_handle_t handle, uint32_t duration_ms)
{
    if (!handle) {
        return ESP_ERR_INVALID_ARG;
    }

    buzzer_handle_t_impl *h = (buzzer_handle_t_impl *)handle;
    if (!h->device || !h->device->ops || !h->device->ops->beep_once) {
        return ESP_ERR_INVALID_STATE;
    }

    return h->device->ops->beep_once(h->device, duration_ms);
}

esp_err_t buzzer_beep_pattern(buzzer_handle_t handle, buzzer_beep_type_t beep_type)
{
    if (!handle) {
        return ESP_ERR_INVALID_ARG;
    }

    buzzer_handle_t_impl *h = (buzzer_handle_t_impl *)handle;
    if (!h->device || !h->device->ops || !h->device->ops->beep_pattern) {
        return ESP_ERR_INVALID_STATE;
    }

    return h->device->ops->beep_pattern(h->device, beep_type);
}

buzzer_state_t buzzer_get_state(buzzer_handle_t handle)
{
    if (!handle) {
        return BUZZER_STATE_ERROR;
    }

    buzzer_handle_t_impl *h = (buzzer_handle_t_impl *)handle;
    if (!h->device || !h->device->ops || !h->device->ops->get_state) {
        return BUZZER_STATE_ERROR;
    }

    return h->device->ops->get_state(h->device);
}

esp_err_t buzzer_set_frequency(buzzer_handle_t handle, uint32_t frequency_hz)
{
    if (!handle) {
        return ESP_ERR_INVALID_ARG;
    }

    buzzer_handle_t_impl *h = (buzzer_handle_t_impl *)handle;
    if (!h->device || !h->device->ops || !h->device->ops->set_frequency) {
        return ESP_ERR_INVALID_STATE;
    }

    return h->device->ops->set_frequency(h->device, frequency_hz);
}

esp_err_t buzzer_set_volume(buzzer_handle_t handle, uint8_t volume_percent)
{
    if (!handle || volume_percent > 100) {
        return ESP_ERR_INVALID_ARG;
    }

    buzzer_handle_t_impl *h = (buzzer_handle_t_impl *)handle;
    if (!h->device || !h->device->ops || !h->device->ops->set_volume) {
        return ESP_ERR_INVALID_STATE;
    }

    return h->device->ops->set_volume(h->device, volume_percent);
}

esp_err_t buzzer_set_callback(buzzer_handle_t handle, buzzer_callback_t callback, void *user_data)
{
    if (!handle) {
        return ESP_ERR_INVALID_ARG;
    }

    buzzer_handle_t_impl *h = (buzzer_handle_t_impl *)handle;
    if (!h->device) {
        return ESP_ERR_INVALID_STATE;
    }

    buzzer_lock_device(h->device);
    h->device->on_complete_cb = callback;
    h->device->callback_user_data = user_data;
    buzzer_unlock_device(h->device);

    return ESP_OK;
}

/* ========================================================================= */
/* PREDEFINED DEVICE FACTORY FUNCTIONS */
/* ========================================================================= */

buzzer_device_t *buzzer_create_gpio_device(uint8_t device_id, gpio_num_t gpio_pin, const char *device_name)
{
    if (device_id >= BUZZER_MAX_DEVICES) {
        ESP_LOGE(TAG, "Invalid device_id %d", device_id);
        return NULL;
    }

    /* Allocate device structure */
    buzzer_device_t *dev = malloc(sizeof(buzzer_device_t));
    if (!dev) {
        ESP_LOGE(TAG, "Failed to allocate device");
        return NULL;
    }

    memset(dev, 0, sizeof(buzzer_device_t));

    /* Allocate private data */
    gpio_buzzer_private_t *priv = malloc(sizeof(gpio_buzzer_private_t));
    if (!priv) {
        ESP_LOGE(TAG, "Failed to allocate private data");
        free(dev);
        return NULL;
    }

    memset(priv, 0, sizeof(gpio_buzzer_private_t));

    /* Create mutex for thread safety */
    SemaphoreHandle_t mutex = xSemaphoreCreateMutex();
    if (!mutex) {
        ESP_LOGE(TAG, "Failed to create device mutex");
        free(priv);
        free(dev);
        return NULL;
    }

    /* Initialize device structure */
    dev->device_id = device_id;
    dev->gpio_pin = gpio_pin;
    dev->name = device_name ? device_name : "gpio_buzzer";
    dev->ops = &gpio_buzzer_ops;
    dev->private_data = priv;
    dev->mutex = mutex;
    dev->state = BUZZER_STATE_IDLE;
    dev->current_volume = 100;
    dev->current_frequency = 0;
    dev->on_complete_cb = NULL;

    ESP_LOGI(TAG, "GPIO buzzer device %d created (GPIO %d)", device_id, gpio_pin);
    return dev;
}

esp_err_t buzzer_destroy_gpio_device(buzzer_device_t *dev)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Free resources */
    if (dev->beep_timer) {
        xTimerDelete(dev->beep_timer, 0);
    }

    if (dev->mutex) {
        vSemaphoreDelete((SemaphoreHandle_t)dev->mutex);
    }

    if (dev->private_data) {
        free(dev->private_data);
    }

    free(dev);
    return ESP_OK;
}

/* ========================================================================= */
/* DEBUG UTILITIES */
/* ========================================================================= */

const buzzer_device_t *buzzer_get_device_info(buzzer_handle_t handle)
{
    if (!handle) {
        return NULL;
    }

    buzzer_handle_t_impl *h = (buzzer_handle_t_impl *)handle;
    return h->device;
}

void buzzer_dump_devices(void)
{
    ESP_LOGI(TAG, "====== BUZZER DEVICE REGISTRY ======");

    buzzer_registry_lock();

    int device_count = 0;
    for (int i = 0; i < BUZZER_MAX_DEVICES; i++) {
        if (g_buzzer_devices[i]) {
            buzzer_device_t *dev = g_buzzer_devices[i];
            const char *state_str = "UNKNOWN";
            switch (dev->state) {
            case BUZZER_STATE_OFF:
                state_str = "OFF";
                break;
            case BUZZER_STATE_ON:
                state_str = "ON";
                break;
            case BUZZER_STATE_BEEPING:
                state_str = "BEEPING";
                break;
            case BUZZER_STATE_IDLE:
                state_str = "IDLE";
                break;
            case BUZZER_STATE_ERROR:
                state_str = "ERROR";
                break;
            }

            ESP_LOGI(TAG, "[%d] Name: %s, GPIO: %d, State: %s, Volume: %u%%",
                     i, dev->name, dev->gpio_pin, state_str, dev->current_volume);
            device_count++;
        }
    }

    if (device_count == 0) {
        ESP_LOGI(TAG, "No devices registered");
    }

    ESP_LOGI(TAG, "====================================");

    buzzer_registry_unlock();
}

