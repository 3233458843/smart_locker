#ifndef _LOCKER_H_
#define _LOCKER_H_

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "xst_pack_t.h"
#include "esp_err.h"



//柜门控制引脚
#define locker_user_info_max_len 100 // 用户信息最大长度

#define LOCKER1_GPIO_PIN GPIO_NUM_38  // 锁控 GPIO 引脚
#define LOCKER2_GPIO_PIN GPIO_NUM_39  // 锁控 GPIO 引脚
#define LOCKER3_GPIO_PIN GPIO_NUM_40  // 锁控 GPIO 引脚
#define LOCKER4_GPIO_PIN GPIO_NUM_41  // 锁控 GPIO 引脚

//柜门检测引脚
#define LOCKER1_Detection_GPIO_PIN GPIO_NUM_15 //柜号1检测是否关闭
#define LOCKER2_Detection_GPIO_PIN GPIO_NUM_16 //柜号2检测是否关闭
#define LOCKER3_Detection_GPIO_PIN GPIO_NUM_17 //柜号3检测是否关闭
#define LOCKER4_Detection_GPIO_PIN GPIO_NUM_18 //柜号4检测是否关闭

typedef struct{
    uint8_t locker_user_info[locker_user_info_max_len]; // 用户名称
    uint8_t locker_user_info_id[2]; // 用户信息 ID
} locker_info_t;

typedef struct{
    locker_info_t locker_info; // 锁用户信息
    uint8_t locker_id;
    bool is_locked; // 锁状态  true代表锁已打开 false代表锁已关闭
    bool have_saved; // 是否存物 true代表改柜号已存放物品 ，false代表该柜号还未存放物品
    uint8_t locker_pin; // 锁控 GPIO 引脚
    uint8_t locker_detection_pin; //检测引脚
    uint8_t password[4]; // 锁密码，4 位数字
} locker_t;

typedef struct {
    uint16_t user_id;      // XST 用户ID
    uint8_t locker_id;     // 分配的柜号 0-3
    uint8_t password[4];   // 4字节密码
    uint32_t timestamp;    // 存件时间戳
    bool is_valid;         // 是否有效
} user_locker_entry_t;

extern locker_t lockers[4]; // 声明 4 个锁实例

// 基础柜门控制接口
void locker_init(void);
void locker_on(locker_t* locker);
void locker_all_on(void);
locker_t* get_locker_by_id(uint8_t* id);
void crumble_password(uint8_t* password);
bool Detection_locker_on_off(const locker_t* locker);
bool has_item_in_locker(const locker_t* locker);
bool is_locker_secured(const locker_t* locker);

esp_err_t locker_db_init(void);                    // 初始化，从NVS恢复
esp_err_t locker_db_add_entry(user_locker_entry_t* entry);  // 添加条目
esp_err_t locker_db_get_entry_by_user(uint16_t user_id, user_locker_entry_t* entry);
esp_err_t locker_db_get_entry_by_locker(uint8_t locker_id, user_locker_entry_t* entry);
esp_err_t locker_db_remove_entry_by_locker(uint8_t locker_id);
uint8_t locker_db_find_free_locker(void);          // 找空闲柜号
esp_err_t locker_db_save_to_nvs(void);             // 保存到NVS

#endif /* _LOCKER_H_ */
