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

    //Write codes taked_item_imgbtn_1
    ui->taked_item_imgbtn_1 = lv_imagebutton_create(ui->taked_item);
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

    //The custom code of taked_item.


    //Update current screen layout.
    lv_obj_update_layout(ui->taked_item);

    //Init events for screen.
    events_init_taked_item(ui);
}
