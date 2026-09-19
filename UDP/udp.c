#include "udp.h"

// 启动UDP模式socket
void UDP_Start(void)
{
    // 1. 获取socket状态
    uint8_t status = getSn_SR(SN);

    // 2. 判断当前状态，执行相应操作，进入下一个状态
    if (status == SOCK_CLOSED)
    {
        // 2.1 如果是关闭状态，就打开socket
        int8_t n = socket(SN, Sn_MR_UDP, 9999, 0);
        if (n == SN)
        {
            printf("socket %d 打开成功！\n", SN);
        }
        else
        {
            printf("socket %d 打开失败，错误码：%d\n", SN, n);
        }
    }
}

// 接收数据
void UDP_RecvData(uint8_t buff[], uint16_t *len, uint8_t *srcIP, uint16_t *srcPort)
{
    // 1. 获取socket状态
    uint8_t status = getSn_SR(SN);

    // 2. 判断如果是UDP状态，就接收数据
    if (status == SOCK_UDP)
    {
        // 根据事件标志位，判断是否接收到数据；如果接收到，就读取到缓冲区
        if (getSn_IR(SN) & Sn_IR_RECV)
        {
            // 清零标志位（写1清0）
            setSn_IR(SN, Sn_IR_RECV);

            // 读取数据长度，包含了8字节的首部
            uint16_t tmp = getSn_RX_RSR(SN);

            // 判断如果tmp > 8，表示接收到数据
            if (tmp > 8)
            {
                *len = tmp - 8;
                // 读取数据
                recvfrom(SN, buff, *len, srcIP, srcPort);
            }
        }
    }
}

// 发送数据
void UDP_SendData(uint8_t data[], uint16_t len, uint8_t *dstIP, uint16_t dstPort)
{
    // 1. 获取socket状态
    uint8_t status = getSn_SR(SN);

    // 2. 判断如果是UDP状态，就发送数据
    if (status == SOCK_UDP)
    {
        sendto(SN, data, len, dstIP, dstPort);
        printf("发送完毕！数据: %.*s\n", len, data);
    }
}
