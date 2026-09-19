#include "software_w25q64.h"
#include "Delay.h"

/*----------------- 硬件 SPI2 初始化（寄存器版）-----------------*/
void HW_SPI_Init(void)
{
    // 1. 使能 GPIOB 和 SPI2 时钟
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;   // SPI2 在 APB1 总线

    // 2. 配置 GPIO
    // PB12 (CS) 推挽输出，软件控制
    GPIOB->CRH &= ~GPIO_CRH_CNF12;
    GPIOB->CRH |= GPIO_CRH_MODE12;
    W25Q64_CS_HIGH();

    // PB13 (SCK) 复用推挽输出
    GPIOB->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
    GPIOB->CRH |= GPIO_CRH_MODE13 | GPIO_CRH_CNF13_1;   // 50MHz AF PP

    // PB15 (MOSI) 复用推挽输出
    GPIOB->CRH &= ~(GPIO_CRH_MODE15 | GPIO_CRH_CNF15);
    GPIOB->CRH |= GPIO_CRH_MODE15 | GPIO_CRH_CNF15_1;

    // PB14 (MISO) 上拉输入
    GPIOB->CRH &= ~(GPIO_CRH_MODE14 | GPIO_CRH_CNF14);
    GPIOB->CRH |= GPIO_CRH_CNF14_1;
    GPIOB->BSRR = GPIO_BSRR_BS14;

     SPI2->CR1 = 0;
    SPI2->CR2 = 0;   // 关闭所有中断/DMA
    // 显式清除 CPOL, CPHA（模式 0）
    SPI2->CR1 = SPI_CR1_MSTR
              | SPI_CR1_SSM
              | SPI_CR1_SSI
              | SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2;  // fPCLK/256
    SPI2->CR1 |= SPI_CR1_SPE;

    Delay_ms(10);    // 等待 Flash 完全上电
}

/*----------------- 硬件 SPI 字节收发 -----------------*/
static uint8_t SPI2_Transfer(uint8_t data)
{
    while (!(SPI2->SR & SPI_SR_TXE));   // 等待发送缓冲区空
    SPI2->DR = data;
    while (!(SPI2->SR & SPI_SR_RXNE));  // 等待接收完成
    return SPI2->DR;
}

/*----------------- 读状态寄存器 -----------------*/
uint8_t W25Q64_ReadSR(void)
{
    uint8_t res;
    W25Q64_CS_LOW();
    SPI2_Transfer(0x05);
    res = SPI2_Transfer(0xFF);
    W25Q64_CS_HIGH();
    return res;
}

/*----------------- 等待忙结束（带超时） -----------------*/
static void W25Q64_WaitBusy(void)
{
    uint16_t timeout = 30000;   // 约 300ms
    while (W25Q64_ReadSR() & 0x01)
    {
        if (--timeout == 0) break;
        Delay_us(10);
    }
}

/*----------------- 写使能 -----------------*/
static void W25Q64_WriteEnable(void)
{
    W25Q64_CS_LOW();
    SPI2_Transfer(0x06);
    W25Q64_CS_HIGH();
}

/*----------------- 扇区擦除（地址为字节地址） -----------------*/
void W25Q64_Erase(uint32_t addr)
{
    W25Q64_WriteEnable();
    W25Q64_WaitBusy();
    W25Q64_CS_LOW();
    SPI2_Transfer(0x20);
    SPI2_Transfer((addr >> 16) & 0xFF);
    SPI2_Transfer((addr >> 8) & 0xFF);
    SPI2_Transfer(addr & 0xFF);
    W25Q64_CS_HIGH();
    W25Q64_WaitBusy();
}


void W25Q64_ReadID(uint8_t *manufacturer, uint8_t *device)
{
    W25Q64_CS_LOW();
    SPI2_Transfer(0x90);            // 读 ID 命令
    SPI2_Transfer(0x00);            // 地址 0
    SPI2_Transfer(0x00);
    SPI2_Transfer(0x00);
    *manufacturer = SPI2_Transfer(0xFF);  // 制造商 ID 应为 0xEF
    *device = SPI2_Transfer(0xFF);        // 设备 ID  应为 0x16 (W25Q64)
    W25Q64_CS_HIGH();
}

/*----------------- 页写入 -----------------*/
void W25Q64_Write(uint8_t *buf, uint32_t addr, uint16_t len)
{
    W25Q64_WriteEnable();
    W25Q64_CS_LOW();
    SPI2_Transfer(0x02);
    SPI2_Transfer((addr >> 16) & 0xFF);
    SPI2_Transfer((addr >> 8) & 0xFF);
    SPI2_Transfer(addr & 0xFF);
    for (uint16_t i = 0; i < len; i++)
        SPI2_Transfer(buf[i]);
    W25Q64_CS_HIGH();
    W25Q64_WaitBusy();
}

/*----------------- 读数据 -----------------*/
void W25Q64_Read(uint8_t *buf, uint32_t addr, uint16_t len)
{
    W25Q64_CS_LOW();
    SPI2_Transfer(0x03);
    SPI2_Transfer((addr >> 16) & 0xFF);
    SPI2_Transfer((addr >> 8) & 0xFF);
    SPI2_Transfer(addr & 0xFF);
    for (uint16_t i = 0; i < len; i++)
        buf[i] = SPI2_Transfer(0xFF);
    W25Q64_CS_HIGH();
}