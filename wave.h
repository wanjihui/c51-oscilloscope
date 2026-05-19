#ifndef __WAVE_H__
#define __WAVE_H__

#include "config.h"

// 方波输出引脚定义（可根据需要修改）
sbit WAVE_OUT = P1^4;

// 可调参数（全局变量，上位机可修改）
extern unsigned int Freq;   // 频率 Hz
extern unsigned char Duty;  // 占空比 0-100

void Wave_Init(void);       // 初始化定时器0和参数
void Wave_Update(void);     // 当Freq或Duty改变后调用，重新计算定时参数

#endif