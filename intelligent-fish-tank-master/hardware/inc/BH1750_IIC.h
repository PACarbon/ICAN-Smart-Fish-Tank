#ifndef __BH1750_IIC_H_
#define __BH1750_IIC_H_


/**********************************
包含头文件
**********************************/
#include "stm32f10x.h"

/**********************************
PIN口定义
**********************************/
#define IIC_SDA_PIN   				GPIO_Pin_11  //								
#define IIC_SCL_PIN    				GPIO_Pin_10			// 

#define  IIC_SDA_H   			GPIO_SetBits(GPIOB,IIC_SDA_PIN)
#define  IIC_SDA_L   			GPIO_ResetBits(GPIOB,IIC_SDA_PIN)

#define  IIC_SCL_H   			GPIO_SetBits(GPIOB,IIC_SCL_PIN)
#define  IIC_SCL_L   			GPIO_ResetBits(GPIOB,IIC_SCL_PIN)



#define  IIC_SDA_Read   GPIO_ReadInputDataBit(GPIOB, IIC_SDA_PIN)

/**********************************
函数声明
**********************************/
void BH1750_GPIO_Init(void);
void SDA_Pin_IN(void);
void SDA_Pin_Output(void);
	

void BH1750_IIC_Init(void);
void BH1750_IIC_start(void); 
void BH1750_IIC_stop(void);
uint8_t BH1750_IIC_Get_ack(void);
void BH1750_IIC_ACK(void);
void BH1750_IIC_NACK(void);
void BH1750_IIC_write_byte(uint8_t Data);
uint8_t BH1750_IIC_read_byte(void);

#endif


