#include "w25q64.h"


// 初始化
void W25Q64_Init(void)
{
    SPI_Init();
}

// 读取ID
void W25Q64_ReadID(uint8_t *mid, uint16_t *did)
{
    SPI_Start();

    // 1. 发送指令 9fh
    SPI_SwapByte(0x9f);

    // 2. 获取制造商ID（为了读取数据，发送什么不重要）
    *mid = SPI_SwapByte(0xff);

    // 3. 获取设备ID
    *did = 0;
    *did |= SPI_SwapByte(0xff) << 8;
    *did |= SPI_SwapByte(0xff) & 0xff;

    SPI_Stop();
}

// 开启写使能
void W25Q64_WriteEnable(void)
{
    SPI_Start();
    SPI_SwapByte(0x06);
    SPI_Stop();
}

// 关闭写使能
void W25Q64_WriteDisable(void)
{
    SPI_Start();
    SPI_SwapByte(0x04);
    SPI_Stop();
}

// 等待状态不为忙（busy）
void W25Q64_WaitNotBusy(void)
{
    SPI_Start();

    // 发送读取状态寄存器指令
    SPI_SwapByte(0x05);

    // 等待收到的数据末位变成0
    while (SPI_SwapByte(0xff) & 0x01)
    {
    }

    SPI_Stop();
}

// 擦除段（sector erase）,地址只需要块号和段号
void W25Q64_EraseSector(uint8_t block, uint8_t sector)
{
    // 首先等待状态不为忙
    W25Q64_WaitNotBusy();

    // 开启写使能
    W25Q64_WriteEnable();

    // 计算要发送的地址（段首地址）
    uint32_t addr = (block << 16) + (sector << 12);

    SPI_Start();

    // 发送指令
    SPI_SwapByte(0x20);

    SPI_SwapByte(addr >> 16 & 0xff); // 第一个字节
    SPI_SwapByte(addr >> 8 & 0xff);  // 第二个字节
    SPI_SwapByte(addr >> 0 & 0xff);  // 第三个字节

    SPI_Stop();

    W25Q64_WriteDisable();
}

// 写入（页写）
void W25Q64_PageWrite(uint8_t block, uint8_t sector, uint8_t page, uint8_t *data, uint16_t len)
{
    // 首先等待状态不为忙
    W25Q64_WaitNotBusy();

    // 开启写使能
    W25Q64_WriteEnable();

    // 计算要发送的地址（页首地址）
    uint32_t addr = (block << 16) + (sector << 12) + (page << 8);

    SPI_Start();

    // 发送指令
    SPI_SwapByte(0x02);

    // 发送24位地址
    SPI_SwapByte(addr >> 16 & 0xff); // 第一个字节
    SPI_SwapByte(addr >> 8 & 0xff);  // 第二个字节
    SPI_SwapByte(addr >> 0 & 0xff);  // 第三个字节

    //  依次发送数据
    for (uint16_t i = 0; i < len; i++)
    {
        SPI_SwapByte(data[i]);
    }

    SPI_Stop();

    W25Q64_WriteDisable();
}

// 读取
void W25Q64_Read(uint8_t block, uint8_t sector, uint8_t page, uint8_t innerAddr, uint8_t *buffer, uint16_t len)
{
    // 首先等待状态不为忙
    W25Q64_WaitNotBusy();

    // 计算要发送的地址
    uint32_t addr = (block << 16) + (sector << 12) + (page << 8) + innerAddr;

    SPI_Start();

    // 发送指令
    SPI_SwapByte(0x03);

    // 发送24位地址
    SPI_SwapByte(addr >> 16 & 0xff); // 第一个字节
    SPI_SwapByte(addr >> 8 & 0xff);  // 第二个字节
    SPI_SwapByte(addr >> 0 & 0xff);  // 第三个字节

    //  依次读取数据
    for (uint16_t i = 0; i < len; i++)
    {
        buffer[i] = SPI_SwapByte(0xff);
    }

    SPI_Stop();
}