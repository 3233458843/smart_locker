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

    //Write codes saved_item_label_2
    ui->saved_item_label_2 = lv_label_create(ui->saved_item_cont_1);
    lv_obj_set_pos(ui->saved_item_label_2, 59, 32);
    lv_obj_set_size(ui->saved_item_label_2, 233, 18);
    lv_label_set_text(ui->saved_item_label_2, "正在录入掌静脉，请靠近...");
    lv_label_set_long_mode(ui->saved_item_label_2, LV_LABEL_LONG_WRAP);

    //Write style for saved_item_label_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->saved_item_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->saved_item_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->saved_item_label_2, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->saved_item_label_2, &lv_font_Lemi_Little_Milk_Foam_Font_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->saved_item_label_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->saved_item_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->saved_item_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->saved_item_label_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->saved_item_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->saved_item_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->saved_item_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->saved_item_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->saved_item_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->saved_item_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes saved_item_label_1
    ui->saved_item_label_1 = lv_label_create(ui->saved_item_cont_1);
    lv_obj_set_pos(ui->saved_item_label_1, 270, 205);
    lv_obj_set_size(ui->saved_item_label_1, 46, 18);
    lv_label_set_text(ui->saved_item_label_1, "");
    lv_label_set_long_mode(ui->saved_item_label_1, LV_LABEL_LONG_WRAP);

    //Write style for saved_item_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->saved_item_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->saved_item_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->saved_item_label_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->saved_item_label_1, &lv_font_Lemi_Little_Milk_Foam_Font_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->saved_item_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->saved_item_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->saved_item_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->saved_item_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->saved_item_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->saved_item_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->saved_item_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->saved_item_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->saved_item_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->saved_item_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes saved_item_imgbtn_1
    ui->saved_item_imgbtn_1 = lv_imagebutton_create(ui->saved_item_cont_1);
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

    //Write codes saved_item_animimg_1
    ui->saved_item_animimg_1 = lv_animimg_create(ui->saved_item_cont_1);
    lv_obj_set_pos(ui->saved_item_animimg_1, 96, 50);
    lv_obj_set_size(ui->saved_item_animimg_1, 139, 135);
    lv_animimg_set_src(ui->saved_item_animimg_1, (const void **) saved_item_animimg_1_imgs, 40);
    lv_animimg_set_duration(ui->saved_item_animimg_1, 30*40);
    lv_animimg_set_repeat_count(ui->saved_item_animimg_1, LV_ANIM_REPEAT_INFINITE);
    lv_animimg_start(ui->saved_item_animimg_1);

    //Write codes saved_item_bar_1
    ui->saved_item_bar_1 = lv_bar_create(ui->saved_item_cont_1);
    lv_obj_set_pos(ui->saved_item_bar_1, 284, 17);
    lv_obj_set_size(ui->saved_item_bar_1, 20, 180);
    lv_obj_set_style_anim_duration(ui->saved_item_bar_1, 1000, 0);
    lv_bar_set_mode(ui->saved_item_bar_1, LV_BAR_MODE_NORMAL);
    lv_bar_set_range(ui->saved_item_bar_1, 0, 100);
    lv_bar_set_value(ui->saved_item_bar_1, 50, LV_ANIM_ON);

    //Write style for saved_item_bar_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->saved_item_bar_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->saved_item_bar_1, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->saved_item_bar_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for saved_item_bar_1, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->saved_item_bar_1, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->saved_item_bar_1, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->saved_item_bar_1, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->saved_item_bar_1, 10, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write codes saved_item_animimg_2
    ui->saved_item_animimg_2 = lv_animimg_create(ui->saved_item_cont_1);
    lv_obj_set_pos(ui->saved_item_animimg_2, 8, 67);
    lv_obj_set_size(ui->saved_item_animimg_2, 90, 90);
    lv_animimg_set_src(ui->saved_item_animimg_2, (const void **) saved_item_animimg_2_imgs, 10);
    lv_animimg_set_duration(ui->saved_item_animimg_2, 30*10);
    lv_animimg_set_repeat_count(ui->saved_item_animimg_2, LV_ANIM_REPEAT_INFINITE);
    lv_animimg_start(ui->saved_item_animimg_2);

    //Write codes saved_item_msgbox_1
    ui->saved_item_msgbox_1 = lv_msgbox_create(ui->saved_item_cont_1);
    lv_obj_set_pos(ui->saved_item_msgbox_1, 71, 24);
    lv_obj_set_size(ui->saved_item_msgbox_1, 243, 203);
    lv_msgbox_add_title(ui->saved_item_msgbox_1, "Locker is ON!!!");
    lv_msgbox_add_text(ui->saved_item_msgbox_1, "");
    lv_obj_align_to(ui->saved_item_msgbox_1, ui->saved_item_cont_1, LV_ALIGN_TOP_LEFT, 71, 24);
    lv_obj_t *saved_item_msgbox_1_close_btn = lv_msgbox_add_close_button(ui->saved_item_msgbox_1);

    //Write style for saved_item_msgbox_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->saved_item_msgbox_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->saved_item_msgbox_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->saved_item_msgbox_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->saved_item_msgbox_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->saved_item_msgbox_1, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->saved_item_msgbox_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_saved_item_msgbox_1_extra_title_main_default
    static lv_style_t style_saved_item_msgbox_1_extra_title_main_default;
    ui_init_style(&style_saved_item_msgbox_1_extra_title_main_default);

    lv_style_set_bg_opa(&style_saved_item_msgbox_1_extra_title_main_default, 255);
    lv_style_set_bg_color(&style_saved_item_msgbox_1_extra_title_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_saved_item_msgbox_1_extra_title_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_text_color(&style_saved_item_msgbox_1_extra_title_main_default, lv_color_hex(0x4e4e4e));
    lv_style_set_text_font(&style_saved_item_msgbox_1_extra_title_main_default, &lv_font_montserratMedium_12);
    lv_style_set_text_opa(&style_saved_item_msgbox_1_extra_title_main_default, 255);
    lv_style_set_text_letter_space(&style_saved_item_msgbox_1_extra_title_main_default, 0);
    lv_style_set_text_line_space(&style_saved_item_msgbox_1_extra_title_main_default, 15);
    lv_obj_add_style(lv_msgbox_get_header(ui->saved_item_msgbox_1), &style_saved_item_msgbox_1_extra_title_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_saved_item_msgbox_1_extra_content_main_default
    static lv_style_t style_saved_item_msgbox_1_extra_content_main_default;
    ui_init_style(&style_saved_item_msgbox_1_extra_content_main_default);

    lv_style_set_text_color(&style_saved_item_msgbox_1_extra_content_main_default, lv_color_hex(0x4e4e4e));
    lv_style_set_text_font(&style_saved_item_msgbox_1_extra_content_main_default, &lv_font_montserratMedium_12);
    lv_style_set_text_opa(&style_saved_item_msgbox_1_extra_content_main_default, 255);
    lv_style_set_text_letter_space(&style_saved_item_msgbox_1_extra_content_main_default, 0);
    lv_style_set_text_line_space(&style_saved_item_msgbox_1_extra_content_main_default, 10);
    lv_obj_add_style(lv_msgbox_get_content(ui->saved_item_msgbox_1), &style_saved_item_msgbox_1_extra_content_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_saved_item_msgbox_1_extra_btns_items_default
    static lv_style_t style_saved_item_msgbox_1_extra_btns_items_default;
    ui_init_style(&style_saved_item_msgbox_1_extra_btns_items_default);

    lv_style_set_bg_opa(&style_saved_item_msgbox_1_extra_btns_items_default, 255);
    lv_style_set_bg_color(&style_saved_item_msgbox_1_extra_btns_items_default, lv_color_hex(0xe6e6e6));
    lv_style_set_bg_grad_dir(&style_saved_item_msgbox_1_extra_btns_items_default, LV_GRAD_DIR_NONE);
    lv_style_set_border_width(&style_saved_item_msgbox_1_extra_btns_items_default, 0);
    lv_style_set_radius(&style_saved_item_msgbox_1_extra_btns_items_default, 10);
    lv_style_set_text_color(&style_saved_item_msgbox_1_extra_btns_items_default, lv_color_hex(0x4e4e4e));
    lv_style_set_text_font(&style_saved_item_msgbox_1_extra_btns_items_default, &lv_font_montserratMedium_12);
    lv_style_set_text_opa(&style_saved_item_msgbox_1_extra_btns_items_default, 255);

    //The custom code of saved_item.


    //Update current screen layout.
    lv_obj_update_layout(ui->saved_item);

    //Init events for screen.
    events_init_saved_item(ui);
}
