#include "DS18B20.h"
#include "ds18b20.h"	
/*
    普通定时器实现us延时
*/
void user_delaynus_tim(uint32_t nus)
{

    uint16_t  differ = 0xffff-nus-5;
    //设置定时器2的技术初始值
  __HAL_TIM_SetCounter(&htim2,differ);
  //开启定时器
  HAL_TIM_Base_Start(&htim2);

  while( differ<0xffff-5)
    {
        differ = __HAL_TIM_GetCounter(&htim2);
    };
 //关闭定时器
  HAL_TIM_Base_Stop(&htim2);
}
/*
    普通定时器实现ms延时，可直接使用HAL库函数HAL_delay（）
*/
void delay_ms_tim(uint16_t nms)
{
    uint32_t i;
    for(i=0;i<nms;i++) user_delaynus_tim(1000);
}

/*
    IO口方向设置：设置数据口方向为输入
*/
void DS18B20_IO_IN(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin = GPIO_PIN_1;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA,&GPIO_InitStructure);
}

/*
    IO口方向设置：设置数据口方向为输出
*/
void DS18B20_IO_OUT(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin = GPIO_PIN_1;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOA,&GPIO_InitStructure);
}

/*
    DS18B20复位函数
*/
void DS18B20_Rst(void){
	DS18B20_IO_OUT();
	DS18B20_DQ_OUT_LOW;
	user_delaynus_tim(750);
	DS18B20_DQ_OUT_HIGH;
	user_delaynus_tim(15);
}

/*
    DS18B20存在检测函数
*/
uint8_t DS18B20_Check(void){
	uint8_t retry = 0;
	DS18B20_IO_IN();
	//检测DS18B20拉低电平时间大于200us小于240us表示系统检测到DS18B20存在
	while(DS18B20_DQ_IN && retry < 200){
		retry++;
		user_delaynus_tim(1);
	}
	
	if(retry >= 200)
		return 1;
	else
		retry = 0;
	
	while(!DS18B20_DQ_IN && retry < 240){
		retry++;
		user_delaynus_tim(1);
	}
	
	if(retry >= 240)
		return 1;
	
	return 0;
}

/*
    DS18B20读一位数据
*/
uint8_t DS18B20_Read_Bit(void){
	uint8_t data;
	DS18B20_IO_OUT();
	DS18B20_DQ_OUT_LOW;
	user_delaynus_tim(2);
	DS18B20_DQ_OUT_HIGH;
	DS18B20_IO_IN();
	user_delaynus_tim(12);
	
	if(DS18B20_DQ_IN)
		data = 1;
	else
		data = 0;
	
	user_delaynus_tim(50);
	return data;
}

/*
    DS18B20读一字节数据
*/
uint8_t DS18B20_Read_Byte(void){
	uint8_t i,j,data;
	data = 0;
	for(i=1;i<=8;i++){
		j = DS18B20_Read_Bit();
		data = (j<<7)|(data>>1);
	}
	return data;
}

/*
    DS18B20写一字节数据
*/
void DS18B20_Write_Byte(uint8_t data){
	uint8_t j;
	uint8_t testb;
	DS18B20_IO_OUT();
	for(j=1;j<=8;j++){
		testb=data&0x01;//
		data=data>>1;
		//写1时序
		if(testb)
			{
			DS18B20_DQ_OUT_LOW;//拉低总线
			user_delaynus_tim(2);//2us
			DS18B20_DQ_OUT_HIGH;//拉高总线
			user_delaynus_tim(60);//60us
			}
		//写0时序
		else
			{
			DS18B20_DQ_OUT_LOW;//拉低总线
			user_delaynus_tim(60);//60us
			DS18B20_DQ_OUT_HIGH;//拉高总线
			user_delaynus_tim(2);//2us
		}
	}
}

/*
    DS18B20启动函数
*/
void DS18B20_Start(void){
	DS18B20_Rst();
	DS18B20_Check();
	DS18B20_Write_Byte(0xcc);
	DS18B20_Write_Byte(0x44);
}

uint8_t DS18B20_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin = GPIO_PIN_1;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	DS18B20_Rst();
	return DS18B20_Check();
}

/*
    DS18B20获取温度函数
*/
short DS18B20_Get_Temperature(void){
	uint8_t temp;
	uint8_t TL,TH;
	short temperature;
	
	DS18B20_Start();
	DS18B20_Rst();
	DS18B20_Check();
	DS18B20_Write_Byte(0xcc);
	DS18B20_Write_Byte(0xbe);
	TL = DS18B20_Read_Byte();//取低八位数据
	TH = DS18B20_Read_Byte();//取高八位数据
	//判断符号位是否全为0，全为0为正数，为1为负数
	if(TH>7){
		TH = ~TH;
		TL = ~TL;
		temp = 0;
	}else
		temp = 1;
	
	temperature = TH;
	temperature <<= 8;
	temperature += TL;
	temperature = (float)temperature*0.625;
	if(temperature)
		return temperature;
	else
		return -temperature;
}

//void DS18B20_Rst(void) //复位操作	   
//	{                 
//	DS18B20_IO_OUT(); //SET PA0 OUTPUT
//	DS18B20_DQ_OUT_LOW;//拉低DQ
////	Clr_DS18B20_DQ_OUT; //拉低DQ
//	user_delaynus_tim(750);    //拉低750us
//	DS18B20_DQ_OUT_HIGH;//拉高DQ	
////	Set_DS18B20_DQ_OUT; //DQ=1 
//	user_delaynus_tim(10);     //15US
//	}

////等待DS18B20的回应
////返回1；未检测到DS18B20的存在  返回0：存在
//uint8_t DS18B20_Check(void)
//{
//  uint8_t retry=0;
//  DS18B20_IO_IN();//SET PA0 INPUT
//  while(DS18B20_DQ_IN&&retry<200)
//  {
//    retry++;
//    user_delaynus_tim(1);
//  }
//  if(retry>=200) return 1;
//  else retry=0;
//  while(!DS18B20_DQ_IN&&retry<240)
//  {
//    retry++;
//    user_delaynus_tim(1);
//  }
//  if(retry>=240)  return 1;
//  return 0;
//}
	
//从DS18B20读取一个位
//返回值：1/0
//uint8_t DS18B20_Read_Bit(void) 			 // read one bit
//{
//	uint8_t data;
//	DS18B20_IO_OUT();//SET PA0 OUTPUT
//	DS18B20_DQ_OUT_LOW;//拉低DQ	
////	Clr_DS18B20_DQ_OUT;
//	user_delaynus_tim(5);
//	DS18B20_DQ_OUT_HIGH;//拉高DQ
////	Set_DS18B20_DQ_OUT; 
//	DS18B20_IO_IN();//SET PA0 INPUT
//	user_delaynus_tim(12);
//	if(DS18B20_DQ_IN)data=1;
//	else data=0;	 
//	user_delaynus_tim(50);           
//	return data;
//}
//	
////从DS18B20读取一个字节
////返回值：读到的数据
//uint8_t DS18B20_Read_Byte(void)    // read one byte
//	{        
//	uint8_t i,dat=0;
//	for (i=0;i<8;i++) 
//		{
//		
//		if(DS18B20_Read_Bit())
//		{
//		dat|=0x80;
//		}
//		dat>>=1;
//		}						    
//	return dat;
//	}
//	
//	
//void DS18B20_Write_Bit(uint8_t bit)
//{	
//	DS18B20_IO_OUT();
//	if(bit)
//	{
////		Clr_DS18B20_DQ_OUT;
//		DS18B20_DQ_OUT_LOW;
//		user_delaynus_tim(5);
//		DS18B20_DQ_OUT_HIGH;
////		Set_DS18B20_DQ_OUT
//		user_delaynus_tim(60);
//	}
//	else
//	{
//		DS18B20_DQ_OUT_LOW;
////		Clr_DS18B20_DQ_OUT;
//		user_delaynus_tim(60);
//		DS18B20_DQ_OUT_HIGH;
//		user_delaynus_tim(2);
////		Set_DS18B20_DQ_OUT
//		
//	}

//}


//void DS18B20_Write_Byte(uint8_t Byte)
//{
//	uint8_t j;
//	volatile uint8_t temp=0x00;
//	for(j=0;j<8;j++)
//	{
//		temp=Byte&(0x01<<j);
//		DS18B20_Write_Bit(temp);
//	}
//}

////开始温度转换
//void DS18B20_Start(void)// ds1820 start convert
//	{   						               
//	DS18B20_Rst();	   
//	DS18B20_Check();	 
//	DS18B20_Write_Byte(0xcc);// skip rom
//	DS18B20_Write_Byte(0x44);// convert
//	} 
//	
////初始化DS18B20的IO口 DQ 同时检测DS的存在
////返回1:不存在
////返回0:存在    	 
//uint8_t DS18B20_Init(void)
//	{
// 
//	DS18B20_Rst();
//	return DS18B20_Check();
//	}  
//	
////从ds18b20得到温度值
////精度：0.1C
////返回值：温度值 （-550~1250） 
//float DS18B20_Get_Temp(void)
//	{
//	uint8_t temp;
//	uint8_t TL,TH;
//	uint16_t tem;
//	DS18B20_Start ();                    // ds1820 start convert
//	DS18B20_Rst();
//	DS18B20_Check();	 
//	DS18B20_Write_Byte(0xcc);// skip rom
//	DS18B20_Write_Byte(0xbe);// convert	    
//	TL=DS18B20_Read_Byte(); // LSB   
//	TH=DS18B20_Read_Byte(); // MSB  
//	
//	if(TH>7)
//		{
//		TH=~TH;
//		TL=~TL; 
//		temp=0;//温度为负  
//		}
//	else temp=1;//温度为正	  	  
//	tem=TH; //获得高八位
//	tem<<=8;    
//	tem+=TL;//获得底八位
//	tem=(float)tem*0.625;//转换     
//	if(temp)return tem; //返回温度值
//	else return -tem;    
//	} 
// 
/**************************
name:DS18B20初始化函数
argument:无
return:无
role:初始化DS18B20
**************************/
//uint8_t DS18B20_Init(void)
//{
//	uint8_t Ack;
//	DS18B20_IO_OUT();
//	DS18B20_DQ_OUT_LOW;
//	user_delaynus_tim(500);
//	DS18B20_DQ_OUT_HIGH;
//	DS18B20_IO_IN();
//	user_delaynus_tim(70);
//	Ack=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1);
//	user_delaynus_tim(500);
//	return Ack;
//}

///**************************
//name:发送一位数据
//argument:无
//return:无
//role:初始化DS18B20
//**************************/
//void DS18B20_SendBit(uint8_t Bit)
//{
//	if(Bit == 1)
//	{	
//	DS18B20_DQ_OUT_LOW;
//	user_delaynus_tim(10);
//	DS18B20_DQ_OUT_HIGH;
//	user_delaynus_tim(50);
//	}
//	if(Bit == 0)
//	{
//	DS18B20_DQ_OUT_LOW;
//	user_delaynus_tim(60);
//	DS18B20_DQ_OUT_HIGH;
//	}

//}

//uint8_t DS18B20_ReciveBit(void)
//{
//	uint8_t bit;
//	DS18B20_DQ_OUT_LOW;
//	user_delaynus_tim(5);
//	DS18B20_DQ_OUT_HIGH;
//	user_delaynus_tim(5);
//	bit = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1);
//	user_delaynus_tim(50);
//	
//return bit;
//}

//void DS18B20_SendByte(uint8_t data)
//{
//	uint8_t i;
//	for(i=0;i<8;i++)
//	{
//		DS18B20_SendBit(data & (0x01<<i));
//	
//	}

//}

//uint8_t DS18B20_ReceiveByte(void)
//{
//	uint8_t i;
//	uint8_t Byte=0x00;
//	for(i=0;i<8;i++)
//	{
//		if(DS18B20_ReciveBit()){Byte |=(0x01<<i);}
//	
//	}
//	return Byte;
//}

//void DS18B20_ConvertT(void)
//{
//	DS18B20_Init();
//	DS18B20_SendByte(0xCC);
//	DS18B20_SendByte(0x44);

//}

//float DS18B20_Read_Tempture(void)
//{
//	uint8_t TLSB,TMSB;
//	uint16_t temp;
//	float Tempture;
//	DS18B20_Init();
//	DS18B20_SendByte(0xCC);
//	DS18B20_SendByte(0xBE);
//	TLSB=DS18B20_ReceiveByte();
//	TMSB=DS18B20_ReceiveByte();
//	
//	temp = (TMSB<<8)|TLSB;
//	Tempture=temp/16.0;
//	return Tempture;
//}
