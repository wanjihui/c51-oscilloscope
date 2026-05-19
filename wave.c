#include "wave.h"
#include <reg52.h>

// 默认值
unsigned int Freq = 100;    // 100Hz
unsigned char Duty = 50;    // 50%

// 定时器0产生 100us 时基，晶振 11.0592MHz，模式1
#define T0_RELOAD  (65536 - 92)   // 100us @ 12T, 近似值
                                  // 100us * 12 = 1.2MHz, 11.0592M/12=921.6k, 65536-92=65444≈100us

static unsigned int high_ticks = 0;  // 高电平计数值
static unsigned int low_ticks  = 0;  // 低电平计数值
static unsigned int tick_count = 0;  // 当前计数
static bit wave_state = 1;           // 当前输出电平

/**
 * @brief 重新计算高/低电平计数值
 */
void Wave_Update(void)
{
    unsigned int total_ticks;
    if (Freq == 0) Freq = 1;                    // 避免除零
    if (Duty > 100) Duty = 100;
    // 总时基个数 = (1000000us / Freq) / 100us = 10000 / Freq
    total_ticks = 10000 / Freq;                 // 一个周期需要的100us时基个数
    if (total_ticks < 2) total_ticks = 2;
    high_ticks = (unsigned int)((unsigned long)total_ticks * Duty / 100);
    if (high_ticks == 0) high_ticks = 1;
    low_ticks = total_ticks - high_ticks;
    if (low_ticks == 0) low_ticks = 1;
    tick_count = 0;
    wave_state = 1;
    WAVE_OUT = 1;
}

/**
 * @brief 定时器0初始化及启动
 */
void Wave_Init(void)
{
    TMOD &= 0xF0;           // 清除定时器0配置
    TMOD |= 0x01;           // 模式1 (16位)
    TH0 = (T0_RELOAD >> 8) & 0xFF;
    TL0 = T0_RELOAD & 0xFF;
    ET0 = 1;                // 允许定时器0中断
    TR0 = 1;                // 启动定时器0
    Wave_Update();
}

/**
 * @brief 定时器0中断服务程序
 */
void Timer0_ISR(void) interrupt 1
{
    // 重装初值
    TH0 = (T0_RELOAD >> 8) & 0xFF;
    TL0 = T0_RELOAD & 0xFF;

    tick_count++;
    if (wave_state)  // 高电平期间
    {
        if (tick_count >= high_ticks)
        {
            wave_state = 0;
            WAVE_OUT = 0;      // 转为低电平
            tick_count = 0;
        }
    }
    else             // 低电平期间
    {
        if (tick_count >= low_ticks)
        {
            wave_state = 1;
            WAVE_OUT = 1;      // 转为高电平
            tick_count = 0;
        }
    }
}