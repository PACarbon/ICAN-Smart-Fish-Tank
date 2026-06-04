#include "relay.h"
#include "bitband.h"
/*
	继电器 PB 12   13   14   15 
	           1    2    3    4
						加氧  加热
*/
void relay_init(void)//继电器初始化
{
	GPIO_InitTypeDef Gpio_Value;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	Gpio_Value.GPIO_Mode = GPIO_Mode_Out_PP;
	Gpio_Value.GPIO_Pin = GPIO_Pin_15;//|GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	Gpio_Value.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&Gpio_Value);

	//PBOut(12) = 1;
//	PBOut(13) = 0;
//	PBOut(14) = 0;
	PBOut(15) = 0;
}
void relay_on(void)//开
{
	PBOut(15) = 1;
}
void relay_off(void)//关
{
	PBOut(15) = 0;

}


