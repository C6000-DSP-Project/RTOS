//
//      新核科技(广州)有限公司
//
//      Copyright (C) 2022 CoreKernel Technology Guangzhou Co., Ltd
//
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//
//      NTCP 服务器端
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
#include <netmain.h>

#include <stdio.h>

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//
//      TCP 服务器
//
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
int TCPServer(SOCKET s, UINT32 unused)
{
    // 获取客户端信息
    struct sockaddr_in addr;
    int len = sizeof(addr);
    getpeername(s, (PSA)&addr, &len);

    unsigned char *val = (unsigned char *)&addr.sin_addr;
    printf("Client Connected IP: %u.%u.%u.%u Port: %d\n", val[0], val[1], val[2], val[3], ntohs(addr.sin_port));

    // 配置超时时间
    struct timeval timeout;
    timeout.tv_sec  = 60;  // 60 秒
    timeout.tv_usec = 0;
    setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    // 发送字符串
    char Title[] = "TCP Server Example...";
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
            {
                break;
            }

            recvncfree(hBuffer);
        }
        else
        {
            break;
        }
    }

    fdClose(s);

    return 1;
}
