#ifndef _XST_PACK_T_H
#define _XST_PACK_T_H

#include <stdio.h>
#include <stdint.h>

#define XST_SYNC_WORD_H                 0xEF
#define XST_SYNC_WORD_L                 0xAA
#define XST_SYNC_WORD                   0xEFAA

#define XST_USER_NAME_SIZE              32
#define XST_MAX_USER_COUNT              50

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
#define MID_ENROLL_SINGLE_ID16          0x1E
#define MID_ENROLL_SINGLE_ID32          0x1F
#define MID_DEL_USER                    0x20
#define MID_DEL_ALL                     0x21
#define MID_GET_USER_INFO               0x22
#define MID_ALGORITHM_RESET             0x23
#define MID_GET_ALL_USER_ID             0x24
#define MID_GET_ALL_USER_INFO           0x25
#define MID_GET_VERSION                 0x30
#define MID_START_OTA                   0x40
#define MID_STOP_OTA                    0x41
#define MID_GET_OTA_STATUS              0x42
#define MID_OTA_HEADER                  0x43
#define MID_OTA_PACKET                  0x44
#define MID_INIT_ENCRYPTION             0x50
#define MID_CONFIG_BAUDRATE             0x51
#define MID_SET_RELEASE_ENC_KEY         0x52
#define MID_SET_DEBUG_ENC_KEY           0x53
#define MID_GET_LOG_FILE_SIZE           0x60
#define MID_UPLOAD_LOG_FILE             0x61
#define MID_ENROLL_BIOTYPE              0xA0
#define MID_ENROLL_BIOTYPE_ID16         0xA1
#define MID_ENROLL_BIOTYPE_ID32         0xA2
#define MID_VERIFY_BIOTYPE              0xA5
#define MID_DEL_USER_BIOTYPE            0xB0
#define MID_DEL_ALL_BIOTYPE             0xB1
#define MID_GET_ALL_USER_ID_BIOTYPE     0xB4
#define MID_GET_USER_INFO_BIOTYPE       0xB5
#define MID_GET_ALL_INFO_BIOTYPE        0xB6
#define MID_GET_DAT_FILE_SIZE           0xC0
#define MID_UPLOAD_DAT_FILE             0xC1
#define MID_INFORM_DAT_FILE_INFO        0xC2
#define MID_DOWNLOAD_DAT_FILE           0xC3
#define MID_QUIT                        0xFF

// 执行结果码定义
typedef enum{
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
    MR_FAILED4_AUTH_FAIL,
    MR_FAILED4_READ_FILE = 19,
    MR_FAILED4_WRITE_FILE = 20,
} xst_result_t;

// 通知ID定义
#define NID_READY                       0
#define NID_FACE_STATE                  1
#define NID_UNKNOWNERROR                2
#define NID_OTA_DONE                    3
#define NID_PALM_STATE                  4
#define NID_AUTHORIZATION               8

// 协议包头结构
typedef struct __attribute__((packed)){
    uint16_t SyncWord;
    uint8_t MsgID;
    uint16_t Size;
} xst_header_t;

// 通用Reply数据结构 (Data部分)
typedef struct __attribute__((packed)){
    uint8_t mid;
    uint8_t result;
    uint8_t payload[];
} xst_reply_body_t;

// 通用Note数据结构 (Data部分)
typedef struct __attribute__((packed)){
    uint8_t nid;
    uint8_t payload[];
} xst_note_body_t;

// 获取用户信息返回结构（协议 6.18）
typedef struct __attribute__((packed)){
    uint16_t id;
    char name[XST_USER_NAME_SIZE];
    uint8_t admin;
} xst_user_info_t;

// 识别返回结构（协议 6.3）
typedef struct __attribute__((packed)){
    uint16_t id;
    char name[XST_USER_NAME_SIZE];
    uint8_t admin;
    uint8_t unlock_status;
} xst_verify_reply_t;

// 单次录入返回结构（协议 6.5）
typedef struct __attribute__((packed)){
    uint16_t id;
    uint8_t direction;
} xst_enroll_reply_t;

// 获取所有用户 ID 返回结构（协议 6.16）
typedef struct __attribute__((packed)){
    uint8_t user_count;
    uint16_t user_ids[XST_MAX_USER_COUNT];
} xst_all_user_ids_reply_t;

// 注册参数结构 (用于 ENROLL_SINGLE)
typedef struct __attribute__((packed)){
    uint8_t admin;
    char user_name[XST_USER_NAME_SIZE];
    uint8_t direction;
    uint8_t timeout;
} xst_enroll_param_t;

#endif
