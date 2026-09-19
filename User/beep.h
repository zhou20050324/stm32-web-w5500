#ifndef __BEEP_H__
#define __BEEP_H__

#include "stm32f10x.h"
void BEEP_Init(void);

#define BEEP_ON()  (GPIOA->BSRR = GPIO_BSRR_BR11)
#define BEEP_OFF() (GPIOA->BSRR = GPIO_BSRR_BS11)


#endif