#include "adc1.h"

void ADC1_Init(void)
{
    // 1. 时钟配置
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->CFGR |= RCC_CFGR_ADCPRE_1;
    RCC->CFGR &= ~RCC_CFGR_ADCPRE_0;

    // 2. 开启扫描模式（双通道必须开）
    ADC1->CR1 |= ADC_CR1_SCAN;
    // 连续转换模式
    ADC1->CR2 |= ADC_CR2_CONT;
    // 数据右对齐
    ADC1->CR2 &= ~ADC_CR2_ALIGN;

    // ✅ 修复：关闭外部触发，使用软件启动（必须加！）
    ADC1->CR2 &= ~ADC_CR2_EXTTRIG;
    ADC1->CR2 |= ADC_CR2_EXTSEL;

    // 3. 开启内部温度传感器 + 内部参考电压
    ADC1->CR2 |= ADC_CR2_TSVREFE;

    // 4. 内部通道采样时间
    ADC1->SMPR1 |= (7 << 18);  // CH16温度
    ADC1->SMPR1 |= (7 << 21);  // CH17参考电压

    // 5. 规则组：2个通道
    ADC1->SQR1 &= ~ADC_SQR1_L;
    ADC1->SQR1 |= ADC_SQR1_L_0;

    // 6. 规则序列：内部通道
    ADC1->SQR3 &= ~ADC_SQR3_SQ1;
    ADC1->SQR3 |= 16 << 0;     // SQ1=温度
    ADC1->SQR3 &= ~ADC_SQR3_SQ2;
    ADC1->SQR3 |= 17 << 5;     // SQ2=电压
}

void ADC1_DMA_Init(void)
{
    // 1. 开启DMA模块时钟
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    // 2. 传输方向：外设->内存
    DMA1_Channel1->CCR &= ~DMA_CCR1_DIR;

    // 3. 数据宽度：16位
    DMA1_Channel1->CCR &= ~DMA_CCR1_PSIZE_1;
    DMA1_Channel1->CCR |= DMA_CCR1_PSIZE_0;
    DMA1_Channel1->CCR &= ~DMA_CCR1_MSIZE_1;
    DMA1_Channel1->CCR |= DMA_CCR1_MSIZE_0;

    // 4. 地址自增
    DMA1_Channel1->CCR &= ~DMA_CCR1_PINC;
    DMA1_Channel1->CCR |= DMA_CCR1_MINC;

    // 5. 循环模式
    DMA1_Channel1->CCR |= DMA_CCR1_CIRC;

    // 6. ADC开启DMA
    ADC1->CR2 |= ADC_CR2_DMA;
}

// ✅ 核心修复：删除死循环，程序不再卡死
void ADC1_DMA_StartConvert(uint32_t destAddr, uint8_t len)
{
    DMA1_Channel1->CPAR = (uint32_t)&(ADC1->DR);
    DMA1_Channel1->CMAR = destAddr;
    DMA1_Channel1->CNDTR = len;
    DMA1_Channel1->CCR |= DMA_CCR1_EN;

    // ADC上电
    ADC1->CR2 |= ADC_CR2_ADON;
    Delay_ms(1);  // 延时等待上电稳定

    // 校准
    ADC1->CR2 |= ADC_CR2_CAL;
    while (ADC1->CR2 & ADC_CR2_CAL){}

    // 启动转换
    ADC1->CR2 |= ADC_CR2_ADON;

    // ❌ 删除这行死循环！！
    // while ((ADC1->SR & ADC_SR_EOC) == 0){}
}