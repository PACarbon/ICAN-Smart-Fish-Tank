#ifndef __DS18B20_H__
#define __DS18B20_H__

#include "main.h"
#include "delay.h"
#include "tim.h"
//IO²Ù×÷º¯Êý											   
 
#define DS18B20_DQ_OUT_HIGH	 HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET)
#define DS18B20_DQ_OUT_LOW	 HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET)
#define DS18B20_DQ_IN				 HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1)
uint8_t DS18B20_Init(void);
void DS18B20_Rst(void);
short DS18B20_Get_Temperature(void);
//float DS18B20_Get_Temp(void);
#endif
