#ifndef __UART_H__
#define __UART_H__

#include "config.h"

// 支持的波特率宏（11.0592MHz 下的配置）
#define BAUD_9600   9600
#define BAUD_19200  19200

void UART_Init(unsigned int baud);
void UART_SendByte(unsigned char dat);
void UART_SendString(char *str);
char putchar(char c);    // printf 重定向

extern bit waveUpdateReq;   // 方波参数更新请求标志

#endif