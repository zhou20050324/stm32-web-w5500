#ifndef __BKP_H__
#define __BKP_H__

#include "stm32f10x.h"

void BKP_Init(void);
void BKP_WriteData(uint16_t data);
uint16_t BKP_ReadData(void);
void BKP_Tamper_Init(void);


#endif