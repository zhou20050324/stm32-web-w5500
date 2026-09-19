#include "stm32f10x.h" 
#include "usart1.h"
#include "adc1.h"
#include "Delay.h"
#include "can.h"
#include <string.h>
#include "eth.h"
#include "tcp.h"
#include "udp.h"
#include "web_server.h"
#include "software_w25q64.h"   // 现在是硬件 SPI2 版本
#include "beep.h"

uint16_t adc_raw_data[2] = {0};
float g_temperature = 0.0f;
float g_vrefint = 0.0f;
float g_max_temp = 0.0f;

#define MAX_TEMP_ADDR     0x7F000
#define ALARM_TEMP        35.0f
#define DEFAULT_MAX_TEMP  25.0f

/*----------------- 保存最高温度（无需关中断）-----------------*/
void Save_MaxTemp(void)
{
    uint8_t buf[4];
    memcpy(buf, &g_max_temp, 4);

    // 直接调用硬件 SPI 操作，完全不用关中断
    W25Q64_Erase(MAX_TEMP_ADDR);
    Delay_ms(5);   // 等待擦除完成
    W25Q64_Write(buf, MAX_TEMP_ADDR, 4);

    // 回读校验
    uint8_t check[4];
    float test;
    W25Q64_Read(check, MAX_TEMP_ADDR, 4);
    memcpy(&test, check, 4);
    if (test != g_max_temp)
    {
        // 重试一次
        W25Q64_Erase(MAX_TEMP_ADDR);
        Delay_ms(5);
        W25Q64_Write(buf, MAX_TEMP_ADDR, 4);
    }
}

/*----------------- 读取最高温度 -----------------*/
void Read_MaxTemp(void)
{
    uint8_t buf[4];
    W25Q64_Read(buf, MAX_TEMP_ADDR, 4);
    memcpy(&g_max_temp, buf, 4);
}




/*----------------- 主函数 -----------------*/
int main(void)
{
    USART_Init();
    BEEP_Init();
    HW_SPI_Init();          // 初始化硬件 SPI2（替换原 SOFTWARE_W25Q64_Init）

    uint8_t sr = W25Q64_ReadSR();
    printf("Status Register: 0x%02X\r\n", sr);

    uint8_t manufacturer, device;
    W25Q64_ReadID(&manufacturer, &device);
    printf("Manufacturer ID: 0x%02X, Device ID: 0x%02X\r\n", manufacturer, device);

    ETH_Init();
    WebServer_Init();

    ADC1_Init();
    ADC1_DMA_Init();
    ADC1_DMA_StartConvert((uint32_t)adc_raw_data, 2);
    Delay_ms(500);

    // 读取历史最高温度
    uint8_t buf[4];
    W25Q64_Read(buf, MAX_TEMP_ADDR, 4);

    uint8_t all_0xFF = 1, all_0x00 = 1;
    for (int i = 0; i < 4; i++)
    {
        if (buf[i] != 0xFF) all_0xFF = 0;
        if (buf[i] != 0x00) all_0x00 = 0;
    }

    if (all_0xFF || all_0x00)
    {
        g_max_temp = DEFAULT_MAX_TEMP;
        Save_MaxTemp();
        printf("Flash empty, set default max temp: %.1f C\r\n", g_max_temp);
    }
    else
    {
        memcpy(&g_max_temp, buf, 4);
        if (g_max_temp < -40.0f || g_max_temp > 125.0f || (g_max_temp != g_max_temp))
        {
            g_max_temp = DEFAULT_MAX_TEMP;
            Save_MaxTemp();
            printf("Invalid max temp, reset to %.1f C\r\n", g_max_temp);
        }
        else
        {
            printf("Read max temp from Flash: %.1f C\r\n", g_max_temp);
        }
    }

    while (1)
    {
        g_vrefint = (float)adc_raw_data[1] * 3.3f / 4095.0f;
        float v_temp = (float)adc_raw_data[0] * 3.3f / 4095.0f;
        g_temperature = (1.43f - v_temp) / 0.0043f + 25.0f;

        if (g_temperature > g_max_temp + 0.5f)
        {
            g_max_temp = g_temperature;
            Save_MaxTemp();
            printf("Max temp updated: %.1f C\r\n", g_max_temp);
        }

        if (g_temperature >= ALARM_TEMP)
            BEEP_ON();
        else
            BEEP_OFF();

        WebServer_Start();
    }
}