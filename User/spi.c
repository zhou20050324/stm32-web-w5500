#include "spi.h"

void SPI_Init(void)
{
    // 1. 开启时钟
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    //cs引脚配置
    GPIOC->CRH |= GPIO_CRH_MODE13;
    GPIOC->CRH &= ~GPIO_CRH_CNF13;
    // sck
    GPIOA->CRL |= GPIO_CRL_MODE5;
    GPIOA->CRL &= ~GPIO_CRL_CNF5;
    GPIOA->CRL |= GPIO_CRL_CNF5_1;
    // mosi
    GPIOA->CRL |= GPIO_CRL_MODE7;
    GPIOA->CRL &= ~GPIO_CRL_CNF7;
    GPIOA->CRL |= GPIO_CRL_CNF7_1;

    // PA6：MISO，浮空输入，CNF = 01，MODE = 00
    GPIOA->CRL &= ~GPIO_CRL_MODE6;
    GPIOA->CRL &= ~GPIO_CRL_CNF6_1;
    GPIOA->CRL |= GPIO_CRL_CNF6_0;

   // 2. 配置SPI1
    SPI1->CR1 |= SPI_CR1_MSTR;
    SPI1->CR1 |= SPI_CR1_SSM;
    SPI1->CR1 |= SPI_CR1_SSI;

    SPI1->CR1 &= ~SPI_CR1_CPOL;
    SPI1->CR1 &= ~SPI_CR1_CPHA;

    SPI1->CR1 &= ~SPI_CR1_BR;
    SPI1->CR1 |= SPI_CR1_BR_0;

    // 3.5 设置数据帧格式
    SPI1->CR1 &= ~SPI_CR1_DFF;

    // 3.6 配置高位先行MSB
    SPI1->CR1 &= ~SPI_CR1_LSBFIRST;

    // 3.7 SPI模块使能
    SPI1->CR1 |= SPI_CR1_SPE;


  
    
}

// 数据传输的开始和结束
void SPI_Start(void)
{
    CS_LOW;
}
void SPI_Stop(void)
{
    CS_HIGH;
}

// 主从设备交换一个字节的数据
uint8_t SPI_SwapByte(uint8_t byte)
{
    while((SPI1->SR & SPI_SR_TXE) ==0);

    SPI1->DR = byte;

    while ((SPI1->SR & SPI_SR_RXNE) ==0);

    return (uint8_t)(SPI1->DR & 0xff);

    
    
    
}
