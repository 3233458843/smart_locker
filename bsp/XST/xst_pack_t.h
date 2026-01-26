#ifndef _XST_PACK_T_H
#define _XST_PACK_T_H

#include <stdio.h>
#include "esp_system.h"

#define XST_SYNC_WORD_H                 0xEF
#define XST_SYNC_WORD_L                 0xAA

// 消息ID定义
#define MID_REPLY                       0x00
#define MID_NOTE                        0x01
#define MID_IMAGE                       0x02
#define MID_LOG                         0x03
#define MID_DAT                         0x04
#define MID_RESET                       0x10
#define MID_GETSTATUS                   0x11
#define MID_VERIFY                      0x12
#define MID_ENROLL                      0x13
#define MID_ENROLL_PROGRESS             0x14
#define MID_SNAP_IMAGE                  0x16
#define MID_GET_SAVED_IMAGE_SIZE        0x17
#define MID_UPLOAD_IMAGE                0x18
#define MID_ENROLL_SINGLE               0x1D
#define MID_DEL_USER                    0x20
#define MID_DEL_ALL                     0x21
#define MID_GET_USER_INFO               0x22
#define MID_GET_ALL_USER_ID             0x24
#define MID_GET_ALL_USER_INFO           0x25
#define MID_QUIT                        0xFF

// 执行结果码定义
#define MR_SUCCESS                      0
#define MR_REJECTED                     1
#define MR_ABORTED                      2
#define MR_FAILED4_CAMERA               4
#define MR_FAILED4_UNKNOWN_REASON       5
#define MR_FAILED4_INVALID_PARAM        6
#define MR_FAILED4_NO_MEMORY            7
#define MR_FAILED4_UNKNOWN_USER         8
#define MR_FAILED4_MAX_USER             9
#define MR_FAILED4_ENROLLED             10
#define MR_FAILED4_LIVENESS_CHECK       12
#define MR_FAILED4_TIME_OUT             13
#define MR_FAILED4_AUTHORIZATION        14

// 通知ID定义
#define NID_READY                       0
#define NID_FACE_STATE                  1
#define NID_UNKNOWNERROR                2
#define NID_OTA_DONE                    3
#define NID_PALM_STATE                  4
#define NID_AUTHORIZATION               8
/************************************master -> XST*****************************************/
//通用模组接收包，应以此格式包向模组发送数据                                 
typedef struct {
    uint8_t SyncWord[2];     
    uint8_t MsgID;    
    uint8_t Size[2];   
    uint8_t *Data;  
    uint8_t ParityCheck;   
}xst_pack_t; 

/************************************XST -> master***************************************/
//////////////////////////////////////////////////////
//模组回复数据包定义
typedef struct {
    uint8_t Result;
    uint8_t NoteID;
    uint8_t *data;
}s_msg_reply_data;
//模组回复数据协议包定义
typedef struct {
    uint8_t SyncWord[2];     
    uint8_t MsgID;    
    uint8_t Size[2];   
    s_msg_reply_data Data;
    uint8_t ParityCheck; 
} xst_pack_reply_t;

//////////////////////////////////////////////////////
//模组通知数据包定义
typedef struct {
    uint8_t nid;
    uint8_t *data;    
}s_msg_note_data;
//模组通知数据协议包定义
typedef struct {
    uint8_t SyncWord[2];     
    uint8_t MsgID;    
    uint8_t Size[2];   
    s_msg_note_data Data;
    uint8_t ParityCheck; 
} xst_pack_note_t;
//////////////////////////////////////////////////////

#endif
