#include "servo.h"
#include "delay.h"
//  PC6 5v 红 vcc 灰 GND
void TIM8_PWM_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
 	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);	// 开启定时器时钟,即内部时钟CK_INT=72M

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//设置输出速度
    GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC_Pin_9
	 
	TIM_TimeBaseStructure.TIM_Period= (20000-1);							 //自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
	TIM_TimeBaseStructure.TIM_Prescaler= (72-1);					 //驱动CNT计数器的时钟 = Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		 //时钟分频因子 ，用于配置死区时间，没用到，随意
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;//计数器计数模式，设置为向上计数
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;					 //重复计数器的值，没用到，可以随意设置
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);					 //初始化定时器
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;						 //配置为PWM模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//输出使能
	TIM_OCInitStructure.TIM_Pulse = 0;													 //设置占空比大小，在主函数compare中又设置一遍
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		 //输出通道电平极性配置
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;	 //输出通道空闲电平极性配置
	TIM_OC1Init(TIM8, &TIM_OCInitStructure);			   						 //初始化通道1
	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);						 //使能通道1输出
	
	TIM_Cmd(TIM8, ENABLE);						//使能计数器
	TIM_CtrlPWMOutputs(TIM8, ENABLE);	// 主输出使能，当使用的是通用定时器时，这句不需要
}


void servo_setAngle(float data)
{
	TIM_SetCompare1(TIM8,data/180*2000+500);

}


/************************************************************************************/
/*以下函数为0-180度舵机角度控制测试函数
	*	PWM 信号与0-180舵机的关系：
	*	0.5ms ---------------- 0度
	*	1ms   ---------------- 45度
	*	1.5ms ---------------- 90度
	*	2ms   ---------------- 135度
	*	2.5ms ---------------- 180度
 
	*	舵机频率与占空比的计算：
	*	设舵机的频率为50HZ，则PWM周期为20ms，0度对应的占空比为2.5%，即0.05ms的高电平输出。
 */
 
//0-180度舵机 --> 0-90-180度控制函数
void Servo_180_Angle(void)
{
	TIM_SetCompare1(TIM8,500);//0度    500
	DelayMs(300);
	
	TIM_SetCompare1(TIM8,1500);//90度  1500
	DelayMs(300);	
	
	TIM_SetCompare1(TIM8,2500);//180度 2500
	DelayMs(500);	
}



