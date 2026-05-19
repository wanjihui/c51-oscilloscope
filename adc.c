#include "adc.h"
#include <intrins.h>

unsigned int adc_Read(void)
{
    unsigned char i;
    unsigned int value = 0;
    unsigned char cmd = 0xE7; // 12位、单端、AUX、一直上电
	
    bit ea_save = EA;
    EA = 0;

    adc_CS = 0;

    // 第一步：发送8位控制字
    for (i = 0; i < 8; i++)
    {
        adc_DCLK = 0;
        adc_DIN = (cmd >> (7 - i)) & 0x01;
        _nop_();
        _nop_();
        adc_DCLK = 1;
        _nop_();
        _nop_();
    }

    // 关键 跳过1个无效的BUSY位时钟
    adc_DCLK = 0;
    _nop_();
    _nop_();
    adc_DCLK = 1;
    _nop_();
    _nop_();

    //读取真正的12位有效数据
    for (i = 0; i < 12; i++)
    {
        adc_DCLK = 0;
        _nop_();
        _nop_();
        value <<= 1;
        if (adc_DOUT) value |= 0x01;
        adc_DCLK = 1;
        _nop_();
        _nop_();
    }

    adc_CS = 1;
    EA = ea_save;

    return value;
}