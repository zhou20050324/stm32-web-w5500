#ifndef __ADC1_H__
#define __ADC1_H__
#include "stm32f10x.h"

void ADC1_Init(void);
void ADC1_DMA_StartConvert(uint32_t destAddr, uint8_t len);
void ADC1_DMA_Init(void);



#endif 
