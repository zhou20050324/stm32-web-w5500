#ifndef __CAN_H
#define __CAN_H

#include "stm32f10x.h"

// 定义结构体，保存接收到的报文信息
typedef struct
{
    uint16_t stdID;
    uint8_t data[8];
    uint8_t len;
} RxMsg;

// 初始化
void CAN_Init(void);

// 发送报文
void CAN_SendMsg(uint16_t stdID, uint8_t * data, uint8_t len);

// 接收报文
void CAN_ReceiveMsg(RxMsg rxMsg[], uint8_t * msgCount);

#endif
