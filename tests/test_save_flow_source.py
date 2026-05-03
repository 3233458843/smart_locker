import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def read(rel):
    return (ROOT / rel).read_text(encoding="utf-8", errors="ignore")


class SaveFlowSourceTests(unittest.TestCase):
    def test_crumble_password_uses_fixed_four_digit_loop_not_pointer_size(self):
        src = read("bsp/locker/locker.c")
        self.assertIn("void crumble_password(uint8_t* password)", src)
        self.assertNotIn("sizeof(password)", src)
        self.assertIn("for (uint8_t i = 0; i < 4; i++)", src)
        self.assertIn("esp_random() % 10", src)

    def test_ready_save_task_persists_binding_with_locker_db_add_entry(self):
        src = read("serve/serve.c")
        self.assertIn("void ready_save_task(void* param)", src)
        self.assertIn("user_locker_entry_t", src)
        self.assertIn("locker_db_add_entry", src)

    def test_ready_save_task_stores_xst_user_id_and_password_in_target_locker(self):
        src = read("serve/serve.c")
        self.assertIn("target_locker->locker_info.locker_user_info", src)
        self.assertIn("target_locker->locker_info.locker_user_info_id[0] = (new_user_id >> 8) & 0xFF", src)
        self.assertIn("target_locker->locker_info.locker_user_info_id[1] = new_user_id & 0xFF", src)
        self.assertIn("crumble_password(target_locker->password)", src)

    def test_power_on_sync_clears_stale_local_locker_bindings_from_xst_user_list(self):
        locker_h = read("bsp/locker/locker.h")
        locker_c = read("bsp/locker/locker.c")
        main_c = read("main/main.c")
        xst_h = read("bsp/XST/xst.h")
        xst_c = read("bsp/XST/xst.c")

        self.assertIn("xst_cmd_get_all_user_ids", xst_h)
        self.assertIn("xst_cmd_get_all_user_ids", xst_c)
        self.assertIn("locker_db_sync_with_xst_users", locker_h)
        self.assertIn("esp_err_t locker_db_sync_with_xst_users(const uint16_t* xst_user_ids, uint16_t xst_user_count)", locker_c)
        self.assertIn("locker_db_clear_all", locker_c)
        self.assertIn("locker_db_remove_entry_by_locker((uint8_t)i)", locker_c)
        self.assertIn("Power-on locker/XST sync", main_c)
        self.assertIn("xst_cmd_get_all_user_ids", main_c)
        self.assertIn("locker_db_sync_with_xst_users", main_c)


if __name__ == "__main__":
    unittest.main()
