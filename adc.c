#include "adc.h"

/**
 * @brief  读取一次 XPT2046 的 ADC 数据（AUX 通道，12位分辨率，单端模式）
 * @retval 12位 ADC 值（0 ~ 4095）
 */
unsigned int adc_Read(void)
{
    unsigned char i;
    unsigned int value = 0;
    // 控制字节：1(起始位) 011(通道AUX) 0(12位) 1(单端) 00(掉电模式)
    unsigned char cmd = 0xB4;

    adc_CS = 0;                     // 拉低片选，启动通信
    // 发送控制字节并接收高8位（共8个时钟）
    for (i = 0; i < 8; i++)
    {
        adc_DCLK = 0;
        adc_DIN = (cmd >> (7 - i)) & 0x01;  // 先发送最高位
        _nop_();
        adc_DCLK = 1;                       // 上升沿 ADC 锁存 DIN，同时输出数据到 DOUT
        _nop_();
        value <<= 1;
        if (adc_DOUT)                        // 读取 DOUT 电平
            value |= 0x01;
    }
    // 再发送 8 个时钟，接收低 8 位（实际只用到低 4 位，但接收完整 8 位）
    for (i = 0; i < 8; i++)
    {
        adc_DCLK = 0;
        adc_DIN = 0;                        // 后续时钟 DIN 可为任意值
        _nop_();
        adc_DCLK = 1;
        _nop_();
        value <<= 1;
        if (adc_DOUT)
            value |= 0x01;
    }
    adc_CS = 1;                             // 结束转换
    return (value >> 3);                    // 右移 3 位，保留高 12 位有效数据
}