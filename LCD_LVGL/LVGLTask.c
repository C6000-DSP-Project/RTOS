/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology Guangzhou Co., Ltd           */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    LVGL 显示任务                                                         */
/*                                                                          */
/*    2022年05月02日                                                        */
/*                                                                          */
/****************************************************************************/
/*
 *    - 希望缄默(bin wang)
 *    - bin@corekernel.net
 *
 *    官网 corekernel.net/.org/.cn
 *    社区 fpga.net.cn
 *
 */
#include <xdc/std.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

#include "LVGL/lvgl.h"

/****************************************************************************/
/*                                                                          */
/*              全局变量                                                    */
/*                                                                          */
/****************************************************************************/
static lv_obj_t *label;

static uint8_t cnt = 0;

/****************************************************************************/
/*                                                                          */
/*              任务线程                                                    */
/*                                                                          */
/****************************************************************************/
Void LVGLTask(UArg a0, UArg a1)
{
    for(;;)
    {
        lv_timer_handler();
        Task_sleep(5);
    }
}

static void btn_event_cb(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);

    if(event == LV_EVENT_CLICKED)
    {
        cnt++;

        lv_label_set_text_fmt(label, "Button: %d", cnt);
    }
}

Void LVGLAppTask(UArg a0, UArg a1)
{
    lv_obj_t *btn = lv_btn_create(lv_scr_act());
    lv_obj_set_pos(btn, 10, 10);
    lv_obj_set_size(btn, 120, 50);
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);

    label = lv_label_create(btn);
    lv_label_set_text(label, "Button");
    lv_obj_center(label);

    for(;;)
    {
        Task_sleep(1000);
    }
}
