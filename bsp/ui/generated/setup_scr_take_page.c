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



void setup_scr_take_page(lv_ui *ui)
{
    //Write codes take_page
    ui->take_page = lv_obj_create(NULL);
    lv_obj_set_size(ui->take_page, 240, 320);
    lv_obj_set_scrollbar_mode(ui->take_page, LV_SCROLLBAR_MODE_OFF);

    //Write style for take_page, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->take_page, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes take_page_cont_1
    ui->take_page_cont_1 = lv_obj_create(ui->take_page);
    lv_obj_set_pos(ui->take_page_cont_1, 0, 0);
    lv_obj_set_size(ui->take_page_cont_1, 240, 320);
    lv_obj_set_scrollbar_mode(ui->take_page_cont_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for take_page_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->take_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->take_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->take_page_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->take_page_cont_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->take_page_cont_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->take_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->take_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->take_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->take_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->take_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes take_page_imgbtn_1
    ui->take_page_imgbtn_1 = lv_imagebutton_create(ui->take_page_cont_1);
    lv_obj_set_pos(ui->take_page_imgbtn_1, 10, 9);
    lv_obj_set_size(ui->take_page_imgbtn_1, 50, 50);
    lv_obj_add_flag(ui->take_page_imgbtn_1, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->take_page_imgbtn_1, LV_IMAGEBUTTON_STATE_RELEASED, &_back_RGB565A8_50x50, NULL, NULL);
    ui->take_page_imgbtn_1_label = lv_label_create(ui->take_page_imgbtn_1);
    lv_label_set_text(ui->take_page_imgbtn_1_label, "");
    lv_label_set_long_mode(ui->take_page_imgbtn_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->take_page_imgbtn_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->take_page_imgbtn_1, 0, LV_STATE_DEFAULT);

    //Write style for take_page_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->take_page_imgbtn_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->take_page_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->take_page_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->take_page_imgbtn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->take_page_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for take_page_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->take_page_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->take_page_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->take_page_imgbtn_1, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->take_page_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->take_page_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->take_page_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for take_page_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->take_page_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->take_page_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->take_page_imgbtn_1, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->take_page_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->take_page_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->take_page_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for take_page_imgbtn_1, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->take_page_imgbtn_1, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->take_page_imgbtn_1, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //Write codes take_page_label_1
    ui->take_page_label_1 = lv_label_create(ui->take_page_cont_1);
    lv_obj_set_pos(ui->take_page_label_1, 0, 80);
    lv_obj_set_size(ui->take_page_label_1, 240, 40);
    lv_label_set_text(ui->take_page_label_1, "请将手掌置于传感器前方10cm左右，正在识别...");
    lv_label_set_long_mode(ui->take_page_label_1, LV_LABEL_LONG_WRAP);

    //Write style for take_page_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->take_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->take_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->take_page_label_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->take_page_label_1, &lv_font_Lemi_Little_Milk_Foam_Font_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->take_page_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->take_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->take_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->take_page_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->take_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->take_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->take_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->take_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->take_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->take_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes take_page_img_1
    ui->take_page_img_1 = lv_image_create(ui->take_page_cont_1);
    lv_obj_set_pos(ui->take_page_img_1, 45, 130);
    lv_obj_set_size(ui->take_page_img_1, 150, 150);
    lv_obj_add_flag(ui->take_page_img_1, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->take_page_img_1, &_10_RGB565A8_150x150);
    lv_image_set_pivot(ui->take_page_img_1, 50,50);
    lv_image_set_rotation(ui->take_page_img_1, 0);

    //Write style for take_page_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->take_page_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->take_page_img_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes take_page_btn_1
    ui->take_page_btn_1 = lv_button_create(ui->take_page_cont_1);
    lv_obj_set_pos(ui->take_page_btn_1, 135, 9);
    lv_obj_set_size(ui->take_page_btn_1, 94, 40);
    ui->take_page_btn_1_label = lv_label_create(ui->take_page_btn_1);
    lv_label_set_text(ui->take_page_btn_1_label, "手机号开柜");
    lv_label_set_long_mode(ui->take_page_btn_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->take_page_btn_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->take_page_btn_1, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->take_page_btn_1_label, LV_PCT(100));

    //Write style for take_page_btn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->take_page_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->take_page_btn_1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->take_page_btn_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->take_page_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->take_page_btn_1, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->take_page_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->take_page_btn_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->take_page_btn_1, &lv_font_Lemi_Little_Milk_Foam_Font_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->take_page_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->take_page_btn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes take_page_cont_2
    ui->take_page_cont_2 = lv_obj_create(ui->take_page);
    lv_obj_set_pos(ui->take_page_cont_2, 0, 0);
    lv_obj_set_size(ui->take_page_cont_2, 240, 320);
    lv_obj_set_scrollbar_mode(ui->take_page_cont_2, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->take_page_cont_2, LV_OBJ_FLAG_HIDDEN);

    //Write style for take_page_cont_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->take_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->take_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->take_page_cont_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->take_page_cont_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->take_page_cont_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->take_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->take_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->take_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->take_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->take_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes take_page_imgbtn_2
    ui->take_page_imgbtn_2 = lv_imagebutton_create(ui->take_page_cont_2);
    lv_obj_set_pos(ui->take_page_imgbtn_2, 10, 10);
    lv_obj_set_size(ui->take_page_imgbtn_2, 50, 50);
    lv_obj_add_flag(ui->take_page_imgbtn_2, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->take_page_imgbtn_2, LV_IMAGEBUTTON_STATE_RELEASED, &_back_RGB565A8_50x50, NULL, NULL);
    ui->take_page_imgbtn_2_label = lv_label_create(ui->take_page_imgbtn_2);
    lv_label_set_text(ui->take_page_imgbtn_2_label, "");
    lv_label_set_long_mode(ui->take_page_imgbtn_2_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->take_page_imgbtn_2_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->take_page_imgbtn_2, 0, LV_STATE_DEFAULT);

    //Write style for take_page_imgbtn_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->take_page_imgbtn_2, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->take_page_imgbtn_2, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->take_page_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->take_page_imgbtn_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->take_page_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for take_page_imgbtn_2, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->take_page_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->take_page_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->take_page_imgbtn_2, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->take_page_imgbtn_2, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->take_page_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->take_page_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for take_page_imgbtn_2, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->take_page_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->take_page_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->take_page_imgbtn_2, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->take_page_imgbtn_2, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->take_page_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->take_page_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for take_page_imgbtn_2, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->take_page_imgbtn_2, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->take_page_imgbtn_2, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //Write codes take_page_btnm_1
    ui->take_page_btnm_1 = lv_buttonmatrix_create(ui->take_page_cont_2);
    lv_obj_set_pos(ui->take_page_btnm_1, 10, 119);
    lv_obj_set_size(ui->take_page_btnm_1, 220, 200);
    static const char *take_page_btnm_1_text_map[] = {"1", "2", "3", "\n", "4", "5", "6", "\n", "7", "8", "9", "\n", "X", "0", "V", "",};
    lv_buttonmatrix_set_map(ui->take_page_btnm_1, take_page_btnm_1_text_map);

    //Write style for take_page_btnm_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->take_page_btnm_1, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->take_page_btnm_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->take_page_btnm_1, lv_color_hex(0xc9c9c9), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->take_page_btnm_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->take_page_btnm_1, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->take_page_btnm_1, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->take_page_btnm_1, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->take_page_btnm_1, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui->take_page_btnm_1, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui->take_page_btnm_1, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->take_page_btnm_1, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->take_page_btnm_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->take_page_btnm_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->take_page_btnm_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for take_page_btnm_1, Part: LV_PART_ITEMS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->take_page_btnm_1, 1, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->take_page_btnm_1, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->take_page_btnm_1, lv_color_hex(0xc9c9c9), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->take_page_btnm_1, LV_BORDER_SIDE_FULL, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->take_page_btnm_1, lv_color_hex(0xffffff), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->take_page_btnm_1, &lv_font_montserratMedium_16, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->take_page_btnm_1, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->take_page_btnm_1, 4, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->take_page_btnm_1, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->take_page_btnm_1, lv_color_hex(0x2195f6), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->take_page_btnm_1, LV_GRAD_DIR_NONE, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->take_page_btnm_1, 0, LV_PART_ITEMS|LV_STATE_DEFAULT);

    //Write codes take_page_label_2
    ui->take_page_label_2 = lv_label_create(ui->take_page_cont_2);
    lv_obj_set_pos(ui->take_page_label_2, 58, 29);
    lv_obj_set_size(ui->take_page_label_2, 178, 27);
    lv_label_set_text(ui->take_page_label_2, "请输入4位手机号正确！柜门已开错误，重试");
    lv_label_set_long_mode(ui->take_page_label_2, LV_LABEL_LONG_WRAP);

    //Write style for take_page_label_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->take_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->take_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->take_page_label_2, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->take_page_label_2, &lv_font_Lemi_Little_Milk_Foam_Font_26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->take_page_label_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->take_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->take_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->take_page_label_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->take_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->take_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->take_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->take_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->take_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->take_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes take_page_label_3
    ui->take_page_label_3 = lv_label_create(ui->take_page_cont_2);
    lv_obj_set_pos(ui->take_page_label_3, 20, 85);
    lv_obj_set_size(ui->take_page_label_3, 30, 20);
    lv_label_set_text(ui->take_page_label_3, "3");
    lv_label_set_long_mode(ui->take_page_label_3, LV_LABEL_LONG_WRAP);

    //Write style for take_page_label_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->take_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->take_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->take_page_label_3, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->take_page_label_3, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->take_page_label_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->take_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->take_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->take_page_label_3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->take_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->take_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->take_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->take_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->take_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->take_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes take_page_label_4
    ui->take_page_label_4 = lv_label_create(ui->take_page_cont_2);
    lv_obj_set_pos(ui->take_page_label_4, 75, 85);
    lv_obj_set_size(ui->take_page_label_4, 30, 20);
    lv_label_set_text(ui->take_page_label_4, "4");
    lv_label_set_long_mode(ui->take_page_label_4, LV_LABEL_LONG_WRAP);

    //Write style for take_page_label_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->take_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->take_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->take_page_label_4, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->take_page_label_4, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->take_page_label_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->take_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->take_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->take_page_label_4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->take_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->take_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->take_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->take_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->take_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->take_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes take_page_label_5
    ui->take_page_label_5 = lv_label_create(ui->take_page_cont_2);
    lv_obj_set_pos(ui->take_page_label_5, 133, 85);
    lv_obj_set_size(ui->take_page_label_5, 30, 20);
    lv_label_set_text(ui->take_page_label_5, "5");
    lv_label_set_long_mode(ui->take_page_label_5, LV_LABEL_LONG_WRAP);

    //Write style for take_page_label_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->take_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->take_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->take_page_label_5, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->take_page_label_5, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->take_page_label_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->take_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->take_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->take_page_label_5, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->take_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->take_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->take_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->take_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->take_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->take_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes take_page_label_6
    ui->take_page_label_6 = lv_label_create(ui->take_page_cont_2);
    lv_obj_set_pos(ui->take_page_label_6, 190, 85);
    lv_obj_set_size(ui->take_page_label_6, 30, 20);
    lv_label_set_text(ui->take_page_label_6, "6");
    lv_label_set_long_mode(ui->take_page_label_6, LV_LABEL_LONG_WRAP);

    //Write style for take_page_label_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->take_page_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->take_page_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->take_page_label_6, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->take_page_label_6, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->take_page_label_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->take_page_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->take_page_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->take_page_label_6, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->take_page_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->take_page_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->take_page_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->take_page_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->take_page_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->take_page_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of take_page.


    //Update current screen layout.
    lv_obj_update_layout(ui->take_page);

    //Init events for screen.
    events_init_take_page(ui);
}
