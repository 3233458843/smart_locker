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
#include <string.h>
#include "esp_log.h"
#include "lvgl.h"
#include "serve.h"

extern uint8_t g_xst_palm_progress;
extern bool g_palm_progress_updated;

// 取件密码输入状态
static char take_input_phone[5] = "";
static uint8_t take_phone_len = 0;

// 设置页密码输入状态
static char setting_input_pwd[5] = "";
static uint8_t setting_pwd_len = 0;

// 存件手机号输入状态
static char save_input_phone[5] = "";
static uint8_t save_phone_len = 0;

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

// ==================== 取件页面 ====================

static void take_page_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOADED:
    {
        lv_ui * ui = &guider_ui;

        lv_obj_clear_flag(ui->take_page_cont_1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui->take_page_cont_2, LV_OBJ_FLAG_HIDDEN);

        lv_label_set_text(ui->take_page_label_2, "请输入手机号");
        lv_label_set_text(ui->take_page_btn_1_label, "手机号开柜");
        serve_reset_palm_progress();

        lv_label_set_text(ui->take_page_label_3, "_");
        lv_label_set_text(ui->take_page_label_4, "_");
        lv_label_set_text(ui->take_page_label_5, "_");
        lv_label_set_text(ui->take_page_label_6, "_");

        // 请求掌纹验证
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
        lv_ui * ui = &guider_ui;

        lv_obj_t * my_btnm = lv_event_get_target(e);
        uint32_t my_id = lv_buttonmatrix_get_selected_button(my_btnm);
        const char * my_txt = lv_buttonmatrix_get_button_text(my_btnm, my_id);

        if(my_txt == NULL) return;

        if(strcmp(my_txt, "X") == 0 || strcmp(my_txt, "x") == 0) {
            if(take_phone_len > 0) {
                take_phone_len--;
                take_input_phone[take_phone_len] = '\0';
            }
            lv_label_set_text(ui->take_page_label_2, "请输入手机号");
        }
        else if(strcmp(my_txt, "V") == 0 || strcmp(my_txt, "v") == 0) {
            if(take_phone_len == 4) {
                uint8_t phone[4] = {
                    (uint8_t)(take_input_phone[0] - '0'),
                    (uint8_t)(take_input_phone[1] - '0'),
                    (uint8_t)(take_input_phone[2] - '0'),
                    (uint8_t)(take_input_phone[3] - '0'),
                };
                if(serve_request_take_by_phone(phone)) {
                    serve_take_status_t take_status = {0};
                    serve_get_take_status(&take_status);
                    lv_label_set_text_fmt(ui->take_page_label_2, "手机号正确！%d号柜已开", take_status.locker_id + 1);
                } else {
                    lv_label_set_text(ui->take_page_label_2, "手机号错误，请重试！");
                }

                take_phone_len = 0;
                memset(take_input_phone, 0, sizeof(take_input_phone));
            } else {
                lv_label_set_text(ui->take_page_label_2, "必须输入4位手机号！");
            }
        }
        else {
            if(take_phone_len < 4) {
                take_input_phone[take_phone_len] = my_txt[0];
                take_phone_len++;
                take_input_phone[take_phone_len] = '\0';
            }
            lv_label_set_text(ui->take_page_label_2, "请输入手机号");
        }

        char char_str[2] = {0, 0};

        if(take_phone_len > 0) { char_str[0] = take_input_phone[0]; lv_label_set_text(ui->take_page_label_3, char_str); }
        else { lv_label_set_text(ui->take_page_label_3, "_"); }

        if(take_phone_len > 1) { char_str[0] = take_input_phone[1]; lv_label_set_text(ui->take_page_label_4, char_str); }
        else { lv_label_set_text(ui->take_page_label_4, "_"); }

        if(take_phone_len > 2) { char_str[0] = take_input_phone[2]; lv_label_set_text(ui->take_page_label_5, char_str); }
        else { lv_label_set_text(ui->take_page_label_5, "_"); }

        if(take_phone_len > 3) { char_str[0] = take_input_phone[3]; lv_label_set_text(ui->take_page_label_6, char_str); }
        else { lv_label_set_text(ui->take_page_label_6, "_"); }
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
    lv_obj_add_event_cb(ui->take_page_btnm_1, take_page_btnm_1_event_handler, LV_EVENT_VALUE_CHANGED, ui);
}

// ==================== 存件页面 ====================

static void save_page_btnm_handler(lv_event_t *e){
    lv_obj_t *btnm = lv_event_get_target(e);
    if (!lv_obj_is_valid(btnm)) return;
    uint32_t id = lv_buttonmatrix_get_selected_button(btnm);
    const char *txt = lv_buttonmatrix_get_button_text(btnm, id);
    if (txt == NULL) return;

    lv_ui *ui = &guider_ui;

    if (strcmp(txt, "X") == 0){
        if (save_phone_len > 0){
            save_phone_len--;
            save_input_phone[save_phone_len] = '\0';
        }
    } else if (strcmp(txt, "V") == 0){
        if (save_phone_len == 4){
            // 切换为掌纹录入界面
            lv_obj_add_flag(ui->save_page_cont_2, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui->save_page_animimg_1, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui->save_page_bar_1, LV_OBJ_FLAG_HIDDEN);
            lv_label_set_text(ui->save_page_label_1, "将手掌悬停于传感器前10cm左右\n等待设备发出滴声，柜门即可弹开");
            lv_obj_set_pos(ui->save_page_label_2, 200, 50);
            lv_obj_set_size(ui->save_page_label_2, 50, 20);
            lv_bar_set_value(ui->save_page_bar_1, 0, LV_ANIM_OFF);
            lv_label_set_text(ui->save_page_label_2, "0%");
            lv_animimg_start(ui->save_page_animimg_1);

            // 启动存件流程 (带手机号)
            {
                uint8_t phone[4] = {
                    (uint8_t)(save_input_phone[0] - '0'),
                    (uint8_t)(save_input_phone[1] - '0'),
                    (uint8_t)(save_input_phone[2] - '0'),
                    (uint8_t)(save_input_phone[3] - '0'),
                };
                if (!serve_request_save_with_phone(phone)){
                    lv_label_set_text(ui->save_page_label_1, "系统繁忙，请稍后重试");
                }
            }
            return; // 跳转到录入界面，不执行底部数字更新
        }
    } else if (save_phone_len < 4){
        save_input_phone[save_phone_len] = txt[0];
        save_phone_len++;
        save_input_phone[save_phone_len] = '\0';
    }

    // 更新4位手机号显示 (label_4~7, 未输入显示"-")
    lv_label_set_text(ui->save_page_label_4, save_phone_len > 0 ? (char[]){save_input_phone[0], 0} : "-");
    lv_label_set_text(ui->save_page_label_5, save_phone_len > 1 ? (char[]){save_input_phone[1], 0} : "-");
    lv_label_set_text(ui->save_page_label_6, save_phone_len > 2 ? (char[]){save_input_phone[2], 0} : "-");
    lv_label_set_text(ui->save_page_label_7, save_phone_len > 3 ? (char[]){save_input_phone[3], 0} : "-");
}

static void save_page_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOADED:
    {
        lv_ui * ui = &guider_ui;

        // 初始化存件页：手机号输入界面 (未输入显示"-")
        lv_obj_clear_flag(ui->save_page_cont_2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui->save_page_animimg_1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui->save_page_bar_1, LV_OBJ_FLAG_HIDDEN);

        lv_label_set_text(ui->save_page_label_1, "请输入手机号(4位)");
        lv_label_set_text(ui->save_page_label_2, "");
        lv_label_set_text(ui->save_page_label_4, "-");
        lv_label_set_text(ui->save_page_label_5, "-");
        lv_label_set_text(ui->save_page_label_6, "-");
        lv_label_set_text(ui->save_page_label_7, "-");

        save_phone_len = 0;
        memset(save_input_phone, 0, sizeof(save_input_phone));
        serve_reset_palm_progress();
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
    lv_obj_add_event_cb(ui->save_page_btnm_1, save_page_btnm_handler, LV_EVENT_VALUE_CHANGED, ui);
}

// ==================== 帮助页面 ====================

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

// ==================== 设置页面 ====================

static void setting_page_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOADED:
    {
        lv_ui * ui = &guider_ui;

        lv_obj_clear_flag(ui->setting_page_cont_2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui->setting_page_cont_1, LV_OBJ_FLAG_HIDDEN);

        lv_label_set_text(ui->setting_page_label_1, "_");
        lv_label_set_text(ui->setting_page_label_2, "_");
        lv_label_set_text(ui->setting_page_label_3, "_");
        lv_label_set_text(ui->setting_page_label_4, "_");
        lv_label_set_text(ui->setting_page_label_5, "请输入管理员密码");
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
        lv_label_set_text(guider_ui.setting_page_NOTE,
            serve_admin_reset_xst() ? "模组复位成功" : "模组复位失败");
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
        int cnt = serve_admin_get_user_count();
        if (cnt >= 0) {
            lv_label_set_text_fmt(guider_ui.setting_page_NOTE, "当前用户数: %d", cnt);
        } else {
            lv_label_set_text(guider_ui.setting_page_NOTE, "读取用户失败");
        }
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
        lv_label_set_text(guider_ui.setting_page_NOTE,
            serve_admin_del_all_users() ? "已删除所有用户" : "删除用户失败");
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
        lv_label_set_text(guider_ui.setting_page_NOTE,
            serve_admin_open_locker(3) ? "4号柜已开" : "4号柜开锁失败");
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
        lv_label_set_text(guider_ui.setting_page_NOTE,
            serve_admin_open_locker(2) ? "3号柜已开" : "3号柜开锁失败");
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
        lv_label_set_text(guider_ui.setting_page_NOTE,
            serve_request_debug_verify() ? "验证指令已发送" : "验证服务不可用");
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
        lv_label_set_text(guider_ui.setting_page_NOTE,
            serve_admin_open_locker(0) ? "1号柜已开" : "1号柜开锁失败");
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
        lv_label_set_text(guider_ui.setting_page_NOTE,
            serve_admin_open_locker(1) ? "2号柜已开" : "2号柜开锁失败");
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
        lv_label_set_text(guider_ui.setting_page_NOTE,
            serve_admin_open_all_lockers() ? "所有柜已开" : "开锁失败");
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
        lv_ui * ui = &guider_ui;

        lv_obj_t * my_btnm = lv_event_get_target(e);
        uint32_t my_id = lv_buttonmatrix_get_selected_button(my_btnm);
        const char * my_txt = lv_buttonmatrix_get_button_text(my_btnm, my_id);

        if(my_txt == NULL) return;

        if(strcmp(my_txt, "X") == 0 || strcmp(my_txt, "x") == 0) {
            if(setting_pwd_len > 0) {
                setting_pwd_len--;
                setting_input_pwd[setting_pwd_len] = '\0';
            }
        }
        else if(strcmp(my_txt, "V") == 0 || strcmp(my_txt, "v") == 0) {
            if(setting_pwd_len == 4) {
                if(serve_admin_verify_password(setting_input_pwd, 4)) {
                    lv_obj_add_flag(ui->setting_page_cont_2, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(ui->setting_page_cont_1, LV_OBJ_FLAG_HIDDEN);
                } else {
                    lv_label_set_text(ui->setting_page_label_5, "密码错误！请重试！");
                }
                setting_pwd_len = 0;
                memset(setting_input_pwd, 0, sizeof(setting_input_pwd));
            } else {
                lv_label_set_text(ui->setting_page_label_5, "必须输入4位密码！");
            }
        }
        else {
            if(setting_pwd_len < 4) {
                setting_input_pwd[setting_pwd_len] = my_txt[0];
                setting_pwd_len++;
                setting_input_pwd[setting_pwd_len] = '\0';
            }
        }

        char char_str[2] = {0, 0};

        if(setting_pwd_len > 0) { char_str[0] = setting_input_pwd[0]; lv_label_set_text(ui->setting_page_label_1, char_str); }
        else { lv_label_set_text(ui->setting_page_label_1, "_"); }

        if(setting_pwd_len > 1) { char_str[0] = setting_input_pwd[1]; lv_label_set_text(ui->setting_page_label_2, char_str); }
        else { lv_label_set_text(ui->setting_page_label_2, "_"); }

        if(setting_pwd_len > 2) { char_str[0] = setting_input_pwd[2]; lv_label_set_text(ui->setting_page_label_3, char_str); }
        else { lv_label_set_text(ui->setting_page_label_3, "_"); }

        if(setting_pwd_len > 3) { char_str[0] = setting_input_pwd[3]; lv_label_set_text(ui->setting_page_label_4, char_str); }
        else { lv_label_set_text(ui->setting_page_label_4, "_"); }
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
    lv_obj_add_event_cb(ui->setting_page_btnm_1, setting_page_btnm_1_event_handler, LV_EVENT_VALUE_CHANGED, ui);
    lv_obj_add_event_cb(ui->setting_page_imgbtn_2, setting_page_imgbtn_2_event_handler, LV_EVENT_ALL, ui);
}

void events_init(lv_ui *ui)
{
}
