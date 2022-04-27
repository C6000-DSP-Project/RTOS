// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//
//      新核科技(广州)有限公司
//
//      Copyright (C) 2022 CoreKernel Technology Guangzhou Co., Ltd
//
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//
//      SYS/BIOS 硬件中断线程
//
//      2022年04月27日
//
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
/*
 *    静态创建硬件中断线程
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
Semaphore_Handle SemHandle;

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//
//      硬件中断线程
//
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Void KEY4Hwi(UArg arg)
{
    if(GPIOPinIntStatus(SOC_GPIO_0_REGS, EVM_KEY4) == GPIO_INT_PEND)
    {
        // 发布信号量
        Semaphore_post(SemHandle);
    }

    // 清除 GPIO 中断状态
    GPIOPinIntClear(SOC_GPIO_0_REGS, EVM_KEY4);
}

Void KEY6Hwi(UArg arg)
{
    if(GPIOPinIntStatus(SOC_GPIO_0_REGS, EVM_KEY6) == GPIO_INT_PEND)
    {
        // 发布信号量
        Semaphore_post(SemHandle);
    }

    // 清除 GPIO 中断状态
    GPIOPinIntClear(SOC_GPIO_0_REGS, EVM_KEY6);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//
//      任务线程
//
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Void MainTask(UArg a0, UArg a1)
{
    System_printf("Enter MainTask()\n");
    System_flush();

    for(;;)
    {
        // 挂起信号量
        Semaphore_pend(SemHandle, BIOS_WAIT_FOREVER);

        LEDControl(SOM_LED2, LED_ON);
        LEDControl(SOM_LED3, LED_ON);
        Task_sleep(1000);

        LEDControl(SOM_LED2, LED_OFF);
        LEDControl(SOM_LED3, LED_OFF);
        Task_sleep(1000);
    }
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
    // 创建信号量
    Semaphore_Params semParams;
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;  // 二进制模式

    SemHandle = Semaphore_create(0, &semParams, NULL);

    // 启动 SYS/BIOS
    BIOS_start();
}
