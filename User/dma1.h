#ifndef __DAM1_H
#define __DAM1_H
#include "stm32f10x.h"

void DMA1_Init(void);
extern uint8_t isfinished;
void DMA1_Transmit(uint32_t src, uint32_t dst, uint16_t dataLen);

#endif 