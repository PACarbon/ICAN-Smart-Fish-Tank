#include "button_led.h"
/*
	°´¼ü pc9 pc8 pA0
*/
button_handler b_h0,b_h1,b_h2;
void button_led_init(void)
{
	GPIO_InitTypeDef Gpio_Value;
	NVIC_InitTypeDef Nvic_Value;
	EXTI_InitTypeDef Exti_Value;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO,ENABLE);
	
	Gpio_Value.GPIO_Mode = GPIO_Mode_IPU;
	Gpio_Value.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8;
	GPIO_Init(GPIOC,&Gpio_Value);
	Gpio_Value.GPIO_Mode = GPIO_Mode_IPD;
	Gpio_Value.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA,&Gpio_Value);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource9);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource8);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
	
	Nvic_Value.NVIC_IRQChannel = EXTI9_5_IRQn;
	Nvic_Value.NVIC_IRQChannelCmd = ENABLE;
	Nvic_Value.NVIC_IRQChannelPreemptionPriority = 2;
	Nvic_Value.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&Nvic_Value);//
	Nvic_Value.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_Init(&Nvic_Value);//
	
	Exti_Value.EXTI_Line = EXTI_Line9 | EXTI_Line8;
	Exti_Value.EXTI_LineCmd = ENABLE;
	Exti_Value.EXTI_Mode = EXTI_Mode_Interrupt;
	Exti_Value.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&Exti_Value);//
	Exti_Value.EXTI_Line = EXTI_Line0;
	Exti_Value.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&Exti_Value);//
}

void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line9)==SET)
	{
		b_h0();
		EXTI_ClearITPendingBit(EXTI_Line9);
	}
	if(EXTI_GetITStatus(EXTI_Line8)==SET)
	{
		b_h1();
		EXTI_ClearITPendingBit(EXTI_Line8);
	}
}

void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line0))
	{
		b_h2();
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

void set_button_handler(button_handler h0,button_handler h1,button_handler h2)
{
	b_h0 = h0;
	b_h1 = h1;
	b_h2 = h2;
}



