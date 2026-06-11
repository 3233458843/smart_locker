#ifndef LOCKER_ACTIONS_H
#define LOCKER_ACTIONS_H

#include "esp_err.h"
#include <stdint.h>

/**
 * @brief Open locker, beep success, delete XST user, clear DB binding.
 *
 * Performs the complete locker release sequence. If XST delete fails,
 * the local binding is still cleared. Returns ESP_OK on full success,
 * or the DB error if binding removal fails.
 *
 * @param locker_id  Locker index (0-3)
 * @param user_id    XST user ID to delete
 * @return esp_err_t ESP_OK on success, or locker_db_remove_entry_by_locker error
 */
esp_err_t serve_release_locker(uint8_t locker_id, uint16_t user_id);

#endif /* LOCKER_ACTIONS_H */
