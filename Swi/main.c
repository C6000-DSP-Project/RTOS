// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//
//      新核科技(广州)有限公司
//
//      Copyright (C) 2022 CoreKernel Technology Guangzhou Co., Ltd
//
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//
//      SYS/BIOS 软件中断线程
//
//      2022年04月27日
//
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
/*
 *    运行时创建软件中断线程
 *    软件中断只能在主函数及任务中创建 实时性比任务线程高
 *
 *    - 希望缄默(bin wang)
 *    - bin@corekernel.net
 *
 *    官网 corekernel.net/.org/.cn
 *    社区 fpga.net.cn
 *
 */
#include <xdc/std.h>

#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

// 外设
#include "LED.h"
#include "KEY.h"

// StarterWare 驱动库
#include "soc_C6748.h"

#include "gpio.h"

#include "interrupt.h"  // 仅使用该文件定义的事件 不使用驱动库

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//
//      全局变量
//
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Swi_Handle LEDSwiHandle;

unsigned char Flag = 0;

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//
//      硬件中断线程
//
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Void KEY4Hwi(UArg arg)
{
    if(GPIOPinIntStatus(SOC_GPIO_0_REGS, EVM_KEY4) == GPIO_INT_PEND)
    {
        // 发布软件中断
        Swi_post(LEDSwiHandle);
    }

    // 清除 GPIO 中断状态
    GPIOPinIntClear(SOC_GPIO_0_REGS, EVM_KEY4);
}

Void KEY6Hwi(UArg arg)
{
    if(GPIOPinIntStatus(SOC_GPIO_0_REGS, EVM_KEY6) == GPIO_INT_PEND)
    {
        // 发布软件中断
        Swi_post(LEDSwiHandle);
    }

    // 清除 GPIO 中断状态
    GPIOPinIntClear(SOC_GPIO_0_REGS, EVM_KEY6);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//
//      软件中断线程
//
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Void LEDSwi(UArg a0, UArg a1)
{
    // 改变 LED 状态
    Flag = !Flag;
    LEDControl(SOM_LED2, Flag);
    LEDControl(SOM_LED3, Flag);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//
//      主函数
//
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Int main()
{
    /* 外设初始化 */
    // LED 初始化
    LEDInit();

    // LED 初始化
    KEYInit();

    /* 线程初始化 */
    // 创建硬件中断线程
    Hwi_Params hwiParams;
    Hwi_Params_init(&hwiParams);

    hwiParams.eventId = SYS_INT_GPIO_B0INT;
    Hwi_create(C674X_MASK_INT4, KEY6Hwi, &hwiParams, NULL);

    hwiParams.eventId = SYS_INT_GPIO_B8INT;
    Hwi_create(C674X_MASK_INT5, KEY4Hwi, &hwiParams, NULL);

    // 创建软件中断线程
    Swi_Params swiParams;
    Swi_Params_init(&swiParams);

    swiParams.trigger = 0;
    LEDSwiHandle = Swi_create(LEDSwi, &swiParams, NULL);

    // 启动 SYS/BIOS
    BIOS_start();
}
