/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"



void setup_scr_setting_page(lv_ui *ui)
{
    //Write codes setting_page
    ui->setting_page = lv_obj_create(NULL);
    lv_obj_set_size(ui->setting_page, 240, 320);
    lv_obj_set_scrollbar_mode(ui->setting_page, LV_SCROLLBAR_MODE_OFF);

    //Write style for setting_page, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->setting_page, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_cont_1
    ui->setting_page_cont_1 = lv_obj_create(ui->setting_page);
    lv_obj_set_pos(ui->setting_page_cont_1, 0, 0);
    lv_obj_set_size(ui->setting_page_cont_1, 240, 320);
    lv_obj_set_scrollbar_mode(ui->setting_page_cont_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for setting_page_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_page_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_page_cont_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_page_cont_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->setting_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_imgbtn_1
    ui->setting_page_imgbtn_1 = lv_imagebutton_create(ui->setting_page_cont_1);
    lv_obj_set_pos(ui->setting_page_imgbtn_1, 10, 9);
    lv_obj_set_size(ui->setting_page_imgbtn_1, 50, 50);
    lv_obj_add_flag(ui->setting_page_imgbtn_1, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->setting_page_imgbtn_1, LV_IMAGEBUTTON_STATE_RELEASED, &_back_RGB565A8_50x50, NULL, NULL);
    ui->setting_page_imgbtn_1_label = lv_label_create(ui->setting_page_imgbtn_1);
    lv_label_set_text(ui->setting_page_imgbtn_1_label, "");
    lv_label_set_long_mode(ui->setting_page_imgbtn_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->setting_page_imgbtn_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->setting_page_imgbtn_1, 0, LV_STATE_DEFAULT);

    //Write style for setting_page_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->setting_page_imgbtn_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_page_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_page_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_page_imgbtn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for setting_page_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->setting_page_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->setting_page_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->setting_page_imgbtn_1, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->setting_page_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->setting_page_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->setting_page_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for setting_page_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->setting_page_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->setting_page_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->setting_page_imgbtn_1, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->setting_page_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->setting_page_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->setting_page_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for setting_page_imgbtn_1, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->setting_page_imgbtn_1, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->setting_page_imgbtn_1, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //Write codes setting_page_spangroup_1
    ui->setting_page_spangroup_1 = lv_spangroup_create(ui->setting_page_cont_1);
    lv_obj_set_pos(ui->setting_page_spangroup_1, 0, 80);
    lv_obj_set_size(ui->setting_page_spangroup_1, 180, 220);
    lv_spangroup_set_align(ui->setting_page_spangroup_1, LV_TEXT_ALIGN_LEFT);
    lv_spangroup_set_overflow(ui->setting_page_spangroup_1, LV_SPAN_OVERFLOW_CLIP);
    lv_spangroup_set_mode(ui->setting_page_spangroup_1, LV_SPAN_MODE_BREAK);
    //create span
    ui->setting_page_spangroup_1_span = lv_spangroup_new_span(ui->setting_page_spangroup_1);
    lv_span_set_text(ui->setting_page_spangroup_1_span, "hello");
    lv_style_set_text_color(lv_span_get_style(ui->setting_page_spangroup_1_span), lv_color_hex(0x000000));
    lv_style_set_text_decor(lv_span_get_style(ui->setting_page_spangroup_1_span), LV_TEXT_DECOR_NONE);
    lv_style_set_text_font(lv_span_get_style(ui->setting_page_spangroup_1_span), &lv_font_montserratMedium_12);

    //Write style state: LV_STATE_DEFAULT for &style_setting_page_spangroup_1_main_main_default
    static lv_style_t style_setting_page_spangroup_1_main_main_default;
    ui_init_style(&style_setting_page_spangroup_1_main_main_default);

    lv_style_set_border_width(&style_setting_page_spangroup_1_main_main_default, 1);
    lv_style_set_border_opa(&style_setting_page_spangroup_1_main_main_default, 255);
    lv_style_set_border_color(&style_setting_page_spangroup_1_main_main_default, lv_color_hex(0x000000));
    lv_style_set_border_side(&style_setting_page_spangroup_1_main_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(&style_setting_page_spangroup_1_main_main_default, 5);
    lv_style_set_bg_opa(&style_setting_page_spangroup_1_main_main_default, 0);
    lv_style_set_pad_top(&style_setting_page_spangroup_1_main_main_default, 0);
    lv_style_set_pad_right(&style_setting_page_spangroup_1_main_main_default, 0);
    lv_style_set_pad_bottom(&style_setting_page_spangroup_1_main_main_default, 0);
    lv_style_set_pad_left(&style_setting_page_spangroup_1_main_main_default, 0);
    lv_style_set_shadow_width(&style_setting_page_spangroup_1_main_main_default, 0);
    lv_obj_add_style(ui->setting_page_spangroup_1, &style_setting_page_spangroup_1_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_spangroup_refr_mode(ui->setting_page_spangroup_1);

    //Write codes setting_page_cont_3
    ui->setting_page_cont_3 = lv_obj_create(ui->setting_page_cont_1);
    lv_obj_set_pos(ui->setting_page_cont_3, 190, 0);
    lv_obj_set_size(ui->setting_page_cont_3, 50, 320);
    lv_obj_set_scrollbar_mode(ui->setting_page_cont_3, LV_SCROLLBAR_MODE_OFF);

    //Write style for setting_page_cont_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_page_cont_3, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->setting_page_cont_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->setting_page_cont_3, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->setting_page_cont_3, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_cont_3, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_page_cont_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_page_cont_3, lv_color_hex(0xf5f5f5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_page_cont_3, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_page_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->setting_page_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_page_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_page_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_RES
    ui->setting_page_RES = lv_button_create(ui->setting_page_cont_3);
    lv_obj_set_pos(ui->setting_page_RES, 0, 0);
    lv_obj_set_size(ui->setting_page_RES, 50, 49);
    ui->setting_page_RES_label = lv_label_create(ui->setting_page_RES);
    lv_label_set_text(ui->setting_page_RES_label, "RES");
    lv_label_set_long_mode(ui->setting_page_RES_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->setting_page_RES_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->setting_page_RES, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->setting_page_RES_label, LV_PCT(100));

    //Write style for setting_page_RES, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->setting_page_RES, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_page_RES, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_page_RES, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->setting_page_RES, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_RES, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_RES, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_page_RES, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_page_RES, &lv_font_Lemi_Little_Milk_Foam_Font_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_page_RES, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_page_RES, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_READ
    ui->setting_page_READ = lv_button_create(ui->setting_page_cont_3);
    lv_obj_set_pos(ui->setting_page_READ, 0, 79);
    lv_obj_set_size(ui->setting_page_READ, 50, 49);
    ui->setting_page_READ_label = lv_label_create(ui->setting_page_READ);
    lv_label_set_text(ui->setting_page_READ_label, "READ");
    lv_label_set_long_mode(ui->setting_page_READ_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->setting_page_READ_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->setting_page_READ, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->setting_page_READ_label, LV_PCT(100));

    //Write style for setting_page_READ, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->setting_page_READ, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_page_READ, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_page_READ, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->setting_page_READ, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_READ, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_READ, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_page_READ, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_page_READ, &lv_font_Lemi_Little_Milk_Foam_Font_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_page_READ, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_page_READ, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_DELL
    ui->setting_page_DELL = lv_button_create(ui->setting_page_cont_3);
    lv_obj_set_pos(ui->setting_page_DELL, 0, 160);
    lv_obj_set_size(ui->setting_page_DELL, 50, 49);
    ui->setting_page_DELL_label = lv_label_create(ui->setting_page_DELL);
    lv_label_set_text(ui->setting_page_DELL_label, "DELL");
    lv_label_set_long_mode(ui->setting_page_DELL_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->setting_page_DELL_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->setting_page_DELL, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->setting_page_DELL_label, LV_PCT(100));

    //Write style for setting_page_DELL, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->setting_page_DELL, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_page_DELL, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_page_DELL, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->setting_page_DELL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_DELL, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_DELL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_page_DELL, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_page_DELL, &lv_font_Lemi_Little_Milk_Foam_Font_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_page_DELL, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_page_DELL, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_locker4
    ui->setting_page_locker4 = lv_button_create(ui->setting_page_cont_3);
    lv_obj_set_pos(ui->setting_page_locker4, 0, 560);
    lv_obj_set_size(ui->setting_page_locker4, 50, 49);
    ui->setting_page_locker4_label = lv_label_create(ui->setting_page_locker4);
    lv_label_set_text(ui->setting_page_locker4_label, "locker4");
    lv_label_set_long_mode(ui->setting_page_locker4_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->setting_page_locker4_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->setting_page_locker4, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->setting_page_locker4_label, LV_PCT(100));

    //Write style for setting_page_locker4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->setting_page_locker4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_page_locker4, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_page_locker4, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->setting_page_locker4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_locker4, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_locker4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_page_locker4, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_page_locker4, &lv_font_Lemi_Little_Milk_Foam_Font_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_page_locker4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_page_locker4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_locker3
    ui->setting_page_locker3 = lv_button_create(ui->setting_page_cont_3);
    lv_obj_set_pos(ui->setting_page_locker3, 0, 480);
    lv_obj_set_size(ui->setting_page_locker3, 50, 49);
    ui->setting_page_locker3_label = lv_label_create(ui->setting_page_locker3);
    lv_label_set_text(ui->setting_page_locker3_label, "locker3");
    lv_label_set_long_mode(ui->setting_page_locker3_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->setting_page_locker3_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->setting_page_locker3, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->setting_page_locker3_label, LV_PCT(100));

    //Write style for setting_page_locker3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->setting_page_locker3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_page_locker3, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_page_locker3, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->setting_page_locker3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_locker3, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_locker3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_page_locker3, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_page_locker3, &lv_font_Lemi_Little_Milk_Foam_Font_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_page_locker3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_page_locker3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_VERI
    ui->setting_page_VERI = lv_button_create(ui->setting_page_cont_3);
    lv_obj_set_pos(ui->setting_page_VERI, 0, 240);
    lv_obj_set_size(ui->setting_page_VERI, 50, 49);
    ui->setting_page_VERI_label = lv_label_create(ui->setting_page_VERI);
    lv_label_set_text(ui->setting_page_VERI_label, "VERI");
    lv_label_set_long_mode(ui->setting_page_VERI_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->setting_page_VERI_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->setting_page_VERI, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->setting_page_VERI_label, LV_PCT(100));

    //Write style for setting_page_VERI, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->setting_page_VERI, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_page_VERI, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_page_VERI, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->setting_page_VERI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_VERI, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_VERI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_page_VERI, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_page_VERI, &lv_font_Lemi_Little_Milk_Foam_Font_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_page_VERI, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_page_VERI, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_locker1
    ui->setting_page_locker1 = lv_button_create(ui->setting_page_cont_3);
    lv_obj_set_pos(ui->setting_page_locker1, -1, 320);
    lv_obj_set_size(ui->setting_page_locker1, 50, 49);
    ui->setting_page_locker1_label = lv_label_create(ui->setting_page_locker1);
    lv_label_set_text(ui->setting_page_locker1_label, "locker1");
    lv_label_set_long_mode(ui->setting_page_locker1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->setting_page_locker1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->setting_page_locker1, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->setting_page_locker1_label, LV_PCT(100));

    //Write style for setting_page_locker1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->setting_page_locker1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_page_locker1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_page_locker1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->setting_page_locker1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_locker1, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_locker1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_page_locker1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_page_locker1, &lv_font_Lemi_Little_Milk_Foam_Font_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_page_locker1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_page_locker1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_locker2
    ui->setting_page_locker2 = lv_button_create(ui->setting_page_cont_3);
    lv_obj_set_pos(ui->setting_page_locker2, 0, 400);
    lv_obj_set_size(ui->setting_page_locker2, 50, 49);
    ui->setting_page_locker2_label = lv_label_create(ui->setting_page_locker2);
    lv_label_set_text(ui->setting_page_locker2_label, "locker2");
    lv_label_set_long_mode(ui->setting_page_locker2_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->setting_page_locker2_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->setting_page_locker2, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->setting_page_locker2_label, LV_PCT(100));

    //Write style for setting_page_locker2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->setting_page_locker2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_page_locker2, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_page_locker2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->setting_page_locker2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_locker2, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_locker2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_page_locker2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_page_locker2, &lv_font_Lemi_Little_Milk_Foam_Font_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_page_locker2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_page_locker2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_ALL
    ui->setting_page_ALL = lv_button_create(ui->setting_page_cont_3);
    lv_obj_set_pos(ui->setting_page_ALL, 0, 640);
    lv_obj_set_size(ui->setting_page_ALL, 50, 49);
    ui->setting_page_ALL_label = lv_label_create(ui->setting_page_ALL);
    lv_label_set_text(ui->setting_page_ALL_label, "all_out");
    lv_label_set_long_mode(ui->setting_page_ALL_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->setting_page_ALL_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->setting_page_ALL, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->setting_page_ALL_label, LV_PCT(100));

    //Write style for setting_page_ALL, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->setting_page_ALL, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_page_ALL, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_page_ALL, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->setting_page_ALL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_ALL, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_ALL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_page_ALL, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_page_ALL, &lv_font_Lemi_Little_Milk_Foam_Font_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_page_ALL, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_page_ALL, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_NOTE
    ui->setting_page_NOTE = lv_label_create(ui->setting_page_cont_1);
    lv_obj_set_pos(ui->setting_page_NOTE, 62, 14);
    lv_obj_set_size(ui->setting_page_NOTE, 120, 34);
    lv_label_set_text(ui->setting_page_NOTE, "");
    lv_label_set_long_mode(ui->setting_page_NOTE, LV_LABEL_LONG_WRAP);

    //Write style for setting_page_NOTE, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_page_NOTE, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_NOTE, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_page_NOTE, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_page_NOTE, &lv_font_Lemi_Little_Milk_Foam_Font_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_page_NOTE, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->setting_page_NOTE, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->setting_page_NOTE, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_page_NOTE, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_page_NOTE, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_page_NOTE, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_page_NOTE, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->setting_page_NOTE, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_page_NOTE, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_NOTE, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_cont_2
    ui->setting_page_cont_2 = lv_obj_create(ui->setting_page);
    lv_obj_set_pos(ui->setting_page_cont_2, 0, 0);
    lv_obj_set_size(ui->setting_page_cont_2, 240, 320);
    lv_obj_set_scrollbar_mode(ui->setting_page_cont_2, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->setting_page_cont_2, LV_OBJ_FLAG_HIDDEN);

    //Write style for setting_page_cont_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_page_cont_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_page_cont_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_page_cont_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->setting_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_btnm_1
    ui->setting_page_btnm_1 = lv_buttonmatrix_create(ui->setting_page_cont_2);
    lv_obj_set_pos(ui->setting_page_btnm_1, 10, 139);
    lv_obj_set_size(ui->setting_page_btnm_1, 220, 170);
    static const char *setting_page_btnm_1_text_map[] = {"1", "2", "3", "\n", "4", "5", "6", "\n", "7", "8", "9", "\n", "X", "0", "V", "",};
    lv_buttonmatrix_set_map(ui->setting_page_btnm_1, setting_page_btnm_1_text_map);

    //Write style for setting_page_btnm_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_page_btnm_1, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->setting_page_btnm_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->setting_page_btnm_1, lv_color_hex(0xc9c9c9), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->setting_page_btnm_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_page_btnm_1, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->setting_page_btnm_1, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_page_btnm_1, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_page_btnm_1, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui->setting_page_btnm_1, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui->setting_page_btnm_1, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_btnm_1, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_page_btnm_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_page_btnm_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_page_btnm_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for setting_page_btnm_1, Part: LV_PART_ITEMS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_page_btnm_1, 1, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->setting_page_btnm_1, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->setting_page_btnm_1, lv_color_hex(0xc9c9c9), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->setting_page_btnm_1, LV_BORDER_SIDE_FULL, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_page_btnm_1, lv_color_hex(0xffffff), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_page_btnm_1, &lv_font_montserratMedium_16, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_page_btnm_1, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_btnm_1, 4, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_page_btnm_1, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_page_btnm_1, lv_color_hex(0x2195f6), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_page_btnm_1, LV_GRAD_DIR_NONE, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_btnm_1, 0, LV_PART_ITEMS|LV_STATE_DEFAULT);

    //Write codes setting_page_label_4
    ui->setting_page_label_4 = lv_label_create(ui->setting_page_cont_2);
    lv_obj_set_pos(ui->setting_page_label_4, 183, 100);
    lv_obj_set_size(ui->setting_page_label_4, 30, 20);
    lv_label_set_text(ui->setting_page_label_4, "4");
    lv_label_set_long_mode(ui->setting_page_label_4, LV_LABEL_LONG_WRAP);

    //Write style for setting_page_label_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_page_label_4, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_page_label_4, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_page_label_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->setting_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->setting_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_page_label_4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->setting_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_label_3
    ui->setting_page_label_3 = lv_label_create(ui->setting_page_cont_2);
    lv_obj_set_pos(ui->setting_page_label_3, 130, 100);
    lv_obj_set_size(ui->setting_page_label_3, 30, 20);
    lv_label_set_text(ui->setting_page_label_3, "3");
    lv_label_set_long_mode(ui->setting_page_label_3, LV_LABEL_LONG_WRAP);

    //Write style for setting_page_label_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_page_label_3, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_page_label_3, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_page_label_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->setting_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->setting_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_page_label_3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->setting_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_label_1
    ui->setting_page_label_1 = lv_label_create(ui->setting_page_cont_2);
    lv_obj_set_pos(ui->setting_page_label_1, 20, 100);
    lv_obj_set_size(ui->setting_page_label_1, 30, 20);
    lv_label_set_text(ui->setting_page_label_1, "1");
    lv_label_set_long_mode(ui->setting_page_label_1, LV_LABEL_LONG_WRAP);

    //Write style for setting_page_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_page_label_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_page_label_1, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_page_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->setting_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->setting_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_page_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->setting_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_imgbtn_2
    ui->setting_page_imgbtn_2 = lv_imagebutton_create(ui->setting_page_cont_2);
    lv_obj_set_pos(ui->setting_page_imgbtn_2, 10, 10);
    lv_obj_set_size(ui->setting_page_imgbtn_2, 50, 50);
    lv_obj_add_flag(ui->setting_page_imgbtn_2, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->setting_page_imgbtn_2, LV_IMAGEBUTTON_STATE_RELEASED, &_back_RGB565A8_50x50, NULL, NULL);
    ui->setting_page_imgbtn_2_label = lv_label_create(ui->setting_page_imgbtn_2);
    lv_label_set_text(ui->setting_page_imgbtn_2_label, "");
    lv_label_set_long_mode(ui->setting_page_imgbtn_2_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->setting_page_imgbtn_2_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->setting_page_imgbtn_2, 0, LV_STATE_DEFAULT);

    //Write style for setting_page_imgbtn_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->setting_page_imgbtn_2, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_page_imgbtn_2, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_page_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_page_imgbtn_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for setting_page_imgbtn_2, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->setting_page_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->setting_page_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->setting_page_imgbtn_2, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->setting_page_imgbtn_2, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->setting_page_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->setting_page_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for setting_page_imgbtn_2, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->setting_page_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->setting_page_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->setting_page_imgbtn_2, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->setting_page_imgbtn_2, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->setting_page_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->setting_page_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for setting_page_imgbtn_2, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->setting_page_imgbtn_2, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->setting_page_imgbtn_2, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //Write codes setting_page_label_2
    ui->setting_page_label_2 = lv_label_create(ui->setting_page_cont_2);
    lv_obj_set_pos(ui->setting_page_label_2, 73, 100);
    lv_obj_set_size(ui->setting_page_label_2, 30, 20);
    lv_label_set_text(ui->setting_page_label_2, "2");
    lv_label_set_long_mode(ui->setting_page_label_2, LV_LABEL_LONG_WRAP);

    //Write style for setting_page_label_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_page_label_2, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_page_label_2, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_page_label_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->setting_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->setting_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_page_label_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->setting_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_img_1
    ui->setting_page_img_1 = lv_image_create(ui->setting_page_cont_2);
    lv_obj_set_pos(ui->setting_page_img_1, 160, 0);
    lv_obj_set_size(ui->setting_page_img_1, 80, 80);
    lv_obj_add_flag(ui->setting_page_img_1, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->setting_page_img_1, &_2_RGB565A8_80x80);
    lv_image_set_pivot(ui->setting_page_img_1, 50,50);
    lv_image_set_rotation(ui->setting_page_img_1, 0);

    //Write style for setting_page_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->setting_page_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->setting_page_img_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_label_5
    ui->setting_page_label_5 = lv_label_create(ui->setting_page_cont_2);
    lv_obj_set_pos(ui->setting_page_label_5, 81, 32);
    lv_obj_set_size(ui->setting_page_label_5, 76, 48);
    lv_label_set_text(ui->setting_page_label_5, "密码对了才能进去哦~omage~\n必须输入4位错误请重试！");
    lv_label_set_long_mode(ui->setting_page_label_5, LV_LABEL_LONG_WRAP);

    //Write style for setting_page_label_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_page_label_5, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_page_label_5, &lv_font_Lemi_Little_Milk_Foam_Font_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_page_label_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->setting_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->setting_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_page_label_5, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->setting_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of setting_page.


    //Update current screen layout.
    lv_obj_update_layout(ui->setting_page);

    //Init events for screen.
    events_init_setting_page(ui);
}
