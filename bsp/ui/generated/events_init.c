/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"

#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif

#include <string.h>
#include <stdio.h>

static char input_pwd[5] = "";
static uint8_t pwd_len = 0;

// 使用宏定义隔离，彻底杜绝“重复定义”报错！
#ifndef CORRECT_PWD_DEF
#define CORRECT_PWD_DEF

const char * correct_pwd = "1234";
#endif

static void main_btnm_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        lv_obj_t * obj = lv_event_get_target(e);
        uint32_t id = lv_buttonmatrix_get_selected_button(obj);
        lv_ui * ui = &guider_ui;

        // 1. 获取按键文本
        lv_obj_t * my_btnm = lv_event_get_target(e);
        uint32_t my_id = lv_buttonmatrix_get_selected_button(my_btnm);
        const char * my_txt = lv_buttonmatrix_get_button_text(my_btnm, my_id);

        if(my_txt == NULL) return; // 点到空白处忽略

        // 2. 核心逻辑判断
        if(strcmp(my_txt, "x") == 0) {
            // 【退格键】
            if(pwd_len > 0) {
                pwd_len--;
                input_pwd[pwd_len] = '\0';
            }
        }
        else if(strcmp(my_txt, "√") == 0 || strcmp(my_txt, "v") == 0) {
            // 【确认键】
            // 💡 优化：不再固定必须输满4位，而是自动获取你设置的密码长度！
            if(pwd_len == strlen(correct_pwd)) {
                if(strcmp(input_pwd, correct_pwd) == 0) {

                    // ✅ 密码正确，准备跳转！
                    printf("Password Correct! Jumping to Setting...\n");

                    // 可选彩蛋：改变鸽鸽的台词
                    lv_label_set_text(ui->main_label_8, "哎呦不错哦，进去吧~");

                    // 🌟 核心跳转代码：加载并进入 setting_item 页面
                    if(ui->setting_item == NULL) {
                        setup_scr_setting_item(ui); // 初始化页面
                    }
                    lv_scr_load(ui->setting_item);  // 切换屏幕

                } else {
                    // ❌ 密码错误
                    printf("Password Error!\n");
                    lv_label_set_text(ui->main_label_8, "密码不对！露出鸡脚了吧~");
                }

                // 验证完后自动清空密码盘，等待重新输入
                pwd_len = 0;
                memset(input_pwd, 0, sizeof(input_pwd));
            } else {
                lv_label_set_text(ui->main_label_8, "密码长度不对哦~");
            }
        }
        else {
            // 【输入数字键】
            if(pwd_len < 4) {
                input_pwd[pwd_len] = my_txt[0];
                pwd_len++;
                input_pwd[pwd_len] = '\0';
            }
        }

        // 3. 屏幕显示更新 (动态改变那四个框框)
        char char_str[2] = {0, 0};

        if(pwd_len > 0) {
            char_str[0] = input_pwd[0];
            lv_label_set_text(ui->main_label_4, char_str);
        }
        else {
            lv_label_set_text(ui->main_label_4, "_");
        }

        if(pwd_len > 1) {
            char_str[0] = input_pwd[1];
            lv_label_set_text(ui->main_label_5, char_str);
        }
        else {
            lv_label_set_text(ui->main_label_5, "_");
        }

        if(pwd_len > 2) {
            char_str[0] = input_pwd[2];
            lv_label_set_text(ui->main_label_6, char_str);
        }
        else {
            lv_label_set_text(ui->main_label_6, "_");
        }

        if(pwd_len > 3) {
            char_str[0] = input_pwd[3];
            lv_label_set_text(ui->main_label_7, char_str);
        }
        else {
            lv_label_set_text(ui->main_label_7, "_");
        }
        break;
    }
    default:
        break;
    }
}

static void main_imgbtn_4_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_remove_flag(guider_ui.main_cont_1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.main_cont_2, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void main_btn_3_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.taked_item, guider_ui.taked_item_del, &guider_ui.main_del, setup_scr_taked_item, LV_SCR_LOAD_ANIM_NONE, 100, 100, true, true);
        break;
    }
    default:
        break;
    }
}

static void main_btn_2_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.saved_item, guider_ui.saved_item_del, &guider_ui.main_del, setup_scr_saved_item, LV_SCR_LOAD_ANIM_NONE, 100, 100, true, true);
        break;
    }
    default:
        break;
    }
}

static void main_imgbtn_3_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_add_flag(guider_ui.main_cont_1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(guider_ui.main_cont_2, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void main_btn_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.help_item, guider_ui.help_item_del, &guider_ui.main_del, setup_scr_help_item, LV_SCR_LOAD_ANIM_NONE, 100, 100, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_main (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->main_btnm_1, main_btnm_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->main_imgbtn_4, main_imgbtn_4_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->main_btn_3, main_btn_3_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->main_btn_2, main_btn_2_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->main_imgbtn_3, main_imgbtn_3_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->main_btn_1, main_btn_1_event_handler, LV_EVENT_ALL, ui);
}

static void saved_item_imgbtn_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.main, guider_ui.main_del, &guider_ui.saved_item_del, setup_scr_main, LV_SCR_LOAD_ANIM_NONE, 100, 100, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_saved_item (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->saved_item_imgbtn_1, saved_item_imgbtn_1_event_handler, LV_EVENT_ALL, ui);
}

static void taked_item_imgbtn_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.main, guider_ui.main_del, &guider_ui.taked_item_del, setup_scr_main, LV_SCR_LOAD_ANIM_NONE, 100, 100, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_taked_item (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->taked_item_imgbtn_1, taked_item_imgbtn_1_event_handler, LV_EVENT_ALL, ui);
}

static void help_item_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.main, guider_ui.main_del, &guider_ui.help_item_del, setup_scr_main, LV_SCR_LOAD_ANIM_NONE, 100, 100, true, true);
        break;
    }
    default:
        break;
    }
}

static void help_item_imgbtn_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.main, guider_ui.main_del, &guider_ui.help_item_del, setup_scr_main, LV_SCR_LOAD_ANIM_NONE, 100, 100, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_help_item (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->help_item, help_item_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->help_item_imgbtn_1, help_item_imgbtn_1_event_handler, LV_EVENT_ALL, ui);
}

static void setting_item_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.main, guider_ui.main_del, &guider_ui.setting_item_del, setup_scr_main, LV_SCR_LOAD_ANIM_NONE, 100, 100, true, true);
        break;
    }
    default:
        break;
    }
}

static void setting_item_imgbtn_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.main, guider_ui.main_del, &guider_ui.setting_item_del, setup_scr_main, LV_SCR_LOAD_ANIM_NONE, 100, 100, true, true);
        break;
    }
    default:
        break;
    }
}

static void setting_item_btn_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_label_set_text(guider_ui.setting_item_label_1, "模组复位");
        break;
    }
    default:
        break;
    }
}

static void setting_item_btn_2_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_label_set_text(guider_ui.setting_item_label_1, "读取所有用户");
        break;
    }
    default:
        break;
    }
}

static void setting_item_btn_3_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_label_set_text(guider_ui.setting_item_label_1, "删除所有用户");
        break;
    }
    default:
        break;
    }
}

static void setting_item_btn_4_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_label_set_text(guider_ui.setting_item_label_1, "开始识别掌纹");
        break;
    }
    default:
        break;
    }
}

void events_init_setting_item (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->setting_item, setting_item_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->setting_item_imgbtn_1, setting_item_imgbtn_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->setting_item_btn_1, setting_item_btn_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->setting_item_btn_2, setting_item_btn_2_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->setting_item_btn_3, setting_item_btn_3_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->setting_item_btn_4, setting_item_btn_4_event_handler, LV_EVENT_ALL, ui);
}


void events_init(lv_ui *ui)
{

}
