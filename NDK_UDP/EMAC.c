//
//      �º˿Ƽ�(����)���޹�˾
//
//      Copyright (C) 2022 CoreKernel Technology Guangzhou Co., Ltd
//
// ������������������������������������������������������������������������������������������������������������������������������������������������������
// ������������������������������������������������������������������������������������������������������������������������������������������������������
//
//      EMAC ����
//
//      2022��04��26��
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
#include <stdio.h>

#include <netmain.h>

#include "hw_types.h"
#include "hw_syscfg0_C6748.h"

#include "soc_C6748.h"

#include "psc.h"

// ������������������������������������������������������������������������������������������������������������������������������������������������������
//
//      ȫ�ֱ���
//
// ������������������������������������������������������������������������������������������������������������������������������������������������������
// MAC ��ַ
unsigned char bMacAddr[8];

// ����״̬
char *LinkStr[] = {"No Link", "10Mb/s Half Duplex", "10Mb/s Full Duplex", "100Mb/s Half Duplex", "100Mb/s Full Duplex"};

// ������������������������������������������������������������������������������������������������������������������������������������������������������
//
//      EMAC �ܽŸ�������
//
// ������������������������������������������������������������������������������������������������������������������������������������������������������
void MDIOGPIOBankPinMuxSet()
{
    #define PINMUX4_MDIO_ENABLE      (SYSCFG_PINMUX4_PINMUX4_7_4_MDIO_D << SYSCFG_PINMUX4_PINMUX4_7_4_SHIFT) | \
                                      (SYSCFG_PINMUX4_PINMUX4_3_0_MDIO_CLK << SYSCFG_PINMUX4_PINMUX4_3_0_SHIFT)
    unsigned int savePinMux = 0;

    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) & \
                       ~(SYSCFG_PINMUX4_PINMUX4_3_0 | \
                         SYSCFG_PINMUX4_PINMUX4_7_4);

    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) = (PINMUX4_MDIO_ENABLE | savePinMux);
}

void EMACMIIGPIOBankPinMuxSet()
{
    #define PINMUX2_EMAC_ENABLE      (SYSCFG_PINMUX2_PINMUX2_31_28_MII_TXD0 << SYSCFG_PINMUX2_PINMUX2_31_28_SHIFT) | \
                                      (SYSCFG_PINMUX2_PINMUX2_27_24_MII_TXD1 << SYSCFG_PINMUX2_PINMUX2_27_24_SHIFT) | \
                                      (SYSCFG_PINMUX2_PINMUX2_23_20_MII_TXD2 << SYSCFG_PINMUX2_PINMUX2_23_20_SHIFT) | \
                                      (SYSCFG_PINMUX2_PINMUX2_19_16_MII_TXD3 << SYSCFG_PINMUX2_PINMUX2_19_16_SHIFT) | \
                                      (SYSCFG_PINMUX2_PINMUX2_15_12_MII_COL << SYSCFG_PINMUX2_PINMUX2_15_12_SHIFT) | \
                                      (SYSCFG_PINMUX2_PINMUX2_11_8_MII_TXCLK << SYSCFG_PINMUX2_PINMUX2_11_8_SHIFT) | \
                                      (SYSCFG_PINMUX2_PINMUX2_7_4_MII_TXEN << SYSCFG_PINMUX2_PINMUX2_7_4_SHIFT)


    #define PINMUX3_EMAC_ENABLE      (SYSCFG_PINMUX3_PINMUX3_31_28_MII_RXD0 << SYSCFG_PINMUX3_PINMUX3_31_28_SHIFT) | \
                                      (SYSCFG_PINMUX3_PINMUX3_27_24_MII_RXD1 << SYSCFG_PINMUX3_PINMUX3_27_24_SHIFT) | \
                                      (SYSCFG_PINMUX3_PINMUX3_23_20_MII_RXD2 << SYSCFG_PINMUX3_PINMUX3_23_20_SHIFT) | \
                                      (SYSCFG_PINMUX3_PINMUX3_19_16_MII_RXD3 << SYSCFG_PINMUX3_PINMUX3_19_16_SHIFT) | \
                                      (SYSCFG_PINMUX3_PINMUX3_15_12_MII_CRS << SYSCFG_PINMUX3_PINMUX3_15_12_SHIFT) | \
                                      (SYSCFG_PINMUX3_PINMUX3_11_8_MII_RXER << SYSCFG_PINMUX3_PINMUX3_11_8_SHIFT) | \
                                      (SYSCFG_PINMUX3_PINMUX3_7_4_MII_RXDV << SYSCFG_PINMUX3_PINMUX3_7_4_SHIFT) | \
                                      (SYSCFG_PINMUX3_PINMUX3_3_0_MII_RXCLK << SYSCFG_PINMUX3_PINMUX3_3_0_SHIFT)

    unsigned int savePinMux = 0;

    savePinMux =  HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(2)) & \
                        ~(SYSCFG_PINMUX2_PINMUX2_31_28 | \
                          SYSCFG_PINMUX2_PINMUX2_27_24 | \
                          SYSCFG_PINMUX2_PINMUX2_23_20 | \
                          SYSCFG_PINMUX2_PINMUX2_19_16 | \
                          SYSCFG_PINMUX2_PINMUX2_15_12 | \
                          SYSCFG_PINMUX2_PINMUX2_11_8 | \
                          SYSCFG_PINMUX2_PINMUX2_7_4);

    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(2)) = (PINMUX2_EMAC_ENABLE | savePinMux);

    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(3)) & \
                       ~(SYSCFG_PINMUX3_PINMUX3_31_28 | \
                         SYSCFG_PINMUX3_PINMUX3_27_24 | \
                         SYSCFG_PINMUX3_PINMUX3_23_20 | \
                         SYSCFG_PINMUX3_PINMUX3_19_16 | \
                         SYSCFG_PINMUX3_PINMUX3_15_12 | \
                         SYSCFG_PINMUX3_PINMUX3_11_8 | \
                         SYSCFG_PINMUX3_PINMUX3_7_4 | \
                         SYSCFG_PINMUX3_PINMUX3_3_0);

    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(3)) = (PINMUX3_EMAC_ENABLE | savePinMux);

    // ʹ�� MII ģʽ
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_CFGCHIP3) &= ~(SYSCFG_CFGCHIP3_RMII_SEL);
}

void EMACRMIIGPIOBankPinMuxSet()
{
    #define PINMUX14_EMAC_ENABLE    (SYSCFG_PINMUX14_PINMUX14_31_28_RMII_RXER << SYSCFG_PINMUX14_PINMUX14_31_28_SHIFT) | \
                                     (SYSCFG_PINMUX14_PINMUX14_27_24_RMII_RXD0 << SYSCFG_PINMUX14_PINMUX14_27_24_SHIFT) | \
                                     (SYSCFG_PINMUX14_PINMUX14_23_20_RMII_RXD1 << SYSCFG_PINMUX14_PINMUX14_23_20_SHIFT) | \
                                     (SYSCFG_PINMUX14_PINMUX14_19_16_RMII_TXEN << SYSCFG_PINMUX14_PINMUX14_19_16_SHIFT) | \
                                     (SYSCFG_PINMUX14_PINMUX14_15_12_RMII_TXD0 << SYSCFG_PINMUX14_PINMUX14_15_12_SHIFT) | \
                                     (SYSCFG_PINMUX14_PINMUX14_11_8_RMII_TXD1 << SYSCFG_PINMUX14_PINMUX14_11_8_SHIFT)

    #define PINMUX15_EMAC_ENABLE    (SYSCFG_PINMUX15_PINMUX15_7_4_RMII_CRS_DV << SYSCFG_PINMUX15_PINMUX15_7_4_SHIFT) | \
                                     (SYSCFG_PINMUX15_PINMUX15_3_0_DEFAULT << SYSCFG_PINMUX15_PINMUX15_3_0_SHIFT)

    unsigned int savePinMux = 0;

    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(14)) & \
                       ~(SYSCFG_PINMUX14_PINMUX14_31_28 | \
                         SYSCFG_PINMUX14_PINMUX14_27_24 | \
                         SYSCFG_PINMUX14_PINMUX14_23_20 | \
                         SYSCFG_PINMUX14_PINMUX14_19_16 | \
                         SYSCFG_PINMUX14_PINMUX14_15_12 | \
                         SYSCFG_PINMUX14_PINMUX14_11_8);

    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(14)) = (PINMUX14_EMAC_ENABLE | savePinMux);

    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(15)) & \
                       ~(SYSCFG_PINMUX15_PINMUX15_7_4 | \
                         SYSCFG_PINMUX15_PINMUX15_3_0);

    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(15)) = (PINMUX15_EMAC_ENABLE | savePinMux);

    // ʹ�� RMII ģʽ
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_CFGCHIP3) &= SYSCFG_CFGCHIP3_RMII_SEL;
}

// ������������������������������������������������������������������������������������������������������������������������������������������������������
//
//      EMAC ��ʼ��
//
// ������������������������������������������������������������������������������������������������������������������������������������������������������
void EMAC_initialize()
{
    // ʹ������
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_EMAC, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

    // ʹ�� MII ģʽ
    MDIOGPIOBankPinMuxSet();
    EMACMIIGPIOBankPinMuxSet();
}

// ������������������������������������������������������������������������������������������������������������������������������������������������������
//
//      ��ȡ MAC ��ַ
//
// ������������������������������������������������������������������������������������������������������������������������������������������������������
// ����������������� ��Ҫ�޸ĺ�����
void EMAC_getConfig(unsigned char *pMacAddr)
{
	// ����оƬ ID ���� MAC ��ַ
	bMacAddr[0] = 0x00;
	bMacAddr[1] = (*(volatile unsigned int *)(0x01C14008) & 0x0000FF00) >> 8;
	bMacAddr[2] = (*(volatile unsigned int *)(0x01C14008) & 0x000000FF) >> 0;
	bMacAddr[3] = (*(volatile unsigned int *)(0x01C1400C) & 0x0000FF00) >> 8;
	bMacAddr[4] = (*(volatile unsigned int *)(0x01C1400C) & 0x000000FF) >> 0;
	bMacAddr[5] = (*(volatile unsigned int *)(0x01C14010) & 0x000000FF) >> 0;
	printf("Using MAC Address: %02X-%02X-%02X-%02X-%02X-%02X\n",
    		bMacAddr[0], bMacAddr[1], bMacAddr[2], bMacAddr[3], bMacAddr[4], bMacAddr[5]);

    // ���� MAC ��ַ
    mmCopy(pMacAddr, bMacAddr, 6);
}

// ������������������������������������������������������������������������������������������������������������������������������������������������������
//
//      ���� MAC ��ַ
//
// ������������������������������������������������������������������������������������������������������������������������������������������������������
void EMAC_setConfig(unsigned char *pMacAddr)
{
    mmCopy(bMacAddr, pMacAddr, 6);
    printf("Setting MAC Addr to: %02x-%02x-%02x-%02x-%02x-%02x\n",
            bMacAddr[0], bMacAddr[1], bMacAddr[2],
            bMacAddr[3], bMacAddr[4], bMacAddr[5]);
}

// ������������������������������������������������������������������������������������������������������������������������������������������������������
//
//      ��ȡ����״̬
//
// ������������������������������������������������������������������������������������������������������������������������������������������������������
void EMAC_linkStatus(unsigned int phy, unsigned int linkStatus)
{
    printf("Link Status: %s on PHY %d\n",LinkStr[linkStatus],phy);
}
