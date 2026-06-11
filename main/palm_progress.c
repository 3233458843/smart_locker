#include "palm_progress.h"
#include "lvgl.h"
#include "../bsp/ui/generated/gui_guider.h"
#include "serve.h"

uint8_t g_xst_palm_progress = 0;
bool g_palm_progress_updated = false;

uint8_t last_palm_progress = 0;
uint8_t last_save_progress = 0;

void palm_progress_update(TickType_t now){
    (void)now;

    // 先检查 save_page，避免因内存复用导致 take_page 指针误匹配
    if (lv_screen_active() == guider_ui.save_page) {
        serve_save_status_t save_st;
        serve_get_save_status(&save_st);
        if (save_st.state == SERVE_FLOW_RUNNING || save_st.state == SERVE_FLOW_PENDING) {
            if (g_xst_palm_progress != last_save_progress || g_palm_progress_updated) {
                if (guider_ui.save_page_bar_1){
                    lv_bar_set_value(guider_ui.save_page_bar_1, g_xst_palm_progress, LV_ANIM_OFF);
                }
                char pct[8];
                snprintf(pct, sizeof(pct), "%d%%", g_xst_palm_progress);
                if (guider_ui.save_page_label_2){
                    lv_label_set_text(guider_ui.save_page_label_2, pct);
                }
                last_save_progress = g_xst_palm_progress;
                g_palm_progress_updated = false;
            }
        }
    } else if (lv_screen_active() == guider_ui.take_page) {
        if (guider_ui.take_page_cont_1 &&
            lv_obj_is_valid(guider_ui.take_page_cont_1) &&
            !lv_obj_has_flag(guider_ui.take_page_cont_1, LV_OBJ_FLAG_HIDDEN)) {
            if (g_xst_palm_progress != last_palm_progress || g_palm_progress_updated) {
                if (guider_ui.take_page_label_1 && lv_obj_is_valid(guider_ui.take_page_label_1)){
                    char buf[100];
                    snprintf(buf, sizeof(buf), "请将手掌置于传感器前方10cm左右\n正在识别中... %d%%", g_xst_palm_progress);
                    lv_label_set_text(guider_ui.take_page_label_1, buf);
                }
                last_palm_progress = g_xst_palm_progress;
                g_palm_progress_updated = false;
            }
        }
    } else {
        last_palm_progress = 0;
        last_save_progress = 0;
    }
}

void palm_progress_reset(void){
    g_xst_palm_progress = 0;
    g_palm_progress_updated = false;
    last_palm_progress = 0;
    last_save_progress = 0;
}

void palm_progress_on_xst_progress(uint8_t progress){
    g_xst_palm_progress = progress;
    g_palm_progress_updated = true;
}
