#ifndef _LOCKER_H_
#define _LOCKER_H_

#include <stdio.h>
#include <stdbool.h> 
#include <string.h>
#include "xst_pack_t.h"

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

typedef struct {
    uint8_t locker_user_info[locker_user_info_max_len]; 
    uint8_t locker_user_info_id[2]; // 用户信息 ID
}locker_info_t;

typedef struct {
    locker_info_t locker_info; // 锁用户信息
    uint8_t locker_id;
    bool is_locked; // 锁状态
    uint8_t locker_pin ;// 锁控 GPIO 引脚
    uint8_t locker_detection_pin ; //检测引脚
    uint8_t password[4]; // 锁密码，4 位数字
} locker_t;

extern locker_t lockers[4]; // 声明 4 个锁实例

void locker_init(void);
void locker_on_off(locker_t *locker, bool lock);
locker_t* get_locker_by_id(uint8_t* id);
void crumble_password(uint8_t* password);
bool Detection_locker_on_off(const locker_t* locker);

#endif /* _LOCKER_H_ */
