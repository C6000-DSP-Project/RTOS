//
//      新核科技(广州)有限公司
//
//      Copyright (C) 2022 CoreKernel Technology Guangzhou Co., Ltd
//
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//
//      TCP 客户端
//
//      2022年04月27日
//
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
/*
 *    - 希望缄默(bin wang)
 *    - bin@corekernel.net
 *
 *    官网 corekernel.net/.org/.cn
 *    社区 fpga.net.cn
 *
 */
#include <ti/sysbios/knl/Task.h>

#include <netmain.h>
#include <console.h>

#include <stdio.h>

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//
//      全局变量
//
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
char *IPStr = "172.26.0.160";
unsigned int Port = 5000;

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//
//      TCP 客户端
//
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void TCPClient()
{
    Task_sleep(5000);

    printf("Trying to connect %s:%d...\n", IPStr, Port);

    // 为任务分配文件描述符并打开一个会话
    fdOpenSession(TaskSelf());

    // 创建套接字
    SOCKET  s;
    s = socket(AF_INET, SOCK_STREAMNC, IPPROTO_TCP);
    if(s == INVALID_SOCKET)
    {
        printf("failed socket create (%d)\n", fdError());

        goto error;
    }

    // IP 地址
    IPN IPAddr;
    if(!ConStrToIPN(IPStr, &IPAddr))
    {
        printf("Invalid address\n\n");
    }

    struct sockaddr_in SinDst;
    mmZeroInit(&SinDst, sizeof(struct sockaddr_in));
    SinDst.sin_family      = AF_INET;
    SinDst.sin_addr.s_addr = IPAddr;
    SinDst.sin_port        = htons(Port);

    // 超时时间  5秒
    // 配置超时时间
    struct timeval timeout;
    timeout.tv_sec  = 60;  // 60 秒
    timeout.tv_usec = 0;
    setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof( timeout ));
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof( timeout ));

    // 建立连接
    if(connect(s, (PSA) &SinDst, sizeof(SinDst)) < 0)
    {
        printf("failed connect (%d)\n", fdError());

        goto error;
    }
    printf("Established the connection.\n");

    char Title[] = "TCP Client Example...";
    send(s, &Title, sizeof(Title), 0);

    HANDLE hBuffer;
    char *pBuf;
    int i;

    for(;;)
    {
        i = (int)recvnc(s, (void **)&pBuf, 0, &hBuffer);

        if(i > 0)
        {
            if(send(s, pBuf, i, 0 ) < 0)
                break;

            recvncfree(hBuffer);
        }
        else
        {
            continue;
        }
    }

error:
    if(s != INVALID_SOCKET)
    {
        fdClose(s);
    }

    fdCloseSession(TaskSelf());
}
