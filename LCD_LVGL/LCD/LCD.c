/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    LCD                                                                   */
/*                                                                          */
/*    2014��10��14��                                                        */
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
#include <xdc/std.h>

#include <ti/sysbios/hal/Hwi.h>

#include "hw_types.h"
#include "hw_syscfg0_C6748.h"
#include "hw_psc_C6748.h"

#include "soc_C6748.h"
#include "psc.h"
#include "gpio.h"
#include "raster.h"

#include "interrupt.h"

#include "LCD.h"

/****************************************************************************/
/*                                                                          */
/*              ȫ�ֱ���                                                    */
/*                                                                          */
/****************************************************************************/
#pragma DATA_ALIGN(g_pucBuffer, 4);
unsigned char g_pucBuffer[4 + (16 * 2) + (LCD_WIDTH * LCD_HEIGHT * 2)];

// ��ɫ��
unsigned short palette_32b[PALETTE_SIZE / 2] =
{
    0x4000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u,
    0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u
};

/****************************************************************************/
/*                                                                          */
/*              GPIO �ܽŸ�������                                           */
/*                                                                          */
/****************************************************************************/
static void GPIOBankPinMuxSet()
{
    // LCD �ź�
    #define PINMUX16_LCD_ENABLE     (SYSCFG_PINMUX16_PINMUX16_31_28_LCD_D2 << SYSCFG_PINMUX16_PINMUX16_31_28_SHIFT) | \
                                     (SYSCFG_PINMUX16_PINMUX16_27_24_LCD_D3 << SYSCFG_PINMUX16_PINMUX16_27_24_SHIFT) | \
                                     (SYSCFG_PINMUX16_PINMUX16_23_20_LCD_D4 << SYSCFG_PINMUX16_PINMUX16_23_20_SHIFT) | \
                                     (SYSCFG_PINMUX16_PINMUX16_19_16_LCD_D5 << SYSCFG_PINMUX16_PINMUX16_19_16_SHIFT) | \
                                     (SYSCFG_PINMUX16_PINMUX16_15_12_LCD_D6 << SYSCFG_PINMUX16_PINMUX16_15_12_SHIFT) | \
                                     (SYSCFG_PINMUX16_PINMUX16_11_8_LCD_D7 << SYSCFG_PINMUX16_PINMUX16_11_8_SHIFT)

    #define PINMUX17_LCD_ENABLE     (SYSCFG_PINMUX17_PINMUX17_31_28_LCD_D10 << SYSCFG_PINMUX17_PINMUX17_31_28_SHIFT) | \
                                     (SYSCFG_PINMUX17_PINMUX17_27_24_LCD_D11 << SYSCFG_PINMUX17_PINMUX17_27_24_SHIFT) | \
                                     (SYSCFG_PINMUX17_PINMUX17_23_20_LCD_D12 << SYSCFG_PINMUX17_PINMUX17_23_20_SHIFT) | \
                                     (SYSCFG_PINMUX17_PINMUX17_19_16_LCD_D13 << SYSCFG_PINMUX17_PINMUX17_19_16_SHIFT) | \
                                     (SYSCFG_PINMUX17_PINMUX17_15_12_LCD_D14 << SYSCFG_PINMUX17_PINMUX17_15_12_SHIFT) | \
                                     (SYSCFG_PINMUX17_PINMUX17_11_8_LCD_D15 << SYSCFG_PINMUX17_PINMUX17_11_8_SHIFT) | \
                                     (SYSCFG_PINMUX17_PINMUX17_7_4_LCD_D0 << SYSCFG_PINMUX17_PINMUX17_7_4_SHIFT) | \
                                     (SYSCFG_PINMUX17_PINMUX17_3_0_LCD_D1 << SYSCFG_PINMUX17_PINMUX17_3_0_SHIFT)

    #define PINMUX18_LCD_ENABLE     (SYSCFG_PINMUX18_PINMUX18_31_28_LCD_MCLK << SYSCFG_PINMUX18_PINMUX18_31_28_SHIFT) | \
                                     (SYSCFG_PINMUX18_PINMUX18_27_24_LCD_PCLK << SYSCFG_PINMUX18_PINMUX18_27_24_SHIFT) | \
                                     (SYSCFG_PINMUX18_PINMUX18_7_4_LCD_D8 << SYSCFG_PINMUX18_PINMUX18_7_4_SHIFT) | \
                                     (SYSCFG_PINMUX18_PINMUX18_3_0_LCD_D9 << SYSCFG_PINMUX18_PINMUX18_3_0_SHIFT)

    #define PINMUX19_LCD_ENABLE     (SYSCFG_PINMUX19_PINMUX19_27_24_NLCD_AC_ENB_CS << SYSCFG_PINMUX19_PINMUX19_27_24_SHIFT) | \
                                     (SYSCFG_PINMUX19_PINMUX19_7_4_LCD_VSYNC << SYSCFG_PINMUX19_PINMUX19_7_4_SHIFT) | \
                                     (SYSCFG_PINMUX19_PINMUX19_3_0_LCD_HSYNC << SYSCFG_PINMUX19_PINMUX19_3_0_SHIFT)

    unsigned int savePinMux = 0;

    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(16)) & \
                       ~(SYSCFG_PINMUX16_PINMUX16_31_28 | \
                         SYSCFG_PINMUX16_PINMUX16_27_24 | \
                         SYSCFG_PINMUX16_PINMUX16_23_20 | \
                         SYSCFG_PINMUX16_PINMUX16_19_16 | \
                         SYSCFG_PINMUX16_PINMUX16_15_12 | \
                         SYSCFG_PINMUX16_PINMUX16_11_8);

    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(16)) = (PINMUX16_LCD_ENABLE | savePinMux);

    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(17)) & \
                       ~(SYSCFG_PINMUX17_PINMUX17_31_28 | \
                         SYSCFG_PINMUX17_PINMUX17_27_24 | \
                         SYSCFG_PINMUX17_PINMUX17_23_20 | \
                         SYSCFG_PINMUX17_PINMUX17_19_16 | \
                         SYSCFG_PINMUX17_PINMUX17_15_12 | \
                         SYSCFG_PINMUX17_PINMUX17_11_8 | \
                         SYSCFG_PINMUX17_PINMUX17_7_4 | \
                         SYSCFG_PINMUX17_PINMUX17_3_0);

    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(17)) = (PINMUX17_LCD_ENABLE | savePinMux);

    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(18)) & \
                       ~(SYSCFG_PINMUX18_PINMUX18_31_28 | \
                         SYSCFG_PINMUX18_PINMUX18_27_24 | \
                         SYSCFG_PINMUX18_PINMUX18_7_4 |  \
                         SYSCFG_PINMUX18_PINMUX18_3_0);

    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(18)) = (PINMUX18_LCD_ENABLE | savePinMux);

    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(19)) & \
                       ~(SYSCFG_PINMUX19_PINMUX19_27_24 | \
                         SYSCFG_PINMUX19_PINMUX19_7_4 | \
                         SYSCFG_PINMUX19_PINMUX19_3_0);

    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(19)) = (PINMUX19_LCD_ENABLE | savePinMux);

    // LCD ����
    savePinMux = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) & ~(SYSCFG_PINMUX1_PINMUX1_3_0));
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) = ((SYSCFG_PINMUX1_PINMUX1_3_0_GPIO0_7 << SYSCFG_PINMUX1_PINMUX1_3_0_SHIFT) | savePinMux);
}

/****************************************************************************/
/*                                                                          */
/*              LCD ����                                                    */
/*                                                                          */
/****************************************************************************/
void LCDBacklightEnable()
{
    // ʹ�ܱ��� GPIO0[7]��Ҳ����ʹ�� ECAP APWM2 ���⣩
    GPIODirModeSet(SOC_GPIO_0_REGS, 8, GPIO_DIR_OUTPUT);
    GPIOPinWrite(SOC_GPIO_0_REGS, 8, 1);
}

void LCDBacklightDisable()
{
    // ���ñ��� GPIO0[7]
    GPIODirModeSet(SOC_GPIO_0_REGS, 8, GPIO_DIR_OUTPUT);
    GPIOPinWrite(SOC_GPIO_0_REGS, 8, 0);
}

/****************************************************************************/
/*                                                                          */
/*              Ӳ���ж��߳�                                                */
/*                                                                          */
/****************************************************************************/
Void LCDHwi(UArg arg)
{
    unsigned int  status;

    status = RasterIntStatus(SOC_LCDC_0_REGS,  RASTER_FIFO_UNDERFLOW_INT_STAT |
                                                 RASTER_END_OF_FRAME0_INT_STAT  |
                                                 RASTER_END_OF_FRAME1_INT_STAT );

    if(status & RASTER_FIFO_UNDERFLOW_INT_STAT)
    {
        RasterDisable(SOC_LCDC_0_REGS);
        RasterEnable(SOC_LCDC_0_REGS);
    }

    status = RasterClearGetIntStatus(SOC_LCDC_0_REGS, status);
}

static Void HwiInit()
{
    Hwi_Params hwiParams;
    Hwi_Params_init(&hwiParams);

    hwiParams.eventId = SYS_INT_LCDC_INT;
    Hwi_create(C674X_MASK_INT4, LCDHwi, &hwiParams, NULL);
}

/****************************************************************************/
/*                                                                          */
/*              �汾ʶ��                                                    */
/*                                                                          */
/****************************************************************************/
unsigned int LCDVersionGet()
{
    return 1;
}

/****************************************************************************/
/*                                                                          */
/*              LCD ��ʼ��                                                  */
/*                                                                          */
/****************************************************************************/
void LCDInit()
{
	// ʹ�� LCD ģ��
	PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_LCDC, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

	// �ܽŸ�������
	GPIOBankPinMuxSet();

    // Ӳ���ж��̳߳�ʼ��
    HwiInit();

	// LCD ��ʼ��
    // ���ù�դ
    RasterDisable(SOC_LCDC_0_REGS);
    
    // ʱ������
    RasterClkConfig(SOC_LCDC_0_REGS, 30000000, LCD_CLK);

    // ���� LCD DMA ������
    RasterDMAConfig(SOC_LCDC_0_REGS, RASTER_DOUBLE_FRAME_BUFFER, RASTER_BURST_SIZE_16, RASTER_FIFO_THRESHOLD_8, RASTER_BIG_ENDIAN_DISABLE);

    // ģʽ����(����:TFT ���� STN,��ɫ���ߺڰ� �ȵ�)
    RasterModeConfig(SOC_LCDC_0_REGS, RASTER_DISPLAY_MODE_TFT, RASTER_PALETTE_DATA, RASTER_COLOR, RASTER_RIGHT_ALIGNED);

    // ֡���������� LSB ��ʽ����
    RasterLSBDataOrderSelect(SOC_LCDC_0_REGS);
    
    // ���� Nibble ģʽ
    RasterNibbleModeDisable(SOC_LCDC_0_REGS);
   
    // ���ù�դ����������
    RasterTiming2Configure(SOC_LCDC_0_REGS, RASTER_FRAME_CLOCK_LOW | RASTER_LINE_CLOCK_LOW | RASTER_PIXEL_CLOCK_HIGH | RASTER_SYNC_EDGE_RISING | RASTER_SYNC_CTRL_ACTIVE | RASTER_AC_BIAS_HIGH, 0, 255);

    // ����ˮƽ / ��ֱ����
    RasterHparamConfig(SOC_LCDC_0_REGS, 800, 30, 210, 45);
    RasterVparamConfig(SOC_LCDC_0_REGS, 480, 10, 21, 22);

    // ���� FIFO DMA ��ʱ
    RasterFIFODMADelayConfig(SOC_LCDC_0_REGS, 2);

    // �����Դ�
    RasterDMAFBConfig(SOC_LCDC_0_REGS, (unsigned int)(g_pucBuffer + PALETTE_OFFSET), (unsigned int)(g_pucBuffer + PALETTE_OFFSET) + sizeof(g_pucBuffer) - 2 - PALETTE_OFFSET, 0);
    RasterDMAFBConfig(SOC_LCDC_0_REGS, (unsigned int)(g_pucBuffer + PALETTE_OFFSET), (unsigned int)(g_pucBuffer + PALETTE_OFFSET) + sizeof(g_pucBuffer) - 2 - PALETTE_OFFSET, 1);

    // ���Ƶ�ɫ�嵽�����Դ���
    unsigned char *src, *dst;
    unsigned int i = 0;

    src = (unsigned char *)palette_32b;
    dst = (unsigned char *)(g_pucBuffer + PALETTE_OFFSET);

    for(i = 4; i < (PALETTE_SIZE + 4); i++)
    {
        *dst++ = *src++;
    }

    // ʹ�� LCD ֡�����ж�
    RasterEndOfFrameIntEnable(SOC_LCDC_0_REGS);
    RasterIntEnable(SOC_LCDC_0_REGS, RASTER_END_OF_FRAME0_INT | RASTER_END_OF_FRAME1_INT);

    // ʹ�ܹ�դ
    RasterEnable(SOC_LCDC_0_REGS);
}
