#include "Delay.h"

void Delay_us(uint32_t us)
{

	// 1. 配置SysTick时钟源 = AHB 72MHz (SysTick_CTRL_CLKSOURCE位)
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE;
    // 2. 设置重装载值: 72MHz = 72次计数=1us → 计数值=72*us -1
    SysTick->LOAD = 72 * us - 1;
    // 3. 清空当前计数值
    SysTick->VAL = 0;
    // 4. 使能SysTick定时器 (bit0=1)
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

    // 等待计数完成 (CTRL位16=1表示计数到0)
    while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));

    // 关闭定时器
    SysTick->CTRL = 0;


}
void Delay_ms(uint32_t ms)
{

	while(ms--)
    {
        Delay_us(1000); // 1ms = 1000us
    }


}
void Delay_s(uint32_t s)
{

	while(s--)
    {
        Delay_ms(1000); // 1s=1000ms
    }

}