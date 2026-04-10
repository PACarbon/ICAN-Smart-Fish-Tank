#ifndef __LED_H
#define __LED_H

/*
输出高电平灯亮
输出低电平灯灭

*/
#include "bitband.h"
#include "stm32f10x_conf.h"

#define LED2 PBOut(3)


extern void led_init(void);//初始化LED灯的管脚

extern void led_on(void);//通过形参nu选择要亮的灯

extern void led_off(void);//灭灯


#endif






