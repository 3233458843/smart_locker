#include "locker_actions.h"
#include "locker.h"
#include "buzzer.h"
#include "xst.h"
#include "serve.h"
#include "esp_log.h"

#undef TAG
#define TAG "LOCKER_ACT"

esp_err_t serve_release_locker(uint8_t locker_id, uint16_t user_id){
    if (locker_id >= 4) return ESP_ERR_INVALID_ARG;

    ESP_LOGI(TAG, "Releasing locker %u for user_id=%u", locker_id + 1, user_id);

    locker_on(&lockers[locker_id]);
    if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_SUCCESS);

    xst_result_t del_res = xst_cmd_del_user(user_id);
    if (del_res != MR_SUCCESS){
        ESP_LOGW(TAG, "XST del user %u failed, res=%d; local binding will still be cleared",
                 user_id, del_res);
    }

    esp_err_t db_err = locker_db_remove_entry_by_locker(locker_id);
    if (db_err != ESP_OK){
        ESP_LOGE(TAG, "Locker opened but failed to clear binding: %s", esp_err_to_name(db_err));
        if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_ERROR);
    }

    return db_err;
}
