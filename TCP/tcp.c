#include "tcp.h"

uint8_t clientIP[4];
uint16_t clientPort;


void TCP_ServerStart(void)
{
    // 1. 获取socket状态
    uint8_t status = getSn_SR(SN);

    // 2. 判断当前状态，执行相应操作，进入下一个状态
    if (status == SOCK_CLOSED)
    {
        // 2.1 如果是关闭状态，就打开socket
        int8_t n = socket(SN, Sn_MR_TCP, 8080, SF_TCP_NODELAY);
        if (n == SN)
        {
            printf("socket %d 打开成功！\n", SN);
        }
        else
        {
            printf("socket %d 打开失败，错误码：%d\n", SN, n);
        }
        // isSaved = 0;
    }
    else if (status == SOCK_INIT)
    {
        // 2.2 如果是INIT状态，就执行监听，启动服务端
        int8_t res = listen(SN);
        if (res == SOCK_OK)
        {
            printf("socket %d 监听成功！\n", SN);
        }
        else
        {
            printf("socket %d 监听失败，错误码：%d\n", SN, res);
        }
        // isSaved = 0;
    }
    // else if (status == SOCK_LISTEN)
    // {
    // }
    else if (status == SOCK_ESTABLISHED)
    {
        if ( getSn_IR(SN) & Sn_IR_CON )
        {
            getSn_DIPR(SN, clientIP);
            clientPort = getSn_DPORT(SN);
            
            printf("客户端连接成功，IP：%d.%d.%d.%d, Port: %d\n", 
                clientIP[0], clientIP[1], clientIP[2], clientIP[3], clientPort);

            // 清零标志位（写1清0）
            setSn_IR(SN, Sn_IR_CON);
        } 
    }
    else if (status == SOCK_CLOSE_WAIT)
    {
        // 2.3 如果是半关闭状态，就直接关闭socket
        printf("失去客户端的连接，准备关闭socket并重新打开...\n");
        close(SN);

        // isSaved = 0;
    }
}


uint8_t serverIP[4] = {192, 168, 1, 10};
uint16_t serverPort = 8888;

void TCP_ClientStart(void)
{
    // 1. 获取socket状态
    uint8_t status = getSn_SR(SN);

    // 2. 判断当前状态，执行相应操作，进入下一个状态
    if (status == SOCK_CLOSED)
    {
        // 2.1 如果是关闭状态，就打开socket
        int8_t n = socket(SN, Sn_MR_TCP, 9999, SF_TCP_NODELAY);
        if (n == SN)
        {
            printf("socket %d 打开成功！\n", SN);
        }
        else
        {
            printf("socket %d 打开失败，错误码：%d\n", SN, n);
        }
    }
    else if (status == SOCK_INIT)
    {
        // 2.2 如果是INIT状态，就连接服务器
        int8_t res = connect(SN, serverIP, serverPort);
        if (res == SOCK_OK)
        {
            printf("客户端连接服务器成功！\n");

            // 向服务器发送初始消息
            TCP_SendData("Hello, this is STM32 TCP Client!", 32);
        }
        else
        {
            printf("客户端连接服务器失败！错误码: %d\n", res);
        }
    }
    // else if (status == SOCK_ESTABLISHED)
    // {
    // }
    else if (status == SOCK_CLOSE_WAIT)
    {
        // 2.3 如果是半关闭状态，就直接关闭socket
        printf("失去服务端的连接，准备关闭socket并重新打开...\n");
        close(SN);
    }
}

void TCP_RecvData(uint8_t buff[], uint16_t *len)
{
    // 1. 获取socket状态
    uint8_t status = getSn_SR(SN);

    // 2. 判断如果是建立连接状态，就接收数据
    if (status == SOCK_ESTABLISHED)
    {
        // // 判断是否保存过，如果没有就从寄存器中提取连接的客户端IP和端口号
        // if (isSaved == 0 && ROLE == SERVER)
        // {
        //     getSn_DIPR(SN, clientIP);
        //     clientPort = getSn_DPORT(SN);
            
        //     printf("客户端连接成功，IP：%d.%d.%d.%d, Port: %d\n", 
        //         clientIP[0], clientIP[1], clientIP[2], clientIP[3], clientPort);

        //     isSaved = 1;
        // } 

        // 根据事件标志位，判断是否接收到数据；如果接收到，就读取到缓冲区
        if (getSn_IR(SN) & Sn_IR_RECV)
        {
            // 清零标志位（写1清0）
            setSn_IR(SN, Sn_IR_RECV);

            // 读取数据长度
            *len = getSn_RX_RSR(SN);
            // 读取数据
            recv(SN, buff, *len);
        }
    }
}

// 发送数据
void TCP_SendData(uint8_t data[], uint16_t len)
{
    // 1. 获取socket状态
    uint8_t status = getSn_SR(SN);

    // 2. 判断如果是建立连接状态，就接收数据
    if (status == SOCK_ESTABLISHED)
    {
        send(SN, data, len);
    }
}

