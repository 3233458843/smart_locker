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
        ui_load_scr_animation(&guider_ui, &guider_ui.setting_item, guider_ui.setting_item_del, &guider_ui.main_del, setup_scr_setting_item, LV_SCR_LOAD_ANIM_NONE, 100, 100, true, true);
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

void events_init_setting_item (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->setting_item, setting_item_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->setting_item_imgbtn_1, setting_item_imgbtn_1_event_handler, LV_EVENT_ALL, ui);
}


void events_init(lv_ui *ui)
{

}
