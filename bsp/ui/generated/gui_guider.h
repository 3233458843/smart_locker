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
	lv_obj_t *main_cont_1;
	lv_obj_t *main_img_2;
	lv_obj_t *main_btn_1;
	lv_obj_t *main_btn_1_label;
	lv_obj_t *main_img_3;
	lv_obj_t *main_img_4;
	lv_obj_t *main_img_1;
	lv_obj_t *main_btn_2;
	lv_obj_t *main_btn_2_label;
	lv_obj_t *main_btn_3;
	lv_obj_t *main_btn_3_label;
	lv_obj_t *main_locker2;
	lv_obj_t *main_locker3;
	lv_obj_t *main_locker4;
	lv_obj_t *main_locker1;
	lv_obj_t *main_animimg_1;
	lv_obj_t *main_label_1;
	lv_obj_t *take_page;
	bool take_page_del;
	lv_obj_t *take_page_cont_1;
	lv_obj_t *take_page_imgbtn_1;
	lv_obj_t *take_page_imgbtn_1_label;
	lv_obj_t *take_page_label_1;
	lv_obj_t *take_page_img_1;
	lv_obj_t *take_page_btn_1;
	lv_obj_t *take_page_btn_1_label;
	lv_obj_t *take_page_cont_2;
	lv_obj_t *take_page_imgbtn_2;
	lv_obj_t *take_page_imgbtn_2_label;
	lv_obj_t *take_page_btnm_1;
	lv_obj_t *take_page_label_2;
	lv_obj_t *take_page_label_3;
	lv_obj_t *take_page_label_4;
	lv_obj_t *take_page_label_5;
	lv_obj_t *take_page_label_6;
	lv_obj_t *save_page;
	bool save_page_del;
	lv_obj_t *save_page_cont_1;
	lv_obj_t *save_page_imgbtn_1;
	lv_obj_t *save_page_imgbtn_1_label;
	lv_obj_t *save_page_animimg_1;
	lv_obj_t *save_page_label_1;
	lv_obj_t *save_page_bar_1;
	lv_obj_t *save_page_label_2;
	lv_obj_t *help_page;
	bool help_page_del;
	lv_obj_t *help_page_cont_1;
	lv_obj_t *help_page_imgbtn_1;
	lv_obj_t *help_page_imgbtn_1_label;
	lv_obj_t *help_page_cont_2;
	lv_obj_t *help_page_label_2;
	lv_obj_t *help_page_label_1;
	lv_obj_t *help_page_label_3;
	lv_obj_t *help_page_label_4;
	lv_obj_t *setting_page;
	bool setting_page_del;
	lv_obj_t *setting_page_cont_1;
	lv_obj_t *setting_page_imgbtn_1;
	lv_obj_t *setting_page_imgbtn_1_label;
	lv_obj_t *setting_page_spangroup_1;
	lv_span_t *setting_page_spangroup_1_span;
	lv_obj_t *setting_page_cont_3;
	lv_obj_t *setting_page_RES;
	lv_obj_t *setting_page_RES_label;
	lv_obj_t *setting_page_READ;
	lv_obj_t *setting_page_READ_label;
	lv_obj_t *setting_page_DELL;
	lv_obj_t *setting_page_DELL_label;
	lv_obj_t *setting_page_locker4;
	lv_obj_t *setting_page_locker4_label;
	lv_obj_t *setting_page_locker3;
	lv_obj_t *setting_page_locker3_label;
	lv_obj_t *setting_page_VERI;
	lv_obj_t *setting_page_VERI_label;
	lv_obj_t *setting_page_locker1;
	lv_obj_t *setting_page_locker1_label;
	lv_obj_t *setting_page_locker2;
	lv_obj_t *setting_page_locker2_label;
	lv_obj_t *setting_page_ALL;
	lv_obj_t *setting_page_ALL_label;
	lv_obj_t *setting_page_NOTE;
	lv_obj_t *setting_page_cont_2;
	lv_obj_t *setting_page_btnm_1;
	lv_obj_t *setting_page_label_4;
	lv_obj_t *setting_page_label_3;
	lv_obj_t *setting_page_label_1;
	lv_obj_t *setting_page_imgbtn_2;
	lv_obj_t *setting_page_imgbtn_2_label;
	lv_obj_t *setting_page_label_2;
	lv_obj_t *setting_page_img_1;
	lv_obj_t *setting_page_label_5;
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
void setup_scr_take_page(lv_ui *ui);
void setup_scr_save_page(lv_ui *ui);
void setup_scr_help_page(lv_ui *ui);
void setup_scr_setting_page(lv_ui *ui);

LV_IMAGE_DECLARE(_3_RGB565A8_240x320);
LV_IMAGE_DECLARE(_locker_RGB565A8_20x20);
LV_IMAGE_DECLARE(main_animimg_112_0);
LV_IMAGE_DECLARE(main_animimg_112_1);
LV_IMAGE_DECLARE(_back_RGB565A8_50x50);
LV_IMAGE_DECLARE(_10_RGB565A8_150x150);
LV_IMAGE_DECLARE(save_page_animimg_19_00_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_01_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_02_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_03_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_04_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_05_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_06_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_07_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_08_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_09_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_10_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_11_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_12_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_13_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_14_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_15_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_16_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_17_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_18_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_19_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_20_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_21_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_22_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_23_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_24_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_25_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_26_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_27_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_28_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_29_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_30_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_31_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_32_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_33_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_34_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_35_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_36_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_37_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_38_tresh);
LV_IMAGE_DECLARE(save_page_animimg_19_39_tresh);
LV_IMAGE_DECLARE(_2_RGB565A8_80x80);

LV_FONT_DECLARE(lv_font_Lemi_Little_Milk_Foam_Font_50)
LV_FONT_DECLARE(lv_font_ArchitectsDaughter_16)
LV_FONT_DECLARE(lv_font_LXGWWenKaiMono_Medium_16)
LV_FONT_DECLARE(lv_font_montserratMedium_12)
LV_FONT_DECLARE(lv_font_Lemi_Little_Milk_Foam_Font_20)
LV_FONT_DECLARE(lv_font_montserratMedium_16)
LV_FONT_DECLARE(lv_font_Lemi_Little_Milk_Foam_Font_26)
LV_FONT_DECLARE(lv_font_Lemi_Little_Milk_Foam_Font_16)
LV_FONT_DECLARE(lv_font_LXGWWenKaiMono_Medium_20)
LV_FONT_DECLARE(lv_font_Lemi_Little_Milk_Foam_Font_33)


#ifdef __cplusplus
}
#endif
#endif
