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



void setup_scr_help_page(lv_ui *ui)
{
    //Write codes help_page
    ui->help_page = lv_obj_create(NULL);
    lv_obj_set_size(ui->help_page, 240, 320);
    lv_obj_set_scrollbar_mode(ui->help_page, LV_SCROLLBAR_MODE_OFF);

    //Write style for help_page, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->help_page, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes help_page_cont_1
    ui->help_page_cont_1 = lv_obj_create(ui->help_page);
    lv_obj_set_pos(ui->help_page_cont_1, 0, 0);
    lv_obj_set_size(ui->help_page_cont_1, 240, 320);
    lv_obj_set_scrollbar_mode(ui->help_page_cont_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for help_page_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->help_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->help_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->help_page_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->help_page_cont_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->help_page_cont_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->help_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->help_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->help_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->help_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->help_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes help_page_imgbtn_1
    ui->help_page_imgbtn_1 = lv_imagebutton_create(ui->help_page_cont_1);
    lv_obj_set_pos(ui->help_page_imgbtn_1, 10, 9);
    lv_obj_set_size(ui->help_page_imgbtn_1, 50, 50);
    lv_obj_add_flag(ui->help_page_imgbtn_1, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->help_page_imgbtn_1, LV_IMAGEBUTTON_STATE_RELEASED, &_back_RGB565A8_50x50, NULL, NULL);
    ui->help_page_imgbtn_1_label = lv_label_create(ui->help_page_imgbtn_1);
    lv_label_set_text(ui->help_page_imgbtn_1_label, "");
    lv_label_set_long_mode(ui->help_page_imgbtn_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->help_page_imgbtn_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->help_page_imgbtn_1, 0, LV_STATE_DEFAULT);

    //Write style for help_page_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->help_page_imgbtn_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->help_page_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->help_page_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->help_page_imgbtn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->help_page_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for help_page_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->help_page_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->help_page_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->help_page_imgbtn_1, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->help_page_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->help_page_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->help_page_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for help_page_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->help_page_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->help_page_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->help_page_imgbtn_1, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->help_page_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->help_page_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->help_page_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for help_page_imgbtn_1, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->help_page_imgbtn_1, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->help_page_imgbtn_1, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //Write codes help_page_cont_2
    ui->help_page_cont_2 = lv_obj_create(ui->help_page_cont_1);
    lv_obj_set_pos(ui->help_page_cont_2, 5, 115);
    lv_obj_set_size(ui->help_page_cont_2, 230, 180);
    lv_obj_set_scrollbar_mode(ui->help_page_cont_2, LV_SCROLLBAR_MODE_OFF);

    //Write style for help_page_cont_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->help_page_cont_2, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->help_page_cont_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->help_page_cont_2, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->help_page_cont_2, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->help_page_cont_2, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->help_page_cont_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->help_page_cont_2, lv_color_hex(0xeaeaea), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->help_page_cont_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->help_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->help_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->help_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->help_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->help_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes help_page_label_2
    ui->help_page_label_2 = lv_label_create(ui->help_page_cont_2);
    lv_obj_set_pos(ui->help_page_label_2, 4, 122);
    lv_obj_set_size(ui->help_page_label_2, 220, 30);
    lv_label_set_text(ui->help_page_label_2, "遇到问题联系管理员同学~\n123456789（刘同学）");
    lv_label_set_long_mode(ui->help_page_label_2, LV_LABEL_LONG_WRAP);

    //Write style for help_page_label_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->help_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->help_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->help_page_label_2, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->help_page_label_2, &lv_font_Lemi_Little_Milk_Foam_Font_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->help_page_label_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->help_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->help_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->help_page_label_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->help_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->help_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->help_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->help_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->help_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->help_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes help_page_label_1
    ui->help_page_label_1 = lv_label_create(ui->help_page_cont_2);
    lv_obj_set_pos(ui->help_page_label_1, 4, 10);
    lv_obj_set_size(ui->help_page_label_1, 220, 100);
    lv_label_set_text(ui->help_page_label_1, "1.点击主界面[存件]或者[取件]\n2.将手掌悬停于传感器前10cm左右\n3.等待设备发出“滴”声，柜门即可弹开\n！！！绿色标识柜门可用，红色标识柜门已被占用！！！");
    lv_label_set_long_mode(ui->help_page_label_1, LV_LABEL_LONG_WRAP);

    //Write style for help_page_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->help_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->help_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->help_page_label_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->help_page_label_1, &lv_font_Lemi_Little_Milk_Foam_Font_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->help_page_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->help_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->help_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->help_page_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->help_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->help_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->help_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->help_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->help_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->help_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes help_page_label_3
    ui->help_page_label_3 = lv_label_create(ui->help_page_cont_1);
    lv_obj_set_pos(ui->help_page_label_3, 62, 28);
    lv_obj_set_size(ui->help_page_label_3, 174, 32);
    lv_label_set_text(ui->help_page_label_3, "ヾ(●゜▽゜●)♡");
    lv_label_set_long_mode(ui->help_page_label_3, LV_LABEL_LONG_WRAP);

    //Write style for help_page_label_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->help_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->help_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->help_page_label_3, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->help_page_label_3, &lv_font_LXGWWenKaiMono_Medium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->help_page_label_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->help_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->help_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->help_page_label_3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->help_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->help_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->help_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->help_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->help_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->help_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes help_page_label_4
    ui->help_page_label_4 = lv_label_create(ui->help_page_cont_1);
    lv_obj_set_pos(ui->help_page_label_4, 50, 65);
    lv_obj_set_size(ui->help_page_label_4, 140, 32);
    lv_label_set_text(ui->help_page_label_4, "使用说明");
    lv_label_set_long_mode(ui->help_page_label_4, LV_LABEL_LONG_WRAP);

    //Write style for help_page_label_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->help_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->help_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->help_page_label_4, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->help_page_label_4, &lv_font_Lemi_Little_Milk_Foam_Font_33, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->help_page_label_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->help_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->help_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->help_page_label_4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->help_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->help_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->help_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->help_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->help_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->help_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of help_page.


    //Update current screen layout.
    lv_obj_update_layout(ui->help_page);

    //Init events for screen.
    events_init_help_page(ui);
}
