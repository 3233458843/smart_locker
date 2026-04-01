# Copyright 2026 NXP
# NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
# accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
# activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
# comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
# terms, then you may not retain, install, activate or otherwise use the software.

import utime as time
import usys as sys
import lvgl as lv
import ustruct
import fs_driver

lv.init()

# Register display driver.
disp_drv = lv.sdl_window_create(320, 240)
lv.sdl_window_set_resizeable(disp_drv, False)
lv.sdl_window_set_title(disp_drv, "Simulator (MicroPython)")

# Regsiter input driver
mouse = lv.sdl_mouse_create()

# Add default theme for bottom layer
bottom_layer = lv.layer_bottom()
lv.theme_apply(bottom_layer)

fs_drv = lv.fs_drv_t()
fs_driver.fs_register(fs_drv, 'Z')

def anim_x_cb(obj, v):
    obj.set_x(v)

def anim_y_cb(obj, v):
    obj.set_y(v)

def anim_width_cb(obj, v):
    obj.set_width(v)

def anim_height_cb(obj, v):
    obj.set_height(v)

def anim_img_zoom_cb(obj, v):
    obj.set_scale(v)

def anim_img_rotate_cb(obj, v):
    obj.set_rotation(v)

global_font_cache = {}
def test_font(font_family, font_size):
    global global_font_cache
    if font_family + str(font_size) in global_font_cache:
        return global_font_cache[font_family + str(font_size)]
    if font_size % 2:
        candidates = [
            (font_family, font_size),
            (font_family, font_size-font_size%2),
            (font_family, font_size+font_size%2),
            ("montserrat", font_size-font_size%2),
            ("montserrat", font_size+font_size%2),
            ("montserrat", 16)
        ]
    else:
        candidates = [
            (font_family, font_size),
            ("montserrat", font_size),
            ("montserrat", 16)
        ]
    for (family, size) in candidates:
        try:
            if eval(f'lv.font_{family}_{size}'):
                global_font_cache[font_family + str(font_size)] = eval(f'lv.font_{family}_{size}')
                if family != font_family or size != font_size:
                    print(f'WARNING: lv.font_{family}_{size} is used!')
                return eval(f'lv.font_{family}_{size}')
        except AttributeError:
            try:
                load_font = lv.binfont_create(f"Z:MicroPython/lv_font_{family}_{size}.fnt")
                global_font_cache[font_family + str(font_size)] = load_font
                return load_font
            except:
                if family == font_family and size == font_size:
                    print(f'WARNING: lv.font_{family}_{size} is NOT supported!')

global_image_cache = {}
def load_image(file):
    global global_image_cache
    if file in global_image_cache:
        return global_image_cache[file]
    try:
        with open(file,'rb') as f:
            data = f.read()
    except:
        print(f'Could not open {file}')
        sys.exit()

    img = lv.image_dsc_t({
        'data_size': len(data),
        'data': data
    })
    global_image_cache[file] = img
    return img

def calendar_event_handler(e,obj):
    code = e.get_code()

    if code == lv.EVENT.VALUE_CHANGED:
        source = lv.calendar.__cast__(e.get_current_target())
        date = lv.calendar_date_t()
        if source.get_pressed_date(date) == lv.RESULT.OK:
            source.set_highlighted_dates([date], 1)

def spinbox_increment_event_cb(e, obj):
    code = e.get_code()
    if code == lv.EVENT.SHORT_CLICKED or code == lv.EVENT.LONG_PRESSED_REPEAT:
        obj.increment()
def spinbox_decrement_event_cb(e, obj):
    code = e.get_code()
    if code == lv.EVENT.SHORT_CLICKED or code == lv.EVENT.LONG_PRESSED_REPEAT:
        obj.decrement()

def digital_clock_cb(timer, obj, current_time, show_second, use_ampm):
    hour = int(current_time[0])
    minute = int(current_time[1])
    second = int(current_time[2])
    ampm = current_time[3]
    second = second + 1
    if second == 60:
        second = 0
        minute = minute + 1
        if minute == 60:
            minute = 0
            hour = hour + 1
            if use_ampm:
                if hour == 12:
                    if ampm == 'AM':
                        ampm = 'PM'
                    elif ampm == 'PM':
                        ampm = 'AM'
                if hour > 12:
                    hour = hour % 12
    hour = hour % 24
    if use_ampm:
        if show_second:
            obj.set_text("%d:%02d:%02d %s" %(hour, minute, second, ampm))
        else:
            obj.set_text("%d:%02d %s" %(hour, minute, ampm))
    else:
        if show_second:
            obj.set_text("%d:%02d:%02d" %(hour, minute, second))
        else:
            obj.set_text("%d:%02d" %(hour, minute))
    current_time[0] = hour
    current_time[1] = minute
    current_time[2] = second
    current_time[3] = ampm

def analog_clock_cb(timer, obj):
    datetime = time.localtime()
    hour = datetime[3]
    if hour >= 12: hour = hour - 12
    obj.set_time(hour, datetime[4], datetime[5])

def datetext_event_handler(e, obj):
    code = e.get_code()
    datetext = lv.label.__cast__(e.get_target())
    if code == lv.EVENT.FOCUSED:
        if obj is None:
            bg = lv.layer_top()
            bg.add_flag(lv.obj.FLAG.CLICKABLE)
            obj = lv.calendar(bg)
            scr = lv.screen_active()
            scr_height = scr.get_height()
            scr_width = scr.get_width()
            obj.set_size(int(scr_width * 0.8), int(scr_height * 0.8))
            datestring = datetext.get_text()
            year = int(datestring.split('/')[0])
            month = int(datestring.split('/')[1])
            day = int(datestring.split('/')[2])
            obj.set_showed_date(year, month)
            highlighted_days=[lv.calendar_date_t({'year':year, 'month':month, 'day':day})]
            obj.set_highlighted_dates(highlighted_days, 1)
            obj.align(lv.ALIGN.CENTER, 0, 0)
            lv.calendar_header_arrow(obj)
            obj.add_event_cb(lambda e: datetext_calendar_event_handler(e, datetext), lv.EVENT.ALL, None)
            scr.update_layout()

def datetext_calendar_event_handler(e, obj):
    code = e.get_code()
    calendar = lv.calendar.__cast__(e.get_current_target())
    if code == lv.EVENT.VALUE_CHANGED:
        date = lv.calendar_date_t()
        if calendar.get_pressed_date(date) == lv.RESULT.OK:
            obj.set_text(f"{date.year}/{date.month}/{date.day}")
            bg = lv.layer_top()
            bg.remove_flag(lv.obj.FLAG.CLICKABLE)
            bg.set_style_bg_opa(lv.OPA.TRANSP, 0)
            calendar.delete()

# Create main
main = lv.obj()
main.set_size(320, 240)
main.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for main, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_cont_2
main_cont_2 = lv.obj(main)
main_cont_2.set_pos(0, 0)
main_cont_2.set_size(320, 240)
main_cont_2.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for main_cont_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_cont_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_2.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_2.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_2.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create main_btnm_1
main_btnm_1 = lv.buttonmatrix(main_cont_2)
main_btnm_1_text_map = ["1", "2", "3", "\n", "4", "5", "6", "\n", "7", "8", "9", "\n", "x", "0", "v", "",]
main_btnm_1.set_map(main_btnm_1_text_map)
main_btnm_1.set_pos(94, 54)
main_btnm_1.set_size(220, 180)
# Set style for main_btnm_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_btnm_1.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btnm_1.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btnm_1.set_style_border_color(lv.color_hex(0xc9c9c9), lv.PART.MAIN|lv.STATE.DEFAULT)
main_btnm_1.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btnm_1.set_style_pad_top(16, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btnm_1.set_style_pad_bottom(16, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btnm_1.set_style_pad_left(16, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btnm_1.set_style_pad_right(16, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btnm_1.set_style_pad_row(8, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btnm_1.set_style_pad_column(8, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btnm_1.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btnm_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btnm_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
main_btnm_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for main_btnm_1, Part: lv.PART.ITEMS, State: lv.STATE.DEFAULT.
main_btnm_1.set_style_border_width(1, lv.PART.ITEMS|lv.STATE.DEFAULT)
main_btnm_1.set_style_border_opa(255, lv.PART.ITEMS|lv.STATE.DEFAULT)
main_btnm_1.set_style_border_color(lv.color_hex(0xc9c9c9), lv.PART.ITEMS|lv.STATE.DEFAULT)
main_btnm_1.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.ITEMS|lv.STATE.DEFAULT)
main_btnm_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.ITEMS|lv.STATE.DEFAULT)
main_btnm_1.set_style_text_font(test_font("montserratMedium", 16), lv.PART.ITEMS|lv.STATE.DEFAULT)
main_btnm_1.set_style_text_opa(255, lv.PART.ITEMS|lv.STATE.DEFAULT)
main_btnm_1.set_style_radius(4, lv.PART.ITEMS|lv.STATE.DEFAULT)
main_btnm_1.set_style_bg_opa(255, lv.PART.ITEMS|lv.STATE.DEFAULT)
main_btnm_1.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.ITEMS|lv.STATE.DEFAULT)
main_btnm_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.ITEMS|lv.STATE.DEFAULT)
main_btnm_1.set_style_shadow_width(0, lv.PART.ITEMS|lv.STATE.DEFAULT)

# Create main_label_4
main_label_4 = lv.label(main_cont_2)
main_label_4.set_text("_")
main_label_4.set_long_mode(lv.label.LONG.WRAP)
main_label_4.set_width(lv.pct(100))
main_label_4.set_pos(103, 15)
main_label_4.set_size(40, 40)
# Set style for main_label_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_label_4.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_4.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_4.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_4.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_4.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_4.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_4.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_4.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_4.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_4.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_4.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_4.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_4.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_label_5
main_label_5 = lv.label(main_cont_2)
main_label_5.set_text("_")
main_label_5.set_long_mode(lv.label.LONG.WRAP)
main_label_5.set_width(lv.pct(100))
main_label_5.set_pos(160, 15)
main_label_5.set_size(40, 40)
# Set style for main_label_5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_label_5.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_5.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_5.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_5.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_5.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_5.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_5.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_5.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_5.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_5.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_5.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_5.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_5.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_5.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_label_6
main_label_6 = lv.label(main_cont_2)
main_label_6.set_text("_")
main_label_6.set_long_mode(lv.label.LONG.WRAP)
main_label_6.set_width(lv.pct(100))
main_label_6.set_pos(214, 15)
main_label_6.set_size(40, 40)
# Set style for main_label_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_label_6.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_6.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_6.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_6.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_6.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_6.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_6.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_6.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_6.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_6.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_6.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_6.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_6.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_6.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_label_7
main_label_7 = lv.label(main_cont_2)
main_label_7.set_text("_")
main_label_7.set_long_mode(lv.label.LONG.WRAP)
main_label_7.set_width(lv.pct(100))
main_label_7.set_pos(270, 15)
main_label_7.set_size(40, 40)
# Set style for main_label_7, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_label_7.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_7.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_7.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_7.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_7.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_7.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_7.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_7.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_7.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_7.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_7.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_7.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_7.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_7.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_imgbtn_4
main_imgbtn_4 = lv.imagebutton(main_cont_2)
main_imgbtn_4.add_flag(lv.obj.FLAG.CHECKABLE)
main_imgbtn_4.set_src(lv.imagebutton.STATE.RELEASED, load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\back_50_50.png"), None, None)
main_imgbtn_4.set_src(lv.imagebutton.STATE.CHECKED_RELEASED, load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\back_50_50.png"), None, None)
main_imgbtn_4.add_flag(lv.obj.FLAG.CHECKABLE)
main_imgbtn_4_label = lv.label(main_imgbtn_4)
main_imgbtn_4_label.set_text("")
main_imgbtn_4_label.set_long_mode(lv.label.LONG.WRAP)
main_imgbtn_4_label.set_width(lv.pct(100))
main_imgbtn_4_label.align(lv.ALIGN.CENTER, 0, 0)
main_imgbtn_4.set_style_pad_all(0, lv.STATE.DEFAULT)
main_imgbtn_4.set_pos(0, 0)
main_imgbtn_4.set_size(50, 50)
# Set style for main_imgbtn_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_imgbtn_4.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
main_imgbtn_4.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
main_imgbtn_4.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_imgbtn_4.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
main_imgbtn_4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for main_imgbtn_4, Part: lv.PART.MAIN, State: lv.STATE.PRESSED.
main_imgbtn_4.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
main_imgbtn_4.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.PRESSED)
main_imgbtn_4.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.PRESSED)
main_imgbtn_4.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
main_imgbtn_4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
# Set style for main_imgbtn_4, Part: lv.PART.MAIN, State: lv.STATE.CHECKED.
main_imgbtn_4.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
main_imgbtn_4.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.CHECKED)
main_imgbtn_4.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.CHECKED)
main_imgbtn_4.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
main_imgbtn_4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.CHECKED)
# Set style for main_imgbtn_4, Part: lv.PART.MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
main_imgbtn_4.set_style_image_opa(255, lv.PART.MAIN|lv.imagebutton.STATE.RELEASED)

# Create main_img_5
main_img_5 = lv.image(main_cont_2)
main_img_5.set_src(load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\2_90_91.png"))
main_img_5.add_flag(lv.obj.FLAG.CLICKABLE)
main_img_5.set_pivot(50,50)
main_img_5.set_rotation(0)
main_img_5.set_pos(3, 135)
main_img_5.set_size(90, 91)
# Set style for main_img_5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_img_5.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_label_8
main_label_8 = lv.label(main_cont_2)
main_label_8.set_text("鸽鸽说密码不对别想进去~阿莫西")
main_label_8.set_long_mode(lv.label.LONG.WRAP)
main_label_8.set_width(lv.pct(100))
main_label_8.set_pos(11, 76)
main_label_8.set_size(77, 59)
# Set style for main_label_8, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_label_8.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_8.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_8.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_8.set_style_text_font(test_font("Lemi_Little_Milk_Foam_Font", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_8.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_8.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_8.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_8.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_8.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_8.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_8.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_8.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_8.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_8.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_cont_1
main_cont_1 = lv.obj(main)
main_cont_1.set_pos(0, 0)
main_cont_1.set_size(320, 240)
main_cont_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for main_cont_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_cont_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_1.set_style_bg_image_src(load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\3_320_240.png"), lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_1.set_style_bg_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_1.set_style_bg_image_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create main_animimg_1
main_animimg_1 = lv.animimg(main_cont_1)
main_animimg_1_imgs = [None]*39
main_animimg_1_imgs[0] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_00_70_125.png")
main_animimg_1_imgs[1] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_01_70_125.png")
main_animimg_1_imgs[2] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_02_70_125.png")
main_animimg_1_imgs[3] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_03_70_125.png")
main_animimg_1_imgs[4] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_04_70_125.png")
main_animimg_1_imgs[5] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_05_70_125.png")
main_animimg_1_imgs[6] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_06_70_125.png")
main_animimg_1_imgs[7] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_07_70_125.png")
main_animimg_1_imgs[8] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_08_70_125.png")
main_animimg_1_imgs[9] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_09_70_125.png")
main_animimg_1_imgs[10] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_10_70_125.png")
main_animimg_1_imgs[11] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_11_70_125.png")
main_animimg_1_imgs[12] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_12_70_125.png")
main_animimg_1_imgs[13] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_13_70_125.png")
main_animimg_1_imgs[14] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_14_70_125.png")
main_animimg_1_imgs[15] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_15_70_125.png")
main_animimg_1_imgs[16] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_16_70_125.png")
main_animimg_1_imgs[17] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_17_70_125.png")
main_animimg_1_imgs[18] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_18_70_125.png")
main_animimg_1_imgs[19] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_19_70_125.png")
main_animimg_1_imgs[20] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_20_70_125.png")
main_animimg_1_imgs[21] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_21_70_125.png")
main_animimg_1_imgs[22] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_22_70_125.png")
main_animimg_1_imgs[23] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_23_70_125.png")
main_animimg_1_imgs[24] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_24_70_125.png")
main_animimg_1_imgs[25] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_25_70_125.png")
main_animimg_1_imgs[26] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_26_70_125.png")
main_animimg_1_imgs[27] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_27_70_125.png")
main_animimg_1_imgs[28] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_28_70_125.png")
main_animimg_1_imgs[29] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_29_70_125.png")
main_animimg_1_imgs[30] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_30_70_125.png")
main_animimg_1_imgs[31] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_31_70_125.png")
main_animimg_1_imgs[32] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_32_70_125.png")
main_animimg_1_imgs[33] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_33_70_125.png")
main_animimg_1_imgs[34] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_34_70_125.png")
main_animimg_1_imgs[35] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_35_70_125.png")
main_animimg_1_imgs[36] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_36_70_125.png")
main_animimg_1_imgs[37] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_37_70_125.png")
main_animimg_1_imgs[38] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\1_38_70_125.png")
main_animimg_1.set_src(main_animimg_1_imgs, 39, False)
main_animimg_1.set_duration(30*39)
main_animimg_1.set_repeat_count(lv.ANIM_REPEAT_INFINITE)
main_animimg_1.start()
main_animimg_1.set_pos(244, 90)
main_animimg_1.set_size(70, 125)

# Create main_btn_3
main_btn_3 = lv.button(main_cont_1)
main_btn_3_label = lv.label(main_btn_3)
main_btn_3_label.set_text("取件")
main_btn_3_label.set_long_mode(lv.label.LONG.WRAP)
main_btn_3_label.set_width(lv.pct(100))
main_btn_3_label.align(lv.ALIGN.CENTER, 0, 0)
main_btn_3.set_style_pad_all(0, lv.STATE.DEFAULT)
main_btn_3.set_pos(140, 120)
main_btn_3.set_size(100, 100)
# Set style for main_btn_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_btn_3.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_3.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_3.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_3.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_3.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_3.set_style_text_font(test_font("LXGWWenKaiMono_Medium", 25), lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_3.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_btn_2
main_btn_2 = lv.button(main_cont_1)
main_btn_2_label = lv.label(main_btn_2)
main_btn_2_label.set_text("存件")
main_btn_2_label.set_long_mode(lv.label.LONG.WRAP)
main_btn_2_label.set_width(lv.pct(100))
main_btn_2_label.align(lv.ALIGN.CENTER, 0, 0)
main_btn_2.set_style_pad_all(0, lv.STATE.DEFAULT)
main_btn_2.set_pos(15, 120)
main_btn_2.set_size(100, 100)
# Set style for main_btn_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_btn_2.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_2.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_2.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_2.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_2.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_2.set_style_text_font(test_font("LXGWWenKaiMono_Medium", 25), lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_2.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_label_1
main_label_1 = lv.label(main_cont_1)
main_label_1.set_text("Ciallo～(∠・ω< )⌒★!")
main_label_1.set_long_mode(lv.label.LONG.WRAP)
main_label_1.set_width(lv.pct(100))
main_label_1.set_pos(12, 92)
main_label_1.set_size(225, 21)
# Set style for main_label_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_label_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_1.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_1.set_style_text_font(test_font("LXGWWenKaiMono_Medium", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_1.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_imgbtn_3
main_imgbtn_3 = lv.imagebutton(main_cont_1)
main_imgbtn_3.add_flag(lv.obj.FLAG.CHECKABLE)
main_imgbtn_3.set_src(lv.imagebutton.STATE.RELEASED, load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\setting_40_40.png"), None, None)
main_imgbtn_3.set_src(lv.imagebutton.STATE.PRESSED, load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\setting_40_40.png"), None, None)
main_imgbtn_3.set_src(lv.imagebutton.STATE.CHECKED_RELEASED, load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\setting_40_40.png"), None, None)
main_imgbtn_3.set_src(lv.imagebutton.STATE.CHECKED_PRESSED, load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\setting_40_40.png"), None, None)
main_imgbtn_3.add_flag(lv.obj.FLAG.CHECKABLE)
main_imgbtn_3_label = lv.label(main_imgbtn_3)
main_imgbtn_3_label.set_text("")
main_imgbtn_3_label.set_long_mode(lv.label.LONG.WRAP)
main_imgbtn_3_label.set_width(lv.pct(100))
main_imgbtn_3_label.align(lv.ALIGN.CENTER, 0, 0)
main_imgbtn_3.set_style_pad_all(0, lv.STATE.DEFAULT)
main_imgbtn_3.set_pos(280, 200)
main_imgbtn_3.set_size(40, 40)
# Set style for main_imgbtn_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_imgbtn_3.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
main_imgbtn_3.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
main_imgbtn_3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_imgbtn_3.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
main_imgbtn_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for main_imgbtn_3, Part: lv.PART.MAIN, State: lv.STATE.PRESSED.
main_imgbtn_3.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
main_imgbtn_3.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.PRESSED)
main_imgbtn_3.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.PRESSED)
main_imgbtn_3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
main_imgbtn_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
# Set style for main_imgbtn_3, Part: lv.PART.MAIN, State: lv.STATE.CHECKED.
main_imgbtn_3.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
main_imgbtn_3.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.CHECKED)
main_imgbtn_3.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.CHECKED)
main_imgbtn_3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
main_imgbtn_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.CHECKED)
# Set style for main_imgbtn_3, Part: lv.PART.MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
main_imgbtn_3.set_style_image_opa(255, lv.PART.MAIN|lv.imagebutton.STATE.RELEASED)

# Create main_btn_1
main_btn_1 = lv.button(main_cont_1)
main_btn_1_label = lv.label(main_btn_1)
main_btn_1_label.set_text("help")
main_btn_1_label.set_long_mode(lv.label.LONG.WRAP)
main_btn_1_label.set_width(lv.pct(100))
main_btn_1_label.align(lv.ALIGN.CENTER, 0, 0)
main_btn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
main_btn_1.set_pos(260, 15)
main_btn_1.set_size(50, 30)
# Set style for main_btn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_btn_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_1.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_1.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_1.set_style_text_font(test_font("LXGWWenKaiMono_Medium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_label_3
main_label_3 = lv.label(main_cont_1)
main_label_3.set_text("(●´ω｀●)ゞ")
main_label_3.set_long_mode(lv.label.LONG.WRAP)
main_label_3.set_width(lv.pct(100))
main_label_3.set_pos(127, 129)
main_label_3.set_size(130, 32)
# Set style for main_label_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_label_3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_3.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_3.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_3.set_style_text_font(test_font("LXGWWenKaiMono_Medium", 17), lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_3.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_3.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_3.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_3.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_3.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_3.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_3.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_3.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_label_2
main_label_2 = lv.label(main_cont_1)
main_label_2.set_text("(´,,•ω•,,)♡")
main_label_2.set_long_mode(lv.label.LONG.WRAP)
main_label_2.set_width(lv.pct(100))
main_label_2.set_pos(7, 129)
main_label_2.set_size(114, 23)
# Set style for main_label_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_label_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_2.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_2.set_style_text_font(test_font("LXGWWenKaiMono_Medium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_2.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_2.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_2.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_img_1
main_img_1 = lv.image(main_cont_1)
main_img_1.set_src(load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\locker_30_30.png"))
main_img_1.add_flag(lv.obj.FLAG.CLICKABLE)
main_img_1.set_pivot(50,50)
main_img_1.set_rotation(0)
main_img_1.set_pos(10, 5)
main_img_1.set_size(30, 30)
# Set style for main_img_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_img_1.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_img_3
main_img_3 = lv.image(main_cont_1)
main_img_3.set_src(load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\locker_30_30.png"))
main_img_3.add_flag(lv.obj.FLAG.CLICKABLE)
main_img_3.set_pivot(50,50)
main_img_3.set_rotation(0)
main_img_3.set_pos(10, 50)
main_img_3.set_size(30, 30)
# Set style for main_img_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_img_3.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_img_2
main_img_2 = lv.image(main_cont_1)
main_img_2.set_src(load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\locker_30_30.png"))
main_img_2.add_flag(lv.obj.FLAG.CLICKABLE)
main_img_2.set_pivot(50,50)
main_img_2.set_rotation(0)
main_img_2.set_pos(70, 5)
main_img_2.set_size(30, 30)
# Set style for main_img_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_img_2.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_img_4
main_img_4 = lv.image(main_cont_1)
main_img_4.set_src(load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\locker_30_30.png"))
main_img_4.add_flag(lv.obj.FLAG.CLICKABLE)
main_img_4.set_pivot(50,50)
main_img_4.set_rotation(0)
main_img_4.set_pos(70, 50)
main_img_4.set_size(30, 30)
# Set style for main_img_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_img_4.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_cont_3
main_cont_3 = lv.obj(main_cont_1)
main_cont_3.set_pos(45, 10)
main_cont_3.set_size(20, 20)
main_cont_3.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for main_cont_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_cont_3.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_3.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_3.set_style_border_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_3.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_3.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_3.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_3.set_style_bg_color(lv.color_hex(0x2FDA64), lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_3.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_3.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_3.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_3.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_3.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_cont_4
main_cont_4 = lv.obj(main_cont_1)
main_cont_4.set_pos(105, 10)
main_cont_4.set_size(20, 20)
main_cont_4.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for main_cont_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_cont_4.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_4.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_4.set_style_border_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_4.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_4.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_4.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_4.set_style_bg_color(lv.color_hex(0x2FDA64), lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_4.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_4.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_4.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_4.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_4.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_cont_5
main_cont_5 = lv.obj(main_cont_1)
main_cont_5.set_pos(45, 55)
main_cont_5.set_size(20, 20)
main_cont_5.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for main_cont_5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_cont_5.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_5.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_5.set_style_border_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_5.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_5.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_5.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_5.set_style_bg_color(lv.color_hex(0x2FDA64), lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_5.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_5.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_5.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_5.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_5.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_5.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_cont_6
main_cont_6 = lv.obj(main_cont_1)
main_cont_6.set_pos(105, 55)
main_cont_6.set_size(20, 20)
main_cont_6.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for main_cont_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_cont_6.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_6.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_6.set_style_border_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_6.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_6.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_6.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_6.set_style_bg_color(lv.color_hex(0x2FDA64), lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_6.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_6.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_6.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_6.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_6.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_6.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_animimg_2
main_animimg_2 = lv.animimg(main_cont_1)
main_animimg_2_imgs = [None]*9
main_animimg_2_imgs[0] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\2_0_106_87.png")
main_animimg_2_imgs[1] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\2_1_106_87.png")
main_animimg_2_imgs[2] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\2_2_106_87.png")
main_animimg_2_imgs[3] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\2_3_106_87.png")
main_animimg_2_imgs[4] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\2_4_106_87.png")
main_animimg_2_imgs[5] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\2_5_106_87.png")
main_animimg_2_imgs[6] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\2_6_106_87.png")
main_animimg_2_imgs[7] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\2_7_106_87.png")
main_animimg_2_imgs[8] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\2_8_106_87.png")
main_animimg_2.set_src(main_animimg_2_imgs, 9, False)
main_animimg_2.set_duration(30*9)
main_animimg_2.set_repeat_count(lv.ANIM_REPEAT_INFINITE)
main_animimg_2.start()
main_animimg_2.set_pos(132, 4)
main_animimg_2.set_size(106, 87)

main.update_layout()
# Create saved_item
saved_item = lv.obj()
saved_item.set_size(320, 240)
saved_item.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for saved_item, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
saved_item.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create saved_item_cont_1
saved_item_cont_1 = lv.obj(saved_item)
saved_item_cont_1.set_pos(0, 0)
saved_item_cont_1.set_size(320, 240)
saved_item_cont_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for saved_item_cont_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
saved_item_cont_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_cont_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_cont_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_cont_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_cont_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_cont_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_cont_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_cont_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_cont_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_cont_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create saved_item_label_2
saved_item_label_2 = lv.label(saved_item_cont_1)
saved_item_label_2.set_text("正在录入掌静脉，请靠近...")
saved_item_label_2.set_long_mode(lv.label.LONG.WRAP)
saved_item_label_2.set_width(lv.pct(100))
saved_item_label_2.set_pos(59, 32)
saved_item_label_2.set_size(233, 18)
# Set style for saved_item_label_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
saved_item_label_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_label_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_label_2.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_label_2.set_style_text_font(test_font("Lemi_Little_Milk_Foam_Font", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_label_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_label_2.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_label_2.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_label_2.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_label_2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_label_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_label_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_label_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_label_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_label_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create saved_item_label_1
saved_item_label_1 = lv.label(saved_item_cont_1)
saved_item_label_1.set_text("Label")
saved_item_label_1.set_long_mode(lv.label.LONG.WRAP)
saved_item_label_1.set_width(lv.pct(100))
saved_item_label_1.set_pos(270, 205)
saved_item_label_1.set_size(46, 18)
# Set style for saved_item_label_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
saved_item_label_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_label_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_label_1.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_label_1.set_style_text_font(test_font("LXGWWenKaiMono_Medium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_label_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_label_1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_label_1.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_label_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_label_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_label_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_label_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_label_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_label_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_label_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create saved_item_imgbtn_1
saved_item_imgbtn_1 = lv.imagebutton(saved_item_cont_1)
saved_item_imgbtn_1.add_flag(lv.obj.FLAG.CHECKABLE)
saved_item_imgbtn_1.set_src(lv.imagebutton.STATE.RELEASED, load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\back_50_50.png"), None, None)
saved_item_imgbtn_1.add_flag(lv.obj.FLAG.CHECKABLE)
saved_item_imgbtn_1_label = lv.label(saved_item_imgbtn_1)
saved_item_imgbtn_1_label.set_text("")
saved_item_imgbtn_1_label.set_long_mode(lv.label.LONG.WRAP)
saved_item_imgbtn_1_label.set_width(lv.pct(100))
saved_item_imgbtn_1_label.align(lv.ALIGN.CENTER, 0, 0)
saved_item_imgbtn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
saved_item_imgbtn_1.set_pos(0, 0)
saved_item_imgbtn_1.set_size(50, 50)
# Set style for saved_item_imgbtn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
saved_item_imgbtn_1.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_imgbtn_1.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_imgbtn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_imgbtn_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_imgbtn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for saved_item_imgbtn_1, Part: lv.PART.MAIN, State: lv.STATE.PRESSED.
saved_item_imgbtn_1.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
saved_item_imgbtn_1.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.PRESSED)
saved_item_imgbtn_1.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.PRESSED)
saved_item_imgbtn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
saved_item_imgbtn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
# Set style for saved_item_imgbtn_1, Part: lv.PART.MAIN, State: lv.STATE.CHECKED.
saved_item_imgbtn_1.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
saved_item_imgbtn_1.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.CHECKED)
saved_item_imgbtn_1.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.CHECKED)
saved_item_imgbtn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
saved_item_imgbtn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.CHECKED)
# Set style for saved_item_imgbtn_1, Part: lv.PART.MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
saved_item_imgbtn_1.set_style_image_opa(255, lv.PART.MAIN|lv.imagebutton.STATE.RELEASED)

# Create saved_item_animimg_1
saved_item_animimg_1 = lv.animimg(saved_item_cont_1)
saved_item_animimg_1_imgs = [None]*40
saved_item_animimg_1_imgs[0] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_00_139_135.png")
saved_item_animimg_1_imgs[1] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_01_139_135.png")
saved_item_animimg_1_imgs[2] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_02_139_135.png")
saved_item_animimg_1_imgs[3] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_03_139_135.png")
saved_item_animimg_1_imgs[4] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_04_139_135.png")
saved_item_animimg_1_imgs[5] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_05_139_135.png")
saved_item_animimg_1_imgs[6] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_06_139_135.png")
saved_item_animimg_1_imgs[7] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_07_139_135.png")
saved_item_animimg_1_imgs[8] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_08_139_135.png")
saved_item_animimg_1_imgs[9] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_09_139_135.png")
saved_item_animimg_1_imgs[10] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_10_139_135.png")
saved_item_animimg_1_imgs[11] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_11_139_135.png")
saved_item_animimg_1_imgs[12] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_12_139_135.png")
saved_item_animimg_1_imgs[13] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_13_139_135.png")
saved_item_animimg_1_imgs[14] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_14_139_135.png")
saved_item_animimg_1_imgs[15] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_15_139_135.png")
saved_item_animimg_1_imgs[16] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_16_139_135.png")
saved_item_animimg_1_imgs[17] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_17_139_135.png")
saved_item_animimg_1_imgs[18] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_18_139_135.png")
saved_item_animimg_1_imgs[19] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_19_139_135.png")
saved_item_animimg_1_imgs[20] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_20_139_135.png")
saved_item_animimg_1_imgs[21] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_21_139_135.png")
saved_item_animimg_1_imgs[22] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_22_139_135.png")
saved_item_animimg_1_imgs[23] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_23_139_135.png")
saved_item_animimg_1_imgs[24] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_24_139_135.png")
saved_item_animimg_1_imgs[25] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_25_139_135.png")
saved_item_animimg_1_imgs[26] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_26_139_135.png")
saved_item_animimg_1_imgs[27] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_27_139_135.png")
saved_item_animimg_1_imgs[28] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_28_139_135.png")
saved_item_animimg_1_imgs[29] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_29_139_135.png")
saved_item_animimg_1_imgs[30] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_30_139_135.png")
saved_item_animimg_1_imgs[31] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_31_139_135.png")
saved_item_animimg_1_imgs[32] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_32_139_135.png")
saved_item_animimg_1_imgs[33] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_33_139_135.png")
saved_item_animimg_1_imgs[34] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_34_139_135.png")
saved_item_animimg_1_imgs[35] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_35_139_135.png")
saved_item_animimg_1_imgs[36] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_36_139_135.png")
saved_item_animimg_1_imgs[37] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_37_139_135.png")
saved_item_animimg_1_imgs[38] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_38_139_135.png")
saved_item_animimg_1_imgs[39] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\9_39_139_135.png")
saved_item_animimg_1.set_src(saved_item_animimg_1_imgs, 40, False)
saved_item_animimg_1.set_duration(30*40)
saved_item_animimg_1.set_repeat_count(lv.ANIM_REPEAT_INFINITE)
saved_item_animimg_1.start()
saved_item_animimg_1.set_pos(96, 50)
saved_item_animimg_1.set_size(139, 135)

# Create saved_item_bar_1
saved_item_bar_1 = lv.bar(saved_item_cont_1)
saved_item_bar_1.set_style_anim_duration(1000, 0)
saved_item_bar_1.set_mode(lv.bar.MODE.NORMAL)
saved_item_bar_1.set_range(0, 100)
saved_item_bar_1.set_value(50, lv.ANIM.ON)
saved_item_bar_1.set_pos(284, 17)
saved_item_bar_1.set_size(20, 180)
# Set style for saved_item_bar_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
saved_item_bar_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_bar_1.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
saved_item_bar_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for saved_item_bar_1, Part: lv.PART.INDICATOR, State: lv.STATE.DEFAULT.
saved_item_bar_1.set_style_bg_opa(255, lv.PART.INDICATOR|lv.STATE.DEFAULT)
saved_item_bar_1.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.INDICATOR|lv.STATE.DEFAULT)
saved_item_bar_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.INDICATOR|lv.STATE.DEFAULT)
saved_item_bar_1.set_style_radius(10, lv.PART.INDICATOR|lv.STATE.DEFAULT)

# Create saved_item_animimg_2
saved_item_animimg_2 = lv.animimg(saved_item_cont_1)
saved_item_animimg_2_imgs = [None]*10
saved_item_animimg_2_imgs[0] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\5_00_90_90.png")
saved_item_animimg_2_imgs[1] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\5_01_90_90.png")
saved_item_animimg_2_imgs[2] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\5_02_90_90.png")
saved_item_animimg_2_imgs[3] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\5_03_90_90.png")
saved_item_animimg_2_imgs[4] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\5_04_90_90.png")
saved_item_animimg_2_imgs[5] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\5_05_90_90.png")
saved_item_animimg_2_imgs[6] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\5_06_90_90.png")
saved_item_animimg_2_imgs[7] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\5_07_90_90.png")
saved_item_animimg_2_imgs[8] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\5_08_90_90.png")
saved_item_animimg_2_imgs[9] = load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\5_09_90_90.png")
saved_item_animimg_2.set_src(saved_item_animimg_2_imgs, 10, False)
saved_item_animimg_2.set_duration(30*10)
saved_item_animimg_2.set_repeat_count(lv.ANIM_REPEAT_INFINITE)
saved_item_animimg_2.start()
saved_item_animimg_2.set_pos(8, 67)
saved_item_animimg_2.set_size(90, 90)

saved_item.update_layout()
# Create taked_item
taked_item = lv.obj()
taked_item.set_size(320, 240)
taked_item.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for taked_item, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
taked_item.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create taked_item_cont_1
taked_item_cont_1 = lv.obj(taked_item)
taked_item_cont_1.set_pos(0, 0)
taked_item_cont_1.set_size(320, 240)
taked_item_cont_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for taked_item_cont_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
taked_item_cont_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
taked_item_cont_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
taked_item_cont_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
taked_item_cont_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
taked_item_cont_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
taked_item_cont_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
taked_item_cont_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
taked_item_cont_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
taked_item_cont_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
taked_item_cont_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create taked_item_imgbtn_1
taked_item_imgbtn_1 = lv.imagebutton(taked_item)
taked_item_imgbtn_1.add_flag(lv.obj.FLAG.CHECKABLE)
taked_item_imgbtn_1.set_src(lv.imagebutton.STATE.RELEASED, load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\back_50_50.png"), None, None)
taked_item_imgbtn_1.add_flag(lv.obj.FLAG.CHECKABLE)
taked_item_imgbtn_1_label = lv.label(taked_item_imgbtn_1)
taked_item_imgbtn_1_label.set_text("")
taked_item_imgbtn_1_label.set_long_mode(lv.label.LONG.WRAP)
taked_item_imgbtn_1_label.set_width(lv.pct(100))
taked_item_imgbtn_1_label.align(lv.ALIGN.CENTER, 0, 0)
taked_item_imgbtn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
taked_item_imgbtn_1.set_pos(0, 0)
taked_item_imgbtn_1.set_size(50, 50)
# Set style for taked_item_imgbtn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
taked_item_imgbtn_1.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
taked_item_imgbtn_1.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
taked_item_imgbtn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
taked_item_imgbtn_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
taked_item_imgbtn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for taked_item_imgbtn_1, Part: lv.PART.MAIN, State: lv.STATE.PRESSED.
taked_item_imgbtn_1.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
taked_item_imgbtn_1.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.PRESSED)
taked_item_imgbtn_1.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.PRESSED)
taked_item_imgbtn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
taked_item_imgbtn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
# Set style for taked_item_imgbtn_1, Part: lv.PART.MAIN, State: lv.STATE.CHECKED.
taked_item_imgbtn_1.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
taked_item_imgbtn_1.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.CHECKED)
taked_item_imgbtn_1.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.CHECKED)
taked_item_imgbtn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
taked_item_imgbtn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.CHECKED)
# Set style for taked_item_imgbtn_1, Part: lv.PART.MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
taked_item_imgbtn_1.set_style_image_opa(255, lv.PART.MAIN|lv.imagebutton.STATE.RELEASED)

# Create taked_item_img_1
taked_item_img_1 = lv.image(taked_item)
taked_item_img_1.set_src(load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\10_100_100.png"))
taked_item_img_1.add_flag(lv.obj.FLAG.CLICKABLE)
taked_item_img_1.set_pivot(50,50)
taked_item_img_1.set_rotation(0)
taked_item_img_1.set_pos(113, 72)
taked_item_img_1.set_size(100, 100)
# Set style for taked_item_img_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
taked_item_img_1.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create taked_item_label_1
taked_item_label_1 = lv.label(taked_item)
taked_item_label_1.set_text("请将手掌置于传感器前方10cm左右，正在识别...")
taked_item_label_1.set_long_mode(lv.label.LONG.WRAP)
taked_item_label_1.set_width(lv.pct(100))
taked_item_label_1.set_pos(65, 32)
taked_item_label_1.set_size(198, 32)
# Set style for taked_item_label_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
taked_item_label_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
taked_item_label_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
taked_item_label_1.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
taked_item_label_1.set_style_text_font(test_font("Lemi_Little_Milk_Foam_Font", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
taked_item_label_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
taked_item_label_1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
taked_item_label_1.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
taked_item_label_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
taked_item_label_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
taked_item_label_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
taked_item_label_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
taked_item_label_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
taked_item_label_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
taked_item_label_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

taked_item.update_layout()
# Create help_item
help_item = lv.obj()
help_item.set_size(320, 240)
help_item.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for help_item, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
help_item.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create help_item_cont_1
help_item_cont_1 = lv.obj(help_item)
help_item_cont_1.set_pos(0, 0)
help_item_cont_1.set_size(320, 240)
help_item_cont_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for help_item_cont_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
help_item_cont_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_cont_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_cont_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_cont_1.set_style_bg_color(lv.color_hex(0xa4d6fb), lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_cont_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_cont_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_cont_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_cont_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_cont_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_cont_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create help_item_cont_2
help_item_cont_2 = lv.obj(help_item_cont_1)
help_item_cont_2.set_pos(52, 45)
help_item_cont_2.set_size(215, 170)
help_item_cont_2.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for help_item_cont_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
help_item_cont_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_cont_2.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_cont_2.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_cont_2.set_style_bg_color(lv.color_hex(0xebcece), lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_cont_2.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_cont_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_cont_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_cont_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_cont_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_cont_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create help_item_label_3
help_item_label_3 = lv.label(help_item_cont_2)
help_item_label_3.set_text("1.点击主界面[存件]或者[取件]\n2.将手掌悬停于传感器前10cm左右\n3.等待设备发出“滴”声，柜门即可弹开\n！！！绿色标识柜门可用，红色标识柜门已被占用！！！")
help_item_label_3.set_long_mode(lv.label.LONG.WRAP)
help_item_label_3.set_width(lv.pct(100))
help_item_label_3.set_pos(2, 2)
help_item_label_3.set_size(210, 109)
# Set style for help_item_label_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
help_item_label_3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_3.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_3.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_3.set_style_text_font(test_font("Lemi_Little_Milk_Foam_Font", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_3.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_3.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_3.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_3.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_3.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_3.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_3.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_3.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create help_item_label_4
help_item_label_4 = lv.label(help_item_cont_2)
help_item_label_4.set_text("遇到问题联系管理员同学~\n123456789（刘同学）")
help_item_label_4.set_long_mode(lv.label.LONG.WRAP)
help_item_label_4.set_width(lv.pct(100))
help_item_label_4.set_pos(6, 130)
help_item_label_4.set_size(203, 32)
# Set style for help_item_label_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
help_item_label_4.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_4.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_4.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_4.set_style_text_font(test_font("Lemi_Little_Milk_Foam_Font", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_4.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_4.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_4.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_4.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_4.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_4.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_4.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_4.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_4.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create help_item_label_1
help_item_label_1 = lv.label(help_item_cont_1)
help_item_label_1.set_text("ヾ(●゜▽゜●)♡")
help_item_label_1.set_long_mode(lv.label.LONG.WRAP)
help_item_label_1.set_width(lv.pct(100))
help_item_label_1.set_pos(41, 9)
help_item_label_1.set_size(168, 30)
# Set style for help_item_label_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
help_item_label_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_1.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_1.set_style_text_font(test_font("LXGWWenKaiMono_Medium", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_1.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create help_item_label_2
help_item_label_2 = lv.label(help_item_cont_1)
help_item_label_2.set_text("使用说明")
help_item_label_2.set_long_mode(lv.label.LONG.WRAP)
help_item_label_2.set_width(lv.pct(100))
help_item_label_2.set_pos(208, 3)
help_item_label_2.set_size(106, 36)
# Set style for help_item_label_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
help_item_label_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_2.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_2.set_style_text_font(test_font("Lemi_Little_Milk_Foam_Font", 29), lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_2.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_2.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_2.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_label_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create help_item_imgbtn_1
help_item_imgbtn_1 = lv.imagebutton(help_item_cont_1)
help_item_imgbtn_1.add_flag(lv.obj.FLAG.CHECKABLE)
help_item_imgbtn_1.set_src(lv.imagebutton.STATE.RELEASED, load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\back_50_50.png"), None, None)
help_item_imgbtn_1.add_flag(lv.obj.FLAG.CHECKABLE)
help_item_imgbtn_1_label = lv.label(help_item_imgbtn_1)
help_item_imgbtn_1_label.set_text("")
help_item_imgbtn_1_label.set_long_mode(lv.label.LONG.WRAP)
help_item_imgbtn_1_label.set_width(lv.pct(100))
help_item_imgbtn_1_label.align(lv.ALIGN.CENTER, 0, 0)
help_item_imgbtn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
help_item_imgbtn_1.set_pos(0, 0)
help_item_imgbtn_1.set_size(50, 50)
# Set style for help_item_imgbtn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
help_item_imgbtn_1.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_imgbtn_1.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_imgbtn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_imgbtn_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
help_item_imgbtn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for help_item_imgbtn_1, Part: lv.PART.MAIN, State: lv.STATE.PRESSED.
help_item_imgbtn_1.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
help_item_imgbtn_1.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.PRESSED)
help_item_imgbtn_1.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.PRESSED)
help_item_imgbtn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
help_item_imgbtn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
# Set style for help_item_imgbtn_1, Part: lv.PART.MAIN, State: lv.STATE.CHECKED.
help_item_imgbtn_1.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
help_item_imgbtn_1.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.CHECKED)
help_item_imgbtn_1.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.CHECKED)
help_item_imgbtn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
help_item_imgbtn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.CHECKED)
# Set style for help_item_imgbtn_1, Part: lv.PART.MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
help_item_imgbtn_1.set_style_image_opa(255, lv.PART.MAIN|lv.imagebutton.STATE.RELEASED)

help_item.update_layout()
# Create setting_item
setting_item = lv.obj()
setting_item.set_size(320, 240)
setting_item.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for setting_item, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_item.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create setting_item_cont_1
setting_item_cont_1 = lv.obj(setting_item)
setting_item_cont_1.set_pos(0, 0)
setting_item_cont_1.set_size(320, 240)
setting_item_cont_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for setting_item_cont_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_item_cont_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_cont_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_cont_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_cont_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_cont_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_cont_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_cont_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_cont_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_cont_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_cont_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create setting_item_spangroup_1
setting_item_spangroup_1 = lv.spangroup(setting_item_cont_1)
setting_item_spangroup_1.set_align(lv.TEXT_ALIGN.LEFT)
setting_item_spangroup_1.set_overflow(lv.SPAN_OVERFLOW.CLIP)
setting_item_spangroup_1.set_mode(lv.SPAN_MODE.BREAK)
# create spans
setting_item_spangroup_1_span = setting_item_spangroup_1.new_span()
setting_item_spangroup_1_span.set_text("hello")
setting_item_spangroup_1_span.style.set_text_color(lv.color_hex(0x000000))
setting_item_spangroup_1_span.style.set_text_decor(lv.TEXT_DECOR.NONE)
setting_item_spangroup_1_span.style.set_text_font(test_font("montserratMedium", 12))
setting_item_spangroup_1.set_pos(0, 60)
setting_item_spangroup_1.set_size(240, 180)
setting_item_spangroup_1.refr_mode()
# Set style for setting_item_spangroup_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_item_spangroup_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_spangroup_1.set_style_radius(20, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_spangroup_1.set_style_bg_opa(110, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_spangroup_1.set_style_bg_color(lv.color_hex(0xd1d2d3), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_spangroup_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_spangroup_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_spangroup_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_spangroup_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_spangroup_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_spangroup_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create setting_item_imgbtn_1
setting_item_imgbtn_1 = lv.imagebutton(setting_item_cont_1)
setting_item_imgbtn_1.add_flag(lv.obj.FLAG.CHECKABLE)
setting_item_imgbtn_1.set_src(lv.imagebutton.STATE.RELEASED, load_image(r"E:\GUIder_Project\smart_locker\smart_locker\generated\MicroPython\back_50_50.png"), None, None)
setting_item_imgbtn_1.add_flag(lv.obj.FLAG.CHECKABLE)
setting_item_imgbtn_1_label = lv.label(setting_item_imgbtn_1)
setting_item_imgbtn_1_label.set_text("")
setting_item_imgbtn_1_label.set_long_mode(lv.label.LONG.WRAP)
setting_item_imgbtn_1_label.set_width(lv.pct(100))
setting_item_imgbtn_1_label.align(lv.ALIGN.CENTER, 0, 0)
setting_item_imgbtn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
setting_item_imgbtn_1.set_pos(0, 0)
setting_item_imgbtn_1.set_size(50, 50)
# Set style for setting_item_imgbtn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_item_imgbtn_1.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_imgbtn_1.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_imgbtn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_imgbtn_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_imgbtn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for setting_item_imgbtn_1, Part: lv.PART.MAIN, State: lv.STATE.PRESSED.
setting_item_imgbtn_1.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
setting_item_imgbtn_1.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.PRESSED)
setting_item_imgbtn_1.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.PRESSED)
setting_item_imgbtn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
setting_item_imgbtn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
# Set style for setting_item_imgbtn_1, Part: lv.PART.MAIN, State: lv.STATE.CHECKED.
setting_item_imgbtn_1.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
setting_item_imgbtn_1.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.CHECKED)
setting_item_imgbtn_1.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.CHECKED)
setting_item_imgbtn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
setting_item_imgbtn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.CHECKED)
# Set style for setting_item_imgbtn_1, Part: lv.PART.MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
setting_item_imgbtn_1.set_style_image_opa(255, lv.PART.MAIN|lv.imagebutton.STATE.RELEASED)

# Create setting_item_cont_2
setting_item_cont_2 = lv.obj(setting_item_cont_1)
setting_item_cont_2.set_pos(255, 0)
setting_item_cont_2.set_size(65, 240)
setting_item_cont_2.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)
# Set style for setting_item_cont_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_item_cont_2.set_style_border_width(2, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_cont_2.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_cont_2.set_style_border_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_cont_2.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_cont_2.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_cont_2.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_cont_2.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_cont_2.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_cont_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_cont_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_cont_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_cont_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_cont_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create setting_item_btn_1
setting_item_btn_1 = lv.button(setting_item_cont_2)
setting_item_btn_1_label = lv.label(setting_item_btn_1)
setting_item_btn_1_label.set_text("RES")
setting_item_btn_1_label.set_long_mode(lv.label.LONG.WRAP)
setting_item_btn_1_label.set_width(lv.pct(100))
setting_item_btn_1_label.align(lv.ALIGN.CENTER, 0, 0)
setting_item_btn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
setting_item_btn_1.set_pos(0, 0)
setting_item_btn_1.set_size(61, 50)
# Set style for setting_item_btn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_item_btn_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_1.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_1.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_1.set_style_text_font(test_font("LXGWWenKaiMono_Medium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create setting_item_btn_2
setting_item_btn_2 = lv.button(setting_item_cont_2)
setting_item_btn_2_label = lv.label(setting_item_btn_2)
setting_item_btn_2_label.set_text("READ")
setting_item_btn_2_label.set_long_mode(lv.label.LONG.WRAP)
setting_item_btn_2_label.set_width(lv.pct(100))
setting_item_btn_2_label.align(lv.ALIGN.CENTER, 0, 0)
setting_item_btn_2.set_style_pad_all(0, lv.STATE.DEFAULT)
setting_item_btn_2.set_pos(0, 80)
setting_item_btn_2.set_size(61, 50)
# Set style for setting_item_btn_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_item_btn_2.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_2.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_2.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_2.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_2.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_2.set_style_text_font(test_font("LXGWWenKaiMono_Medium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_2.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create setting_item_btn_3
setting_item_btn_3 = lv.button(setting_item_cont_2)
setting_item_btn_3_label = lv.label(setting_item_btn_3)
setting_item_btn_3_label.set_text("DEL")
setting_item_btn_3_label.set_long_mode(lv.label.LONG.WRAP)
setting_item_btn_3_label.set_width(lv.pct(100))
setting_item_btn_3_label.align(lv.ALIGN.CENTER, 0, 0)
setting_item_btn_3.set_style_pad_all(0, lv.STATE.DEFAULT)
setting_item_btn_3.set_pos(0, 160)
setting_item_btn_3.set_size(61, 50)
# Set style for setting_item_btn_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_item_btn_3.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_3.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_3.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_3.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_3.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_3.set_style_text_font(test_font("LXGWWenKaiMono_Medium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_3.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create setting_item_btn_4
setting_item_btn_4 = lv.button(setting_item_cont_2)
setting_item_btn_4_label = lv.label(setting_item_btn_4)
setting_item_btn_4_label.set_text("VERI")
setting_item_btn_4_label.set_long_mode(lv.label.LONG.WRAP)
setting_item_btn_4_label.set_width(lv.pct(100))
setting_item_btn_4_label.align(lv.ALIGN.CENTER, 0, 0)
setting_item_btn_4.set_style_pad_all(0, lv.STATE.DEFAULT)
setting_item_btn_4.set_pos(0, 240)
setting_item_btn_4.set_size(61, 50)
# Set style for setting_item_btn_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_item_btn_4.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_4.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_4.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_4.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_4.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_4.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_4.set_style_text_font(test_font("LXGWWenKaiMono_Medium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_4.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_btn_4.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create setting_item_label_1
setting_item_label_1 = lv.label(setting_item_cont_1)
setting_item_label_1.set_text("")
setting_item_label_1.set_long_mode(lv.label.LONG.WRAP)
setting_item_label_1.set_width(lv.pct(100))
setting_item_label_1.set_pos(77, 12)
setting_item_label_1.set_size(147, 33)
# Set style for setting_item_label_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_item_label_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_label_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_label_1.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_label_1.set_style_text_font(test_font("LXGWWenKaiMono_Medium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_label_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_label_1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_label_1.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_label_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_label_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_label_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_label_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_label_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_label_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_item_label_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

setting_item.update_layout()

def main_btnm_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.VALUE_CHANGED):
        pass
        

main_btnm_1.add_event_cb(lambda e: main_btnm_1_event_handler(e), lv.EVENT.ALL, None)

def main_imgbtn_4_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        main_cont_1.remove_flag(lv.obj.FLAG.HIDDEN)
        
        main_cont_2.add_flag(lv.obj.FLAG.HIDDEN)
        
main_imgbtn_4.add_event_cb(lambda e: main_imgbtn_4_event_handler(e), lv.EVENT.ALL, None)

def main_btn_3_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(taked_item, lv.SCR_LOAD_ANIM.NONE, 100, 100, False)
main_btn_3.add_event_cb(lambda e: main_btn_3_event_handler(e), lv.EVENT.ALL, None)

def main_btn_2_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(saved_item, lv.SCR_LOAD_ANIM.NONE, 100, 100, False)
main_btn_2.add_event_cb(lambda e: main_btn_2_event_handler(e), lv.EVENT.ALL, None)

def main_imgbtn_3_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        main_cont_1.add_flag(lv.obj.FLAG.HIDDEN)
        
        main_cont_2.remove_flag(lv.obj.FLAG.HIDDEN)
        
main_imgbtn_3.add_event_cb(lambda e: main_imgbtn_3_event_handler(e), lv.EVENT.ALL, None)

def main_btn_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(help_item, lv.SCR_LOAD_ANIM.NONE, 100, 100, False)
main_btn_1.add_event_cb(lambda e: main_btn_1_event_handler(e), lv.EVENT.ALL, None)

def saved_item_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOADED):
        pass
        

saved_item.add_event_cb(lambda e: saved_item_event_handler(e), lv.EVENT.ALL, None)

def saved_item_imgbtn_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(main, lv.SCR_LOAD_ANIM.NONE, 100, 100, False)
saved_item_imgbtn_1.add_event_cb(lambda e: saved_item_imgbtn_1_event_handler(e), lv.EVENT.ALL, None)

def saved_item_bar_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.VALUE_CHANGED):
        pass
        

saved_item_bar_1.add_event_cb(lambda e: saved_item_bar_1_event_handler(e), lv.EVENT.ALL, None)

def taked_item_imgbtn_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(main, lv.SCR_LOAD_ANIM.NONE, 100, 100, False)
taked_item_imgbtn_1.add_event_cb(lambda e: taked_item_imgbtn_1_event_handler(e), lv.EVENT.ALL, None)

def help_item_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(main, lv.SCR_LOAD_ANIM.NONE, 100, 100, False)
help_item.add_event_cb(lambda e: help_item_event_handler(e), lv.EVENT.ALL, None)

def help_item_imgbtn_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(main, lv.SCR_LOAD_ANIM.NONE, 100, 100, False)
help_item_imgbtn_1.add_event_cb(lambda e: help_item_imgbtn_1_event_handler(e), lv.EVENT.ALL, None)

def setting_item_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(main, lv.SCR_LOAD_ANIM.NONE, 100, 100, False)
setting_item.add_event_cb(lambda e: setting_item_event_handler(e), lv.EVENT.ALL, None)

def setting_item_imgbtn_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(main, lv.SCR_LOAD_ANIM.NONE, 100, 100, False)
setting_item_imgbtn_1.add_event_cb(lambda e: setting_item_imgbtn_1_event_handler(e), lv.EVENT.ALL, None)

def setting_item_btn_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        setting_item_label_1.set_text("模组复位")
setting_item_btn_1.add_event_cb(lambda e: setting_item_btn_1_event_handler(e), lv.EVENT.ALL, None)

def setting_item_btn_2_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        setting_item_label_1.set_text("读取所有用户")
setting_item_btn_2.add_event_cb(lambda e: setting_item_btn_2_event_handler(e), lv.EVENT.ALL, None)

def setting_item_btn_3_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        setting_item_label_1.set_text("删除所有用户")
setting_item_btn_3.add_event_cb(lambda e: setting_item_btn_3_event_handler(e), lv.EVENT.ALL, None)

def setting_item_btn_4_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        setting_item_label_1.set_text("开始识别掌纹")
setting_item_btn_4.add_event_cb(lambda e: setting_item_btn_4_event_handler(e), lv.EVENT.ALL, None)

# content from custom.py

# Load the default screen
lv.screen_load(main)

if __name__ == '__main__':
    while True:
        lv.task_handler()
        time.sleep_ms(5)
