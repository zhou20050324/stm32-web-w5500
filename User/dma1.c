#include "dma1.h"

void DMA1_Init(void)
{
    // 1. 配置时钟
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;

  
     DMA1_Channel4->CCR |= DMA_CCR4_DIR;

    DMA1_Channel4->CCR &=~DMA_CCR4_PSIZE;
    DMA1_Channel4->CCR &=~DMA_CCR4_MSIZE;
    
    DMA1_Channel4->CCR |= DMA_CCR4_MINC;
    DMA1_Channel4->CCR &= ~DMA_CCR4_PINC;

    DMA1_Channel4->CCR |= DMA_CCR4_TCIE;

    DMA1_Channel4->CCR |=DMA_CCR4_CIRC;

    USART1->CR3 |= USART_CR3_DMAT;

    NVIC_SetPriorityGrouping(3);
    NVIC_SetPriority(DMA1_Channel4_IRQn, 0);
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);

}

void DMA1_Transmit(uint32_t src, uint32_t dst, uint16_t dataLen)
{
   DMA1_Channel4->CPAR=dst;
   DMA1_Channel4->CMAR=src;
   DMA1_Channel4->CNDTR=dataLen;
   
   DMA1_Channel4->CCR |= DMA_CCR4_EN;
}

void DMA1_Channel4_IRQHandler(void)
{
    if(DMA1->ISR & DMA_ISR_TCIF4)
    {
        DMA1->IFCR |= DMA_IFCR_CTCIF4;
        // 清除中断标志位
        DMA1_Channel4->CCR &= ~DMA_CCR4_EN;



        
    }
}
    