#include "led.h"

void LED_Init(void)
{
    // 1. 使能 GPIOB 时钟（官方宏：RCC_APB2ENR_IOPBEN）
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;  

    // 2. 配置 PB11：清零配置位 + 设置为50MHz通用推挽输出
    // 官方宏：GPIO_CRH_CNF11 + GPIO_CRH_MODE11 对应PB11的配置位
    GPIOB->CRH &= ~(GPIO_CRH_CNF11 | GPIO_CRH_MODE11); 
    // 配置模式：50MHz推挽输出（MODE11置1，CNF11置0）
    GPIOB->CRH |= GPIO_CRH_MODE11; 
    
    GPIOB->BSRR = GPIO_BSRR_BS11;// 初始化时关闭LED
}


void LED_On(void)
{
     GPIOB->BSRR = GPIO_BSRR_BR11;
}

void LED_Off(void)
{
     GPIOB->BSRR = GPIO_BSRR_BS11;
}

void LED_Toggle(void)
{
     GPIOB->ODR ^= GPIO_ODR_ODR11;
}
