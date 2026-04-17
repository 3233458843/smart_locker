# Buzzer Driver Module

## Overview

The Buzzer Driver (`bsp/buzzer/`) provides a comprehensive framework for managing buzzer devices in the ESP32-S3 Smart Locker system. It implements a professional-grade driver architecture with:

- **External Device Registration** - Dynamic device management (up to 4 devices)
- **Handle-Based Access** - Opaque pointer pattern for resource encapsulation
- **Internal State Machine** - Finite state machine with 5 states
- **Device Abstraction** - OPS interface for hardware abstraction
- **Thread Safety** - FreeRTOS mutex protection for concurrent access
- **Async Control** - FreeRTOS Timer-based beep pattern automation

---

## Key Features

### 1. External Device Registration
```c
// Create device
buzzer_device_t *dev = buzzer_create_gpio_device(0, GPIO_NUM_46, "buzzer0");

// Register to driver
buzzer_register_device(dev);

// Initialize all registered devices
buzzer_init();
```

### 2. Handle-Based API
```c
// Get handle to device
buzzer_handle_t handle = buzzer_get_handle(0);

// All operations use handle
buzzer_beep_pattern(handle, BUZZER_BEEP_SUCCESS);
buzzer_get_state(handle);
```

### 3. Predefined Beep Patterns
- `BUZZER_BEEP_SHORT` - 100ms beep
- `BUZZER_BEEP_LONG` - 500ms beep
- `BUZZER_BEEP_DOUBLE` - Double beep
- `BUZZER_BEEP_TRIPLE` - Triple beep
- `BUZZER_BEEP_ERROR` - Error tone
- `BUZZER_BEEP_SUCCESS` - Success tone

### 4. State Machine
```
OFF → IDLE → ON/BEEPING → IDLE/OFF
  ↓
ERROR
```

States: OFF, ON, BEEPING, IDLE, ERROR

### 5. Async Beep Control
Uses FreeRTOS Timer to automatically manage beep patterns without blocking the application.

### 6. Thread-Safe Operations
- Global registry protected by mutex
- Each device protected by individual mutex
- Safe concurrent access from multiple tasks

---

## File Structure

```
bsp/buzzer/
├── buzzer.h                      # API declarations and type definitions
├── buzzer.c                      # Driver implementation (~850 lines)
├── CMakeLists.txt               # Build configuration
├── README.md                    # This file
├── BUZZER_ARCHITECTURE.md       # Detailed architecture design
└── BUZZER_USAGE_EXAMPLE.md      # Usage examples and integration guide
```

---

## Hardware Configuration

### GPIO-Based Buzzer
- **Pin**: GPIO46 (recommended) or any GPIO
- **Logic**: High = buzzer ON, Low = buzzer OFF
- **Power**: Direct GPIO control (max 40mA at 3.3V)

### Typical Connection
```
ESP32-S3 GPIO46 → Buzzer+ (via current-limiting resistor)
ESP32-S3 GND    → Buzzer-
```

---

## API Quick Reference

### Initialization
```c
esp_err_t buzzer_init(void);
esp_err_t buzzer_deinit(void);
```

### Device Management
```c
esp_err_t buzzer_register_device(buzzer_device_t *dev);
esp_err_t buzzer_unregister_device(uint8_t device_id);
buzzer_handle_t buzzer_get_handle(uint8_t device_id);
```

### Control
```c
esp_err_t buzzer_on(buzzer_handle_t handle);
esp_err_t buzzer_off(buzzer_handle_t handle);
esp_err_t buzzer_beep_once(buzzer_handle_t handle, uint32_t duration_ms);
esp_err_t buzzer_beep_pattern(buzzer_handle_t handle, buzzer_beep_type_t beep_type);
```

### Configuration
```c
esp_err_t buzzer_set_frequency(buzzer_handle_t handle, uint32_t frequency_hz);
esp_err_t buzzer_set_volume(buzzer_handle_t handle, uint8_t volume_percent);
esp_err_t buzzer_set_callback(buzzer_handle_t handle, buzzer_callback_t callback, void *user_data);
```

### Query
```c
buzzer_state_t buzzer_get_state(buzzer_handle_t handle);
const buzzer_device_t *buzzer_get_device_info(buzzer_handle_t handle);
void buzzer_dump_devices(void);
```

### Factory Functions
```c
buzzer_device_t *buzzer_create_gpio_device(uint8_t device_id, gpio_num_t gpio_pin, const char *device_name);
esp_err_t buzzer_destroy_gpio_device(buzzer_device_t *dev);
```

---

## Integration Steps

### Step 1: Add Include
```c
#include "buzzer.h"
```

### Step 2: Create Device
```c
buzzer_device_t *buzzer_dev = buzzer_create_gpio_device(0, GPIO_NUM_46, "buzzer");
```

### Step 3: Register Device
```c
buzzer_register_device(buzzer_dev);
```

### Step 4: Initialize Driver
```c
buzzer_init();
```

### Step 5: Use in Application
```c
buzzer_handle_t handle = buzzer_get_handle(0);
buzzer_beep_pattern(handle, BUZZER_BEEP_SUCCESS);
```

---

## Typical Use Cases

### 1. User Authentication Success
```c
void on_authentication_success(void) {
    buzzer_handle_t h = buzzer_get_handle(0);
    buzzer_beep_pattern(h, BUZZER_BEEP_SUCCESS);
}
```

### 2. User Authentication Failure
```c
void on_authentication_failed(void) {
    buzzer_handle_t h = buzzer_get_handle(0);
    buzzer_beep_pattern(h, BUZZER_BEEP_ERROR);
}
```

### 3. Button Click Feedback
```c
void on_button_pressed(void) {
    buzzer_handle_t h = buzzer_get_handle(0);
    buzzer_beep_once(h, 50);  // 50ms beep
}
```

### 4. Item Saved Confirmation
```c
void on_item_saved(void) {
    buzzer_handle_t h = buzzer_get_handle(0);
    buzzer_beep_pattern(h, BUZZER_BEEP_DOUBLE);
}
```

### 5. System Alert
```c
void alert_user(void) {
    buzzer_handle_t h = buzzer_get_handle(0);
    buzzer_beep_pattern(h, BUZZER_BEEP_TRIPLE);
}
```

---

## Architecture Highlights

### Layered Design
```
┌─ Application Layer ─┐
│  (main.c, UI)       │
└──────────┬──────────┘
           │
┌──────────▼──────────────────┐
│ Device Abstraction Layer    │
│ (OPS interface)             │
└──────────┬──────────────────┘
           │
┌──────────▼──────────────────┐
│ Driver Core Layer           │
│ (Registry, State Machine)   │
└──────────┬──────────────────┘
           │
┌──────────▼──────────────────┐
│ System Adaptation Layer     │
│ (FreeRTOS, GPIO, ESP-IDF)   │
└─────────────────────────────┘
```

### Multi-Device Support
- Global registry supports up to 4 devices
- Each device has independent state and resources
- Lock-free thread-safe access through handles

### State Machine
- 5 states for complete lifecycle management
- Automatic state transitions during beep sequences
- Callback notification on completion

### Thread Safety
- Registry-level mutex for registration operations
- Device-level mutex for state changes
- FreeRTOS Timer for async beep control

---

## Code Examples

### Simple Beep
```c
buzzer_handle_t h = buzzer_get_handle(0);
buzzer_beep_once(h, 100);  // 100ms beep
```

### Play Pattern
```c
buzzer_handle_t h = buzzer_get_handle(0);
buzzer_beep_pattern(h, BUZZER_BEEP_SUCCESS);
```

### Continuous Beep
```c
buzzer_handle_t h = buzzer_get_handle(0);
buzzer_on(h);     // Start
// ... do something ...
buzzer_off(h);    // Stop
```

### With Callback
```c
void on_complete(void *user_data) {
    ESP_LOGI(TAG, "Beep completed");
}

buzzer_handle_t h = buzzer_get_handle(0);
buzzer_set_callback(h, on_complete, NULL);
buzzer_beep_pattern(h, BUZZER_BEEP_SUCCESS);
```

---

## Documentation

### Detailed Documentation
- **Architecture Design**: See `BUZZER_ARCHITECTURE.md` for detailed system design
- **Usage Examples**: See `BUZZER_USAGE_EXAMPLE.md` for comprehensive integration examples
- **API Reference**: See `buzzer.h` for complete API documentation

### Key Sections in Architecture Document
- Layered architecture overview
- Component design and data structures
- Data flow diagrams for initialization and beep playback
- Interface design with operation definitions
- Complete state machine description
- Thread-safety analysis and concurrent scenarios
- Extension guide for adding new hardware implementations

---

## Dependencies

### ESP-IDF Components
- `driver/gpio` - GPIO control
- `freertos/FreeRTOS` - Task and timer support
- `freertos/timers` - Software timer for async beep
- `esp_log` - Logging

### Configuration
- Add to `CMakeLists.txt`:
```cmake
idf_component_register(
    SRCS "buzzer.c"
    INCLUDE_DIRS "."
    REQUIRES driver freertos log
)
```

---

## Performance Characteristics

| Metric | Value | Note |
|--------|-------|------|
| Max Devices | 4 | Configurable via BUZZER_MAX_DEVICES |
| Max Pattern Length | 16 | Steps in beep sequence |
| Pattern Check Interval | 50ms | Timer resolution |
| Thread Safe | Yes | Mutex protected |
| Memory per Device | ~300 bytes | + private data |
| Initialization Time | <10ms | Per device |

---

## Limitations & Future Enhancements

### Current Limitations
- GPIO buzzer doesn't support frequency/volume adjustment
- No support for complex beep patterns with varying pitch
- Fixed pattern library (can be extended)

### Planned Enhancements
- PWM-based buzzer implementation (frequency control)
- I2S-based buzzer for audio playback
- Beep sequence support (multiple patterns in sequence)
- NVS persistence for device configuration
- Advanced pattern editor and player

---

## Troubleshooting

### Issue: Buzzer doesn't make sound
- Check GPIO pin is correctly configured
- Verify buzzer is connected with correct polarity
- Check current limiting resistor (if needed)

### Issue: State doesn't update
- Ensure `buzzer_init()` is called before operations
- Check handle validity with `buzzer_get_device_info()`

### Issue: Beep pattern doesn't complete
- Verify callback is properly registered
- Check FreeRTOS timer creation succeeded
- Review debug output from `buzzer_dump_devices()`

---

## License & Support

Part of the ESP32-S3 Smart Locker project.
For issues or questions, please refer to the project repository.

---

## Related Components

- **Display** (`bsp/display/`) - Visual feedback
- **Locker** (`bsp/locker/`) - Door control
- **UI** (`bsp/ui/`) - User interface
- **XST** (`bsp/XST/`) - Palmprint recognition

The Buzzer driver complements these components to provide complete user feedback in the smart locker system.

