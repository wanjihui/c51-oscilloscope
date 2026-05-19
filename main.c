#include "config.h"
#include "adc.h"
#include "uart.h"
#include "wave.h"
#include <stdio.h>

extern bit ackRequired;     // 引用 uart.c 中的标志位
extern bit waveUpdateReq;		//引用标志位

void Delay_ms(unsigned int ms)   //ms延时
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 114; j > 0; j--);
}

void main()
{
    unsigned int adcValue;

    UART_Init(BAUD_9600);
    Wave_Init();            // 启动方波输出

    while (1)
    {
        adcValue = adc_Read();
        printf("%u\n", adcValue);
				if (waveUpdateReq)
				{
						Wave_Update();      
						waveUpdateReq = 0;
				}
				
        if (ackRequired)
        {
						printf("%u\n", Value);     
						// 发送 Value 的数值 + 换行，FireWater会把它当作一个数据点
            ackRequired = 0;
        }

        Delay_ms(5);      // 采样率约200Hz
    }
}