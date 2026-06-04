#include "led.h"
// LED1 PA8 低电平 亮
// LED2 PB3 高电平 亮
void led_init(void)//初始化LED灯的管脚
{
	GPIO_InitTypeDef Gpio_Value;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB,ENABLE);
	//使能GPIOC组的时钟
	Gpio_Value.GPIO_Mode = GPIO_Mode_Out_PP;
	Gpio_Value.GPIO_Pin = GPIO_Pin_8;
	Gpio_Value.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&Gpio_Value);
	PAOut(8)=1;
	
	Gpio_Value.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOB,&Gpio_Value);
	PBOut(3)=0;
}
void led_on(void)//通过形参nu选择要亮的灯
{
	PAOut(8)=0;

}
void led_off(void)//灭灯
{
	PAOut(8)=1;
}










