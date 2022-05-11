/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    LVGL DSP C6748 �ײ�ӿ���ֲ                                           */
/*                                                                          */
/*    2022��04��06��                                                        */
/*                                                                          */
/****************************************************************************/
/*
 *    - ϣ����Ĭ(bin wang)
 *    - bin@corekernel.net
 *
 *    ���� corekernel.net/.org/.cn
 *    ���� fpga.net.cn
 *
 */
#if 1

#include "lv_port_disp_c674x.h"

#include "LCD.h"

#include <xdc/std.h>

#include <ti/sysbios/family/c64p/cache.h>

#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

/****************************************************************************/
/*                                                                          */
/*              ��������                                                    */
/*                                                                          */
/****************************************************************************/
static void disp_init(void);
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);

extern Void LVGLTask(UArg a0, UArg a1);
extern Void LVGLAppTask(UArg a0, UArg a1);

/****************************************************************************/
/*                                                                          */
/*              �߳�                                                        */
/*                                                                          */
/****************************************************************************/
Void LVGLTick(UArg arg)
{
    // 1ms ����
    lv_tick_inc(1);
}

/****************************************************************************/
/*                                                                          */
/*              LVGL ��ʾ��ʼ��                                             */
/*                                                                          */
/****************************************************************************/
void lv_port_disp_init(void)
{
    /*-------------------------
     * LCD ��������ʼ��
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * ������ʾ������
     *----------------------------*/

    /**
     * LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed to your display driver's `flush_cb` to copy its content to your display.
     * The buffer has to be greater than 1 display row
     *
     * There are 3 buffering configurations:
     * 1. Create ONE buffer:
     *      LVGL will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer:
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Double buffering
     *      Set 2 screens sized buffers and set disp_drv.full_refresh = 1.
     *      This way LVGL will always provide the whole rendered screen in `flush_cb`
     *      and you only need to change the frame buffer's address.
     */

    /* ˫��ʾ������ */
    static lv_disp_draw_buf_t draw_buf_dsc;
    static lv_color_t buf_1[LCD_WIDTH * LCD_HEIGHT];  /* 10 �� */
    static lv_color_t buf_2[LCD_WIDTH * LCD_HEIGHT];  /* 10 �� */
    lv_disp_draw_buf_init(&draw_buf_dsc, buf_1, buf_2, LCD_WIDTH * LCD_HEIGHT);

    /*-----------------------------------
     * ע�� LVGL ��ʾ�豸
     *----------------------------------*/
    static lv_disp_drv_t disp_drv;            /* ��ʾ���������� */
    lv_disp_drv_init(&disp_drv);

    /* ������Ļ�ֱ��� */
    disp_drv.hor_res = LCD_WIDTH;
    disp_drv.ver_res = LCD_HEIGHT;

    /* �ص����� */
    disp_drv.flush_cb = disp_flush;

    /* ������ʾ������ */
    disp_drv.draw_buf = &draw_buf_dsc;

    /* ǿ��ȫ��ˢ�� */
    //disp_drv.full_refresh = 1;

    /* ��ת */
//  disp_drv.sw_rotate = 1;
//  disp_drv.rotated = LV_DISP_ROT_90;

    /* ע������ */
    lv_disp_drv_register(&disp_drv);
}

/****************************************************************************/
/*                                                                          */
/*              ��ʾ��������ʼ��                                            */
/*                                                                          */
/****************************************************************************/
static void disp_init(void)
{
    /* TODO ���ⲿ��ʼ�� LCD ������ */

    /* ʱ�ӳ�ʼ�� */
    Clock_Params ClkParams;
    Clock_Params_init(&ClkParams);

    ClkParams.period = 1;
    ClkParams.startFlag = TRUE;
    Clock_create(LVGLTick, 1000, &ClkParams, NULL);

    /* �����ʼ�� */
    Task_Params taskParams;
    Task_Params_init(&taskParams);

    taskParams.priority = 15;
    taskParams.stackSize = 128 * 1024;
    Task_create(LVGLTask, &taskParams, NULL);

    taskParams.priority = 1;
    taskParams.stackSize = 128 * 1024;
    Task_create(LVGLAppTask, &taskParams, NULL);
}

/****************************************************************************/
/*                                                                          */
/*              �ص�����                                                    */
/*                                                                          */
/****************************************************************************/
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    int32_t len = (area->y2 - area->y1 + 1) * (area->x2 - area->x1 + 1) * sizeof(unsigned short);

    /* ά������һ���� */
//    Cache_inv(color_p, len, Cache_Type_ALLD, true);
    Cache_wbInvAll();

    unsigned short *LCDBuf = (unsigned short *)(g_pucBuffer + (PALETTE_OFFSET + PALETTE_SIZE));

    int32_t y;
    for(y = area->y1; y <= area->y2; y++)
    {
        memcpy((void *)&LCDBuf[y * LCD_WIDTH + area->x1], color_p, (area->x2 - area->x1 + 1) * sizeof(unsigned short));
        color_p += area->x2 - area->x1 + 1;
    }

    /* ά������һ���� */
    Cache_wbInvAll();

    /* �޸������λ */
    RasterDisable(SOC_LCDC_0_REGS);
    RasterEnable(SOC_LCDC_0_REGS);

//    Cache_wb(&LCDBuf, (LCD_WIDTH * LCD_HEIGHT) * sizeof(unsigned short), Cache_Type_ALLD, true);

    /* ��Ҫ!!!
     * ֪ͨͼ�ο���ʾ���¾��� */
    lv_disp_flush_ready(disp_drv);
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
