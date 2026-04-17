/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include "gui_guider.h"
#include "widgets_init.h"
#include <stdlib.h>
#include <string.h>


__attribute__((unused)) void kb_event_cb (lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *kb = lv_event_get_target(e);
    if(code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

__attribute__((unused)) void ta_event_cb (lv_event_t *e) {
#if LV_USE_KEYBOARD
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    lv_obj_t * kb = lv_event_get_user_data(e);

    if(code == LV_EVENT_FOCUSED) {
        if(lv_indev_get_type(lv_indev_active()) != LV_INDEV_TYPE_KEYPAD) {
            lv_keyboard_set_textarea(kb, ta);
            lv_obj_remove_flag(kb, LV_OBJ_FLAG_HIDDEN);
        }
    } else if(code == LV_EVENT_READY) {
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_state(ta, LV_STATE_FOCUSED);
        lv_indev_reset(NULL, ta);
    } else if(code == LV_EVENT_DEFOCUSED) {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
#endif
}

void clock_count(int *hour, int *min, int *sec)
{
    (*sec)++;
    if(*sec == 60)
    {
        *sec = 0;
        (*min)++;
    }
    if(*min == 60)
    {
        *min = 0;
        if(*hour < 12)
        {
            (*hour)++;
        } else {
            (*hour)++;
            *hour = *hour %12;
        }
    }
}

void digital_clock_count(int * hour, int * minute, int * seconds, char * meridiem)
{

    (*seconds)++;
    if(*seconds == 60) {
        *seconds = 0;
        (*minute)++;
    }
    if(*minute == 60) {
        *minute = 0;
        if(*hour < 12) {
            (*hour)++;
        }
        else {
            (*hour)++;
            (*hour) = (*hour) % 12;
        }
    }
    if(*hour == 12 && *seconds == 0 && *minute == 0) {
        if((lv_strcmp(meridiem, "PM") == 0)) {
            lv_strcpy(meridiem, "AM");
        }
        else {
            lv_strcpy(meridiem, "PM");
        }
    }
}


const lv_image_dsc_t * main_animimg_1_imgs[2] = {
    &main_animimg_112_0,
    &main_animimg_112_1,
};
const lv_image_dsc_t * save_page_animimg_1_imgs[40] = {
    &save_page_animimg_19_00_tresh,
    &save_page_animimg_19_01_tresh,
    &save_page_animimg_19_02_tresh,
    &save_page_animimg_19_03_tresh,
    &save_page_animimg_19_04_tresh,
    &save_page_animimg_19_05_tresh,
    &save_page_animimg_19_06_tresh,
    &save_page_animimg_19_07_tresh,
    &save_page_animimg_19_08_tresh,
    &save_page_animimg_19_09_tresh,
    &save_page_animimg_19_10_tresh,
    &save_page_animimg_19_11_tresh,
    &save_page_animimg_19_12_tresh,
    &save_page_animimg_19_13_tresh,
    &save_page_animimg_19_14_tresh,
    &save_page_animimg_19_15_tresh,
    &save_page_animimg_19_16_tresh,
    &save_page_animimg_19_17_tresh,
    &save_page_animimg_19_18_tresh,
    &save_page_animimg_19_19_tresh,
    &save_page_animimg_19_20_tresh,
    &save_page_animimg_19_21_tresh,
    &save_page_animimg_19_22_tresh,
    &save_page_animimg_19_23_tresh,
    &save_page_animimg_19_24_tresh,
    &save_page_animimg_19_25_tresh,
    &save_page_animimg_19_26_tresh,
    &save_page_animimg_19_27_tresh,
    &save_page_animimg_19_28_tresh,
    &save_page_animimg_19_29_tresh,
    &save_page_animimg_19_30_tresh,
    &save_page_animimg_19_31_tresh,
    &save_page_animimg_19_32_tresh,
    &save_page_animimg_19_33_tresh,
    &save_page_animimg_19_34_tresh,
    &save_page_animimg_19_35_tresh,
    &save_page_animimg_19_36_tresh,
    &save_page_animimg_19_37_tresh,
    &save_page_animimg_19_38_tresh,
    &save_page_animimg_19_39_tresh,
};
