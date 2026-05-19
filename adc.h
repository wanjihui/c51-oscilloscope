#ifndef __ADC_H__
#define __ADC_H__

#include <reg52.h>
#include <intrins.h>

// 引脚定义，可根据实际接线修改
sbit adc_CS   = P3^5;
sbit adc_DCLK = P3^6;
sbit adc_DIN  = P3^4;
sbit adc_DOUT = P3^7;

unsigned int adc_Read(void);

#endif