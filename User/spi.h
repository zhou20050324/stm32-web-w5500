#ifndef __SPI_H__
#define __SPI_H__
#include "stm32f10x.h"


#define CS_HIGH (GPIOC->ODR |= GPIO_ODR_ODR13)
#define CS_LOW (GPIOC->ODR &= ~GPIO_ODR_ODR13)


void SPI_Init(void);

void SPI_Start(void);
void SPI_Stop(void);

uint8_t SPI_SwapByte(uint8_t byte);

#endif

