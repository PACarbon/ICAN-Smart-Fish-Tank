#include "delay.h"
#include "DS18B20.h"
// PC10 5v

#define DQ_H     GPIO_SetBits(GPIOC,GPIO_Pin_10)        			//PB1拉高
#define DQ_L     GPIO_ResetBits(GPIOC,GPIO_Pin_10)						//PB1拉低
#define DQ_Read  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_10)    //PB1读取

u8 ds18b20_flag = 0;

void DS18B20_Init(void) 																		//PB1初始化
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;             //DQ=PB1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void DS18B20_Output(void)      //输出模式            
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;             
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void DS18B20_Input(void)      //输入模式
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;             
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

uint8_t DS18B20_Reset(void)      //复位
{
	uint8_t flag;
	DS18B20_Output();
	DQ_H;
	DelayUs(5);
	
	DQ_L;
	DelayUs(480);
	DQ_H;
	DelayUs(60);
	DS18B20_Input();
	flag=DQ_Read;
	DelayUs(480);
	DS18B20_Output();
	DQ_H;
	return flag;
}

void DS18B20_WriteData(uint8_t data)     // 写数据
{
	uint8_t i = 0;
	for(i=0;i<8;i++)
	{
		DS18B20_Output();  //输出状态
		DQ_L;
		DelayUs(2);
		if(data&0x01)     //低位开始，看上一节视频有详细讲解
		{
			DQ_H;
		}
		else
		{
			DQ_L;
		}
		DelayUs(60);
		DQ_H;
		data = data>>1;
	}
	
}

uint8_t DS18B20_ReadData(void)      //读数据
{
	uint8_t data =0;
	uint8_t i = 0;
	
	for(i=0;i<8;i++)
	{
		data=data>>1;
		DS18B20_Output();   //输出状态
		DQ_L;
		DelayUs(2);
		DQ_H;
		DelayUs(2);
		DS18B20_Input();   //输入状态
		if(DQ_Read)
			data|=0x80;     //放入高位，再移位到低位，看上一节视频有详细讲解
		DelayUs(60);
	}
	return data;
}

float DS18B20_ReadTemp(void)   //读取温度
{
	uint8_t DL,DH;
	uint16_t data;	
	float Temperature=0;
	ds18b20_flag=0;
	DS18B20_Reset();              //复位
	DS18B20_WriteData(0XCC);      //跳过ROM检测
	DS18B20_WriteData(0X44);      //启动温度转换
	DelayMs(750);                //延时，等待转换完成
	DS18B20_Reset();              //复位
	DS18B20_WriteData(0XCC);      //跳过ROM检测
	DS18B20_WriteData(0XBE);      //读取暂存器指令
	DL=DS18B20_ReadData();        //读温度低位
	DH=DS18B20_ReadData();        //读温度高位
	data=DH;
	data=data<<8;
	data|=DL;
	if((data&0XF800)==0XF800)    
	{
		data=~data+0X01;
		ds18b20_flag=1;
	}
		
	Temperature=(float)data * 0.0625 ;  //*10;
	return Temperature;
}

