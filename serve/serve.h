/**
 * @file      serve.h
 * @brief     ${USER_PROMPT}
 * @author    ${AUTHOR_NAME} (${AUTHOR_EMAIL})
 * @version   1.0
 * @date      2026-04-21
 * 
 * @copyright Copyright (c) 2026 All rights reserved.
 * 
 * @note      ${NOTE}
 */

#ifndef _SERVE_H
#define _SERVE_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "esp_err.h"
#include "freertos/FREERTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

/* C++ ------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
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

/* Exported variables --------------------------------------------------------*/
extern SemaphoreHandle_t ready_save  ; // 通知能够进行存件
extern SemaphoreHandle_t ready_take  ; // 通知能够进行取件
extern SemaphoreHandle_t verify_debug  ; // 测试识别指令

extern  QueueHandle_t save_verify_process ;

extern uint16_t user_num[128] ; //
/* Exported functions --------------------------------------------------------*/
void serve_init(void);
void serve_main(void);

bool serve_request_save(void);
bool serve_request_take_by_palm(void);
bool serve_request_take_by_password(const uint8_t password[4]);
bool serve_request_debug_verify(void);
void serve_get_save_status(serve_save_status_t *out);
void serve_get_take_status(serve_take_status_t *out);
/* C++  ------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif /*_SERVE_H */