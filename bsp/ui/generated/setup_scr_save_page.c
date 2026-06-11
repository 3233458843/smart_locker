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



void setup_scr_save_page(lv_ui *ui)
{
    //Write codes save_page
    ui->save_page = lv_obj_create(NULL);
    lv_obj_set_size(ui->save_page, 240, 320);
    lv_obj_set_scrollbar_mode(ui->save_page, LV_SCROLLBAR_MODE_OFF);

    //Write style for save_page, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->save_page, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes save_page_cont_1
    ui->save_page_cont_1 = lv_obj_create(ui->save_page);
    lv_obj_set_pos(ui->save_page_cont_1, 0, 0);
    lv_obj_set_size(ui->save_page_cont_1, 240, 320);
    lv_obj_set_scrollbar_mode(ui->save_page_cont_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for save_page_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->save_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->save_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->save_page_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->save_page_cont_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->save_page_cont_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->save_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->save_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->save_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->save_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->save_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes save_page_imgbtn_1
    ui->save_page_imgbtn_1 = lv_imagebutton_create(ui->save_page_cont_1);
    lv_obj_set_pos(ui->save_page_imgbtn_1, 10, 10);
    lv_obj_set_size(ui->save_page_imgbtn_1, 50, 50);
    lv_obj_add_flag(ui->save_page_imgbtn_1, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->save_page_imgbtn_1, LV_IMAGEBUTTON_STATE_RELEASED, &_back_RGB565A8_50x50, NULL, NULL);
    ui->save_page_imgbtn_1_label = lv_label_create(ui->save_page_imgbtn_1);
    lv_label_set_text(ui->save_page_imgbtn_1_label, "");
    lv_label_set_long_mode(ui->save_page_imgbtn_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->save_page_imgbtn_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->save_page_imgbtn_1, 0, LV_STATE_DEFAULT);

    //Write style for save_page_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->save_page_imgbtn_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->save_page_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->save_page_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->save_page_imgbtn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->save_page_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for save_page_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->save_page_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->save_page_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->save_page_imgbtn_1, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->save_page_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->save_page_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->save_page_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for save_page_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->save_page_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->save_page_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->save_page_imgbtn_1, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->save_page_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->save_page_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->save_page_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for save_page_imgbtn_1, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->save_page_imgbtn_1, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->save_page_imgbtn_1, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //Write codes save_page_animimg_1
    ui->save_page_animimg_1 = lv_animimg_create(ui->save_page_cont_1);
    lv_obj_set_pos(ui->save_page_animimg_1, 40, 150);
    lv_obj_set_size(ui->save_page_animimg_1, 160, 160);
    lv_animimg_set_src(ui->save_page_animimg_1, (const void **) save_page_animimg_1_imgs, 40);
    lv_animimg_set_duration(ui->save_page_animimg_1, 60*40);
    lv_animimg_set_repeat_count(ui->save_page_animimg_1, LV_ANIM_REPEAT_INFINITE);
    lv_animimg_start(ui->save_page_animimg_1);

    //Write codes save_page_label_1
    ui->save_page_label_1 = lv_label_create(ui->save_page_cont_1);
    lv_obj_set_pos(ui->save_page_label_1, 27, 85);
    lv_obj_set_size(ui->save_page_label_1, 185, 69);
    lv_label_set_text(ui->save_page_label_1, "将手掌悬停于传感器前10cm左右\n等待设备发出“滴”声，柜门即可弹开");
    lv_label_set_long_mode(ui->save_page_label_1, LV_LABEL_LONG_WRAP);

    //Write style for save_page_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->save_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->save_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->save_page_label_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->save_page_label_1, &lv_font_Lemi_Little_Milk_Foam_Font_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->save_page_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->save_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->save_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->save_page_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->save_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->save_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->save_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->save_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->save_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->save_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes save_page_bar_1
    ui->save_page_bar_1 = lv_bar_create(ui->save_page_cont_1);
    lv_obj_set_pos(ui->save_page_bar_1, 98, 50);
    lv_obj_set_size(ui->save_page_bar_1, 100, 20);
    lv_obj_set_style_anim_duration(ui->save_page_bar_1, 1000, 0);
    lv_bar_set_mode(ui->save_page_bar_1, LV_BAR_MODE_NORMAL);
    lv_bar_set_range(ui->save_page_bar_1, 0, 100);
    lv_bar_set_value(ui->save_page_bar_1, 50, LV_ANIM_OFF);

    //Write style for save_page_bar_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->save_page_bar_1, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->save_page_bar_1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->save_page_bar_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->save_page_bar_1, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->save_page_bar_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for save_page_bar_1, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->save_page_bar_1, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->save_page_bar_1, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->save_page_bar_1, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->save_page_bar_1, 10, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write codes save_page_label_2
    ui->save_page_label_2 = lv_label_create(ui->save_page_cont_1);
    lv_obj_set_pos(ui->save_page_label_2, 137, 23);
    lv_obj_set_size(ui->save_page_label_2, 20, 15);
    lv_label_set_text(ui->save_page_label_2, "2");
    lv_label_set_long_mode(ui->save_page_label_2, LV_LABEL_LONG_WRAP);

    //Write style for save_page_label_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->save_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->save_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->save_page_label_2, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->save_page_label_2, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->save_page_label_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->save_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->save_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->save_page_label_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->save_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->save_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->save_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->save_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->save_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->save_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes save_page_cont_2
    ui->save_page_cont_2 = lv_obj_create(ui->save_page);
    lv_obj_set_pos(ui->save_page_cont_2, 0, 0);
    lv_obj_set_size(ui->save_page_cont_2, 240, 320);
    lv_obj_set_scrollbar_mode(ui->save_page_cont_2, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->save_page_cont_2, LV_OBJ_FLAG_HIDDEN);

    //Write style for save_page_cont_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->save_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->save_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->save_page_cont_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->save_page_cont_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->save_page_cont_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->save_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->save_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->save_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->save_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->save_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes save_page_imgbtn_2
    ui->save_page_imgbtn_2 = lv_imagebutton_create(ui->save_page_cont_2);
    lv_obj_set_pos(ui->save_page_imgbtn_2, 10, 10);
    lv_obj_set_size(ui->save_page_imgbtn_2, 50, 50);
    lv_obj_add_flag(ui->save_page_imgbtn_2, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->save_page_imgbtn_2, LV_IMAGEBUTTON_STATE_RELEASED, &_back_RGB565A8_50x50, NULL, NULL);
    ui->save_page_imgbtn_2_label = lv_label_create(ui->save_page_imgbtn_2);
    lv_label_set_text(ui->save_page_imgbtn_2_label, "");
    lv_label_set_long_mode(ui->save_page_imgbtn_2_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->save_page_imgbtn_2_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->save_page_imgbtn_2, 0, LV_STATE_DEFAULT);

    //Write style for save_page_imgbtn_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->save_page_imgbtn_2, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->save_page_imgbtn_2, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->save_page_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->save_page_imgbtn_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->save_page_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for save_page_imgbtn_2, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->save_page_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->save_page_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->save_page_imgbtn_2, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->save_page_imgbtn_2, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->save_page_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->save_page_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for save_page_imgbtn_2, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->save_page_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->save_page_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->save_page_imgbtn_2, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->save_page_imgbtn_2, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->save_page_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->save_page_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for save_page_imgbtn_2, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->save_page_imgbtn_2, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->save_page_imgbtn_2, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //Write codes save_page_label_3
    ui->save_page_label_3 = lv_label_create(ui->save_page_cont_2);
    lv_obj_set_pos(ui->save_page_label_3, 63, 32);
    lv_obj_set_size(ui->save_page_label_3, 166, 26);
    lv_label_set_text(ui->save_page_label_3, "请输入4位手机号");
    lv_label_set_long_mode(ui->save_page_label_3, LV_LABEL_LONG_WRAP);

    //Write style for save_page_label_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->save_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->save_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->save_page_label_3, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->save_page_label_3, &lv_font_Lemi_Little_Milk_Foam_Font_21, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->save_page_label_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->save_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->save_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->save_page_label_3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->save_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->save_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->save_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->save_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->save_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->save_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes save_page_label_4
    ui->save_page_label_4 = lv_label_create(ui->save_page_cont_2);
    lv_obj_set_pos(ui->save_page_label_4, 4, 70);
    lv_obj_set_size(ui->save_page_label_4, 50, 30);
    lv_label_set_text(ui->save_page_label_4, "1");
    lv_label_set_long_mode(ui->save_page_label_4, LV_LABEL_LONG_WRAP);

    //Write style for save_page_label_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->save_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->save_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->save_page_label_4, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->save_page_label_4, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->save_page_label_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->save_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->save_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->save_page_label_4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->save_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->save_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->save_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->save_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->save_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->save_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes save_page_label_5
    ui->save_page_label_5 = lv_label_create(ui->save_page_cont_2);
    lv_obj_set_pos(ui->save_page_label_5, 63, 70);
    lv_obj_set_size(ui->save_page_label_5, 50, 30);
    lv_label_set_text(ui->save_page_label_5, "2");
    lv_label_set_long_mode(ui->save_page_label_5, LV_LABEL_LONG_WRAP);

    //Write style for save_page_label_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->save_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->save_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->save_page_label_5, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->save_page_label_5, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->save_page_label_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->save_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->save_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->save_page_label_5, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->save_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->save_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->save_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->save_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->save_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->save_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes save_page_label_6
    ui->save_page_label_6 = lv_label_create(ui->save_page_cont_2);
    lv_obj_set_pos(ui->save_page_label_6, 126, 70);
    lv_obj_set_size(ui->save_page_label_6, 50, 30);
    lv_label_set_text(ui->save_page_label_6, "3");
    lv_label_set_long_mode(ui->save_page_label_6, LV_LABEL_LONG_WRAP);

    //Write style for save_page_label_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->save_page_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->save_page_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->save_page_label_6, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->save_page_label_6, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->save_page_label_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->save_page_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->save_page_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->save_page_label_6, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->save_page_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->save_page_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->save_page_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->save_page_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->save_page_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->save_page_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes save_page_label_7
    ui->save_page_label_7 = lv_label_create(ui->save_page_cont_2);
    lv_obj_set_pos(ui->save_page_label_7, 184, 70);
    lv_obj_set_size(ui->save_page_label_7, 50, 30);
    lv_label_set_text(ui->save_page_label_7, "4");
    lv_label_set_long_mode(ui->save_page_label_7, LV_LABEL_LONG_WRAP);

    //Write style for save_page_label_7, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->save_page_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->save_page_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->save_page_label_7, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->save_page_label_7, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->save_page_label_7, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->save_page_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->save_page_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->save_page_label_7, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->save_page_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->save_page_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->save_page_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->save_page_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->save_page_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->save_page_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes save_page_btnm_1
    ui->save_page_btnm_1 = lv_buttonmatrix_create(ui->save_page_cont_2);
    lv_obj_set_pos(ui->save_page_btnm_1, 5, 110);
    lv_obj_set_size(ui->save_page_btnm_1, 230, 200);
    static const char *save_page_btnm_1_text_map[] = {"1", "2", "3", "\n", "4", "5", "6", "\n", "7", "8", "9", "\n", "X", "0", "V", "",};
    lv_buttonmatrix_set_map(ui->save_page_btnm_1, save_page_btnm_1_text_map);

    //Write style for save_page_btnm_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->save_page_btnm_1, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->save_page_btnm_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->save_page_btnm_1, lv_color_hex(0xc9c9c9), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->save_page_btnm_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->save_page_btnm_1, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->save_page_btnm_1, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->save_page_btnm_1, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->save_page_btnm_1, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui->save_page_btnm_1, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui->save_page_btnm_1, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->save_page_btnm_1, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->save_page_btnm_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->save_page_btnm_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->save_page_btnm_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for save_page_btnm_1, Part: LV_PART_ITEMS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->save_page_btnm_1, 1, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->save_page_btnm_1, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->save_page_btnm_1, lv_color_hex(0xc9c9c9), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->save_page_btnm_1, LV_BORDER_SIDE_FULL, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->save_page_btnm_1, lv_color_hex(0xffffff), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->save_page_btnm_1, &lv_font_montserratMedium_16, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->save_page_btnm_1, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->save_page_btnm_1, 4, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->save_page_btnm_1, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->save_page_btnm_1, lv_color_hex(0x2195f6), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->save_page_btnm_1, LV_GRAD_DIR_NONE, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->save_page_btnm_1, 0, LV_PART_ITEMS|LV_STATE_DEFAULT);

    //The custom code of save_page.


    //Update current screen layout.
    lv_obj_update_layout(ui->save_page);

    //Init events for screen.
    events_init_save_page(ui);
}
