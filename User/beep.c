#include "beep.h"

void BEEP_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    // 配置PA11为推挽输出 50MHz
    GPIOA->CRH &= ~(GPIO_CRH_CNF11);
    GPIOA->CRH |= GPIO_CRH_MODE11;
    BEEP_OFF();
}