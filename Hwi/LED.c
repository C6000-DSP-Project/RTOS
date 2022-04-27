// ������������������������������������������������������������������������������������������������������������������������������������������������������
//
//      �º˿Ƽ�(����)���޹�˾
//
//      Copyright (C) 2022 CoreKernel Technology Guangzhou Co., Ltd
//
// ������������������������������������������������������������������������������������������������������������������������������������������������������
// ������������������������������������������������������������������������������������������������������������������������������������������������������
//
//      GPIO LED
//
//      2022��04��27��
//
// ������������������������������������������������������������������������������������������������������������������������������������������������������
/*
 *    - ϣ����Ĭ(bin wang)
 *    - bin@corekernel.net
 *
 *    ���� corekernel.net/.org/.cn
 *    ���� fpga.net.cn
 *
 */
#include "hw_types.h"
#include "hw_syscfg0_C6748.h"

#include "soc_C6748.h"

#include "psc.h"
#include "gpio.h"

#include "LED.h"

// ������������������������������������������������������������������������������������������������������������������������������������������������������
//
//      GPIO �ܽŸ�������
//
// ������������������������������������������������������������������������������������������������������������������������������������������������������
static void GPIOBankPinMuxSet()
{
    // ������Ӧ�� GPIO �ڹ���Ϊ��ͨ���������
    // ���İ�
    // GPIO6[12]
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(13)) = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(13)) & (~(SYSCFG_PINMUX13_PINMUX13_15_12))) |
                                                    ((SYSCFG_PINMUX13_PINMUX13_15_12_GPIO6_12 << SYSCFG_PINMUX13_PINMUX13_15_12_SHIFT));

    // GPIO6[13]
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(13)) = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(13)) & (~(SYSCFG_PINMUX13_PINMUX13_11_8))) |
                                                    ((SYSCFG_PINMUX13_PINMUX13_11_8_GPIO6_13 << SYSCFG_PINMUX13_PINMUX13_11_8_SHIFT));

    // �װ�
    // GPIO2[15]
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(05)) = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(05)) & (~(SYSCFG_PINMUX5_PINMUX5_3_0))) |
                                                    ((SYSCFG_PINMUX5_PINMUX5_3_0_GPIO2_15 << SYSCFG_PINMUX5_PINMUX5_3_0_SHIFT));

    // GPIO4[00]
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(10)) = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(10)) & (~(SYSCFG_PINMUX10_PINMUX10_31_28))) |
                                                    ((SYSCFG_PINMUX10_PINMUX10_31_28_GPIO4_0 << SYSCFG_PINMUX10_PINMUX10_31_28_SHIFT));
}

// ������������������������������������������������������������������������������������������������������������������������������������������������������
//
//      GPIO �ܽų�ʼ��
//
// ������������������������������������������������������������������������������������������������������������������������������������������������������
static void GPIOBankPinInit()
{
    // ���� LED ��Ӧ�ܽ�Ϊ����ܽ�
    // OMAP-L138 �� TMS320C6748 ���� 144 �� GPIO
    // GPIO0[0] 1-16
    // GPIO1[0] 17-32
    // GPIO2[0] 33-48
    // GPIO3[0] 49-64
    // GPIO4[0] 65-80
    // GPIO5[0] 81-96
    // GPIO6[0] 97-112
    // GPIO7[0] 113-128
    // GPIO8[0] 129-144

    // ���İ�
    GPIODirModeSet(SOC_GPIO_0_REGS, SOM_LED3, GPIO_DIR_OUTPUT);  // GPIO6[12] LED3
    GPIODirModeSet(SOC_GPIO_0_REGS, SOM_LED2, GPIO_DIR_OUTPUT);  // GPIO6[13] LED2

    // �װ�
    GPIODirModeSet(SOC_GPIO_0_REGS, EVM_LED4, GPIO_DIR_OUTPUT);   // GPIO2[15] LED4
    GPIODirModeSet(SOC_GPIO_0_REGS, EVM_LED3, GPIO_DIR_OUTPUT);   // GPIO4[00] LED3
}

// ������������������������������������������������������������������������������������������������������������������������������������������������������
//
//      LED ����
//
// ������������������������������������������������������������������������������������������������������������������������������������������������������
void LEDControl(unsigned char LED, char Status)
{
    switch(LED)
    {
        case SOM_LED2 : Status ? GPIOPinWrite(SOC_GPIO_0_REGS, SOM_LED2, GPIO_PIN_HIGH) : \
                                  GPIOPinWrite(SOC_GPIO_0_REGS, SOM_LED2, GPIO_PIN_LOW); break;   // GPIO6[13] LED2

        case SOM_LED3 : Status ? GPIOPinWrite(SOC_GPIO_0_REGS, SOM_LED3, GPIO_PIN_HIGH) : \
                                   GPIOPinWrite(SOC_GPIO_0_REGS, SOM_LED3, GPIO_PIN_LOW); break;  // GPIO6[12] LED3

        case EVM_LED3  : Status ? GPIOPinWrite(SOC_GPIO_0_REGS, EVM_LED3, GPIO_PIN_HIGH) : \
                                   GPIOPinWrite(SOC_GPIO_0_REGS, EVM_LED3, GPIO_PIN_LOW); break;  // GPIO4[00] LED3

        case EVM_LED4  : Status ? GPIOPinWrite(SOC_GPIO_0_REGS, EVM_LED4, GPIO_PIN_HIGH) : \
                                  GPIOPinWrite(SOC_GPIO_0_REGS, EVM_LED4, GPIO_PIN_LOW); break;   // GPIO2[15] LED4
    }
}

// ������������������������������������������������������������������������������������������������������������������������������������������������������
//
//      LED ��ʼ��
//
// ������������������������������������������������������������������������������������������������������������������������������������������������������
void LEDInit()
{
    // ʹ������
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_GPIO, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

    // �ܽŸ�������
    GPIOBankPinMuxSet();

    // GPIO �ܽų�ʼ��
    GPIOBankPinInit();
}
