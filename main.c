#include "config.h"
#include "adc.h"
#include "uart.h"
#include "wave.h"
#include <stdio.h>

extern bit ackRequired;     // 引用 uart.c 中的标志位
extern bit waveUpdateReq;//引用标志位

void Delay_ms(unsigned int ms)   // 如果使用定时器0，Delay 可能需要调整，此处保留不影响
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
    UART_SendString("System Ready. Wave on P1.4\r\n");

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
            UART_SendString("OK\n");
            ackRequired = 0;
        }

        Delay_ms(100);      // 采样率约10Hz
    }
}