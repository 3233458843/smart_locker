/**
 * @file      serve.c
 * @brief     Business service layer
 */

#include "serve.h"

#include "freertos/FREERTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include <string.h>

#include "xst.h"
#include "locker.h"
#include "buzzer.h"
#include "locker_actions.h"
#include "lwip/sockets.h"

#define SERVE_TAG "serve"

/* Private variables ---------------------------------------------------------*/
static SemaphoreHandle_t ready_save = NULL;
static SemaphoreHandle_t ready_take = NULL;

static serve_save_status_t g_save_status = {
    .state = SERVE_FLOW_IDLE,
    .locker_id = 0xFF,
    .password = {0, 0, 0, 0},
    .has_password = false,
    .user_id = 0,
    .err = ESP_OK,
    .message = "idle",
};

static serve_take_status_t g_take_status = {
    .state = SERVE_FLOW_IDLE,
    .locker_id = 0xFF,
    .user_id = 0,
    .err = ESP_OK,
    .message = "idle",
};

buzzer_handle_t g_buzzer_handle = NULL;

static void serve_save_status_reset(const char* message){
    g_save_status.state = SERVE_FLOW_IDLE;
    g_save_status.locker_id = 0xFF;
    memset(g_save_status.password, 0, sizeof(g_save_status.password));
    g_save_status.has_password = false;
    g_save_status.user_id = 0;
    g_save_status.err = ESP_OK;
    strncpy(g_save_status.message, message, sizeof(g_save_status.message) - 1);
    g_save_status.message[sizeof(g_save_status.message) - 1] = '\0';
}

static void serve_save_status_fail(esp_err_t err, const char* message){
    g_save_status.state = SERVE_FLOW_FAILED;
    g_save_status.err = err;
    strncpy(g_save_status.message, message, sizeof(g_save_status.message) - 1);
    g_save_status.message[sizeof(g_save_status.message) - 1] = '\0';
}

static void serve_take_status_set(serve_flow_state_t state, uint8_t locker_id, uint16_t user_id, esp_err_t err,
                                  const char* message){
    g_take_status.state = state;
    g_take_status.locker_id = locker_id;
    g_take_status.user_id = user_id;
    g_take_status.err = err;
    strncpy(g_take_status.message, message, sizeof(g_take_status.message) - 1);
    g_take_status.message[sizeof(g_take_status.message) - 1] = '\0';
}

/* Private function prototypes -----------------------------------------------*/
void ready_take_task(void* param);
void ready_save_task(void* param);

void serve_init(void){
    ready_save = xSemaphoreCreateBinary();
    ready_take = xSemaphoreCreateBinary();

    if (ready_save == NULL || ready_take == NULL){
        ESP_LOGE(SERVE_TAG, "Failed to create semaphores");
        return;
    }

    xTaskCreatePinnedToCore(ready_take_task, "ready_take_task", 4096, NULL, 10, NULL, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(ready_save_task, "ready_save_task", 4096, NULL, 10, NULL, tskNO_AFFINITY);
}

bool serve_request_save(void){
    if (ready_save == NULL) return false;
    if (g_save_status.state == SERVE_FLOW_PENDING || g_save_status.state == SERVE_FLOW_RUNNING) return false;

    serve_save_status_reset("save requested");
    g_save_status.state = SERVE_FLOW_PENDING;
    return xSemaphoreGive(ready_save) == pdTRUE;
}

static uint8_t g_pending_phone[4] = {0};
static bool g_has_pending_phone = false;

bool serve_request_save_with_phone(const uint8_t phone[4]){
    if (phone == NULL || ready_save == NULL) return false;
    if (g_save_status.state == SERVE_FLOW_PENDING || g_save_status.state == SERVE_FLOW_RUNNING) return false;

    serve_save_status_reset("save requested with phone");
    memcpy(g_pending_phone, phone, 4);
    g_has_pending_phone = true;
    g_save_status.state = SERVE_FLOW_PENDING;
    return xSemaphoreGive(ready_save) == pdTRUE;
}

bool serve_request_take_by_palm(void){
    if (ready_take == NULL) return false;
    serve_take_status_set(SERVE_FLOW_PENDING, 0xFF, 0, ESP_OK, "palm take requested");
    return xSemaphoreGive(ready_take) == pdTRUE;
}

bool serve_request_take_by_password(const uint8_t password[4]){
    if (password == NULL){
        serve_take_status_set(SERVE_FLOW_FAILED, 0xFF, 0, ESP_ERR_INVALID_ARG, "Invalid password input");
        return false;
    }

    user_locker_entry_t locker_entry = {0};
    serve_take_status_set(SERVE_FLOW_RUNNING, 0xFF, 0, ESP_OK, "password take running");

    esp_err_t db_err = locker_db_get_entry_by_password(password, &locker_entry);
    if (db_err != ESP_OK){
        ESP_LOGW(SERVE_TAG, "No locker binding found for input password");
        if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_ERROR);
        serve_take_status_set(SERVE_FLOW_FAILED, 0xFF, 0, db_err, "Password not found");
        return false;
    }

    if (locker_entry.locker_id >= 4){
        ESP_LOGE(SERVE_TAG, "Invalid locker_id=%u for password take", locker_entry.locker_id);
        if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_ERROR);
        serve_take_status_set(SERVE_FLOW_FAILED, locker_entry.locker_id, locker_entry.user_id, ESP_ERR_INVALID_STATE, "Invalid locker id");
        return false;
    }

    ESP_LOGI(SERVE_TAG, "Password take success for user_id=%u, opening locker %u",
             locker_entry.user_id, locker_entry.locker_id + 1);
    esp_err_t release_err = serve_release_locker(locker_entry.locker_id, locker_entry.user_id);
    if (release_err != ESP_OK){
        serve_take_status_set(SERVE_FLOW_FAILED, locker_entry.locker_id, locker_entry.user_id, release_err, "Failed to clear binding");
        return false;
    }

    serve_take_status_set(SERVE_FLOW_SUCCESS, locker_entry.locker_id, locker_entry.user_id, ESP_OK, "password take completed");
    return true;
}

bool serve_request_take_by_phone(const uint8_t phone[4]){
    if (phone == NULL){
        serve_take_status_set(SERVE_FLOW_FAILED, 0xFF, 0, ESP_ERR_INVALID_ARG, "Invalid phone input");
        return false;
    }

    user_locker_entry_t locker_entry = {0};
    serve_take_status_set(SERVE_FLOW_RUNNING, 0xFF, 0, ESP_OK, "phone take running");

    esp_err_t db_err = locker_db_get_entry_by_phone(phone, &locker_entry);
    if (db_err != ESP_OK){
        ESP_LOGW(SERVE_TAG, "No locker binding found for input phone");
        if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_ERROR);
        serve_take_status_set(SERVE_FLOW_FAILED, 0xFF, 0, db_err, "Phone not found");
        return false;
    }

    if (locker_entry.locker_id >= 4){
        ESP_LOGE(SERVE_TAG, "Invalid locker_id=%u for phone take", locker_entry.locker_id);
        if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_ERROR);
        serve_take_status_set(SERVE_FLOW_FAILED, locker_entry.locker_id, locker_entry.user_id, ESP_ERR_INVALID_STATE, "Invalid locker id");
        return false;
    }

    ESP_LOGI(SERVE_TAG, "Phone take success for user_id=%u, opening locker %u",
             locker_entry.user_id, locker_entry.locker_id + 1);

    // 直接开柜+清绑定，不调用 xst_cmd_del_user (避免与 ready_take_task 的 VERIFY 命令冲突)
    locker_on(&lockers[locker_entry.locker_id]);
    if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_SUCCESS);

    esp_err_t release_err = locker_db_remove_entry_by_locker(locker_entry.locker_id);
    if (release_err != ESP_OK){
        ESP_LOGE(SERVE_TAG, "Phone take: failed to clear locker %u binding: %s",
                 locker_entry.locker_id + 1, esp_err_to_name(release_err));
        if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_ERROR);
        serve_take_status_set(SERVE_FLOW_FAILED, locker_entry.locker_id, locker_entry.user_id, release_err, "Failed to clear binding");
        return false;
    }

    serve_take_status_set(SERVE_FLOW_SUCCESS, locker_entry.locker_id, locker_entry.user_id, ESP_OK, "phone take completed");
    return true;
}

bool serve_request_debug_verify(void){
    ESP_LOGW(SERVE_TAG, "Debug verify requested but task removed");
    return false;
}

void serve_get_save_status(serve_save_status_t *out){
    if (out == NULL) return;
    memcpy(out, &g_save_status, sizeof(*out));
}

void serve_get_take_status(serve_take_status_t *out){
    if (out == NULL) return;
    memcpy(out, &g_take_status, sizeof(*out));
}

/* ============== Admin API ============== */
bool serve_admin_reset_xst(void){
    return xst_cmd_reset() == MR_SUCCESS;
}

bool serve_admin_del_all_users(void){
    return xst_cmd_del_all() == MR_SUCCESS;
}

int serve_admin_get_user_count(void){
    uint16_t count = 0;
    xst_result_t res = xst_cmd_get_user_count(&count);
    return (res == MR_SUCCESS) ? (int)count : -1;
}

bool serve_admin_open_locker(uint8_t locker_id){
    if (locker_id >= 4) return false;
    locker_on(&lockers[locker_id]);
    return true;
}

bool serve_admin_open_all_lockers(void){
    locker_all_on();
    return true;
}

bool serve_admin_verify_password(const char *input, uint8_t len){
    if (input == NULL || len != 4) return false;
    const char *admin_pwd = "1234";
    return (input[0] == admin_pwd[0] && input[1] == admin_pwd[1] &&
            input[2] == admin_pwd[2] && input[3] == admin_pwd[3]);
}

extern uint8_t g_xst_palm_progress;
extern bool g_palm_progress_updated;
extern uint8_t last_palm_progress;
extern uint8_t last_save_progress;

void serve_reset_palm_progress(void){
    g_xst_palm_progress = 0;
    g_palm_progress_updated = false;
    last_palm_progress = 0;
    last_save_progress = 0;
}

/* Private functions ---------------------------------------------------------*/
void ready_take_task(void* param){
    while (1){
        if (xSemaphoreTake(ready_take, portMAX_DELAY) == pdTRUE){
            ESP_LOGI(SERVE_TAG, "Received ready_take signal, can start take process...");
            serve_take_status_set(SERVE_FLOW_RUNNING, 0xFF, 0, ESP_OK, "palm take running");

            uint16_t verified_user_id = 0;
            char verified_name[32] = {0};
            user_locker_entry_t locker_entry = {0};

            xst_result_t verify_res = xst_cmd_verify(10, &verified_user_id, verified_name);
            if (verify_res != MR_SUCCESS){
                ESP_LOGE(SERVE_TAG, "Failed to verify user, res=%d", verify_res);
                if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_ERROR);
                serve_take_status_set(SERVE_FLOW_FAILED, 0xFF, 0, ESP_FAIL, "Palm verify failed");
                continue;
            }

            esp_err_t db_err = locker_db_get_entry_by_user(verified_user_id, &locker_entry);
            if (db_err != ESP_OK){
                ESP_LOGW(SERVE_TAG, "No locker binding found for user_id=%u, deleting orphaned user", verified_user_id);
                if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_ERROR);
                serve_take_status_set(SERVE_FLOW_FAILED, 0xFF, verified_user_id, db_err, "No locker binding found");
                xst_cmd_del_user(verified_user_id);
                continue;
            }

            if (locker_entry.locker_id >= 4){
                ESP_LOGE(SERVE_TAG, "Invalid locker_id=%u for user_id=%u", locker_entry.locker_id, verified_user_id);
                if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_ERROR);
                serve_take_status_set(SERVE_FLOW_FAILED, locker_entry.locker_id, verified_user_id, ESP_ERR_INVALID_STATE, "Invalid locker id");
                continue;
            }

            ESP_LOGI(SERVE_TAG, "Verify success for user_id=%u name=%s, opening locker %u",
                     verified_user_id, verified_name, locker_entry.locker_id + 1);
            esp_err_t release_err = serve_release_locker(locker_entry.locker_id, verified_user_id);
            if (release_err != ESP_OK){
                serve_take_status_set(SERVE_FLOW_FAILED, locker_entry.locker_id, verified_user_id, release_err, "Failed to clear binding");
                continue;
            }

            serve_take_status_set(SERVE_FLOW_SUCCESS, locker_entry.locker_id, verified_user_id, ESP_OK, "palm take completed");
            ESP_LOGI(SERVE_TAG, "Take flow completed for user_id=%u, locker %u released",
                     verified_user_id, locker_entry.locker_id + 1);
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void ready_save_task(void* param){
    while (1){
        if (xSemaphoreTake(ready_save, portMAX_DELAY) == pdTRUE){
            ESP_LOGI(SERVE_TAG, "Received ready_save signal, can start save process...");
            g_save_status.state = SERVE_FLOW_RUNNING;
            g_save_status.err = ESP_OK;
            strncpy(g_save_status.message, "saving in progress", sizeof(g_save_status.message) - 1);
            g_save_status.message[sizeof(g_save_status.message) - 1] = '\0';

            // 清理 XST 中无绑定的孤立用户，避免干扰掌纹识别
            {
                uint16_t ids[50] = {0};
                uint16_t cnt = 0;
                if (xst_cmd_get_all_user_ids(ids, 50, &cnt) == MR_SUCCESS){
                    for (uint16_t i = 0; i < cnt; i++){
                        user_locker_entry_t e;
                        if (locker_db_get_entry_by_user(ids[i], &e) != ESP_OK){
                            ESP_LOGW(SERVE_TAG, "Cleaning orphan XST user %u before enroll", ids[i]);
                            xst_cmd_del_user(ids[i]);
                        }
                    }
                }
            }

            uint8_t locker_id = locker_db_find_free_locker();
            if (locker_id >= 4){
                ESP_LOGW(SERVE_TAG, "No free locker available for save flow");
                if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_ERROR);
                serve_save_status_fail(ESP_ERR_NOT_FOUND, "No free locker available");
                continue;
            }

            uint16_t new_user_id = 0;
            locker_t* target_locker = &lockers[locker_id];

            uint8_t retry_count = 0;
            xst_result_t enroll_res = MR_REJECTED;

            while (retry_count < 3 && enroll_res != MR_SUCCESS){
                ESP_LOGI(SERVE_TAG, "Enroll attempt %u/3 for locker %d", retry_count + 1, target_locker->locker_id + 1);

                enroll_res = xst_cmd_enroll_single((const char*)target_locker->locker_info.locker_user_info,
                                                   1, 10, &new_user_id);

                if (enroll_res == MR_SUCCESS){
                    ESP_LOGI(SERVE_TAG, "Enroll success for locker %d with user_id=%u",
                             target_locker->locker_id + 1, new_user_id);
                    target_locker->have_saved = true;
                    if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_SHORT);
                    break;
                }

                retry_count++;
                if (retry_count < 3) vTaskDelay(pdMS_TO_TICKS(1000));
            }

            if (enroll_res != MR_SUCCESS){
                ESP_LOGE(SERVE_TAG, "Failed to enroll user for locker %d after 3 attempts, res=%d",
                         target_locker->locker_id + 1, enroll_res);
                if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_ERROR);
                serve_save_status_fail(ESP_FAIL, "Failed to enroll user (timeout)");
                target_locker->have_saved = false;
                continue;
            }

            g_save_status.user_id = new_user_id;
            target_locker->locker_info.locker_user_info_id[0] = (new_user_id >> 8) & 0xFF;
            target_locker->locker_info.locker_user_info_id[1] = new_user_id & 0xFF;
            crumble_password(target_locker->password);

            user_locker_entry_t locker_entry = {
                .user_id = new_user_id,
                .locker_id = target_locker->locker_id,
                .timestamp = (uint32_t)xTaskGetTickCount(),
                .is_valid = true,
            };
            memcpy(locker_entry.password, target_locker->password, sizeof(locker_entry.password));
            if (g_has_pending_phone){
                memcpy(locker_entry.phone, g_pending_phone, 4);
                g_has_pending_phone = false;
            }

            esp_err_t db_err = locker_db_add_entry(&locker_entry);
            if (db_err != ESP_OK){
                ESP_LOGE(SERVE_TAG, "Failed to save locker binding for locker %d: %s",
                         target_locker->locker_id + 1, esp_err_to_name(db_err));
                if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_ERROR);
                serve_save_status_fail(db_err, "Failed to persist binding");
                xst_cmd_del_user(new_user_id);
                continue;
            }

            g_save_status.locker_id = target_locker->locker_id;
            memcpy(g_save_status.password, locker_entry.password, sizeof(g_save_status.password));
            g_save_status.has_password = true;

            ESP_LOGI(SERVE_TAG, "Save flow success: user_id=%u locker=%u password=%d%d%d%d",
                     new_user_id, target_locker->locker_id + 1,
                     locker_entry.password[0], locker_entry.password[1],
                     locker_entry.password[2], locker_entry.password[3]);

            vTaskDelay(pdMS_TO_TICKS(1000));
            locker_on(target_locker);
            if (g_buzzer_handle) buzzer_beep_pattern(g_buzzer_handle, BUZZER_BEEP_SUCCESS);
            g_save_status.state = SERVE_FLOW_SUCCESS;
            g_save_status.err = ESP_OK;
            strncpy(g_save_status.message, "save flow completed", sizeof(g_save_status.message) - 1);
            g_save_status.message[sizeof(g_save_status.message) - 1] = '\0';
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
