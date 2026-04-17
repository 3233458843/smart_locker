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
disp_drv = lv.sdl_window_create(240, 320)
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
main.set_size(240, 320)
main.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for main, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_cont_1
main_cont_1 = lv.obj(main)
main_cont_1.set_pos(0, 0)
main_cont_1.set_size(240, 320)
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
main_cont_1.set_style_bg_image_src(load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\3_240_320.png"), lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_1.set_style_bg_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_1.set_style_bg_image_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_cont_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create main_img_2
main_img_2 = lv.image(main_cont_1)
main_img_2.set_src(load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\locker_20_20.png"))
main_img_2.add_flag(lv.obj.FLAG.CLICKABLE)
main_img_2.set_pivot(50,50)
main_img_2.set_rotation(0)
main_img_2.set_pos(45, 10)
main_img_2.set_size(20, 20)
# Set style for main_img_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_img_2.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_btn_1
main_btn_1 = lv.button(main_cont_1)
main_btn_1_label = lv.label(main_btn_1)
main_btn_1_label.set_text("存件")
main_btn_1_label.set_long_mode(lv.label.LONG.WRAP)
main_btn_1_label.set_width(lv.pct(100))
main_btn_1_label.align(lv.ALIGN.CENTER, 0, 0)
main_btn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
main_btn_1.set_pos(20, 120)
main_btn_1.set_size(200, 80)
# Set style for main_btn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_btn_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_1.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_1.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_1.set_style_text_font(test_font("Lemi_Little_Milk_Foam_Font", 50), lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_img_3
main_img_3 = lv.image(main_cont_1)
main_img_3.set_src(load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\locker_20_20.png"))
main_img_3.add_flag(lv.obj.FLAG.CLICKABLE)
main_img_3.set_pivot(50,50)
main_img_3.set_rotation(0)
main_img_3.set_pos(5, 40)
main_img_3.set_size(20, 20)
# Set style for main_img_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_img_3.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_img_4
main_img_4 = lv.image(main_cont_1)
main_img_4.set_src(load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\locker_20_20.png"))
main_img_4.add_flag(lv.obj.FLAG.CLICKABLE)
main_img_4.set_pivot(50,50)
main_img_4.set_rotation(0)
main_img_4.set_pos(45, 40)
main_img_4.set_size(20, 20)
# Set style for main_img_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_img_4.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_img_1
main_img_1 = lv.image(main_cont_1)
main_img_1.set_src(load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\locker_20_20.png"))
main_img_1.add_flag(lv.obj.FLAG.CLICKABLE)
main_img_1.set_pivot(50,50)
main_img_1.set_rotation(0)
main_img_1.set_pos(6, 10)
main_img_1.set_size(20, 20)
# Set style for main_img_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_img_1.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_btn_2
main_btn_2 = lv.button(main_cont_1)
main_btn_2_label = lv.label(main_btn_2)
main_btn_2_label.set_text("取件")
main_btn_2_label.set_long_mode(lv.label.LONG.WRAP)
main_btn_2_label.set_width(lv.pct(100))
main_btn_2_label.align(lv.ALIGN.CENTER, 0, 0)
main_btn_2.set_style_pad_all(0, lv.STATE.DEFAULT)
main_btn_2.set_pos(19, 220)
main_btn_2.set_size(200, 80)
# Set style for main_btn_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_btn_2.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_2.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_2.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_2.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_2.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_2.set_style_text_font(test_font("Lemi_Little_Milk_Foam_Font", 50), lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_2.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_btn_3
main_btn_3 = lv.button(main_cont_1)
main_btn_3_label = lv.label(main_btn_3)
main_btn_3_label.set_text("help")
main_btn_3_label.set_long_mode(lv.label.LONG.WRAP)
main_btn_3_label.set_width(lv.pct(100))
main_btn_3_label.align(lv.ALIGN.CENTER, 0, 0)
main_btn_3.set_style_pad_all(0, lv.STATE.DEFAULT)
main_btn_3.set_pos(174, 5)
main_btn_3.set_size(60, 40)
# Set style for main_btn_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_btn_3.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_3.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_3.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_3.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_3.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_3.set_style_text_font(test_font("ArchitectsDaughter", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_btn_3.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_locker2
main_locker2 = lv.obj(main_cont_1)
main_locker2.set_pos(70, 15)
main_locker2.set_size(10, 10)
main_locker2.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for main_locker2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_locker2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker2.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker2.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker2.set_style_bg_color(lv.color_hex(0x2FDA64), lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker2.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_locker3
main_locker3 = lv.obj(main_cont_1)
main_locker3.set_pos(30, 45)
main_locker3.set_size(10, 10)
main_locker3.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for main_locker3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_locker3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker3.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker3.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker3.set_style_bg_color(lv.color_hex(0x2FDA64), lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker3.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker3.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker3.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker3.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker3.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_locker4
main_locker4 = lv.obj(main_cont_1)
main_locker4.set_pos(70, 45)
main_locker4.set_size(10, 10)
main_locker4.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for main_locker4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_locker4.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker4.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker4.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker4.set_style_bg_color(lv.color_hex(0x2FDA64), lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker4.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker4.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker4.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker4.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker4.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_locker1
main_locker1 = lv.obj(main_cont_1)
main_locker1.set_pos(30, 15)
main_locker1.set_size(10, 10)
main_locker1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for main_locker1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_locker1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker1.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker1.set_style_bg_color(lv.color_hex(0x2FDA64), lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_locker1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create main_animimg_1
main_animimg_1 = lv.animimg(main_cont_1)
main_animimg_1_imgs = [None]*2
main_animimg_1_imgs[0] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\12_0_80_80.png")
main_animimg_1_imgs[1] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\12_1_80_80.png")
main_animimg_1.set_src(main_animimg_1_imgs, 2, False)
main_animimg_1.set_duration(80*2)
main_animimg_1.set_repeat_count(100000)
main_animimg_1.start()
main_animimg_1.set_pos(85, 0)
main_animimg_1.set_size(80, 80)

# Create main_label_1
main_label_1 = lv.label(main_cont_1)
main_label_1.set_text("Ciallo～(∠・ω< )⌒★!")
main_label_1.set_long_mode(lv.label.LONG.WRAP)
main_label_1.set_width(lv.pct(100))
main_label_1.set_pos(4, 96)
main_label_1.set_size(204, 21)
# Set style for main_label_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
main_label_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_1.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
main_label_1.set_style_text_font(test_font("LXGWWenKaiMono_Medium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
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

main.update_layout()
# Create take_page
take_page = lv.obj()
take_page.set_size(240, 320)
take_page.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for take_page, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
take_page.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create take_page_cont_1
take_page_cont_1 = lv.obj(take_page)
take_page_cont_1.set_pos(0, 0)
take_page_cont_1.set_size(240, 320)
take_page_cont_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for take_page_cont_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
take_page_cont_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_cont_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_cont_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_cont_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_cont_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_cont_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_cont_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_cont_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_cont_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_cont_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create take_page_imgbtn_1
take_page_imgbtn_1 = lv.imagebutton(take_page_cont_1)
take_page_imgbtn_1.add_flag(lv.obj.FLAG.CHECKABLE)
take_page_imgbtn_1.set_src(lv.imagebutton.STATE.RELEASED, load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\back_50_50.png"), None, None)
take_page_imgbtn_1.add_flag(lv.obj.FLAG.CHECKABLE)
take_page_imgbtn_1_label = lv.label(take_page_imgbtn_1)
take_page_imgbtn_1_label.set_text("")
take_page_imgbtn_1_label.set_long_mode(lv.label.LONG.WRAP)
take_page_imgbtn_1_label.set_width(lv.pct(100))
take_page_imgbtn_1_label.align(lv.ALIGN.CENTER, 0, 0)
take_page_imgbtn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
take_page_imgbtn_1.set_pos(10, 9)
take_page_imgbtn_1.set_size(50, 50)
# Set style for take_page_imgbtn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
take_page_imgbtn_1.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_imgbtn_1.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_imgbtn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_imgbtn_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_imgbtn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for take_page_imgbtn_1, Part: lv.PART.MAIN, State: lv.STATE.PRESSED.
take_page_imgbtn_1.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
take_page_imgbtn_1.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.PRESSED)
take_page_imgbtn_1.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.PRESSED)
take_page_imgbtn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
take_page_imgbtn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
# Set style for take_page_imgbtn_1, Part: lv.PART.MAIN, State: lv.STATE.CHECKED.
take_page_imgbtn_1.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
take_page_imgbtn_1.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.CHECKED)
take_page_imgbtn_1.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.CHECKED)
take_page_imgbtn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
take_page_imgbtn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.CHECKED)
# Set style for take_page_imgbtn_1, Part: lv.PART.MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
take_page_imgbtn_1.set_style_image_opa(255, lv.PART.MAIN|lv.imagebutton.STATE.RELEASED)

# Create take_page_label_1
take_page_label_1 = lv.label(take_page_cont_1)
take_page_label_1.set_text("请将手掌置于传感器前方10cm左右，正在识别...")
take_page_label_1.set_long_mode(lv.label.LONG.WRAP)
take_page_label_1.set_width(lv.pct(100))
take_page_label_1.set_pos(0, 80)
take_page_label_1.set_size(240, 40)
# Set style for take_page_label_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
take_page_label_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_label_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_label_1.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_label_1.set_style_text_font(test_font("Lemi_Little_Milk_Foam_Font", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_label_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_label_1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_label_1.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_label_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_label_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_label_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_label_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_label_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_label_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_label_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create take_page_img_1
take_page_img_1 = lv.image(take_page_cont_1)
take_page_img_1.set_src(load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\10_150_150.png"))
take_page_img_1.add_flag(lv.obj.FLAG.CLICKABLE)
take_page_img_1.set_pivot(50,50)
take_page_img_1.set_rotation(0)
take_page_img_1.set_pos(45, 130)
take_page_img_1.set_size(150, 150)
# Set style for take_page_img_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
take_page_img_1.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create take_page_btn_1
take_page_btn_1 = lv.button(take_page_cont_1)
take_page_btn_1_label = lv.label(take_page_btn_1)
take_page_btn_1_label.set_text("密码开柜")
take_page_btn_1_label.set_long_mode(lv.label.LONG.WRAP)
take_page_btn_1_label.set_width(lv.pct(100))
take_page_btn_1_label.align(lv.ALIGN.CENTER, 0, 0)
take_page_btn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
take_page_btn_1.set_pos(150, 10)
take_page_btn_1.set_size(80, 40)
# Set style for take_page_btn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
take_page_btn_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_btn_1.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_btn_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_btn_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_btn_1.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_btn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_btn_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_btn_1.set_style_text_font(test_font("Lemi_Little_Milk_Foam_Font", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_btn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_btn_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create take_page_cont_2
take_page_cont_2 = lv.obj(take_page)
take_page_cont_2.set_pos(0, 0)
take_page_cont_2.set_size(240, 320)
take_page_cont_2.add_flag(lv.obj.FLAG.HIDDEN)
take_page_cont_2.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for take_page_cont_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
take_page_cont_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_cont_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_cont_2.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_cont_2.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_cont_2.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_cont_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_cont_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_cont_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_cont_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_cont_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create take_page_imgbtn_2
take_page_imgbtn_2 = lv.imagebutton(take_page_cont_2)
take_page_imgbtn_2.add_flag(lv.obj.FLAG.CHECKABLE)
take_page_imgbtn_2.set_src(lv.imagebutton.STATE.RELEASED, load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\back_50_50.png"), None, None)
take_page_imgbtn_2.add_flag(lv.obj.FLAG.CHECKABLE)
take_page_imgbtn_2_label = lv.label(take_page_imgbtn_2)
take_page_imgbtn_2_label.set_text("")
take_page_imgbtn_2_label.set_long_mode(lv.label.LONG.WRAP)
take_page_imgbtn_2_label.set_width(lv.pct(100))
take_page_imgbtn_2_label.align(lv.ALIGN.CENTER, 0, 0)
take_page_imgbtn_2.set_style_pad_all(0, lv.STATE.DEFAULT)
take_page_imgbtn_2.set_pos(10, 10)
take_page_imgbtn_2.set_size(50, 50)
# Set style for take_page_imgbtn_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
take_page_imgbtn_2.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_imgbtn_2.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_imgbtn_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_imgbtn_2.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
take_page_imgbtn_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for take_page_imgbtn_2, Part: lv.PART.MAIN, State: lv.STATE.PRESSED.
take_page_imgbtn_2.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
take_page_imgbtn_2.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.PRESSED)
take_page_imgbtn_2.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.PRESSED)
take_page_imgbtn_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
take_page_imgbtn_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
# Set style for take_page_imgbtn_2, Part: lv.PART.MAIN, State: lv.STATE.CHECKED.
take_page_imgbtn_2.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
take_page_imgbtn_2.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.CHECKED)
take_page_imgbtn_2.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.CHECKED)
take_page_imgbtn_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
take_page_imgbtn_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.CHECKED)
# Set style for take_page_imgbtn_2, Part: lv.PART.MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
take_page_imgbtn_2.set_style_image_opa(255, lv.PART.MAIN|lv.imagebutton.STATE.RELEASED)

take_page.update_layout()
# Create save_page
save_page = lv.obj()
save_page.set_size(240, 320)
save_page.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for save_page, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
save_page.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create save_page_cont_1
save_page_cont_1 = lv.obj(save_page)
save_page_cont_1.set_pos(0, 0)
save_page_cont_1.set_size(240, 320)
save_page_cont_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for save_page_cont_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
save_page_cont_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
save_page_cont_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
save_page_cont_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
save_page_cont_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
save_page_cont_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
save_page_cont_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
save_page_cont_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
save_page_cont_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
save_page_cont_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
save_page_cont_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create save_page_imgbtn_1
save_page_imgbtn_1 = lv.imagebutton(save_page_cont_1)
save_page_imgbtn_1.add_flag(lv.obj.FLAG.CHECKABLE)
save_page_imgbtn_1.set_src(lv.imagebutton.STATE.RELEASED, load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\back_50_50.png"), None, None)
save_page_imgbtn_1.add_flag(lv.obj.FLAG.CHECKABLE)
save_page_imgbtn_1_label = lv.label(save_page_imgbtn_1)
save_page_imgbtn_1_label.set_text("")
save_page_imgbtn_1_label.set_long_mode(lv.label.LONG.WRAP)
save_page_imgbtn_1_label.set_width(lv.pct(100))
save_page_imgbtn_1_label.align(lv.ALIGN.CENTER, 0, 0)
save_page_imgbtn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
save_page_imgbtn_1.set_pos(10, 10)
save_page_imgbtn_1.set_size(50, 50)
# Set style for save_page_imgbtn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
save_page_imgbtn_1.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
save_page_imgbtn_1.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
save_page_imgbtn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
save_page_imgbtn_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
save_page_imgbtn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for save_page_imgbtn_1, Part: lv.PART.MAIN, State: lv.STATE.PRESSED.
save_page_imgbtn_1.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
save_page_imgbtn_1.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.PRESSED)
save_page_imgbtn_1.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.PRESSED)
save_page_imgbtn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
save_page_imgbtn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
# Set style for save_page_imgbtn_1, Part: lv.PART.MAIN, State: lv.STATE.CHECKED.
save_page_imgbtn_1.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
save_page_imgbtn_1.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.CHECKED)
save_page_imgbtn_1.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.CHECKED)
save_page_imgbtn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
save_page_imgbtn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.CHECKED)
# Set style for save_page_imgbtn_1, Part: lv.PART.MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
save_page_imgbtn_1.set_style_image_opa(255, lv.PART.MAIN|lv.imagebutton.STATE.RELEASED)

# Create save_page_animimg_1
save_page_animimg_1 = lv.animimg(save_page_cont_1)
save_page_animimg_1_imgs = [None]*40
save_page_animimg_1_imgs[0] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_00_160_160.png")
save_page_animimg_1_imgs[1] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_01_160_160.png")
save_page_animimg_1_imgs[2] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_02_160_160.png")
save_page_animimg_1_imgs[3] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_03_160_160.png")
save_page_animimg_1_imgs[4] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_04_160_160.png")
save_page_animimg_1_imgs[5] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_05_160_160.png")
save_page_animimg_1_imgs[6] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_06_160_160.png")
save_page_animimg_1_imgs[7] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_07_160_160.png")
save_page_animimg_1_imgs[8] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_08_160_160.png")
save_page_animimg_1_imgs[9] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_09_160_160.png")
save_page_animimg_1_imgs[10] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_10_160_160.png")
save_page_animimg_1_imgs[11] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_11_160_160.png")
save_page_animimg_1_imgs[12] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_12_160_160.png")
save_page_animimg_1_imgs[13] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_13_160_160.png")
save_page_animimg_1_imgs[14] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_14_160_160.png")
save_page_animimg_1_imgs[15] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_15_160_160.png")
save_page_animimg_1_imgs[16] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_16_160_160.png")
save_page_animimg_1_imgs[17] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_17_160_160.png")
save_page_animimg_1_imgs[18] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_18_160_160.png")
save_page_animimg_1_imgs[19] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_19_160_160.png")
save_page_animimg_1_imgs[20] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_20_160_160.png")
save_page_animimg_1_imgs[21] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_21_160_160.png")
save_page_animimg_1_imgs[22] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_22_160_160.png")
save_page_animimg_1_imgs[23] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_23_160_160.png")
save_page_animimg_1_imgs[24] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_24_160_160.png")
save_page_animimg_1_imgs[25] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_25_160_160.png")
save_page_animimg_1_imgs[26] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_26_160_160.png")
save_page_animimg_1_imgs[27] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_27_160_160.png")
save_page_animimg_1_imgs[28] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_28_160_160.png")
save_page_animimg_1_imgs[29] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_29_160_160.png")
save_page_animimg_1_imgs[30] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_30_160_160.png")
save_page_animimg_1_imgs[31] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_31_160_160.png")
save_page_animimg_1_imgs[32] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_32_160_160.png")
save_page_animimg_1_imgs[33] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_33_160_160.png")
save_page_animimg_1_imgs[34] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_34_160_160.png")
save_page_animimg_1_imgs[35] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_35_160_160.png")
save_page_animimg_1_imgs[36] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_36_160_160.png")
save_page_animimg_1_imgs[37] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_37_160_160.png")
save_page_animimg_1_imgs[38] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_38_160_160.png")
save_page_animimg_1_imgs[39] = load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\9_39_160_160.png")
save_page_animimg_1.set_src(save_page_animimg_1_imgs, 40, False)
save_page_animimg_1.set_duration(60*40)
save_page_animimg_1.set_repeat_count(lv.ANIM_REPEAT_INFINITE)
save_page_animimg_1.start()
save_page_animimg_1.set_pos(40, 150)
save_page_animimg_1.set_size(160, 160)

# Create save_page_label_1
save_page_label_1 = lv.label(save_page_cont_1)
save_page_label_1.set_text("将手掌悬停于传感器前10cm左右\n等待设备发出“滴”声，柜门即可弹开")
save_page_label_1.set_long_mode(lv.label.LONG.WRAP)
save_page_label_1.set_width(lv.pct(100))
save_page_label_1.set_pos(27, 74)
save_page_label_1.set_size(185, 69)
# Set style for save_page_label_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
save_page_label_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
save_page_label_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
save_page_label_1.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
save_page_label_1.set_style_text_font(test_font("Lemi_Little_Milk_Foam_Font", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
save_page_label_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
save_page_label_1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
save_page_label_1.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
save_page_label_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
save_page_label_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
save_page_label_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
save_page_label_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
save_page_label_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
save_page_label_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
save_page_label_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

save_page.update_layout()
# Create help_page
help_page = lv.obj()
help_page.set_size(240, 320)
help_page.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for help_page, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
help_page.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create help_page_cont_1
help_page_cont_1 = lv.obj(help_page)
help_page_cont_1.set_pos(0, 0)
help_page_cont_1.set_size(240, 320)
help_page_cont_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for help_page_cont_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
help_page_cont_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_cont_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_cont_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_cont_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_cont_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_cont_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_cont_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_cont_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_cont_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_cont_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create help_page_imgbtn_1
help_page_imgbtn_1 = lv.imagebutton(help_page_cont_1)
help_page_imgbtn_1.add_flag(lv.obj.FLAG.CHECKABLE)
help_page_imgbtn_1.set_src(lv.imagebutton.STATE.RELEASED, load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\back_50_50.png"), None, None)
help_page_imgbtn_1.add_flag(lv.obj.FLAG.CHECKABLE)
help_page_imgbtn_1_label = lv.label(help_page_imgbtn_1)
help_page_imgbtn_1_label.set_text("")
help_page_imgbtn_1_label.set_long_mode(lv.label.LONG.WRAP)
help_page_imgbtn_1_label.set_width(lv.pct(100))
help_page_imgbtn_1_label.align(lv.ALIGN.CENTER, 0, 0)
help_page_imgbtn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
help_page_imgbtn_1.set_pos(10, 9)
help_page_imgbtn_1.set_size(50, 50)
# Set style for help_page_imgbtn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
help_page_imgbtn_1.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_imgbtn_1.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_imgbtn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_imgbtn_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_imgbtn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for help_page_imgbtn_1, Part: lv.PART.MAIN, State: lv.STATE.PRESSED.
help_page_imgbtn_1.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
help_page_imgbtn_1.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.PRESSED)
help_page_imgbtn_1.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.PRESSED)
help_page_imgbtn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
help_page_imgbtn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
# Set style for help_page_imgbtn_1, Part: lv.PART.MAIN, State: lv.STATE.CHECKED.
help_page_imgbtn_1.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
help_page_imgbtn_1.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.CHECKED)
help_page_imgbtn_1.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.CHECKED)
help_page_imgbtn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
help_page_imgbtn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.CHECKED)
# Set style for help_page_imgbtn_1, Part: lv.PART.MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
help_page_imgbtn_1.set_style_image_opa(255, lv.PART.MAIN|lv.imagebutton.STATE.RELEASED)

# Create help_page_cont_2
help_page_cont_2 = lv.obj(help_page_cont_1)
help_page_cont_2.set_pos(5, 115)
help_page_cont_2.set_size(230, 180)
help_page_cont_2.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for help_page_cont_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
help_page_cont_2.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_cont_2.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_cont_2.set_style_border_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_cont_2.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_cont_2.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_cont_2.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_cont_2.set_style_bg_color(lv.color_hex(0xeaeaea), lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_cont_2.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_cont_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_cont_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_cont_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_cont_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_cont_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create help_page_label_2
help_page_label_2 = lv.label(help_page_cont_2)
help_page_label_2.set_text("遇到问题联系管理员同学~\n123456789（刘同学）")
help_page_label_2.set_long_mode(lv.label.LONG.WRAP)
help_page_label_2.set_width(lv.pct(100))
help_page_label_2.set_pos(4, 122)
help_page_label_2.set_size(220, 30)
# Set style for help_page_label_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
help_page_label_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_2.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_2.set_style_text_font(test_font("Lemi_Little_Milk_Foam_Font", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_2.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_2.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_2.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create help_page_label_1
help_page_label_1 = lv.label(help_page_cont_2)
help_page_label_1.set_text("1.点击主界面[存件]或者[取件]\n2.将手掌悬停于传感器前10cm左右\n3.等待设备发出“滴”声，柜门即可弹开\n！！！绿色标识柜门可用，红色标识柜门已被占用！！！")
help_page_label_1.set_long_mode(lv.label.LONG.WRAP)
help_page_label_1.set_width(lv.pct(100))
help_page_label_1.set_pos(4, 10)
help_page_label_1.set_size(220, 100)
# Set style for help_page_label_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
help_page_label_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_1.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_1.set_style_text_font(test_font("Lemi_Little_Milk_Foam_Font", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_1.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create help_page_label_3
help_page_label_3 = lv.label(help_page_cont_1)
help_page_label_3.set_text("ヾ(●゜▽゜●)♡")
help_page_label_3.set_long_mode(lv.label.LONG.WRAP)
help_page_label_3.set_width(lv.pct(100))
help_page_label_3.set_pos(62, 28)
help_page_label_3.set_size(174, 32)
# Set style for help_page_label_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
help_page_label_3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_3.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_3.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_3.set_style_text_font(test_font("LXGWWenKaiMono_Medium", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_3.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_3.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_3.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_3.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_3.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_3.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_3.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_3.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create help_page_label_4
help_page_label_4 = lv.label(help_page_cont_1)
help_page_label_4.set_text("使用说明")
help_page_label_4.set_long_mode(lv.label.LONG.WRAP)
help_page_label_4.set_width(lv.pct(100))
help_page_label_4.set_pos(50, 80)
help_page_label_4.set_size(140, 32)
# Set style for help_page_label_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
help_page_label_4.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_4.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_4.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_4.set_style_text_font(test_font("Lemi_Little_Milk_Foam_Font", 33), lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_4.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_4.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_4.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_4.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_4.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_4.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_4.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_4.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_4.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
help_page_label_4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

help_page.update_layout()
# Create setting_page
setting_page = lv.obj()
setting_page.set_size(240, 320)
setting_page.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for setting_page, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_page.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create setting_page_cont_1
setting_page_cont_1 = lv.obj(setting_page)
setting_page_cont_1.set_pos(0, 0)
setting_page_cont_1.set_size(240, 320)
setting_page_cont_1.add_flag(lv.obj.FLAG.HIDDEN)
setting_page_cont_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for setting_page_cont_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_page_cont_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create setting_page_imgbtn_1
setting_page_imgbtn_1 = lv.imagebutton(setting_page_cont_1)
setting_page_imgbtn_1.add_flag(lv.obj.FLAG.CHECKABLE)
setting_page_imgbtn_1.set_src(lv.imagebutton.STATE.RELEASED, load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\back_50_50.png"), None, None)
setting_page_imgbtn_1.add_flag(lv.obj.FLAG.CHECKABLE)
setting_page_imgbtn_1_label = lv.label(setting_page_imgbtn_1)
setting_page_imgbtn_1_label.set_text("")
setting_page_imgbtn_1_label.set_long_mode(lv.label.LONG.WRAP)
setting_page_imgbtn_1_label.set_width(lv.pct(100))
setting_page_imgbtn_1_label.align(lv.ALIGN.CENTER, 0, 0)
setting_page_imgbtn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
setting_page_imgbtn_1.set_pos(10, 9)
setting_page_imgbtn_1.set_size(50, 50)
# Set style for setting_page_imgbtn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_page_imgbtn_1.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_imgbtn_1.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_imgbtn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_imgbtn_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_imgbtn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for setting_page_imgbtn_1, Part: lv.PART.MAIN, State: lv.STATE.PRESSED.
setting_page_imgbtn_1.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
setting_page_imgbtn_1.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.PRESSED)
setting_page_imgbtn_1.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.PRESSED)
setting_page_imgbtn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
setting_page_imgbtn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
# Set style for setting_page_imgbtn_1, Part: lv.PART.MAIN, State: lv.STATE.CHECKED.
setting_page_imgbtn_1.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
setting_page_imgbtn_1.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.CHECKED)
setting_page_imgbtn_1.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.CHECKED)
setting_page_imgbtn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
setting_page_imgbtn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.CHECKED)
# Set style for setting_page_imgbtn_1, Part: lv.PART.MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
setting_page_imgbtn_1.set_style_image_opa(255, lv.PART.MAIN|lv.imagebutton.STATE.RELEASED)

# Create setting_page_spangroup_1
setting_page_spangroup_1 = lv.spangroup(setting_page_cont_1)
setting_page_spangroup_1.set_align(lv.TEXT_ALIGN.LEFT)
setting_page_spangroup_1.set_overflow(lv.SPAN_OVERFLOW.CLIP)
setting_page_spangroup_1.set_mode(lv.SPAN_MODE.BREAK)
# create spans
setting_page_spangroup_1_span = setting_page_spangroup_1.new_span()
setting_page_spangroup_1_span.set_text("hello")
setting_page_spangroup_1_span.style.set_text_color(lv.color_hex(0x000000))
setting_page_spangroup_1_span.style.set_text_decor(lv.TEXT_DECOR.NONE)
setting_page_spangroup_1_span.style.set_text_font(test_font("montserratMedium", 12))
setting_page_spangroup_1.set_pos(0, 80)
setting_page_spangroup_1.set_size(180, 220)
setting_page_spangroup_1.refr_mode()
# Set style for setting_page_spangroup_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_page_spangroup_1.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_spangroup_1.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_spangroup_1.set_style_border_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_spangroup_1.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_spangroup_1.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_spangroup_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_spangroup_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_spangroup_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_spangroup_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_spangroup_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_spangroup_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create setting_page_cont_3
setting_page_cont_3 = lv.obj(setting_page_cont_1)
setting_page_cont_3.set_pos(190, 0)
setting_page_cont_3.set_size(50, 320)
setting_page_cont_3.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for setting_page_cont_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_page_cont_3.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_3.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_3.set_style_border_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_3.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_3.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_3.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_3.set_style_bg_color(lv.color_hex(0xf5f5f5), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_3.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_3.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_3.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_3.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_3.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create setting_page_RES
setting_page_RES = lv.button(setting_page_cont_3)
setting_page_RES_label = lv.label(setting_page_RES)
setting_page_RES_label.set_text("RES")
setting_page_RES_label.set_long_mode(lv.label.LONG.WRAP)
setting_page_RES_label.set_width(lv.pct(100))
setting_page_RES_label.align(lv.ALIGN.CENTER, 0, 0)
setting_page_RES.set_style_pad_all(0, lv.STATE.DEFAULT)
setting_page_RES.set_pos(0, 0)
setting_page_RES.set_size(50, 50)
# Set style for setting_page_RES, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_page_RES.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_RES.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_RES.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_RES.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_RES.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_RES.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_RES.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_RES.set_style_text_font(test_font("Lemi_Little_Milk_Foam_Font", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_RES.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_RES.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create setting_page_READ
setting_page_READ = lv.button(setting_page_cont_3)
setting_page_READ_label = lv.label(setting_page_READ)
setting_page_READ_label.set_text("READ")
setting_page_READ_label.set_long_mode(lv.label.LONG.WRAP)
setting_page_READ_label.set_width(lv.pct(100))
setting_page_READ_label.align(lv.ALIGN.CENTER, 0, 0)
setting_page_READ.set_style_pad_all(0, lv.STATE.DEFAULT)
setting_page_READ.set_pos(0, 79)
setting_page_READ.set_size(50, 50)
# Set style for setting_page_READ, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_page_READ.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_READ.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_READ.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_READ.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_READ.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_READ.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_READ.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_READ.set_style_text_font(test_font("Lemi_Little_Milk_Foam_Font", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_READ.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_READ.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create setting_page_DELL
setting_page_DELL = lv.button(setting_page_cont_3)
setting_page_DELL_label = lv.label(setting_page_DELL)
setting_page_DELL_label.set_text("DELL")
setting_page_DELL_label.set_long_mode(lv.label.LONG.WRAP)
setting_page_DELL_label.set_width(lv.pct(100))
setting_page_DELL_label.align(lv.ALIGN.CENTER, 0, 0)
setting_page_DELL.set_style_pad_all(0, lv.STATE.DEFAULT)
setting_page_DELL.set_pos(0, 160)
setting_page_DELL.set_size(50, 50)
# Set style for setting_page_DELL, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_page_DELL.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_DELL.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_DELL.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_DELL.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_DELL.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_DELL.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_DELL.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_DELL.set_style_text_font(test_font("Lemi_Little_Milk_Foam_Font", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_DELL.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_DELL.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create setting_page_locker4
setting_page_locker4 = lv.button(setting_page_cont_3)
setting_page_locker4_label = lv.label(setting_page_locker4)
setting_page_locker4_label.set_text("locker4")
setting_page_locker4_label.set_long_mode(lv.label.LONG.WRAP)
setting_page_locker4_label.set_width(lv.pct(100))
setting_page_locker4_label.align(lv.ALIGN.CENTER, 0, 0)
setting_page_locker4.set_style_pad_all(0, lv.STATE.DEFAULT)
setting_page_locker4.set_pos(0, 560)
setting_page_locker4.set_size(50, 50)
# Set style for setting_page_locker4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_page_locker4.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker4.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker4.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker4.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker4.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker4.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker4.set_style_text_font(test_font("Lemi_Little_Milk_Foam_Font", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker4.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker4.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create setting_page_locker3
setting_page_locker3 = lv.button(setting_page_cont_3)
setting_page_locker3_label = lv.label(setting_page_locker3)
setting_page_locker3_label.set_text("locker3")
setting_page_locker3_label.set_long_mode(lv.label.LONG.WRAP)
setting_page_locker3_label.set_width(lv.pct(100))
setting_page_locker3_label.align(lv.ALIGN.CENTER, 0, 0)
setting_page_locker3.set_style_pad_all(0, lv.STATE.DEFAULT)
setting_page_locker3.set_pos(0, 480)
setting_page_locker3.set_size(50, 50)
# Set style for setting_page_locker3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_page_locker3.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker3.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker3.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker3.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker3.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker3.set_style_text_font(test_font("Lemi_Little_Milk_Foam_Font", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker3.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create setting_page_VERI
setting_page_VERI = lv.button(setting_page_cont_3)
setting_page_VERI_label = lv.label(setting_page_VERI)
setting_page_VERI_label.set_text("VERI")
setting_page_VERI_label.set_long_mode(lv.label.LONG.WRAP)
setting_page_VERI_label.set_width(lv.pct(100))
setting_page_VERI_label.align(lv.ALIGN.CENTER, 0, 0)
setting_page_VERI.set_style_pad_all(0, lv.STATE.DEFAULT)
setting_page_VERI.set_pos(0, 240)
setting_page_VERI.set_size(50, 50)
# Set style for setting_page_VERI, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_page_VERI.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_VERI.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_VERI.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_VERI.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_VERI.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_VERI.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_VERI.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_VERI.set_style_text_font(test_font("Lemi_Little_Milk_Foam_Font", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_VERI.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_VERI.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create setting_page_locker1
setting_page_locker1 = lv.button(setting_page_cont_3)
setting_page_locker1_label = lv.label(setting_page_locker1)
setting_page_locker1_label.set_text("locker1")
setting_page_locker1_label.set_long_mode(lv.label.LONG.WRAP)
setting_page_locker1_label.set_width(lv.pct(100))
setting_page_locker1_label.align(lv.ALIGN.CENTER, 0, 0)
setting_page_locker1.set_style_pad_all(0, lv.STATE.DEFAULT)
setting_page_locker1.set_pos(-1, 320)
setting_page_locker1.set_size(50, 50)
# Set style for setting_page_locker1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_page_locker1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker1.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker1.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker1.set_style_text_font(test_font("Lemi_Little_Milk_Foam_Font", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create setting_page_locker2
setting_page_locker2 = lv.button(setting_page_cont_3)
setting_page_locker2_label = lv.label(setting_page_locker2)
setting_page_locker2_label.set_text("locker2")
setting_page_locker2_label.set_long_mode(lv.label.LONG.WRAP)
setting_page_locker2_label.set_width(lv.pct(100))
setting_page_locker2_label.align(lv.ALIGN.CENTER, 0, 0)
setting_page_locker2.set_style_pad_all(0, lv.STATE.DEFAULT)
setting_page_locker2.set_pos(0, 400)
setting_page_locker2.set_size(50, 50)
# Set style for setting_page_locker2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_page_locker2.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker2.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker2.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker2.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker2.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker2.set_style_text_font(test_font("Lemi_Little_Milk_Foam_Font", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_locker2.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create setting_page_ALL
setting_page_ALL = lv.button(setting_page_cont_3)
setting_page_ALL_label = lv.label(setting_page_ALL)
setting_page_ALL_label.set_text("all_out")
setting_page_ALL_label.set_long_mode(lv.label.LONG.WRAP)
setting_page_ALL_label.set_width(lv.pct(100))
setting_page_ALL_label.align(lv.ALIGN.CENTER, 0, 0)
setting_page_ALL.set_style_pad_all(0, lv.STATE.DEFAULT)
setting_page_ALL.set_pos(0, 640)
setting_page_ALL.set_size(50, 50)
# Set style for setting_page_ALL, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_page_ALL.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_ALL.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_ALL.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_ALL.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_ALL.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_ALL.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_ALL.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_ALL.set_style_text_font(test_font("Lemi_Little_Milk_Foam_Font", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_ALL.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_ALL.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create setting_page_NOTE
setting_page_NOTE = lv.label(setting_page_cont_1)
setting_page_NOTE.set_text("")
setting_page_NOTE.set_long_mode(lv.label.LONG.WRAP)
setting_page_NOTE.set_width(lv.pct(100))
setting_page_NOTE.set_pos(62, 14)
setting_page_NOTE.set_size(120, 34)
# Set style for setting_page_NOTE, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_page_NOTE.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_NOTE.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_NOTE.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_NOTE.set_style_text_font(test_font("Lemi_Little_Milk_Foam_Font", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_NOTE.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_NOTE.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_NOTE.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_NOTE.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_NOTE.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_NOTE.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_NOTE.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_NOTE.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_NOTE.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_NOTE.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create setting_page_cont_2
setting_page_cont_2 = lv.obj(setting_page)
setting_page_cont_2.set_pos(0, 0)
setting_page_cont_2.set_size(240, 320)
setting_page_cont_2.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for setting_page_cont_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_page_cont_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_2.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_2.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_2.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_cont_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create setting_page_btnm_1
setting_page_btnm_1 = lv.buttonmatrix(setting_page_cont_2)
setting_page_btnm_1_text_map = ["1", "2", "3", "\n", "4", "5", "6", "\n", "7", "8", "9", "\n", "X", "0", "V", "",]
setting_page_btnm_1.set_map(setting_page_btnm_1_text_map)
setting_page_btnm_1.set_pos(10, 139)
setting_page_btnm_1.set_size(220, 170)
# Set style for setting_page_btnm_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_page_btnm_1.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_btnm_1.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_btnm_1.set_style_border_color(lv.color_hex(0xc9c9c9), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_btnm_1.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_btnm_1.set_style_pad_top(16, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_btnm_1.set_style_pad_bottom(16, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_btnm_1.set_style_pad_left(16, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_btnm_1.set_style_pad_right(16, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_btnm_1.set_style_pad_row(8, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_btnm_1.set_style_pad_column(8, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_btnm_1.set_style_radius(4, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_btnm_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_btnm_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_btnm_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for setting_page_btnm_1, Part: lv.PART.ITEMS, State: lv.STATE.DEFAULT.
setting_page_btnm_1.set_style_border_width(1, lv.PART.ITEMS|lv.STATE.DEFAULT)
setting_page_btnm_1.set_style_border_opa(255, lv.PART.ITEMS|lv.STATE.DEFAULT)
setting_page_btnm_1.set_style_border_color(lv.color_hex(0xc9c9c9), lv.PART.ITEMS|lv.STATE.DEFAULT)
setting_page_btnm_1.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.ITEMS|lv.STATE.DEFAULT)
setting_page_btnm_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.ITEMS|lv.STATE.DEFAULT)
setting_page_btnm_1.set_style_text_font(test_font("montserratMedium", 16), lv.PART.ITEMS|lv.STATE.DEFAULT)
setting_page_btnm_1.set_style_text_opa(255, lv.PART.ITEMS|lv.STATE.DEFAULT)
setting_page_btnm_1.set_style_radius(4, lv.PART.ITEMS|lv.STATE.DEFAULT)
setting_page_btnm_1.set_style_bg_opa(255, lv.PART.ITEMS|lv.STATE.DEFAULT)
setting_page_btnm_1.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.ITEMS|lv.STATE.DEFAULT)
setting_page_btnm_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.ITEMS|lv.STATE.DEFAULT)
setting_page_btnm_1.set_style_shadow_width(0, lv.PART.ITEMS|lv.STATE.DEFAULT)

# Create setting_page_label_4
setting_page_label_4 = lv.label(setting_page_cont_2)
setting_page_label_4.set_text("4")
setting_page_label_4.set_long_mode(lv.label.LONG.WRAP)
setting_page_label_4.set_width(lv.pct(100))
setting_page_label_4.set_pos(183, 100)
setting_page_label_4.set_size(30, 20)
# Set style for setting_page_label_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_page_label_4.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_4.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_4.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_4.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_4.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_4.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_4.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_4.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_4.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_4.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_4.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_4.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_4.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create setting_page_label_3
setting_page_label_3 = lv.label(setting_page_cont_2)
setting_page_label_3.set_text("3")
setting_page_label_3.set_long_mode(lv.label.LONG.WRAP)
setting_page_label_3.set_width(lv.pct(100))
setting_page_label_3.set_pos(130, 100)
setting_page_label_3.set_size(30, 20)
# Set style for setting_page_label_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_page_label_3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_3.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_3.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_3.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_3.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_3.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_3.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_3.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_3.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_3.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_3.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_3.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create setting_page_label_1
setting_page_label_1 = lv.label(setting_page_cont_2)
setting_page_label_1.set_text("1")
setting_page_label_1.set_long_mode(lv.label.LONG.WRAP)
setting_page_label_1.set_width(lv.pct(100))
setting_page_label_1.set_pos(21, 100)
setting_page_label_1.set_size(30, 20)
# Set style for setting_page_label_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_page_label_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_1.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_1.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_1.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create setting_page_imgbtn_2
setting_page_imgbtn_2 = lv.imagebutton(setting_page_cont_2)
setting_page_imgbtn_2.add_flag(lv.obj.FLAG.CHECKABLE)
setting_page_imgbtn_2.set_src(lv.imagebutton.STATE.RELEASED, load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\back_50_50.png"), None, None)
setting_page_imgbtn_2.add_flag(lv.obj.FLAG.CHECKABLE)
setting_page_imgbtn_2_label = lv.label(setting_page_imgbtn_2)
setting_page_imgbtn_2_label.set_text("")
setting_page_imgbtn_2_label.set_long_mode(lv.label.LONG.WRAP)
setting_page_imgbtn_2_label.set_width(lv.pct(100))
setting_page_imgbtn_2_label.align(lv.ALIGN.CENTER, 0, 0)
setting_page_imgbtn_2.set_style_pad_all(0, lv.STATE.DEFAULT)
setting_page_imgbtn_2.set_pos(10, 10)
setting_page_imgbtn_2.set_size(50, 50)
# Set style for setting_page_imgbtn_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_page_imgbtn_2.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_imgbtn_2.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_imgbtn_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_imgbtn_2.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_imgbtn_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for setting_page_imgbtn_2, Part: lv.PART.MAIN, State: lv.STATE.PRESSED.
setting_page_imgbtn_2.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
setting_page_imgbtn_2.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.PRESSED)
setting_page_imgbtn_2.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.PRESSED)
setting_page_imgbtn_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
setting_page_imgbtn_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
# Set style for setting_page_imgbtn_2, Part: lv.PART.MAIN, State: lv.STATE.CHECKED.
setting_page_imgbtn_2.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
setting_page_imgbtn_2.set_style_text_color(lv.color_hex(0xFF33FF), lv.PART.MAIN|lv.STATE.CHECKED)
setting_page_imgbtn_2.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.CHECKED)
setting_page_imgbtn_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
setting_page_imgbtn_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.CHECKED)
# Set style for setting_page_imgbtn_2, Part: lv.PART.MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
setting_page_imgbtn_2.set_style_image_opa(255, lv.PART.MAIN|lv.imagebutton.STATE.RELEASED)

# Create setting_page_label_2
setting_page_label_2 = lv.label(setting_page_cont_2)
setting_page_label_2.set_text("2")
setting_page_label_2.set_long_mode(lv.label.LONG.WRAP)
setting_page_label_2.set_width(lv.pct(100))
setting_page_label_2.set_pos(73, 100)
setting_page_label_2.set_size(30, 20)
# Set style for setting_page_label_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_page_label_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_2.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_2.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_2.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_2.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_2.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create setting_page_img_1
setting_page_img_1 = lv.image(setting_page_cont_2)
setting_page_img_1.set_src(load_image(r"E:\GUIder_Project\smart_locker_v2\smart_locker_v2\generated\MicroPython\2_80_80.png"))
setting_page_img_1.add_flag(lv.obj.FLAG.CLICKABLE)
setting_page_img_1.set_pivot(50,50)
setting_page_img_1.set_rotation(0)
setting_page_img_1.set_pos(160, 0)
setting_page_img_1.set_size(80, 80)
# Set style for setting_page_img_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_page_img_1.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create setting_page_label_5
setting_page_label_5 = lv.label(setting_page_cont_2)
setting_page_label_5.set_text("密码对了才能进去哦~omage~\n必须输入4位错误请重试！")
setting_page_label_5.set_long_mode(lv.label.LONG.WRAP)
setting_page_label_5.set_width(lv.pct(100))
setting_page_label_5.set_pos(81, 32)
setting_page_label_5.set_size(76, 48)
# Set style for setting_page_label_5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
setting_page_label_5.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_5.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_5.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_5.set_style_text_font(test_font("Lemi_Little_Milk_Foam_Font", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_5.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_5.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_5.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_5.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_5.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_5.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_5.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_5.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_5.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
setting_page_label_5.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

setting_page.update_layout()

def main_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOADED):
        pass
        

main.add_event_cb(lambda e: main_event_handler(e), lv.EVENT.ALL, None)

def main_btn_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(save_page, lv.SCR_LOAD_ANIM.NONE, 100, 100, False)
main_btn_1.add_event_cb(lambda e: main_btn_1_event_handler(e), lv.EVENT.ALL, None)

def main_btn_2_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(take_page, lv.SCR_LOAD_ANIM.NONE, 100, 100, False)
main_btn_2.add_event_cb(lambda e: main_btn_2_event_handler(e), lv.EVENT.ALL, None)

def main_btn_3_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(help_page, lv.SCR_LOAD_ANIM.NONE, 50, 50, False)
    if (code == lv.EVENT.LONG_PRESSED):
        pass
        lv.screen_load_anim(setting_page, lv.SCR_LOAD_ANIM.NONE, 50, 50, False)
main_btn_3.add_event_cb(lambda e: main_btn_3_event_handler(e), lv.EVENT.ALL, None)

def take_page_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(main, lv.SCR_LOAD_ANIM.NONE, 100, 100, False)
take_page.add_event_cb(lambda e: take_page_event_handler(e), lv.EVENT.ALL, None)

def take_page_imgbtn_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(main, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
take_page_imgbtn_1.add_event_cb(lambda e: take_page_imgbtn_1_event_handler(e), lv.EVENT.ALL, None)

def take_page_btn_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        take_page_cont_2.remove_flag(lv.obj.FLAG.HIDDEN)
        
        take_page_cont_1.add_flag(lv.obj.FLAG.HIDDEN)
        
take_page_btn_1.add_event_cb(lambda e: take_page_btn_1_event_handler(e), lv.EVENT.ALL, None)

def take_page_imgbtn_2_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        take_page_cont_2.add_flag(lv.obj.FLAG.HIDDEN)
        
        take_page_cont_1.remove_flag(lv.obj.FLAG.HIDDEN)
        
take_page_imgbtn_2.add_event_cb(lambda e: take_page_imgbtn_2_event_handler(e), lv.EVENT.ALL, None)

def save_page_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(main, lv.SCR_LOAD_ANIM.NONE, 100, 100, False)
save_page.add_event_cb(lambda e: save_page_event_handler(e), lv.EVENT.ALL, None)

def save_page_imgbtn_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(main, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
save_page_imgbtn_1.add_event_cb(lambda e: save_page_imgbtn_1_event_handler(e), lv.EVENT.ALL, None)

def help_page_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(main, lv.SCR_LOAD_ANIM.NONE, 100, 100, False)
help_page.add_event_cb(lambda e: help_page_event_handler(e), lv.EVENT.ALL, None)

def help_page_imgbtn_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(main, lv.SCR_LOAD_ANIM.NONE, 100, 100, False)
help_page_imgbtn_1.add_event_cb(lambda e: help_page_imgbtn_1_event_handler(e), lv.EVENT.ALL, None)

def setting_page_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOADED):
        pass
        

setting_page.add_event_cb(lambda e: setting_page_event_handler(e), lv.EVENT.ALL, None)

def setting_page_imgbtn_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(main, lv.SCR_LOAD_ANIM.NONE, 100, 100, False)
setting_page_imgbtn_1.add_event_cb(lambda e: setting_page_imgbtn_1_event_handler(e), lv.EVENT.ALL, None)

def setting_page_RES_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        setting_page_NOTE.set_text("模组复位")
setting_page_RES.add_event_cb(lambda e: setting_page_RES_event_handler(e), lv.EVENT.ALL, None)

def setting_page_READ_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        setting_page_NOTE.set_text("模组读取用户")
setting_page_READ.add_event_cb(lambda e: setting_page_READ_event_handler(e), lv.EVENT.ALL, None)

def setting_page_DELL_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        setting_page_NOTE.set_text("模组删除所有用户")
setting_page_DELL.add_event_cb(lambda e: setting_page_DELL_event_handler(e), lv.EVENT.ALL, None)

def setting_page_locker4_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        setting_page_NOTE.set_text("4号柜打开")
setting_page_locker4.add_event_cb(lambda e: setting_page_locker4_event_handler(e), lv.EVENT.ALL, None)

def setting_page_locker3_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        setting_page_NOTE.set_text("3号柜打开")
setting_page_locker3.add_event_cb(lambda e: setting_page_locker3_event_handler(e), lv.EVENT.ALL, None)

def setting_page_VERI_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        setting_page_NOTE.set_text("模组开始识别")
setting_page_VERI.add_event_cb(lambda e: setting_page_VERI_event_handler(e), lv.EVENT.ALL, None)

def setting_page_locker1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        setting_page_NOTE.set_text("1号柜打开")
setting_page_locker1.add_event_cb(lambda e: setting_page_locker1_event_handler(e), lv.EVENT.ALL, None)

def setting_page_locker2_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        setting_page_NOTE.set_text("2号柜打开")
setting_page_locker2.add_event_cb(lambda e: setting_page_locker2_event_handler(e), lv.EVENT.ALL, None)

def setting_page_ALL_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        setting_page_NOTE.set_text("所有柜打开")
setting_page_ALL.add_event_cb(lambda e: setting_page_ALL_event_handler(e), lv.EVENT.ALL, None)

def setting_page_btnm_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.VALUE_CHANGED):
        pass
        

setting_page_btnm_1.add_event_cb(lambda e: setting_page_btnm_1_event_handler(e), lv.EVENT.ALL, None)

def setting_page_imgbtn_2_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.screen_load_anim(main, lv.SCR_LOAD_ANIM.NONE, 100, 100, False)
setting_page_imgbtn_2.add_event_cb(lambda e: setting_page_imgbtn_2_event_handler(e), lv.EVENT.ALL, None)

# content from custom.py

# Load the default screen
lv.screen_load(main)

if __name__ == '__main__':
    while True:
        lv.task_handler()
        time.sleep_ms(5)
