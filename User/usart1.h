/*
 * @Author: wushengran
 * @Date: 2024-05-28 15:30:14
 * @Description: 
 * 
 * Copyright (c) 2024 by atguigu, All Rights Reserved. 
 */
#ifndef __USART1_H
#define __USART1_H

#include "stm32f10x.h"
#include <stdio.h>

// ��ʼ��
void USART_Init(void);

// ����һ���ַ�
void USART_SendChar(uint8_t ch);

// ����һ���ַ�
uint8_t USART_ReceiveChar(void);

// �����ַ���
void USART_SendString(uint8_t *str, uint8_t size);

// �����ַ���
void USART_ReceiveString(uint8_t buffer[], uint8_t *size);

#endif
