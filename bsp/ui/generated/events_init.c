/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/
#include "events_init.h"

#include <esp_log.h>
#include <stdio.h>
#include "lvgl.h"

#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "XST/xst.h"
#include "locker/locker.h"
#include "serve.h"

extern uint8_t g_xst_palm_progress;
extern bool g_palm_progress_updated;

// 静态变量保存当前取件输入的密码
static char take_input_pwd[5] = "";
static uint8_t take_pwd_len = 0;
#include <string.h>
#include <stdio.h>

// 静态变量保存当前输入的密码
static char setting_input_pwd[5] = "";
static uint8_t setting_pwd_len = 0;

// 这里设置你的管理员密码 (1234)
#ifndef ADMIN_PWD_DEF
#define ADMIN_PWD_DEF
const char * admin_pwd = "1234";
#endif

static void main_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOADED:
    {
        lv_animimg_start(guider_ui.main_animimg_1);
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
        ui_load_scr_animation(&guider_ui, &guider_ui.save_page, guider_ui.save_page_del, &guider_ui.main_del, setup_scr_save_page, LV_SCR_LOAD_ANIM_NONE, 100, 100, true, true);

        if (serve_request_save()){
            ESP_LOGI("EVENT", "Requested save flow");
        } else {
            ESP_LOGW("EVENT", "Save flow request rejected");
        }
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
        ui_load_scr_animation(&guider_ui, &guider_ui.take_page, guider_ui.take_page_del, &guider_ui.main_del, setup_scr_take_page, LV_SCR_LOAD_ANIM_NONE, 100, 100, true, true);
        if (serve_request_take_by_palm()){
            ESP_LOGI("EVENT", "Requested palm take flow");
        } else {
            ESP_LOGW("EVENT", "Palm take flow request rejected");
        }
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
        ui_load_scr_animation(&guider_ui, &guider_ui.help_page, guider_ui.help_page_del, &guider_ui.main_del, setup_scr_help_page, LV_SCR_LOAD_ANIM_NONE, 50, 50, true, true);
        break;
    }
    case LV_EVENT_LONG_PRESSED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.setting_page, guider_ui.setting_page_del, &guider_ui.main_del, setup_scr_setting_page, LV_SCR_LOAD_ANIM_NONE, 50, 50, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_main (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->main, main_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->main_btn_1, main_btn_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->main_btn_2, main_btn_2_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->main_btn_3, main_btn_3_event_handler, LV_EVENT_ALL, ui);
}

static void take_page_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOADED:
    {
        lv_ui * ui = &guider_ui;

        // 1. 确保刚进页面时，显示扫脉界面 (cont_1)，隐藏密码界面 (cont_2)
        lv_obj_clear_flag(ui->take_page_cont_1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui->take_page_cont_2, LV_OBJ_FLAG_HIDDEN);

        // 2. 复位提示词
        lv_label_set_text(ui->take_page_label_2, "请输入密码");
        g_xst_palm_progress = 0;
        g_palm_progress_updated = false;


        // 3. 把你那 4 个数字框强制变成下划线
        lv_label_set_text(ui->take_page_label_3, "_");
        lv_label_set_text(ui->take_page_label_4, "_");
        lv_label_set_text(ui->take_page_label_5, "_");
        lv_label_set_text(ui->take_page_label_6, "_");
        break;
    }
    default:
        break;
    }
}

static void take_page_imgbtn_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.main, guider_ui.main_del, &guider_ui.take_page_del, setup_scr_main, LV_SCR_LOAD_ANIM_NONE, 200, 200, false, true);
        break;
    }
    default:
        break;
    }
}

static void take_page_btn_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_remove_flag(guider_ui.take_page_cont_2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.take_page_cont_1, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void take_page_imgbtn_2_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_add_flag(guider_ui.take_page_cont_2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(guider_ui.take_page_cont_1, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void take_page_btnm_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        lv_obj_t * obj = lv_event_get_target(e);
        uint32_t id = lv_buttonmatrix_get_selected_button(obj);
        lv_ui * ui = &guider_ui;

        // 1. 获取当前点击的按键文本
        lv_obj_t * my_btnm = lv_event_get_target(e);
        uint32_t my_id = lv_buttonmatrix_get_selected_button(my_btnm);
        const char * my_txt = lv_buttonmatrix_get_button_text(my_btnm, my_id);

        if(my_txt == NULL) return;

        // 2. 核心逻辑判断
        if(strcmp(my_txt, "X") == 0 || strcmp(my_txt, "x") == 0) {
            // 【退格键】
            if(take_pwd_len > 0) {
                take_pwd_len--;
                take_input_pwd[take_pwd_len] = '\0';
            }
            // 恢复上方提示语
            lv_label_set_text(ui->take_page_label_2, "请输入密码");
        }
        else if(strcmp(my_txt, "V") == 0 || strcmp(my_txt, "v") == 0) {
            // 【确认键】
            if(take_pwd_len == 4) {
                uint8_t password[4] = {
                    (uint8_t)(take_input_pwd[0] - '0'),
                    (uint8_t)(take_input_pwd[1] - '0'),
                    (uint8_t)(take_input_pwd[2] - '0'),
                    (uint8_t)(take_input_pwd[3] - '0'),
                };

                if(serve_request_take_by_password(password)) {
                    serve_take_status_t take_status = {0};
                    serve_get_take_status(&take_status);
                    printf("Take Password Correct!\n");
                    lv_label_set_text_fmt(ui->take_page_label_2, "密码正确！%d号柜已开", take_status.locker_id + 1);
                } else {
                    lv_label_set_text(ui->take_page_label_2, "密码错误，请重试！");
                }

                // 验证完后自动清空密码盘
                take_pwd_len = 0;
                memset(take_input_pwd, 0, sizeof(take_input_pwd));

            } else {
                lv_label_set_text(ui->take_page_label_2, "必须输入4位密码！");
            }
        }
        else {
            // 【数字键】
            if(take_pwd_len < 4) {
                take_input_pwd[take_pwd_len] = my_txt[0];
                take_pwd_len++;
                take_input_pwd[take_pwd_len] = '\0';
            }
            lv_label_set_text(ui->take_page_label_2, "请输入密码");
        }

        // 3. 更新你设置的 4 个独立密码框 (label_3, 4, 5, 6)
        char char_str[2] = {0, 0};

        // 第一位
        if(take_pwd_len > 0) {
            char_str[0] = take_input_pwd[0];
            lv_label_set_text(ui->take_page_label_3, char_str);
        }
        else {
            lv_label_set_text(ui->take_page_label_3, "_");
        }

        // 第二位
        if(take_pwd_len > 1) {
            char_str[0] = take_input_pwd[1];
            lv_label_set_text(ui->take_page_label_4, char_str);
        }
        else {
            lv_label_set_text(ui->take_page_label_4, "_");
        }

        // 第三位
        if(take_pwd_len > 2) {
            char_str[0] = take_input_pwd[2];
            lv_label_set_text(ui->take_page_label_5, char_str);
        }
        else {
            lv_label_set_text(ui->take_page_label_5, "_");
        }

        // 第四位
        if(take_pwd_len > 3) {
            char_str[0] = take_input_pwd[3];
            lv_label_set_text(ui->take_page_label_6, char_str);
        }
        else {
            lv_label_set_text(ui->take_page_label_6, "_");
        }
        break;
    }
    default:
        break;
    }
}

void events_init_take_page (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->take_page, take_page_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->take_page_imgbtn_1, take_page_imgbtn_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->take_page_btn_1, take_page_btn_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->take_page_imgbtn_2, take_page_imgbtn_2_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->take_page_btnm_1, take_page_btnm_1_event_handler, LV_EVENT_ALL, ui);
}

static void save_page_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOADED:
    {
        lv_ui * ui = &guider_ui;

        // 重置进度条 (缩短动画时长以实时响应进度变化)
        lv_obj_set_style_anim_duration(ui->save_page_bar_1, 100, 0);
        lv_bar_set_value(ui->save_page_bar_1, 0, LV_ANIM_OFF);

        // 重置百分比标签并移到进度条右侧
        lv_label_set_text(ui->save_page_label_2, "0%");
        lv_obj_set_pos(ui->save_page_label_2, 205, 50);
        lv_obj_set_size(ui->save_page_label_2, 40, 20);

        // 重置主提示文案
        lv_label_set_text(ui->save_page_label_1, "将手掌悬停于传感器前10cm左右\n等待设备发出滴0.声，柜门即可弹开");

        // 重置掌纹进度全局变量
        g_xst_palm_progress = 0;
        g_palm_progress_updated = false;
        break;
    }
    default:
        break;
    }
}

static void save_page_imgbtn_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.main, guider_ui.main_del, &guider_ui.save_page_del, setup_scr_main, LV_SCR_LOAD_ANIM_NONE, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_save_page (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->save_page, save_page_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->save_page_imgbtn_1, save_page_imgbtn_1_event_handler, LV_EVENT_ALL, ui);
}

static void help_page_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.main, guider_ui.main_del, &guider_ui.help_page_del, setup_scr_main, LV_SCR_LOAD_ANIM_NONE, 100, 100, true, true);
        break;
    }
    default:
        break;
    }
}

static void help_page_imgbtn_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.main, guider_ui.main_del, &guider_ui.help_page_del, setup_scr_main, LV_SCR_LOAD_ANIM_NONE, 100, 100, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_help_page (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->help_page, help_page_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->help_page_imgbtn_1, help_page_imgbtn_1_event_handler, LV_EVENT_ALL, ui);
}

static void setting_page_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOADED:
    {
        lv_ui * ui = &guider_ui;

        // 1. 强制显示密码框 (cont_2)，隐藏真实设置菜单 (cont_1)
        lv_obj_clear_flag(ui->setting_page_cont_2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui->setting_page_cont_1, LV_OBJ_FLAG_HIDDEN);

        // 2. 清空四位密码显示符
        lv_label_set_text(ui->setting_page_label_1, "_");
        lv_label_set_text(ui->setting_page_label_2, "_");
        lv_label_set_text(ui->setting_page_label_3, "_");
        lv_label_set_text(ui->setting_page_label_4, "_");

        // 3. 复位表情包文案
        lv_label_set_text(ui->setting_page_label_5, "密码对了才能进去哦~omage~");
        break;
    }
    default:
        break;
    }
}

static void setting_page_imgbtn_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.main, guider_ui.main_del, &guider_ui.setting_page_del, setup_scr_main, LV_SCR_LOAD_ANIM_NONE, 100, 100, true, true);
        break;
    }
    default:
        break;
    }
}

static void setting_page_RES_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_label_set_text(guider_ui.setting_page_NOTE, "模组复位");
        xst_cmd_reset();
        break;
    }
    default:
        break;
    }
}

static void setting_page_READ_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_label_set_text(guider_ui.setting_page_NOTE, "模组读取用户");
        xst_cmd_get_user_count(user_num);
        break;
    }
    default:
        break;
    }
}

static void setting_page_DELL_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_label_set_text(guider_ui.setting_page_NOTE, "模组删除所有用户");
        xst_cmd_del_all();
        break;
    }
    default:
        break;
    }
}

static void setting_page_locker4_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_label_set_text(guider_ui.setting_page_NOTE, "4号柜打开");
        locker_on(&lockers[3]);
        break;
    }
    default:
        break;
    }
}

static void setting_page_locker3_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_label_set_text(guider_ui.setting_page_NOTE, "3号柜打开");
        locker_on(&lockers[2]);
        break;
    }
    default:
        break;
    }
}

static void setting_page_VERI_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_label_set_text(guider_ui.setting_page_NOTE, "模组开始识别");
        if (serve_request_debug_verify()){
            ESP_LOGI("EVENT", "Requested debug verify flow");
        } else {
            ESP_LOGW("EVENT", "Debug verify flow request rejected");
        }
        break;
    }
    default:
        break;
    }
}

static void setting_page_locker1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_label_set_text(guider_ui.setting_page_NOTE, "1号柜打开");
        locker_on(&lockers[0]);
        break;
    }
    default:
        break;
    }
}

static void setting_page_locker2_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_label_set_text(guider_ui.setting_page_NOTE, "2号柜打开");
        locker_on(&lockers[1]);
        break;
    }
    default:
        break;
    }
}

static void setting_page_ALL_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_label_set_text(guider_ui.setting_page_NOTE, "所有柜打开");
        locker_all_on();
        break;
    }
    default:
        break;
    }
}

static void setting_page_btnm_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        lv_obj_t * obj = lv_event_get_target(e);
        uint32_t id = lv_buttonmatrix_get_selected_button(obj);
        lv_ui * ui = &guider_ui;

        // 1. 获取当前点击的按键文本
        lv_obj_t * my_btnm = lv_event_get_target(e);
        uint32_t my_id = lv_buttonmatrix_get_selected_button(my_btnm);
        const char * my_txt = lv_buttonmatrix_get_button_text(my_btnm, my_id);

        if(my_txt == NULL) return; // 点到空白处忽略

        // 2. 核心逻辑判断
        if(strcmp(my_txt, "X") == 0 || strcmp(my_txt, "x") == 0) {
            // 【退格键】
            if(setting_pwd_len > 0) {
                setting_pwd_len--;
                setting_input_pwd[setting_pwd_len] = '\0';
            }
        }
        else if(strcmp(my_txt, "V") == 0 || strcmp(my_txt, "v") == 0) {
            // 【确认键】
            if(setting_pwd_len == 4) {
                if(strcmp(setting_input_pwd, admin_pwd) == 0) {

                    // ✅ 密码正确，核心动作：隐藏密码框，显示真实的设置菜单！
                    lv_obj_add_flag(ui->setting_page_cont_2, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(ui->setting_page_cont_1, LV_OBJ_FLAG_HIDDEN);

                } else {
                    // ❌ 密码错误
                    lv_label_set_text(ui->setting_page_label_5, "密码错误！请重试！");
                }

                // 验证完后自动清空密码内部缓存，等待下次输入
                setting_pwd_len = 0;
                memset(setting_input_pwd, 0, sizeof(setting_input_pwd));

            } else {
                lv_label_set_text(ui->setting_page_label_5, "必须输入4位密码！");
            }
        }
        else {
            // 【输入数字键】
            if(setting_pwd_len < 4) {
                setting_input_pwd[setting_pwd_len] = my_txt[0];
                setting_pwd_len++;
                setting_input_pwd[setting_pwd_len] = '\0';
            }
        }

        // 3. 屏幕显示更新 (动态改变那四个标签)
        char char_str[2] = {0, 0};

        if(setting_pwd_len > 0) {
            char_str[0] = setting_input_pwd[0];
            lv_label_set_text(ui->setting_page_label_1, char_str);
        }
        else {
            lv_label_set_text(ui->setting_page_label_1, "_");
        }

        if(setting_pwd_len > 1) {
            char_str[0] = setting_input_pwd[1];
            lv_label_set_text(ui->setting_page_label_2, char_str);
        }
        else {
            lv_label_set_text(ui->setting_page_label_2, "_");
        }

        if(setting_pwd_len > 2) {
            char_str[0] = setting_input_pwd[2];
            lv_label_set_text(ui->setting_page_label_3, char_str);
        }
        else {
            lv_label_set_text(ui->setting_page_label_3, "_");
        }

        if(setting_pwd_len > 3) {
            char_str[0] = setting_input_pwd[3];
            lv_label_set_text(ui->setting_page_label_4, char_str);
        }
        else {
            lv_label_set_text(ui->setting_page_label_4, "_");
        }
        break;
    }
    default:
        break;
    }
}

static void setting_page_imgbtn_2_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.main, guider_ui.main_del, &guider_ui.setting_page_del, setup_scr_main, LV_SCR_LOAD_ANIM_NONE, 100, 100, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_setting_page (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->setting_page, setting_page_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->setting_page_imgbtn_1, setting_page_imgbtn_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->setting_page_RES, setting_page_RES_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->setting_page_READ, setting_page_READ_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->setting_page_DELL, setting_page_DELL_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->setting_page_locker4, setting_page_locker4_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->setting_page_locker3, setting_page_locker3_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->setting_page_VERI, setting_page_VERI_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->setting_page_locker1, setting_page_locker1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->setting_page_locker2, setting_page_locker2_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->setting_page_ALL, setting_page_ALL_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->setting_page_btnm_1, setting_page_btnm_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->setting_page_imgbtn_2, setting_page_imgbtn_2_event_handler, LV_EVENT_ALL, ui);
}


void events_init(lv_ui *ui)
{

}
