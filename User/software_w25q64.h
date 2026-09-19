#ifndef __SOFTWARE_W25Q64_H__
#define __SOFTWARE_W25Q64_H__

#include "stm32f10x.h"

// CS 引脚控制（PB12）
#define W25Q64_CS_HIGH()   (GPIOB->BSRR = GPIO_BSRR_BS12)
#define W25Q64_CS_LOW()    (GPIOB->BSRR = GPIO_BSRR_BR12)

void HW_SPI_Init(void);
void W25Q64_Erase(uint32_t addr);
void W25Q64_Write(uint8_t *buf, uint32_t addr, uint16_t len);
void W25Q64_Read(uint8_t *buf, uint32_t addr, uint16_t len);
uint8_t W25Q64_ReadSR(void);
void W25Q64_ReadID(uint8_t *manufacturer, uint8_t *device);

#endif