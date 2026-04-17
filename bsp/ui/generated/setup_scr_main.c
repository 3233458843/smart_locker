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



void setup_scr_main(lv_ui *ui)
{
    //Write codes main
    ui->main = lv_obj_create(NULL);
    lv_obj_set_size(ui->main, 240, 320);
    lv_obj_set_scrollbar_mode(ui->main, LV_SCROLLBAR_MODE_OFF);

    //Write style for main, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes main_cont_1
    ui->main_cont_1 = lv_obj_create(ui->main);
    lv_obj_set_pos(ui->main_cont_1, 0, 0);
    lv_obj_set_size(ui->main_cont_1, 240, 320);
    lv_obj_set_scrollbar_mode(ui->main_cont_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for main_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->main_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->main_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->main_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->main_cont_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->main_cont_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->main_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->main_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->main_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->main_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_src(ui->main_cont_1, &_3_RGB565A8_240x320, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_opa(ui->main_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(ui->main_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->main_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes main_img_2
    ui->main_img_2 = lv_image_create(ui->main_cont_1);
    lv_obj_set_pos(ui->main_img_2, 45, 10);
    lv_obj_set_size(ui->main_img_2, 20, 20);
    lv_obj_add_flag(ui->main_img_2, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->main_img_2, &_locker_RGB565A8_20x20);
    lv_image_set_pivot(ui->main_img_2, 50,50);
    lv_image_set_rotation(ui->main_img_2, 0);

    //Write style for main_img_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->main_img_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->main_img_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes main_btn_1
    ui->main_btn_1 = lv_button_create(ui->main_cont_1);
    lv_obj_set_pos(ui->main_btn_1, 20, 120);
    lv_obj_set_size(ui->main_btn_1, 200, 80);
    ui->main_btn_1_label = lv_label_create(ui->main_btn_1);
    lv_label_set_text(ui->main_btn_1_label, "存件");
    lv_label_set_long_mode(ui->main_btn_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->main_btn_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->main_btn_1, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->main_btn_1_label, LV_PCT(100));

    //Write style for main_btn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->main_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->main_btn_1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->main_btn_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->main_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->main_btn_1, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->main_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->main_btn_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->main_btn_1, &lv_font_Lemi_Little_Milk_Foam_Font_50, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->main_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->main_btn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes main_img_3
    ui->main_img_3 = lv_image_create(ui->main_cont_1);
    lv_obj_set_pos(ui->main_img_3, 5, 40);
    lv_obj_set_size(ui->main_img_3, 20, 20);
    lv_obj_add_flag(ui->main_img_3, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->main_img_3, &_locker_RGB565A8_20x20);
    lv_image_set_pivot(ui->main_img_3, 50,50);
    lv_image_set_rotation(ui->main_img_3, 0);

    //Write style for main_img_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->main_img_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->main_img_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes main_img_4
    ui->main_img_4 = lv_image_create(ui->main_cont_1);
    lv_obj_set_pos(ui->main_img_4, 45, 40);
    lv_obj_set_size(ui->main_img_4, 20, 20);
    lv_obj_add_flag(ui->main_img_4, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->main_img_4, &_locker_RGB565A8_20x20);
    lv_image_set_pivot(ui->main_img_4, 50,50);
    lv_image_set_rotation(ui->main_img_4, 0);

    //Write style for main_img_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->main_img_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->main_img_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes main_img_1
    ui->main_img_1 = lv_image_create(ui->main_cont_1);
    lv_obj_set_pos(ui->main_img_1, 6, 10);
    lv_obj_set_size(ui->main_img_1, 20, 20);
    lv_obj_add_flag(ui->main_img_1, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->main_img_1, &_locker_RGB565A8_20x20);
    lv_image_set_pivot(ui->main_img_1, 50,50);
    lv_image_set_rotation(ui->main_img_1, 0);

    //Write style for main_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->main_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->main_img_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes main_btn_2
    ui->main_btn_2 = lv_button_create(ui->main_cont_1);
    lv_obj_set_pos(ui->main_btn_2, 19, 220);
    lv_obj_set_size(ui->main_btn_2, 200, 80);
    ui->main_btn_2_label = lv_label_create(ui->main_btn_2);
    lv_label_set_text(ui->main_btn_2_label, "取件");
    lv_label_set_long_mode(ui->main_btn_2_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->main_btn_2_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->main_btn_2, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->main_btn_2_label, LV_PCT(100));

    //Write style for main_btn_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->main_btn_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->main_btn_2, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->main_btn_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->main_btn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->main_btn_2, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->main_btn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->main_btn_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->main_btn_2, &lv_font_Lemi_Little_Milk_Foam_Font_50, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->main_btn_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->main_btn_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes main_btn_3
    ui->main_btn_3 = lv_button_create(ui->main_cont_1);
    lv_obj_set_pos(ui->main_btn_3, 174, 5);
    lv_obj_set_size(ui->main_btn_3, 60, 40);
    ui->main_btn_3_label = lv_label_create(ui->main_btn_3);
    lv_label_set_text(ui->main_btn_3_label, "help");
    lv_label_set_long_mode(ui->main_btn_3_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->main_btn_3_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->main_btn_3, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->main_btn_3_label, LV_PCT(100));

    //Write style for main_btn_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->main_btn_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->main_btn_3, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->main_btn_3, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->main_btn_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->main_btn_3, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->main_btn_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->main_btn_3, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->main_btn_3, &lv_font_ArchitectsDaughter_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->main_btn_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->main_btn_3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes main_locker2
    ui->main_locker2 = lv_obj_create(ui->main_cont_1);
    lv_obj_set_pos(ui->main_locker2, 70, 15);
    lv_obj_set_size(ui->main_locker2, 10, 10);
    lv_obj_set_scrollbar_mode(ui->main_locker2, LV_SCROLLBAR_MODE_OFF);

    //Write style for main_locker2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->main_locker2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->main_locker2, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->main_locker2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->main_locker2, lv_color_hex(0x2FDA64), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->main_locker2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->main_locker2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->main_locker2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->main_locker2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->main_locker2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->main_locker2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes main_locker3
    ui->main_locker3 = lv_obj_create(ui->main_cont_1);
    lv_obj_set_pos(ui->main_locker3, 30, 45);
    lv_obj_set_size(ui->main_locker3, 10, 10);
    lv_obj_set_scrollbar_mode(ui->main_locker3, LV_SCROLLBAR_MODE_OFF);

    //Write style for main_locker3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->main_locker3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->main_locker3, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->main_locker3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->main_locker3, lv_color_hex(0x2FDA64), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->main_locker3, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->main_locker3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->main_locker3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->main_locker3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->main_locker3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->main_locker3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes main_locker4
    ui->main_locker4 = lv_obj_create(ui->main_cont_1);
    lv_obj_set_pos(ui->main_locker4, 70, 45);
    lv_obj_set_size(ui->main_locker4, 10, 10);
    lv_obj_set_scrollbar_mode(ui->main_locker4, LV_SCROLLBAR_MODE_OFF);

    //Write style for main_locker4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->main_locker4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->main_locker4, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->main_locker4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->main_locker4, lv_color_hex(0x2FDA64), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->main_locker4, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->main_locker4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->main_locker4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->main_locker4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->main_locker4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->main_locker4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes main_locker1
    ui->main_locker1 = lv_obj_create(ui->main_cont_1);
    lv_obj_set_pos(ui->main_locker1, 30, 15);
    lv_obj_set_size(ui->main_locker1, 10, 10);
    lv_obj_set_scrollbar_mode(ui->main_locker1, LV_SCROLLBAR_MODE_OFF);

    //Write style for main_locker1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->main_locker1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->main_locker1, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->main_locker1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->main_locker1, lv_color_hex(0x2FDA64), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->main_locker1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->main_locker1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->main_locker1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->main_locker1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->main_locker1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->main_locker1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes main_animimg_1
    ui->main_animimg_1 = lv_animimg_create(ui->main_cont_1);
    lv_obj_set_pos(ui->main_animimg_1, 85, 0);
    lv_obj_set_size(ui->main_animimg_1, 80, 80);
    lv_animimg_set_src(ui->main_animimg_1, (const void **) main_animimg_1_imgs, 2);
    lv_animimg_set_duration(ui->main_animimg_1, 80*2);
    lv_animimg_set_repeat_count(ui->main_animimg_1, 100000);
    lv_animimg_start(ui->main_animimg_1);

    //Write codes main_label_1
    ui->main_label_1 = lv_label_create(ui->main_cont_1);
    lv_obj_set_pos(ui->main_label_1, 4, 96);
    lv_obj_set_size(ui->main_label_1, 204, 21);
    lv_label_set_text(ui->main_label_1, "Ciallo～(∠・ω< )⌒★!");
    lv_label_set_long_mode(ui->main_label_1, LV_LABEL_LONG_WRAP);

    //Write style for main_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->main_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->main_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->main_label_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->main_label_1, &lv_font_LXGWWenKaiMono_Medium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->main_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->main_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->main_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->main_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->main_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->main_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->main_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->main_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->main_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->main_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of main.


    //Update current screen layout.
    lv_obj_update_layout(ui->main);

    //Init events for screen.
    events_init_main(ui);
}
