//
//      �º˿Ƽ�(����)���޹�˾
//
//      Copyright (C) 2022 CoreKernel Technology Guangzhou Co., Ltd
//
// ������������������������������������������������������������������������������������������������������������������������������������������������������
// ������������������������������������������������������������������������������������������������������������������������������������������������������
//
//      NTCP ��������
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
#include <netmain.h>

#include <stdio.h>

// ������������������������������������������������������������������������������������������������������������������������������������������������������
//
//      TCP ������
//
// ������������������������������������������������������������������������������������������������������������������������������������������������������
int TCPServer(SOCKET s, UINT32 unused)
{
    // ��ȡ�ͻ�����Ϣ
    struct sockaddr_in addr;
    int len = sizeof(addr);
    getpeername(s, (PSA)&addr, &len);

    unsigned char *val = (unsigned char *)&addr.sin_addr;
    printf("Client Connected IP: %u.%u.%u.%u Port: %d\n", val[0], val[1], val[2], val[3], ntohs(addr.sin_port));

    // ���ó�ʱʱ��
    struct timeval timeout;
    timeout.tv_sec  = 60;  // 60 ��
    timeout.tv_usec = 0;
    setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    // �����ַ���
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
