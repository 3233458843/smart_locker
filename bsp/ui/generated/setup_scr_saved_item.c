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



void setup_scr_saved_item(lv_ui *ui)
{
    //Write codes saved_item
    ui->saved_item = lv_obj_create(NULL);
    lv_obj_set_size(ui->saved_item, 320, 240);
    lv_obj_set_scrollbar_mode(ui->saved_item, LV_SCROLLBAR_MODE_OFF);

    //Write style for saved_item, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->saved_item, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes saved_item_cont_1
    ui->saved_item_cont_1 = lv_obj_create(ui->saved_item);
    lv_obj_set_pos(ui->saved_item_cont_1, 0, 0);
    lv_obj_set_size(ui->saved_item_cont_1, 320, 240);
    lv_obj_set_scrollbar_mode(ui->saved_item_cont_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for saved_item_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->saved_item_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->saved_item_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->saved_item_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->saved_item_cont_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->saved_item_cont_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->saved_item_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->saved_item_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->saved_item_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->saved_item_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->saved_item_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes saved_item_imgbtn_1
    ui->saved_item_imgbtn_1 = lv_imagebutton_create(ui->saved_item);
    lv_obj_set_pos(ui->saved_item_imgbtn_1, 0, 0);
    lv_obj_set_size(ui->saved_item_imgbtn_1, 50, 50);
    lv_obj_add_flag(ui->saved_item_imgbtn_1, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->saved_item_imgbtn_1, LV_IMAGEBUTTON_STATE_RELEASED, &_back_RGB565A8_50x50, NULL, NULL);
    ui->saved_item_imgbtn_1_label = lv_label_create(ui->saved_item_imgbtn_1);
    lv_label_set_text(ui->saved_item_imgbtn_1_label, "");
    lv_label_set_long_mode(ui->saved_item_imgbtn_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->saved_item_imgbtn_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->saved_item_imgbtn_1, 0, LV_STATE_DEFAULT);

    //Write style for saved_item_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->saved_item_imgbtn_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->saved_item_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->saved_item_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->saved_item_imgbtn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->saved_item_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for saved_item_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->saved_item_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->saved_item_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->saved_item_imgbtn_1, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->saved_item_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->saved_item_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->saved_item_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for saved_item_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->saved_item_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->saved_item_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->saved_item_imgbtn_1, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->saved_item_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->saved_item_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->saved_item_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for saved_item_imgbtn_1, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->saved_item_imgbtn_1, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->saved_item_imgbtn_1, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //The custom code of saved_item.


    //Update current screen layout.
    lv_obj_update_layout(ui->saved_item);

    //Init events for screen.
    events_init_saved_item(ui);
}
