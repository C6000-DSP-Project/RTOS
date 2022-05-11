// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//
//    新核科技(广州)有限公司
//
//    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd
//
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//
//    LVGL 输入设备 DSP C6748 底层接口移植
//
//    2022年04月07日
//
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
/*
 *    - 希望缄默(bin wang)
 *    - bin@corekernel.net
 *
 *    官网 corekernel.net/.org/.cn
 *    社区 fpga.net.cn
 *
 */
#if 1

#include "lv_port_indev_c674x.h"
#include "../lvgl.h"

#include "Touch.h"

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//
//     全局变量
//
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
lv_indev_t * indev_touchpad;
//lv_indev_t * indev_mouse;
//lv_indev_t * indev_keypad;
//lv_indev_t * indev_encoder;
//lv_indev_t * indev_button;

//static int32_t encoder_diff;
//static lv_indev_state_t encoder_state;

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//
//     函数声明
//
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
static void touchpad_init(void);
static void touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
static bool touchpad_is_pressed(void);
static void touchpad_get_xy(lv_coord_t *x, lv_coord_t *y);

/*
static void mouse_init(void);
static void mouse_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
static bool mouse_is_pressed(void);
static void mouse_get_xy(lv_coord_t *x, lv_coord_t *y);

static void keypad_init(void);
static void keypad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
static uint32_t keypad_get_key(void);

static void encoder_init(void);
static void encoder_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
static void encoder_handler(void);

static void button_init(void);
static void button_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
static int8_t button_get_pressed_id(void);
static bool button_is_pressed(uint8_t id);
*/

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//
//     LVGL 输入设备初始化
//
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void lv_port_indev_init(void)
{
    /**
     * Here you will find example implementation of input devices supported by LittelvGL:
     *  - Touchpad
     *  - Mouse (with cursor support)
     *  - Keypad (supports GUI usage only with key)
     *  - Encoder (supports GUI usage only with: left, right, push)
     *  - Button (external buttons to press points on the screen)
     *
     *  The `..._read()` function are only examples.
     *  You should shape them according to your hardware
     */

    static lv_indev_drv_t indev_drv;

    /*------------------
     * 触控设备
     * -----------------*/

    /* 触控设备初始化 */
    touchpad_init();

    /* 注册触控输入设备 */
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;
    indev_touchpad = lv_indev_drv_register(&indev_drv);

//    /*------------------
//     * 鼠标
//     * -----------------*/
//
//    /* 鼠标初始化 */
//    mouse_init();
//
//    /* 注册鼠标输入设备 */
//    lv_indev_drv_init(&indev_drv);
//    indev_drv.type = LV_INDEV_TYPE_POINTER;
//    indev_drv.read_cb = mouse_read;
//    indev_mouse = lv_indev_drv_register(&indev_drv);
//
//    /* 设置指针 */
//    lv_obj_t * mouse_cursor = lv_img_create(lv_scr_act());
//    lv_img_set_src(mouse_cursor, LV_SYMBOL_HOME);
//    lv_indev_set_cursor(indev_mouse, mouse_cursor);

//    /*------------------
//     * 键盘
//     * -----------------*/
//
//    /* 键盘初始化 */
//    keypad_init();
//
//    /* 注册键盘输入设备 */
//    lv_indev_drv_init(&indev_drv);
//    indev_drv.type = LV_INDEV_TYPE_KEYPAD;
//    indev_drv.read_cb = keypad_read;
//    indev_keypad = lv_indev_drv_register(&indev_drv);
//
//    /*Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
//     *add objects to the group with `lv_group_add_obj(group, obj)`
//     *and assign this input device to group to navigate in it:
//     *`lv_indev_set_group(indev_keypad, group);`*/

//    /*------------------
//     * 编码器
//     * -----------------*/
//
//    /* 编码器初始化 */
//    encoder_init();
//
//    /* 注册编码器输入设备 */
//    lv_indev_drv_init(&indev_drv);
//    indev_drv.type = LV_INDEV_TYPE_ENCODER;
//    indev_drv.read_cb = encoder_read;
//    indev_encoder = lv_indev_drv_register(&indev_drv);
//
//    /*Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
//     *add objects to the group with `lv_group_add_obj(group, obj)`
//     *and assign this input device to group to navigate in it:
//     *`lv_indev_set_group(indev_encoder, group);`*/

//    /*------------------
//     * 按键
//     * -----------------*/
//
//    /* 按键初始化 */
//    button_init();
//
//    /* 注册按键输入设备 */
//    lv_indev_drv_init(&indev_drv);
//    indev_drv.type = LV_INDEV_TYPE_BUTTON;
//    indev_drv.read_cb = button_read;
//    indev_button = lv_indev_drv_register(&indev_drv);
//
//    /*Assign buttons to points on the screen*/
//    static const lv_point_t btn_points[2] = {
//        {10, 10},   /*Button 0 -> x:10; y:10*/
//        {40, 100},  /*Button 1 -> x:40; y:100*/
//    };
//    lv_indev_set_button_points(indev_button, btn_points);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//
//     触控设备
//
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
static void touchpad_init(void)
{
    /* TODO 在外部初始化触摸屏 */
}

static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static lv_coord_t last_x = 0;
    static lv_coord_t last_y = 0;

    /* 保存按压坐标和状态 */
    if(touchpad_is_pressed())
    {
        touchpad_get_xy(&last_x, &last_y);
        data->state = LV_INDEV_STATE_PR;
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }

    /* 设置上一次按压坐标 */
    data->point.x = last_x;
    data->point.y = last_y;
}

/* 返回  true 表示被按下 */
static bool touchpad_is_pressed(void)
{
    return TouchInfo.Flag;
}

/* 获取 x, y 坐标*/
static void touchpad_get_xy(lv_coord_t * x, lv_coord_t * y)
{
    (*x) = TouchInfo.X[0];
    (*y) = TouchInfo.Y[0];
}

//// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
////
////     鼠标
////
//// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//static void mouse_init(void)
//{
//    /*Your code comes here*/
//}
//
///*Will be called by the library to read the mouse*/
//static void mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
//{
//    /*Get the current x and y coordinates*/
//    mouse_get_xy(&data->point.x, &data->point.y);
//
//    /*Get whether the mouse button is pressed or released*/
//    if(mouse_is_pressed()) {
//        data->state = LV_INDEV_STATE_PR;
//    }
//    else {
//        data->state = LV_INDEV_STATE_REL;
//    }
//}
//
///*Return true is the mouse button is pressed*/
//static bool mouse_is_pressed(void)
//{
//    /*Your code comes here*/
//
//    return false;
//}
//
///*Get the x and y coordinates if the mouse is pressed*/
//static void mouse_get_xy(lv_coord_t * x, lv_coord_t * y)
//{
//    /*Your code comes here*/
//
//    (*x) = 0;
//    (*y) = 0;
//}
//
//// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
////
////     键盘
////
//// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//static void keypad_init(void)
//{
//    /*Your code comes here*/
//}
//
///*Will be called by the library to read the mouse*/
//static void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
//{
//    static uint32_t last_key = 0;
//
//    /*Get the current x and y coordinates*/
//    mouse_get_xy(&data->point.x, &data->point.y);
//
//    /*Get whether the a key is pressed and save the pressed key*/
//    uint32_t act_key = keypad_get_key();
//    if(act_key != 0) {
//        data->state = LV_INDEV_STATE_PR;
//
//        /*Translate the keys to LVGL control characters according to your key definitions*/
//        switch(act_key) {
//            case 1:
//                act_key = LV_KEY_NEXT;
//                break;
//            case 2:
//                act_key = LV_KEY_PREV;
//                break;
//            case 3:
//                act_key = LV_KEY_LEFT;
//                break;
//            case 4:
//                act_key = LV_KEY_RIGHT;
//                break;
//            case 5:
//                act_key = LV_KEY_ENTER;
//                break;
//        }
//
//        last_key = act_key;
//    }
//    else {
//        data->state = LV_INDEV_STATE_REL;
//    }
//
//    data->key = last_key;
//}
//
///*Get the currently being pressed key.  0 if no key is pressed*/
//static uint32_t keypad_get_key(void)
//{
//    /*Your code comes here*/
//
//    return 0;
//}
//
//// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
////
////     编码器
////
//// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//static void encoder_init(void)
//{
//    /*Your code comes here*/
//}
//
///*Will be called by the library to read the encoder*/
//static void encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
//{
//
//    data->enc_diff = encoder_diff;
//    data->state = encoder_state;
//}
//
///*Call this function in an interrupt to process encoder events (turn, press)*/
//static void encoder_handler(void)
//{
//    /*Your code comes here*/
//
//    encoder_diff += 0;
//    encoder_state = LV_INDEV_STATE_REL;
//}
//
//// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
////
////     按键
////
//// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//static void button_init(void)
//{
//    /*Your code comes here*/
//}
//
///*Will be called by the library to read the button*/
//static void button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
//{
//
//    static uint8_t last_btn = 0;
//
//    /*Get the pressed button's ID*/
//    int8_t btn_act = button_get_pressed_id();
//
//    if(btn_act >= 0) {
//        data->state = LV_INDEV_STATE_PR;
//        last_btn = btn_act;
//    }
//    else {
//        data->state = LV_INDEV_STATE_REL;
//    }
//
//    /*Save the last pressed button's ID*/
//    data->btn_id = last_btn;
//}
//
///*Get ID  (0, 1, 2 ..) of the pressed button*/
//static int8_t button_get_pressed_id(void)
//{
//    uint8_t i;
//
//    /*Check to buttons see which is being pressed (assume there are 2 buttons)*/
//    for(i = 0; i < 2; i++) {
//        /*Return the pressed button's ID*/
//        if(button_is_pressed(i)) {
//            return i;
//        }
//    }
//
//    /*No button pressed*/
//    return -1;
//}
//
///*Test if `id` button is pressed or not*/
//static bool button_is_pressed(uint8_t id)
//{
//
//    /*Your code comes here*/
//
//    return false;
//}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
