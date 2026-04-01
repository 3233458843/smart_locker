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



void setup_scr_taked_item(lv_ui *ui)
{
    //Write codes taked_item
    ui->taked_item = lv_obj_create(NULL);
    lv_obj_set_size(ui->taked_item, 320, 240);
    lv_obj_set_scrollbar_mode(ui->taked_item, LV_SCROLLBAR_MODE_OFF);

    //Write style for taked_item, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->taked_item, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes taked_item_cont_2
    ui->taked_item_cont_2 = lv_obj_create(ui->taked_item);
    lv_obj_set_pos(ui->taked_item_cont_2, 0, 0);
    lv_obj_set_size(ui->taked_item_cont_2, 320, 240);
    lv_obj_set_scrollbar_mode(ui->taked_item_cont_2, LV_SCROLLBAR_MODE_OFF);

    //Write style for taked_item_cont_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->taked_item_cont_2, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->taked_item_cont_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->taked_item_cont_2, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->taked_item_cont_2, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->taked_item_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->taked_item_cont_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->taked_item_cont_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->taked_item_cont_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->taked_item_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->taked_item_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->taked_item_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->taked_item_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->taked_item_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes taked_item_label_2
    ui->taked_item_label_2 = lv_label_create(ui->taked_item_cont_2);
    lv_obj_set_pos(ui->taked_item_label_2, 54, 23);
    lv_obj_set_size(ui->taked_item_label_2, 217, 17);
    lv_label_set_text(ui->taked_item_label_2, "请输入存货时获取的4位密码");
    lv_label_set_long_mode(ui->taked_item_label_2, LV_LABEL_LONG_WRAP);

    //Write style for taked_item_label_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->taked_item_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->taked_item_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->taked_item_label_2, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->taked_item_label_2, &lv_font_Lemi_Little_Milk_Foam_Font_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->taked_item_label_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->taked_item_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->taked_item_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->taked_item_label_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->taked_item_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->taked_item_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->taked_item_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->taked_item_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->taked_item_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->taked_item_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes taked_item_imgbtn_2
    ui->taked_item_imgbtn_2 = lv_imagebutton_create(ui->taked_item_cont_2);
    lv_obj_set_pos(ui->taked_item_imgbtn_2, 0, 0);
    lv_obj_set_size(ui->taked_item_imgbtn_2, 50, 50);
    lv_obj_add_flag(ui->taked_item_imgbtn_2, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->taked_item_imgbtn_2, LV_IMAGEBUTTON_STATE_RELEASED, &_back_RGB565A8_50x50, NULL, NULL);
    lv_imagebutton_set_src(ui->taked_item_imgbtn_2, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED, &_back_RGB565A8_50x50, NULL, NULL);
    ui->taked_item_imgbtn_2_label = lv_label_create(ui->taked_item_imgbtn_2);
    lv_label_set_text(ui->taked_item_imgbtn_2_label, "");
    lv_label_set_long_mode(ui->taked_item_imgbtn_2_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->taked_item_imgbtn_2_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->taked_item_imgbtn_2, 0, LV_STATE_DEFAULT);

    //Write style for taked_item_imgbtn_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->taked_item_imgbtn_2, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->taked_item_imgbtn_2, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->taked_item_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->taked_item_imgbtn_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->taked_item_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for taked_item_imgbtn_2, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->taked_item_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->taked_item_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->taked_item_imgbtn_2, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->taked_item_imgbtn_2, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->taked_item_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->taked_item_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for taked_item_imgbtn_2, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->taked_item_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->taked_item_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->taked_item_imgbtn_2, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->taked_item_imgbtn_2, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->taked_item_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->taked_item_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for taked_item_imgbtn_2, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->taked_item_imgbtn_2, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->taked_item_imgbtn_2, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //Write codes taked_item_label_3
    ui->taked_item_label_3 = lv_label_create(ui->taked_item_cont_2);
    lv_obj_set_pos(ui->taked_item_label_3, 10, 55);
    lv_obj_set_size(ui->taked_item_label_3, 50, 20);
    lv_label_set_text(ui->taked_item_label_3, "3");
    lv_label_set_long_mode(ui->taked_item_label_3, LV_LABEL_LONG_WRAP);

    //Write style for taked_item_label_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->taked_item_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->taked_item_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->taked_item_label_3, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->taked_item_label_3, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->taked_item_label_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->taked_item_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->taked_item_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->taked_item_label_3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->taked_item_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->taked_item_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->taked_item_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->taked_item_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->taked_item_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->taked_item_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes taked_item_label_4
    ui->taked_item_label_4 = lv_label_create(ui->taked_item_cont_2);
    lv_obj_set_pos(ui->taked_item_label_4, 90, 55);
    lv_obj_set_size(ui->taked_item_label_4, 50, 20);
    lv_label_set_text(ui->taked_item_label_4, "4");
    lv_label_set_long_mode(ui->taked_item_label_4, LV_LABEL_LONG_WRAP);

    //Write style for taked_item_label_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->taked_item_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->taked_item_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->taked_item_label_4, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->taked_item_label_4, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->taked_item_label_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->taked_item_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->taked_item_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->taked_item_label_4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->taked_item_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->taked_item_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->taked_item_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->taked_item_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->taked_item_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->taked_item_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes taked_item_label_5
    ui->taked_item_label_5 = lv_label_create(ui->taked_item_cont_2);
    lv_obj_set_pos(ui->taked_item_label_5, 170, 55);
    lv_obj_set_size(ui->taked_item_label_5, 50, 20);
    lv_label_set_text(ui->taked_item_label_5, "5");
    lv_label_set_long_mode(ui->taked_item_label_5, LV_LABEL_LONG_WRAP);

    //Write style for taked_item_label_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->taked_item_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->taked_item_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->taked_item_label_5, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->taked_item_label_5, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->taked_item_label_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->taked_item_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->taked_item_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->taked_item_label_5, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->taked_item_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->taked_item_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->taked_item_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->taked_item_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->taked_item_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->taked_item_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes taked_item_label_6
    ui->taked_item_label_6 = lv_label_create(ui->taked_item_cont_2);
    lv_obj_set_pos(ui->taked_item_label_6, 260, 55);
    lv_obj_set_size(ui->taked_item_label_6, 50, 20);
    lv_label_set_text(ui->taked_item_label_6, "6\n");
    lv_label_set_long_mode(ui->taked_item_label_6, LV_LABEL_LONG_WRAP);

    //Write style for taked_item_label_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->taked_item_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->taked_item_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->taked_item_label_6, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->taked_item_label_6, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->taked_item_label_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->taked_item_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->taked_item_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->taked_item_label_6, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->taked_item_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->taked_item_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->taked_item_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->taked_item_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->taked_item_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->taked_item_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes taked_item_btnm_1
    ui->taked_item_btnm_1 = lv_buttonmatrix_create(ui->taked_item_cont_2);
    lv_obj_set_pos(ui->taked_item_btnm_1, 60, 85);
    lv_obj_set_size(ui->taked_item_btnm_1, 200, 150);
    static const char *taked_item_btnm_1_text_map[] = {"1", "2", "3", "\n", "4", "5", "6", "\n", "7", "8", "9", "",};
    lv_buttonmatrix_set_map(ui->taked_item_btnm_1, taked_item_btnm_1_text_map);

    //Write style for taked_item_btnm_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->taked_item_btnm_1, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->taked_item_btnm_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->taked_item_btnm_1, lv_color_hex(0xc9c9c9), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->taked_item_btnm_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->taked_item_btnm_1, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->taked_item_btnm_1, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->taked_item_btnm_1, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->taked_item_btnm_1, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui->taked_item_btnm_1, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui->taked_item_btnm_1, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->taked_item_btnm_1, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->taked_item_btnm_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->taked_item_btnm_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->taked_item_btnm_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for taked_item_btnm_1, Part: LV_PART_ITEMS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->taked_item_btnm_1, 1, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->taked_item_btnm_1, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->taked_item_btnm_1, lv_color_hex(0xc9c9c9), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->taked_item_btnm_1, LV_BORDER_SIDE_FULL, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->taked_item_btnm_1, lv_color_hex(0xffffff), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->taked_item_btnm_1, &lv_font_montserratMedium_16, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->taked_item_btnm_1, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->taked_item_btnm_1, 4, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->taked_item_btnm_1, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->taked_item_btnm_1, lv_color_hex(0x2195f6), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->taked_item_btnm_1, LV_GRAD_DIR_NONE, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->taked_item_btnm_1, 0, LV_PART_ITEMS|LV_STATE_DEFAULT);

    //Write codes taked_item_cont_1
    ui->taked_item_cont_1 = lv_obj_create(ui->taked_item);
    lv_obj_set_pos(ui->taked_item_cont_1, 0, 0);
    lv_obj_set_size(ui->taked_item_cont_1, 320, 240);
    lv_obj_set_scrollbar_mode(ui->taked_item_cont_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for taked_item_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->taked_item_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->taked_item_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->taked_item_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->taked_item_cont_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->taked_item_cont_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->taked_item_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->taked_item_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->taked_item_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->taked_item_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->taked_item_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes taked_item_label_1
    ui->taked_item_label_1 = lv_label_create(ui->taked_item_cont_1);
    lv_obj_set_pos(ui->taked_item_label_1, 65, 32);
    lv_obj_set_size(ui->taked_item_label_1, 198, 32);
    lv_label_set_text(ui->taked_item_label_1, "请将手掌置于传感器前方10cm左右，正在识别...");
    lv_label_set_long_mode(ui->taked_item_label_1, LV_LABEL_LONG_WRAP);

    //Write style for taked_item_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->taked_item_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->taked_item_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->taked_item_label_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->taked_item_label_1, &lv_font_Lemi_Little_Milk_Foam_Font_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->taked_item_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->taked_item_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->taked_item_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->taked_item_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->taked_item_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->taked_item_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->taked_item_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->taked_item_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->taked_item_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->taked_item_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes taked_item_img_1
    ui->taked_item_img_1 = lv_image_create(ui->taked_item_cont_1);
    lv_obj_set_pos(ui->taked_item_img_1, 113, 72);
    lv_obj_set_size(ui->taked_item_img_1, 100, 100);
    lv_obj_add_flag(ui->taked_item_img_1, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->taked_item_img_1, &_10_RGB565A8_100x100);
    lv_image_set_pivot(ui->taked_item_img_1, 50,50);
    lv_image_set_rotation(ui->taked_item_img_1, 0);

    //Write style for taked_item_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->taked_item_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->taked_item_img_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes taked_item_imgbtn_1
    ui->taked_item_imgbtn_1 = lv_imagebutton_create(ui->taked_item_cont_1);
    lv_obj_set_pos(ui->taked_item_imgbtn_1, 0, 0);
    lv_obj_set_size(ui->taked_item_imgbtn_1, 50, 50);
    lv_obj_add_flag(ui->taked_item_imgbtn_1, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->taked_item_imgbtn_1, LV_IMAGEBUTTON_STATE_RELEASED, &_back_RGB565A8_50x50, NULL, NULL);
    ui->taked_item_imgbtn_1_label = lv_label_create(ui->taked_item_imgbtn_1);
    lv_label_set_text(ui->taked_item_imgbtn_1_label, "");
    lv_label_set_long_mode(ui->taked_item_imgbtn_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->taked_item_imgbtn_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->taked_item_imgbtn_1, 0, LV_STATE_DEFAULT);

    //Write style for taked_item_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->taked_item_imgbtn_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->taked_item_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->taked_item_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->taked_item_imgbtn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->taked_item_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for taked_item_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->taked_item_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->taked_item_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->taked_item_imgbtn_1, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->taked_item_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->taked_item_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->taked_item_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for taked_item_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->taked_item_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->taked_item_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->taked_item_imgbtn_1, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->taked_item_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->taked_item_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->taked_item_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for taked_item_imgbtn_1, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->taked_item_imgbtn_1, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->taked_item_imgbtn_1, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //Write codes taked_item_btn_1
    ui->taked_item_btn_1 = lv_button_create(ui->taked_item_cont_1);
    lv_obj_set_pos(ui->taked_item_btn_1, 232, 201);
    lv_obj_set_size(ui->taked_item_btn_1, 85, 36);
    ui->taked_item_btn_1_label = lv_label_create(ui->taked_item_btn_1);
    lv_label_set_text(ui->taked_item_btn_1_label, "密码开柜");
    lv_label_set_long_mode(ui->taked_item_btn_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->taked_item_btn_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->taked_item_btn_1, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->taked_item_btn_1_label, LV_PCT(100));

    //Write style for taked_item_btn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->taked_item_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->taked_item_btn_1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->taked_item_btn_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->taked_item_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->taked_item_btn_1, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->taked_item_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->taked_item_btn_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->taked_item_btn_1, &lv_font_Lemi_Little_Milk_Foam_Font_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->taked_item_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->taked_item_btn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of taked_item.


    //Update current screen layout.
    lv_obj_update_layout(ui->taked_item);

    //Init events for screen.
    events_init_taked_item(ui);
}
