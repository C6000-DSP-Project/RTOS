// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//
//      新核科技(广州)有限公司
//
//      Copyright (C) 2022 CoreKernel Technology Guangzhou Co., Ltd
//
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//
//      SYS/BIOS 任务信号量
//
//      2022年04月27日
//
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
/*
 *    信号量用于任务之间同步
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
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//
//      全局变量
//
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Task_Handle MainTaskHandle;
Task_Handle TestTaskHandle;

Semaphore_Handle SemHandle;

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
        // 发布信号量
        // 信号量可以在硬件中断线程 Hwi/软件中断线程 Swi/任务线性 Task 中被发布
        Semaphore_post(SemHandle);

        Task_sleep(1000);
    }
}

Void TestTask(UArg a0, UArg a1)
{
    for(;;)
    {
        // 挂起信号量
        Semaphore_pend(SemHandle, BIOS_WAIT_FOREVER);

        System_printf("Semaphore Posted.\n");
        System_flush();

        // 休眠以允许低优先级任务运行
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
    // 创建任务
    Task_Params taskParams;
    Task_Params_init(&taskParams);
    taskParams.priority = 1;

    MainTaskHandle = Task_create(MainTask, &taskParams, NULL);

    taskParams.priority = 15;
    TestTaskHandle = Task_create(TestTask, &taskParams, NULL);

    // 创建信号量
    Semaphore_Params semParams;
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;  // 二进制模式

    SemHandle = Semaphore_create(0, &semParams, NULL);

    // 启动 SYS/BIOS
    BIOS_start();
}
