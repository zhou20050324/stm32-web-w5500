#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "web_server.h"


// 引用main.c的全局变量
extern float g_temperature;
extern float g_vrefint;
extern float g_max_temp;

#define ALARM_TEMP 35.0f

uint8_t led_state = 0;       // 0=灭 1=亮
uint8_t html_buf[2048] = {0};

// 库专用收发缓冲区
uint8_t txBuff[2048] = {0};
uint8_t rxBuff[2048] = {0};
uint8_t socketCount = 8;
uint8_t socketList[] = {0,1,2,3,4,5,6,7};
uint8_t *contentName = "index.html";



void generate_web_page(uint8_t *buf)
{  char *alarm_color;
    char *alarm_text;

    if(g_temperature >= ALARM_TEMP)
    {
        alarm_color = "#ff4444";
        alarm_text = "ALARM";
    }
    else
    {
        alarm_color = led_state ? "#7effb2" : "#ff8fa3";
        alarm_text = led_state ? "ON" : "OFF";
    }

    sprintf((char*)buf,
"<!DOCTYPE html>\n"
"<html>\n"
"<head>\n"
"<meta charset=\"UTF-8\">\n"
"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
"<title>My Device</title>\n"
"<style>\n"
"*{margin:0;padding:0;box-sizing:border-box;font-family:Arial;}\n"
"body{background:#ffeff5;display:flex;justify-content:center;align-items:center;min-height:100vh;}\n"
".container{background:#fff;border:3px solid #ffb6d6;padding:30px;border-radius:25px;width:340px;text-align:center;}\n"
".status{padding:10px;border-radius:15px;color:#fff;margin:10px 0;background:%s;}\n"
".adc_box{margin:15px 0;padding:15px;border:2px solid #e5c0ff;border-radius:20px;}\n"
"button{width:100%%;height:45px;border:none;background:#ffd1e7;border-radius:18px;color:#ff7eb9;margin:6px 0;}\n"
"</style>\n"
"</head>\n"
"<body>\n"
"<div class=\"container\">\n"
"<div class=\"status\">%s</div>\n"
"<div class=\"adc_box\">\n"
"<p>Temp: %.1f C</p>\n"
"<p>Vref: %.2f V</p>\n"
"<p>Max Temp: %.1f C</p>\n"
"</div>\n"
"<a href=\"?action=1\"><button>LED ON</button></a>\n"
"<a href=\"?action=2\"><button>LED OFF</button></a>\n"
"<a href=\"?action=3\"><button>TOGGLE</button></a>\n"
"</div>\n"

// ✅ 保留：清空URL参数 → LED点击灵敏
// ✅ 保留：1秒自动刷新 → 数据实时更新
"<script>\n"
"history.replaceState({}, document.title, window.location.pathname);\n"
"setTimeout(function(){window.location.reload();},1000);\n"
"</script>\n"

"</body>\n"
"</html>",
alarm_color, alarm_text, g_temperature, g_vrefint, g_max_temp);
}

void WebServer_Init(void)
{
    LED_Init();
    httpServer_init(txBuff, rxBuff, socketCount, socketList);
}

void WebServer_Start(void)
{
	
        generate_web_page(html_buf);
        reg_httpServer_webContent(contentName, html_buf);
    

    // 只运行服务器，不重复注册，速度拉满
    for(uint8_t i = 0; i < sizeof(socketList); i++)
    {
        httpServer_run(i);
    }
}

void handler_user_function(uint8_t *url)
{
    uint8_t action = 0;
    char* pAction = strstr((char *)url, "action=");
    if(pAction != NULL) 
    {
        action = *(pAction+7);
        switch(action)
        {
            case '1': LED_On();  led_state = 1; break;
            case '2': LED_Off(); led_state = 0; break;
            case '3': LED_Toggle(); led_state = !led_state; break;
            default: break;
        }
           
    }

    
}