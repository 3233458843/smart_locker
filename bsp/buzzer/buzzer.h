/**
 * @file      buzzer.h
 * @brief     Buzzer Driver with Device Abstraction, State Machine, Handle Management,
 *            External Registration, and OPS Interface
 * @author    Locker Team
 * @version   1.0
 * @date      2026-04-17
 * 
 * @copyright Copyright (c) 2026 All rights reserved.
 * 
 * @note      Provides a complete buzzer driver framework with:
 *            - Device abstraction through ops interface
 *            - Internal state machine for beep control
 *            - Handle-based resource management
 *            - Multi-device registration support
 *            - Inheritance through base device structure
 */

#ifndef _BUZZER_H
#define _BUZZER_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <esp_err.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>

/* C++ ------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/* Exported macros -----------------------------------------------------------*/
#define TAG "BUZZER"
#define BUZZER_MAX_DEVICES 4
#define BUZZER_BEEP_PATTERN_SIZE 16

/* ========================================================================= */
/* EXPORTED TYPES - Device State & Configuration */
/* ========================================================================= */

/**
 * @brief Buzzer device states
 */
typedef enum {
    BUZZER_STATE_OFF = 0,      /*!< Buzzer is off */
    BUZZER_STATE_ON,           /*!< Buzzer is on continuously */
    BUZZER_STATE_BEEPING,      /*!< Buzzer is in beep pattern mode */
    BUZZER_STATE_IDLE,         /*!< Buzzer is idle (ready to use) */
    BUZZER_STATE_ERROR,        /*!< Buzzer encountered an error */
} buzzer_state_t;

/**
 * @brief Buzzer beep types (predefined patterns)
 */
typedef enum {
    BUZZER_BEEP_SHORT = 0,     /*!< Short beep (100ms on, 50ms off) */
    BUZZER_BEEP_LONG,          /*!< Long beep (500ms on) */
    BUZZER_BEEP_DOUBLE,        /*!< Double beep (100ms on, 50ms off, 100ms on) */
    BUZZER_BEEP_TRIPLE,        /*!< Triple beep (100ms on, 50ms off, 100ms on, 50ms off, 100ms on) */
    BUZZER_BEEP_ERROR,         /*!< Error tone (150ms on, 100ms off, 150ms on) */
    BUZZER_BEEP_SUCCESS,       /*!< Success tone (100ms on, 50ms off, 100ms on, 50ms off, 100ms on, 50ms off, 100ms on) */
} buzzer_beep_type_t;

/**
 * @brief Buzzer notification callback function type
 */
typedef void (*buzzer_callback_t)(void *user_data);

/* ========================================================================= */
/* FORWARD DECLARATIONS */
/* ========================================================================= */

typedef struct buzzer_device buzzer_device_t;
typedef struct buzzer_handle_s *buzzer_handle_t;

/* ========================================================================= */
/* DEVICE OPS INTERFACE - Abstract base operations */
/* ========================================================================= */

/**
 * @brief Buzzer device operations interface (inheritance-like structure)
 *
 * This structure defines the operations that a buzzer device must support.
 * Different hardware implementations can provide their own ops implementations.
 */
typedef struct {
    /* Lifecycle operations */
    esp_err_t (*init)(buzzer_device_t *dev);
    esp_err_t (*deinit)(buzzer_device_t *dev);

    /* Basic control operations */
    esp_err_t (*turn_on)(buzzer_device_t *dev);
    esp_err_t (*turn_off)(buzzer_device_t *dev);

    /* Beep pattern operations */
    esp_err_t (*beep_once)(buzzer_device_t *dev, uint32_t duration_ms);
    esp_err_t (*beep_pattern)(buzzer_device_t *dev, buzzer_beep_type_t beep_type);

    /* Frequency/volume control (optional, device-specific) */
    esp_err_t (*set_frequency)(buzzer_device_t *dev, uint32_t frequency_hz);
    esp_err_t (*set_volume)(buzzer_device_t *dev, uint8_t volume_percent);

    /* State query operations */
    buzzer_state_t (*get_state)(buzzer_device_t *dev);

} buzzer_ops_t;

/* ========================================================================= */
/* DEVICE STRUCTURE - Base device abstraction */
/* ========================================================================= */

/**
 * @brief Buzzer device base structure (abstract device)
 *
 * This structure encapsulates the device abstraction layer and can be
 * inherited by specific buzzer implementations (e.g., GPIO buzzer, PWM buzzer).
 */
typedef struct buzzer_device {
    /* Base device info */
    const char *name;                      /*!< Device name/identifier */
    uint8_t device_id;                     /*!< Device index (0-BUZZER_MAX_DEVICES-1) */

    /* Hardware configuration */
    gpio_num_t gpio_pin;                   /*!< GPIO pin for buzzer control */

    /* State machine */
    buzzer_state_t state;                  /*!< Current device state */

    /* Operations (inheritance) */
    const buzzer_ops_t *ops;               /*!< Operations interface (function pointers) */

    /* Internal state */
    uint32_t current_frequency;            /*!< Current PWM frequency (if applicable) */
    uint8_t current_volume;                /*!< Current volume level (0-100%) */

    /* Beep pattern state */
    uint32_t beep_pattern[BUZZER_BEEP_PATTERN_SIZE];  /*!< On/off pattern in ms */
    uint8_t beep_pattern_len;              /*!< Pattern length */
    uint8_t beep_pattern_idx;              /*!< Current pattern index */

    /* Timer for async operations */
    TimerHandle_t beep_timer;              /*!< FreeRTOS timer for beep patterns */

    /* Callback for notifications */
    buzzer_callback_t on_complete_cb;      /*!< Callback when beep completes */
    void *callback_user_data;              /*!< User data for callback */

    /* Private device data (for implementation-specific data) */
    void *private_data;                    /*!< Device-specific private data pointer */

    /* Mutex for thread-safe operations */
    void *mutex;                           /*!< FreeRTOS mutex handle */

} buzzer_device_t;

/* ========================================================================= */
/* HANDLE & REGISTRY */
/* ========================================================================= */

/**
 * @brief Opaque handle structure for buzzer device (private)
 *
 * Applications use this opaque handle to interact with buzzer devices.
 * The actual implementation is hidden in the C file.
 */
typedef struct buzzer_handle_s {
    buzzer_device_t *device;               /*!< Pointer to actual device */
} buzzer_handle_t_impl;

/* ========================================================================= */
/* PUBLIC API FUNCTIONS */
/* ========================================================================= */

/**
 * @brief Register a new buzzer device with the driver
 *
 * @param dev Pointer to buzzer_device_t structure (pre-configured)
 * @return
 *   - ESP_OK: Successfully registered
 *   - ESP_ERR_INVALID_ARG: Invalid arguments
 *   - ESP_ERR_NO_MEM: No space in device registry
 */
esp_err_t buzzer_register_device(buzzer_device_t *dev);

/**
 * @brief Unregister a buzzer device
 *
 * @param device_id Device ID to unregister
 * @return
 *   - ESP_OK: Successfully unregistered
 *   - ESP_ERR_NOT_FOUND: Device not found
 */
esp_err_t buzzer_unregister_device(uint8_t device_id);

/**
 * @brief Get a handle to a registered buzzer device
 *
 * @param device_id Device ID (0-BUZZER_MAX_DEVICES-1)
 * @return
 *   - Valid handle on success
 *   - NULL if device not found
 */
buzzer_handle_t buzzer_get_handle(uint8_t device_id);

/**
 * @brief Initialize the buzzer driver and all registered devices
 *
 * @return
 *   - ESP_OK: All devices initialized successfully
 *   - ESP_FAIL: One or more devices failed to initialize
 */
esp_err_t buzzer_init(void);

/**
 * @brief Deinitialize the buzzer driver and all registered devices
 *
 * @return
 *   - ESP_OK: All devices deinitialized successfully
 *   - ESP_FAIL: One or more devices failed to deinitialize
 */
esp_err_t buzzer_deinit(void);

/* ========================================================================= */
/* DEVICE CONTROL API */
/* ========================================================================= */

/**
 * @brief Turn on buzzer (continuous sound)
 *
 * @param handle Device handle
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t buzzer_on(buzzer_handle_t handle);

/**
 * @brief Turn off buzzer
 *
 * @param handle Device handle
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t buzzer_off(buzzer_handle_t handle);

/**
 * @brief Single beep with specified duration
 *
 * @param handle Device handle
 * @param duration_ms Duration in milliseconds
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t buzzer_beep_once(buzzer_handle_t handle, uint32_t duration_ms);

/**
 * @brief Play predefined beep pattern
 *
 * @param handle Device handle
 * @param beep_type Type of beep pattern
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t buzzer_beep_pattern(buzzer_handle_t handle, buzzer_beep_type_t beep_type);

/**
 * @brief Get current buzzer state
 *
 * @param handle Device handle
 * @return Current state enum
 */
buzzer_state_t buzzer_get_state(buzzer_handle_t handle);

/**
 * @brief Set frequency (for PWM-based buzzers)
 *
 * @param handle Device handle
 * @param frequency_hz Frequency in Hz
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t buzzer_set_frequency(buzzer_handle_t handle, uint32_t frequency_hz);

/**
 * @brief Set volume level (for PWM-based buzzers)
 *
 * @param handle Device handle
 * @param volume_percent Volume in percentage (0-100)
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t buzzer_set_volume(buzzer_handle_t handle, uint8_t volume_percent);

/**
 * @brief Register callback for beep completion notification
 *
 * @param handle Device handle
 * @param callback Callback function pointer
 * @param user_data User data to pass to callback
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t buzzer_set_callback(buzzer_handle_t handle, buzzer_callback_t callback, void *user_data);

/* ========================================================================= */
/* PREDEFINED DEVICE FACTORY FUNCTIONS */
/* ========================================================================= */

/**
 * @brief Create a GPIO-based buzzer device (simple on/off)
 *
 * This function creates a basic GPIO buzzer device that supports
 * simple on/off control and beep patterns via GPIO toggling.
 *
 * @param device_id Device ID to assign
 * @param gpio_pin GPIO pin number
 * @param device_name Device name string
 * @return Pointer to initialized buzzer_device_t, or NULL on error
 *
 * @note The returned device must be registered using buzzer_register_device()
 */
buzzer_device_t *buzzer_create_gpio_device(uint8_t device_id, gpio_num_t gpio_pin, const char *device_name);

/**
 * @brief Destroy a GPIO buzzer device (free resources)
 *
 * @param dev Pointer to device created by buzzer_create_gpio_device()
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t buzzer_destroy_gpio_device(buzzer_device_t *dev);

/* ========================================================================= */
/* STATE QUERY & DEBUG UTILITIES */
/* ========================================================================= */

/**
 * @brief Get device information for debugging
 *
 * @param handle Device handle
 * @return Pointer to device structure (read-only), or NULL if handle is invalid
 */
const buzzer_device_t *buzzer_get_device_info(buzzer_handle_t handle);

/**
 * @brief Print all registered devices (debug utility)
 */
void buzzer_dump_devices(void);

/* C++  ------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif /*_BUZZER_H */