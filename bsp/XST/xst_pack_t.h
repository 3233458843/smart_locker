#ifndef _XST_PACK_T_H
#define _XST_PACK_T_H

#include <stdio.h>
#include <stdint.h>

#define XST_SYNC_WORD_H                 0xEF
#define XST_SYNC_WORD_L                 0xAA
#define XST_SYNC_WORD                   0xEFAA

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
#define MID_GET_VERSION                 0x30
#define MID_QUIT                        0xFF

// 执行结果码定义
typedef enum {
    MR_SUCCESS = 0,
    MR_REJECTED = 1,
    MR_ABORTED = 2,
    MR_FAILED4_CAMERA = 4,
    MR_FAILED4_UNKNOWN_REASON = 5,
    MR_FAILED4_INVALID_PARAM = 6,
    MR_FAILED4_NO_MEMORY = 7,
    MR_FAILED4_UNKNOWN_USER = 8,
    MR_FAILED4_MAX_USER = 9,
    MR_FAILED4_ENROLLED = 10,
    MR_FAILED4_LIVENESS_CHECK = 12,
    MR_FAILED4_TIME_OUT = 13,
    MR_FAILED4_AUTHORIZATION = 14,
    MR_FAILED4_READ_FILE = 19,
    MR_FAILED4_WRITE_FILE = 20
} xst_result_t;

// 通知ID定义
#define NID_READY                       0
#define NID_FACE_STATE                  1
#define NID_UNKNOWNERROR                2
#define NID_OTA_DONE                    3
#define NID_PALM_STATE                  4
#define NID_AUTHORIZATION               8

// 协议包头结构 
typedef struct __attribute__((packed)) {
    uint16_t SyncWord; 
    uint8_t MsgID;
    uint16_t Size;     
} xst_header_t;

// 通用Reply数据结构 (Data部分)
typedef struct __attribute__((packed)) {
    uint8_t mid;       // 对应的发送命令MsgID
    uint8_t result;    // 执行结果
    uint8_t payload[]; // 变长数据
} xst_reply_body_t;

// 通用Note数据结构 (Data部分)
typedef struct __attribute__((packed)) {
    uint8_t nid;       // Note ID
    uint8_t payload[]; // 变长数据
} xst_note_body_t;

// 用户信息结构 
typedef struct __attribute__((packed)) {
    uint16_t id;
    uint8_t admin;
    char name[32];
} xst_user_info_t;

// 注册参数结构 (用于 ENROLL_SINGLE)
typedef struct __attribute__((packed)) {
    uint8_t admin;
    char user_name[32];
    uint8_t direction; // Invalid, set 0
    uint8_t timeout;
} xst_enroll_param_t;

#endif
