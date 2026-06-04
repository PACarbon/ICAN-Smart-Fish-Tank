#ifndef __SERVO_H
#define __SERVO_H

#include "stm32f10x_conf.h"

/*

 *	舵机驱动头文件
 	舵机棕色线 --> GND
	舵机红色线 --> 5V
	舵机橙色线 --> GPIO_Pin_6
 */

extern void TIM8_PWM_Init(void);
extern void servo_setAngle(float data);//控制角度 0~180度
extern void Servo_180_Angle(void);//可以使舵机在0~180度来回动



#endif

