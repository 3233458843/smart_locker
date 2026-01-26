#ifndef _XST_H_
#define _XST_H_

#include <stdio.h>
#include <stdint.h>             
#include <stddef.h>             
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/uart.h"

#include "xst_pack_t.h"

#define XST_TAG         "xst"

#define xst_uart_num    UART_NUM_2
#define xst_rx_pin      GPIO_NUM_4
#define xst_tx_pin      GPIO_NUM_6

/***********************************用户信息******************************************/
typedef struct {
    char name[32];
    int id;
} User;
/***********************************柜门状态******************************************/
typedef enum {
    LOCKER_FREE = 0,         // 空闲
    LOCKER_OCCUPIED = 1,     // 占用
    LOCKER_FAULT = 2,        // 故障
    LOCKER_RESERVED = 3      // 预留
} LockerStatus;



#endif // _XST_H_
