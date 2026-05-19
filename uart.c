#include "uart.h"
#include <string.h>
#include <stdlib.h>  
#include "wave.h"

#define RECV_BUF_SIZE 32

// 静态接收缓冲区
static unsigned char recv_buf[RECV_BUF_SIZE];
static unsigned char recv_idx = 0;

// 全局变量定义（与 config.h 中的 extern 对应）
unsigned int Value = 100;   // 默认初始值
bit ackRequired = 0;   			// 修改Value确认信息
bit waveUpdateReq = 0;      // 方波参数更新请求标志

/**
 * @brief 串口初始化
 * @param baud 波特率选择宏
 */
void UART_Init(unsigned int baud)
{
    SCON = 0x50;        // 模式1，8位UART，允许接收
    TMOD &= 0x0F;
    TMOD |= 0x20;       // 定时器1，模式2（8位自动重装）

    // 根据波特率设置定时器初值与 SMOD
    if (baud == BAUD_9600)
    {
        TH1 = 0xFD;
        TL1 = 0xFD;
        PCON &= 0x7F;   // SMOD = 0
    }
    else if (baud == BAUD_19200)
    {
        TH1 = 0xFD;
        TL1 = 0xFD;
        PCON |= 0x80;   // SMOD = 1
    }
    
    else   // 默认 9600
    {
        TH1 = 0xFD;
        TL1 = 0xFD;
        PCON &= 0x7F;
    }

    TR1 = 1;            // 启动定时器1
    ES  = 1;            // 开启串口中断
    EA  = 1;            // 总中断允许
}

//查询方式发送单字节
void UART_SendByte(unsigned char dat)
{
    SBUF = dat;//数据写入寄存器
    while (!TI);//中断标志
    TI = 0;
}

//发送字符串（以 '\0' 结尾）
void UART_SendString(char *str)
{
    while (*str)//循环一次自动往后一位
    {
        UART_SendByte(*str++);
    }
}

// printf 重定向底层函数
char putchar(char c)
{
    UART_SendByte(c); 
    return c;
}

//检测并执行上位机下发的指令
static void CMD_Parse(unsigned char *cmd)
{
    if (strncmp(cmd, "Value=", 6) == 0)//strncmp检查字符是否相同，同则return 0
    {
        Value = atoi(cmd + 6);//atoi字符串转整形
				ackRequired = 1;
    }
    else if (strncmp(cmd, "Freq=", 5) == 0)
    {
        Freq = atoi(cmd + 5);
				waveUpdateReq = 1;// 使用标志位由主循环发送确认
    }
    else if (strncmp(cmd, "Duty=", 5) == 0)
    {
        Duty = atoi(cmd + 5);
				waveUpdateReq = 1;
    }
}

//串口中断服务函数
void UART_ISR(void) interrupt 4
{
    unsigned char ch;
    if (RI)                     // RI中断标志
    {
        RI = 0;
        ch = SBUF;						 //接收串口寄存器数据
        if (ch == '\n')        // 以换行符作为指令结束
        {
            recv_buf[recv_idx] = '\0';//把\n结束符换成\0
            recv_idx = 0;
            CMD_Parse(recv_buf);
        }
        else if (recv_idx < RECV_BUF_SIZE - 1)
        {
            recv_buf[recv_idx++] = ch;
        }
        // 若缓冲区溢出，自动丢弃（可增加错误处理）
    }
    // 发送中断一般不用，但若出现需清除标志
    if (TI)
    {
        // 查询方式发送时不进入中断，为安全可保留空处理
    }
}