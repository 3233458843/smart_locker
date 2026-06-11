/**
 * @file      serve.h
 * @brief     Business service layer API
 */

#ifndef _SERVE_H
#define _SERVE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "esp_err.h"
#include "freertos/FREERTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

struct buzzer_handle_s;
typedef struct buzzer_handle_s *buzzer_handle_t;

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SERVE_FLOW_IDLE = 0,
    SERVE_FLOW_PENDING,
    SERVE_FLOW_RUNNING,
    SERVE_FLOW_SUCCESS,
    SERVE_FLOW_FAILED,
} serve_flow_state_t;

typedef struct {
    serve_flow_state_t state;
    uint8_t locker_id;
    uint8_t password[4];
    bool has_password;
    uint16_t user_id;
    esp_err_t err;
    char message[64];
} serve_save_status_t;

typedef struct {
    serve_flow_state_t state;
    uint8_t locker_id;
    uint16_t user_id;
    esp_err_t err;
    char message[64];
} serve_take_status_t;

extern buzzer_handle_t g_buzzer_handle;

void serve_init(void);

bool serve_request_save(void);
bool serve_request_save_with_phone(const uint8_t phone[4]);
bool serve_request_take_by_palm(void);
bool serve_request_take_by_password(const uint8_t password[4]);
bool serve_request_take_by_phone(const uint8_t phone[4]);
bool serve_request_debug_verify(void);
void serve_get_save_status(serve_save_status_t *out);
void serve_get_take_status(serve_take_status_t *out);

/* Admin/debug API — routes hardware ops through serve layer */
bool serve_admin_reset_xst(void);
bool serve_admin_del_all_users(void);
int  serve_admin_get_user_count(void);
bool serve_admin_open_locker(uint8_t locker_id);
bool serve_admin_open_all_lockers(void);
bool serve_admin_verify_password(const char *input, uint8_t len);
void serve_reset_palm_progress(void);

#ifdef __cplusplus
}
#endif

#endif /*_SERVE_H */
