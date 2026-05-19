#ifndef __WAVE_H__
#define __WAVE_H__

#include "config.h"

// 方波输出引脚定义
sbit WAVE_OUT = P1^4;

void Wave_Init(void);       // 初始化定时器0和参数
void Wave_Update(void);     // 当Freq或Duty改变后调用，重新计算定时参数

#endif