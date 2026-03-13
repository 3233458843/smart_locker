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


const lv_image_dsc_t * main_animimg_1_imgs[39] = {
    &main_animimg_11_00_floyd,
    &main_animimg_11_01_floyd,
    &main_animimg_11_02_floyd,
    &main_animimg_11_03_floyd,
    &main_animimg_11_04_floyd,
    &main_animimg_11_05_floyd,
    &main_animimg_11_06_floyd,
    &main_animimg_11_07_floyd,
    &main_animimg_11_08_floyd,
    &main_animimg_11_09_floyd,
    &main_animimg_11_10_floyd,
    &main_animimg_11_11_floyd,
    &main_animimg_11_12_floyd,
    &main_animimg_11_13_floyd,
    &main_animimg_11_14_floyd,
    &main_animimg_11_15_floyd,
    &main_animimg_11_16_floyd,
    &main_animimg_11_17_floyd,
    &main_animimg_11_18_floyd,
    &main_animimg_11_19_floyd,
    &main_animimg_11_20_floyd,
    &main_animimg_11_21_floyd,
    &main_animimg_11_22_floyd,
    &main_animimg_11_23_floyd,
    &main_animimg_11_24_floyd,
    &main_animimg_11_25_floyd,
    &main_animimg_11_26_floyd,
    &main_animimg_11_27_floyd,
    &main_animimg_11_28_floyd,
    &main_animimg_11_29_floyd,
    &main_animimg_11_30_floyd,
    &main_animimg_11_31_floyd,
    &main_animimg_11_32_floyd,
    &main_animimg_11_33_floyd,
    &main_animimg_11_34_floyd,
    &main_animimg_11_35_floyd,
    &main_animimg_11_36_floyd,
    &main_animimg_11_37_floyd,
    &main_animimg_11_38_floyd,
};
