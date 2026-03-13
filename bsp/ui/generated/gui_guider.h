/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"


typedef struct
{
  
	lv_obj_t *main;
	bool main_del;
	lv_obj_t *main_cont_2;
	lv_obj_t *main_btnm_1;
	lv_obj_t *main_label_4;
	lv_obj_t *main_label_5;
	lv_obj_t *main_label_6;
	lv_obj_t *main_label_7;
	lv_obj_t *main_imgbtn_4;
	lv_obj_t *main_imgbtn_4_label;
	lv_obj_t *main_img_5;
	lv_obj_t *main_label_8;
	lv_obj_t *main_cont_1;
	lv_obj_t *main_btn_3;
	lv_obj_t *main_btn_3_label;
	lv_obj_t *main_btn_2;
	lv_obj_t *main_btn_2_label;
	lv_obj_t *main_label_1;
	lv_obj_t *main_imgbtn_3;
	lv_obj_t *main_imgbtn_3_label;
	lv_obj_t *main_btn_1;
	lv_obj_t *main_btn_1_label;
	lv_obj_t *main_label_3;
	lv_obj_t *main_label_2;
	lv_obj_t *main_led_4;
	lv_obj_t *main_animimg_1;
	lv_obj_t *main_img_1;
	lv_obj_t *main_led_3;
	lv_obj_t *main_img_3;
	lv_obj_t *main_img_2;
	lv_obj_t *main_img_4;
	lv_obj_t *main_led_1;
	lv_obj_t *main_led_2;
	lv_obj_t *saved_item;
	bool saved_item_del;
	lv_obj_t *saved_item_cont_1;
	lv_obj_t *saved_item_imgbtn_1;
	lv_obj_t *saved_item_imgbtn_1_label;
	lv_obj_t *taked_item;
	bool taked_item_del;
	lv_obj_t *taked_item_cont_1;
	lv_obj_t *taked_item_imgbtn_1;
	lv_obj_t *taked_item_imgbtn_1_label;
	lv_obj_t *help_item;
	bool help_item_del;
	lv_obj_t *help_item_cont_1;
	lv_obj_t *help_item_cont_2;
	lv_obj_t *help_item_label_3;
	lv_obj_t *help_item_label_4;
	lv_obj_t *help_item_label_1;
	lv_obj_t *help_item_label_2;
	lv_obj_t *help_item_imgbtn_1;
	lv_obj_t *help_item_imgbtn_1_label;
	lv_obj_t *setting_item;
	bool setting_item_del;
	lv_obj_t *setting_item_cont_1;
	lv_obj_t *setting_item_spangroup_1;
	lv_span_t *setting_item_spangroup_1_span;
	lv_obj_t *setting_item_imgbtn_1;
	lv_obj_t *setting_item_imgbtn_1_label;
	lv_obj_t *setting_item_cont_2;
	lv_obj_t *setting_item_btn_1;
	lv_obj_t *setting_item_btn_1_label;
	lv_obj_t *setting_item_btn_2;
	lv_obj_t *setting_item_btn_2_label;
	lv_obj_t *setting_item_btn_3;
	lv_obj_t *setting_item_btn_3_label;
	lv_obj_t *setting_item_btn_4;
	lv_obj_t *setting_item_btn_4_label;
	lv_obj_t *setting_item_label_1;
}lv_ui;

typedef void (*ui_setup_scr_t)(lv_ui * ui);

void ui_init_style(lv_style_t * style);

void ui_load_scr_animation(lv_ui *ui, lv_obj_t ** new_scr, bool new_scr_del, bool * old_scr_del, ui_setup_scr_t setup_scr,
                           lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay, bool is_clean, bool auto_del);

void ui_animation(void * var, uint32_t duration, int32_t delay, int32_t start_value, int32_t end_value, lv_anim_path_cb_t path_cb,
                  uint32_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                  lv_anim_exec_xcb_t exec_cb, lv_anim_start_cb_t start_cb, lv_anim_completed_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);


void init_scr_del_flag(lv_ui *ui);

void setup_bottom_layer(void);

void setup_ui(lv_ui *ui);

void video_play(lv_ui *ui);

void init_keyboard(lv_ui *ui);

extern lv_ui guider_ui;


void setup_scr_main(lv_ui *ui);
void setup_scr_saved_item(lv_ui *ui);
void setup_scr_taked_item(lv_ui *ui);
void setup_scr_help_item(lv_ui *ui);
void setup_scr_setting_item(lv_ui *ui);
LV_IMAGE_DECLARE(_back_RGB565A8_50x50);
LV_IMAGE_DECLARE(_2_RGB565A8_90x91);
LV_IMAGE_DECLARE(_setting_RGB565A8_40x40);
LV_IMAGE_DECLARE(main_animimg_11_00_floyd);
LV_IMAGE_DECLARE(main_animimg_11_01_floyd);
LV_IMAGE_DECLARE(main_animimg_11_02_floyd);
LV_IMAGE_DECLARE(main_animimg_11_03_floyd);
LV_IMAGE_DECLARE(main_animimg_11_04_floyd);
LV_IMAGE_DECLARE(main_animimg_11_05_floyd);
LV_IMAGE_DECLARE(main_animimg_11_06_floyd);
LV_IMAGE_DECLARE(main_animimg_11_07_floyd);
LV_IMAGE_DECLARE(main_animimg_11_08_floyd);
LV_IMAGE_DECLARE(main_animimg_11_09_floyd);
LV_IMAGE_DECLARE(main_animimg_11_10_floyd);
LV_IMAGE_DECLARE(main_animimg_11_11_floyd);
LV_IMAGE_DECLARE(main_animimg_11_12_floyd);
LV_IMAGE_DECLARE(main_animimg_11_13_floyd);
LV_IMAGE_DECLARE(main_animimg_11_14_floyd);
LV_IMAGE_DECLARE(main_animimg_11_15_floyd);
LV_IMAGE_DECLARE(main_animimg_11_16_floyd);
LV_IMAGE_DECLARE(main_animimg_11_17_floyd);
LV_IMAGE_DECLARE(main_animimg_11_18_floyd);
LV_IMAGE_DECLARE(main_animimg_11_19_floyd);
LV_IMAGE_DECLARE(main_animimg_11_20_floyd);
LV_IMAGE_DECLARE(main_animimg_11_21_floyd);
LV_IMAGE_DECLARE(main_animimg_11_22_floyd);
LV_IMAGE_DECLARE(main_animimg_11_23_floyd);
LV_IMAGE_DECLARE(main_animimg_11_24_floyd);
LV_IMAGE_DECLARE(main_animimg_11_25_floyd);
LV_IMAGE_DECLARE(main_animimg_11_26_floyd);
LV_IMAGE_DECLARE(main_animimg_11_27_floyd);
LV_IMAGE_DECLARE(main_animimg_11_28_floyd);
LV_IMAGE_DECLARE(main_animimg_11_29_floyd);
LV_IMAGE_DECLARE(main_animimg_11_30_floyd);
LV_IMAGE_DECLARE(main_animimg_11_31_floyd);
LV_IMAGE_DECLARE(main_animimg_11_32_floyd);
LV_IMAGE_DECLARE(main_animimg_11_33_floyd);
LV_IMAGE_DECLARE(main_animimg_11_34_floyd);
LV_IMAGE_DECLARE(main_animimg_11_35_floyd);
LV_IMAGE_DECLARE(main_animimg_11_36_floyd);
LV_IMAGE_DECLARE(main_animimg_11_37_floyd);
LV_IMAGE_DECLARE(main_animimg_11_38_floyd);
LV_IMAGE_DECLARE(_locker_RGB565A8_30x30);

LV_FONT_DECLARE(lv_font_montserratMedium_16)
LV_FONT_DECLARE(lv_font_montserratMedium_12)
LV_FONT_DECLARE(lv_font_Lemi_Little_Milk_Foam_Font_16)
LV_FONT_DECLARE(lv_font_LXGWWenKaiMono_Medium_25)
LV_FONT_DECLARE(lv_font_LXGWWenKaiMono_Medium_20)
LV_FONT_DECLARE(lv_font_LXGWWenKaiMono_Medium_16)
LV_FONT_DECLARE(lv_font_LXGWWenKaiMono_Medium_17)
LV_FONT_DECLARE(lv_font_Lemi_Little_Milk_Foam_Font_29)


#ifdef __cplusplus
}
#endif
#endif
