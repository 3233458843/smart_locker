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

/* Exported variables --------------------------------------------------------*/
extern SemaphoreHandle_t ready_save  ; // 通知能够进行存件
extern SemaphoreHandle_t ready_take  ; // 通知能够进行取件
extern SemaphoreHandle_t verify_debug  ; // 测试识别指令

extern  QueueHandle_t save_verify_process ;

extern uint16_t user_num[128] ; //
/* Exported functions --------------------------------------------------------*/
void serve_init(void);
void serve_main(void);
/* C++  ------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif /*_SERVE_H */