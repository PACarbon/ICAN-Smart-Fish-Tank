#ifndef __BH1750_H_
#define __BH1750_H_

/**********************************
包含头文件
**********************************/
#include "BH1750_IIC.h"
#include "stm32f10x.h"


#define BH1750_Wr 0x46	 //写地址
#define BH1750_Rd 0x47  //读地址

extern float BH1750_temp;
/**********************************
函数声明
**********************************/
void Multiple_read_BH1750(void);
void Single_Write_BH1750(uint8_t REG_Address);
void BH1750_Init(void);
void BH1750_Read_Data(void);

#endif






