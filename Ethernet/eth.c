#include "eth.h"
#include "Delay.h"


uint8_t ip[4] = {192, 168, 1, 100};
uint8_t mac[6] = {0x00, 0x08, 0xDC, 0x11, 0x22, 0x33};
uint8_t submask[4] = {255, 255, 255, 0};
uint8_t gateway[4] = {192, 168, 1, 1};

// 复位W5500
static void ETH_Reset(void);
// 设置 MAC 地址
static void ETH_SetMac(void);
// 设置 IP 地址（包括网关和子网掩码）
static void ETH_SetIP(void);

void ETH_Init(void)
{
    // 0. SPI 初始化
    SPI_Init();

    // 1. 注册自定义回调函数
    user_register_function();

    // 2. 复位W5500
    ETH_Reset();

    // 3. 设置 MAC 地址
    ETH_SetMac();

    // 4. 设置 IP 地址（包括网关和子网掩码）
    ETH_SetIP();
}


// 复位W5500
static void ETH_Reset(void)
{
    // 1. 配置RST引脚-PA1
    // 1.1 开启时钟
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // 1.2 设置工作模式：通用推挽输出，MODE - 11，CNF - 00
    GPIOA->CRL |= GPIO_CRL_MODE1;
    GPIOA->CRL &= ~GPIO_CRL_CNF1;

    // 2. 拉低RST，保持500us以上
    GPIOA->ODR &= ~GPIO_ODR_ODR1;

    Delay_us(800);

    GPIOA->ODR |= GPIO_ODR_ODR1;

    printf("W5500 复位完成！\n");
}

// 设置 MAC 地址
static void ETH_SetMac(void)
{
    printf("ETH_SetMac...\n");
    setSHAR(mac);
    printf("ETH_SetMac done! %X-%X-%X-%X-%X-%X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

// 设置 IP 地址（包括网关和子网掩码）
static void ETH_SetIP(void)
{
    printf("ETH_SetIP...\n");

    // 设置IP
    setSIPR(ip);
    // 设置子网掩码
    setSUBR(submask);
    // 设置网关地址
    setGAR(gateway);

    printf("ETH_SetIP done! %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
}

