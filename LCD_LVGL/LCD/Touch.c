/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    LCD ������                                                            */
/*                                                                          */
/*    2022��03��27��                                                        */
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
#include "i2c.h"

#include "interrupt.h"

#include "Touch.h"

/****************************************************************************/
/*                                                                          */
/*              �궨��                                                      */
/*                                                                          */
/****************************************************************************/
// GPIO ����
#define TouchResetPin          98
#define TouchIntPin            139

// I2C �豸��ַ
#define ADDRESS                0x5D

// ����оƬ�Ĵ���
#define GT9XX_CFG_VER          0x8047     // ���ð汾
#define GT9XX_PRODUCT_ID       0x8140     // ��Ʒ ID �Ĵ��� 0x8140(LSB) 0x8141 0x8142 0x8143(MSB)
#define GT9XX_C00RDINATE_INFO  0x814E     // ������Ϣ

// ���ص���
#define TouchMax               5

/****************************************************************************/
/*                                                                          */
/*              ȫ�ֱ���                                                    */
/*                                                                          */
/****************************************************************************/
stTouchInfo TouchInfo;

/****************************************************************************/
/*                                                                          */
/*              GPIO �ܽŸ�������                                           */
/*                                                                          */
/****************************************************************************/
static void GPIOBankPinMuxSet()
{
    // GPIO6[01] CTP_RESET
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(19)) = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(19)) & (~(SYSCFG_PINMUX19_PINMUX19_23_20))) |
                                                    (SYSCFG_PINMUX19_PINMUX19_23_20_GPIO6_1 << SYSCFG_PINMUX19_PINMUX19_23_20_SHIFT);

    // GPIO8[10] CTP_INT
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(18)) = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(18)) & (~(SYSCFG_PINMUX18_PINMUX18_31_28))) |
                                                    (SYSCFG_PINMUX18_PINMUX18_31_28_GPIO8_10 << SYSCFG_PINMUX18_PINMUX18_31_28_SHIFT);
}

// ������������������������������������������������������������������������������������������������������������������������������������������������������
//
//      ��ʱ���Ǿ�ȷ��
//
// ������������������������������������������������������������������������������������������������������������������������������������������������������
static void Delay(volatile unsigned int delay)
{
    while(delay--);
}

/****************************************************************************/
/*                                                                          */
/*              ��λ                                                        */
/*                                                                          */
/****************************************************************************/
static void TouchReset()
{
    // ��λ������ I2C �豸��ַΪ 0x5D(д��ַ 0xBA ����ַ 0xBB)
    GPIODirModeSet(SOC_GPIO_0_REGS, TouchResetPin, GPIO_DIR_OUTPUT);
    GPIODirModeSet(SOC_GPIO_0_REGS, TouchIntPin, GPIO_DIR_OUTPUT);

    GPIOPinWrite(SOC_GPIO_0_REGS, TouchResetPin, GPIO_PIN_HIGH);
    GPIOPinWrite(SOC_GPIO_0_REGS, TouchIntPin, GPIO_PIN_LOW);
    Delay(0x00FFFFFF);  // ���� 100us

    // ��λ
    GPIOPinWrite(SOC_GPIO_0_REGS, TouchResetPin, GPIO_PIN_LOW);
    Delay(0x00FFFFFF);  // ���� 5ms

    GPIOPinWrite(SOC_GPIO_0_REGS, TouchResetPin, GPIO_PIN_HIGH);
    Delay(0x00FFFFFF);

    // �����ж�����Ϊ����
    GPIODirModeSet(SOC_GPIO_0_REGS, TouchIntPin, GPIO_DIR_INPUT);
    Delay(0x00FFFFFF);

    // �����жϴ�����ʽ
    GPIOIntTypeSet(SOC_GPIO_0_REGS, TouchIntPin, GPIO_INT_TYPE_RISEDGE);

    // ʹ�� GPIO BANK �ж�
    GPIOBankIntEnable(SOC_GPIO_0_REGS, 8);
}

/****************************************************************************/
/*                                                                          */
/*              ��ȡ��������                                                */
/*                                                                          */
/****************************************************************************/
void TouchXYGet()
{
    // ���ô��豸��ַ
    I2CMasterSlaveAddrSet(SOC_I2C_0_REGS, ADDRESS);

    unsigned char val[TouchMax * 8 + 1];

    // ��ȡ����״̬
    val[0] = I2CHWRegRead(SOC_I2C_0_REGS, GT9XX_C00RDINATE_INFO);
    TouchInfo.Num = val[0] & 0x0F;

    if((TouchInfo.Num >= 1) && (TouchInfo.Num <= TouchMax))
    {
        // ��ȡ��������
        int i;
        for(i = 0; i < 8 * TouchInfo.Num; i++)
        {
            val[i] = I2CHWRegRead(SOC_I2C_0_REGS, GT9XX_C00RDINATE_INFO + 1 + i);
        }

        // д������
        for(i = 0; i < TouchInfo.Num; i++)
        {
            TouchInfo.X[i] = (val[i * 8 + 2] << 8) | val[i * 8 + 1];
            TouchInfo.Y[i] = (val[i * 8 + 4] << 8) | val[i * 8 + 3];
        }

        TouchInfo.Flag = true;
    }
    else
    {
        TouchInfo.Flag = false;
    }

    // �����־λ
    I2CHWRegWrite(SOC_I2C_0_REGS, GT9XX_C00RDINATE_INFO, 0);
}

/****************************************************************************/
/*                                                                          */
/*              �������жϷ�����                                          */
/*                                                                          */
/****************************************************************************/
Void TouchHwi(UArg arg)
{
    GPIOBankIntDisable(SOC_GPIO_0_REGS, 8);

    if(GPIOPinIntStatus(SOC_GPIO_0_REGS, TouchIntPin) == GPIO_INT_PEND)
    {
        TouchXYGet();
    }

    GPIOPinIntClear(SOC_GPIO_0_REGS, TouchIntPin);

    GPIOBankIntEnable(SOC_GPIO_0_REGS, 8);
}

static Void HwiInit()
{
    Hwi_Params hwiParams;
    Hwi_Params_init(&hwiParams);

    hwiParams.eventId = SYS_INT_GPIO_B8INT;
    Hwi_create(C674X_MASK_INT5, TouchHwi, &hwiParams, NULL);
}

/****************************************************************************/
/*                                                                          */
/*              ��������ʼ��                                                */
/*                                                                          */
/****************************************************************************/
void TouchInit()
{
    // ʹ������
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_GPIO, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

    // �ܽŸ�������
    GPIOBankPinMuxSet();

    // ��λ������ I2C �豸��ַ
    TouchReset();

    // I2C ����
    I2CInit(SOC_I2C_0_REGS, ADDRESS);

    // Ӳ���ж��̳߳�ʼ��
    HwiInit();

    /*
    // ��ȡ����оƬ��Ϣ
    ConsoleWrite("\r\nTouch IC Info\r\n");
    unsigned char val[16];
    val[0] = I2CHWRegRead(SOC_I2C_0_REGS, GT9XX_CFG_VER);
    ConsoleWrite("Config Version: 0x%x\n", val[0]);

    int i;
    for(i = 0; i < 11; i++)
    {
        val[i] = I2CHWRegRead(SOC_I2C_0_REGS, GT9XX_PRODUCT_ID + i);
    }

    char str[64];
    sprintf(str, "Product ID: GT%c%c%c%c", val[0], val[1], val[2], val[3]);
    ConsoleWrite("%s\n", str);

    ConsoleWrite("Firmware Version: 0x%x\n", (val[5] << 8) | val[4]);
    ConsoleWrite("X/Y Coordinate Resolution: %dx%d\n", (val[7] << 8) | val[6], (val[9] << 8) | val[8]);
    ConsoleWrite("Vendor ID: 0x%x\r\n\r\n", val[10]);
    */
}
