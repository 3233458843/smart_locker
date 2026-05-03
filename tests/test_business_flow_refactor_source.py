import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def read(rel):
    return (ROOT / rel).read_text(encoding="utf-8", errors="ignore")


class BusinessFlowRefactorSourceTests(unittest.TestCase):
    def test_serve_header_exposes_request_api(self):
        src = read("serve/serve.h")
        self.assertIn("typedef enum", src)
        self.assertIn("serve_request_save", src)
        self.assertIn("serve_request_take_by_palm", src)
        self.assertIn("serve_request_debug_verify", src)
        self.assertIn("serve_get_save_status", src)

    def test_ui_events_use_serve_request_api_instead_of_direct_semaphore_give(self):
        src = read("bsp/ui/generated/events_init.c")
        self.assertIn("serve_request_save()", src)
        self.assertIn("serve_request_take_by_palm()", src)
        self.assertIn("serve_request_debug_verify()", src)
        self.assertNotIn("xSemaphoreGive(ready_save)", src)
        self.assertNotIn("xSemaphoreGive(ready_take)", src)
        self.assertNotIn("xSemaphoreGive(verify_debug)", src)

    def test_save_flow_has_explicit_status_update_points(self):
        src = read("serve/serve.c")
        self.assertIn("serve_get_save_status", src)
        self.assertIn("SERVE_FLOW_RUNNING", src)
        self.assertIn("SERVE_FLOW_SUCCESS", src)
        self.assertIn("SERVE_FLOW_FAILED", src)
        self.assertIn("No free locker available for save flow", src)

    def test_locker_domain_exposes_password_lookup_api(self):
        header = read("bsp/locker/locker.h")
        src = read("bsp/locker/locker.c")
        self.assertIn("locker_db_get_entry_by_password", header)
        self.assertIn("locker_db_get_entry_by_password", src)
        self.assertIn("memcmp(user_locker_db[i].password, password, 4) == 0", src)

    def test_password_takeout_uses_service_api_not_hardcoded_password(self):
        header = read("serve/serve.h")
        serve = read("serve/serve.c")
        ui = read("bsp/ui/generated/events_init.c")
        self.assertIn("serve_request_take_by_password", header)
        self.assertIn("serve_get_take_status", header)
        self.assertIn("locker_db_get_entry_by_password", serve)
        self.assertIn("locker_on(&lockers[locker_entry.locker_id])", serve)
        self.assertIn("locker_db_remove_entry_by_locker(locker_entry.locker_id)", serve)
        self.assertIn("serve_request_take_by_password(password)", ui)
        self.assertNotIn("take_correct_pwd", ui)
        self.assertNotIn('"0000"', ui)

    def test_palm_takeout_clears_local_binding_even_if_xst_delete_warns(self):
        serve = read("serve/serve.c")
        self.assertIn("local locker binding will still be cleared", serve)
        self.assertIn("locker_db_remove_entry_by_locker(locker_entry.locker_id)", serve)
        self.assertIn('"palm take completed, XST delete warning"', serve)

    def test_main_locker_status_refresh_uses_have_saved_not_door_state(self):
        src = read("main/main.c")
        self.assertIn("main_locker_status_update_task", src)
        self.assertIn("main_locker_status_styles_init", src)
        self.assertIn("main_locker_status_cache_reset", src)
        self.assertIn("main_locker_status_widget_is_valid", src)
        self.assertIn("main_locker_status_need_refresh", src)
        self.assertIn("main_locker_status_apply", src)
        self.assertIn("g_locker_status_bound_main", src)
        self.assertIn("lv_obj_is_valid(obj)", src)
        self.assertIn("main_locker_status_cache_reset();", src)
        self.assertIn("main_locker_status_need_refresh()", src)
        self.assertIn("main_locker_status_styles_init();", src)
        self.assertIn("has_item_in_locker(&lockers[i])", src)
        self.assertIn("main_locker_status_apply(widget, g_locker_status_shown[i]);", src)
        self.assertIn("main_locker_status_apply(locker_widgets[i], occupied);", src)
        self.assertIn("occupied ? lv_color_hex(0xFF0000) : lv_color_hex(0x2FDA64)", src)
        self.assertIn("lv_screen_active() != guider_ui.main", src)
        self.assertIn("pdMS_TO_TICKS(1000)", src)
        self.assertIn("lv_obj_set_style_radius(widget, LV_RADIUS_CIRCLE", src)
        self.assertNotIn("lv_obj_add_state(locker_widgets[i], LV_STATE_USER_1)", src)
        self.assertNotIn("lv_obj_remove_state(locker_widgets[i], LV_STATE_USER_1)", src)
        self.assertNotIn("LV_STATE_USER_1", src)
        self.assertIn("lv_obj_set_style_bg_color(widget,", src)
        self.assertNotIn("lv_obj_set_style_bg_color(guider_ui.main_locker1", src)
        self.assertNotIn("main_locker_status_overlay_create", src)
        self.assertNotIn("g_locker_status_red[", src)
        self.assertNotIn("g_locker_status_green[", src)


if __name__ == "__main__":
    unittest.main()
