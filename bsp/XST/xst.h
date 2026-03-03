#ifndef _XST_H_
#define _XST_H_

#include <stdio.h>
#include <stdint.h>             
#include <stddef.h>    
#include "xst_pack_t.h"

#define XST_TAG         "xst"

#define XST_UART_NUM    UART_NUM_1
#define XST_TX_PIN      GPIO_NUM_4
#define XST_RX_PIN      GPIO_NUM_5
#define XST_BAUD_RATE   115200

typedef void (*xst_note_callback_t)(uint8_t nid, uint8_t *data, uint16_t len);

void xst_init(xst_note_callback_t callback);

xst_result_t xst_cmd_reset(void);
xst_result_t xst_cmd_get_status(uint8_t *status);
xst_result_t xst_cmd_enroll_single(const char *name, uint8_t admin, uint8_t timeout, uint16_t *out_user_id);
xst_result_t xst_cmd_del_user(uint16_t user_id);
xst_result_t xst_cmd_del_all(void);
xst_result_t xst_cmd_get_user_count(uint16_t *count);
xst_result_t xst_cmd_get_user_info(uint16_t query_id, xst_user_info_t *out_info);
xst_result_t xst_cmd_verify(uint8_t timeout, uint16_t *out_user_id, char *out_name);
#endif // _XST_H_
