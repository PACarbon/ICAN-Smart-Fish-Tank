#ifndef __BUTTON_LED_H
#define __BUTTON_LED_H

#include "stm32f10x_conf.h"

typedef void (*button_handler)(void);

extern void button_led_init(void);//按键控制灯初始化

extern void set_button_handler(button_handler h0,button_handler h1,button_handler h2);

#endif

