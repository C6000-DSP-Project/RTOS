/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    �����ն�                                                              */
/*                                                                          */
/*    2014��07��12��                                                        */
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
#include "hw_types.h"

#include "soc_C6748.h"
#include "hw_syscfg0_C6748.h"

#include "uart.h"
#include "psc.h"

/****************************************************************************/
/*                                                                          */
/*              �궨��                                                      */
/*                                                                          */
/****************************************************************************/
#define UART_STDIO_INSTANCE             (2)
#define UART_CONSOLE_BASE               (SOC_UART_2_REGS)

/****************************************************************************/
/*                                                                          */
/*              ��������                                                    */
/*                                                                          */
/****************************************************************************/
static void UARTStdioInitExpClk(unsigned int baudRate, unsigned int rxTrigLevel);
void UARTConsolePutc(unsigned char data);
unsigned char UARTConsoleGetc(void);
void UARTConsoleInit(void);

/****************************************************************************/
/*                                                                          */
/*              ���ڿ���̨����                                              */
/*                                                                          */
/****************************************************************************/
static void UARTStdioInitExpClk(unsigned int baudRate, unsigned int rxTrigLevel)
{
     // ʹ�ܽ��� / ����
     UARTEnable(UART_CONSOLE_BASE);

     // ���ڲ�������
     // 8λ����λ 1λֹͣλ ��У��
     UARTConfigSetExpClk(UART_CONSOLE_BASE, 
                         SOC_UART_2_MODULE_FREQ,
                         baudRate, 
                         UART_WORDL_8BITS,
                         UART_OVER_SAMP_RATE_16);


     // ʹ�ܽ��� / ���� FIFO
     UARTFIFOEnable(UART_CONSOLE_BASE);

     // ���ý��� FIFO ����
     UARTFIFOLevelSet(UART_CONSOLE_BASE, rxTrigLevel);

}

/****************************************************************************/
/*                                                                          */
/*              ���ڿ���̨��ʼ��                                            */
/*                                                                          */
/****************************************************************************/
void UARTConsoleInit(void)
{
     #if (0 == UART_STDIO_INSTANCE)
     {
          PSCModuleControl(SOC_PSC_0_REGS,9, 0, PSC_MDCTL_NEXT_ENABLE);
          UARTPinMuxSetup(0, FALSE);
     }
     
     #elif (1 == UART_STDIO_INSTANCE)
     {
          PSCModuleControl(SOC_PSC_1_REGS,12, 0, PSC_MDCTL_NEXT_ENABLE);
          UARTPinMuxSetup(1, FALSE);
     }

     #else 
     {
          PSCModuleControl(SOC_PSC_1_REGS,13, 0, PSC_MDCTL_NEXT_ENABLE);

          // �����ƴ��� ��ʹ������
          HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) &
                                                         (~(SYSCFG_PINMUX4_PINMUX4_23_20 |
                                                            SYSCFG_PINMUX4_PINMUX4_19_16))) |
                                                         ((SYSCFG_PINMUX4_PINMUX4_23_20_UART2_TXD << SYSCFG_PINMUX4_PINMUX4_23_20_SHIFT) |
                                                          (SYSCFG_PINMUX4_PINMUX4_19_16_UART2_RXD << SYSCFG_PINMUX4_PINMUX4_19_16_SHIFT));

          #define PINMUX4_UART2_TXD_ENABLE    (SYSCFG_PINMUX4_PINMUX4_23_20_UART2_TXD << SYSCFG_PINMUX4_PINMUX4_23_20_SHIFT)
          #define PINMUX4_UART2_RXD_ENABLE    (SYSCFG_PINMUX4_PINMUX4_19_16_UART2_RXD << SYSCFG_PINMUX4_PINMUX4_19_16_SHIFT)

          unsigned int savePinmux = 0;

          savePinmux = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) & ~(SYSCFG_PINMUX4_PINMUX4_23_20 | SYSCFG_PINMUX4_PINMUX4_19_16));
          HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) = ((PINMUX4_UART2_TXD_ENABLE | PINMUX4_UART2_RXD_ENABLE) | savePinmux);
     }
     #endif
     
     UARTStdioInitExpClk(BAUD_115200, UART_RX_TRIG_LEVEL_1);
}

/****************************************************************************/
/*                                                                          */
/*              ���һ���ַ�                                                */
/*                                                                          */
/****************************************************************************/
void UARTConsolePutc(unsigned char data)
{
     UARTCharPut(UART_CONSOLE_BASE, data);
}

/****************************************************************************/
/*                                                                          */
/*              ȡ��һ���ַ�                                                */
/*                                                                          */
/****************************************************************************/
unsigned char UARTConsoleGetc(void)
{
    return ((unsigned char)UARTCharGet(UART_CONSOLE_BASE));
}
