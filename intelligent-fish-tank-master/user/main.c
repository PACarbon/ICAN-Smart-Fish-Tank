/***************
LCD屏幕 PB 4 5 6 7 8 9
esp8266	PA 2 3
usart1  PA 9 10
ds18b20 PC 10	5v
tds 	PB 0	3.3v或5v
bh1750	PB 10 11 5v
舵机	PC 6
继电器	PB 12 13 14 15
按键 	pc9 pc8 pA0
LED1	PA8 低电平 亮
LED2 	 高电平 亮 PC7
**************/

//单片机头文件
#include "stm32f10x.h"

//网络协议层
#include "onenet.h"

//网络设备
#include "esp8266.h"

//硬件驱动
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "button_led.h"
#include "DS18B20.h"
#include "lcd.h"
#include "lcd_init.h"
#include "relay.h"
#include "tds.h"
#include "BH1750.h"
#include "PWM.h"   //LED灯
//C库
#include <string.h>
#include <stdio.h>
//mqtts.heclouds.com:1883
#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"RHEB0REAN2.iotcloud.tencentdevices.com\",1883\r\n"


/*************************************/
void h0(void);			//按键3
void h1(void);			//按键2
void h2(void);			//按键1
void show1(void);		//主界面
void show2(void);		//阈值处理界面
void _show2(void);		//界面2的子函数
void heat(void);		//阈值处理函数
/************************************/
void Hardware_Init(void) //硬件初始化函数
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断控制器分组设置

	Delay_Init();									//systick初始化
	
	Usart1_Init(115200);							//串口1，打印信息用  调试用
	
	Usart2_Init(115200);							//串口2，驱动ESP8266用
	
	LCD_Init();
	
	led_init();

	relay_init();									//继电器 控制 加水 抽水 加氧 加热
	
	DS18B20_Init();									//DS18B20初始化
	tds_init();                   					//tds水质传感器
	button_led_init();
	BH1750_GPIO_Init();								//bh1750光照强度传感器
	PWM_Init();
	
	UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");
	LCD_Fill(0,0,LCD_W,LCD_H,WHITE); //清屏
}

/*
************************************************************
*	函数名称：	main
*
*	函数功能：	
*
*	入口参数：	无
*
*	返回参数：	0
*
*	说明：		
************************************************************
*/
float temp,water_quality;

//extern int temp_up,temp_down;
u8 light_led = 0;		  //存储灯光是否开启
u8 alarm_lamp = 0;	  //存储水质报警灯是否开启
u8 temp_limit = 20;   //温度阈值
u8 water_limit = 30;  //水质阈值
u8 led_limit = 30;	  //灯光阈值
u8 lcd_page = 0; 	//用于切换界面
u8 lcd_clean = 0;  //用于判断是否清屏
u8 hot_state = 0;  //记录加热状态
int main(void)
{
	
	unsigned short timeCount = 0;	//发送间隔变量
	
	unsigned char *dataPtr = NULL;  //接收云平台下发的数据
	
	Hardware_Init();				//初始化外围硬件
	
	ESP8266_Init();					//初始化ESP8266

	UsartPrintf(USART_DEBUG, "Connect MQTTs Server...\r\n");
	
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))  //连接服务器
		DelayXms(500);
	UsartPrintf(USART_DEBUG, "Connect MQTTs Server Success\r\n");
	
	while(OneNet_DevLink())			//接入OneNET 
		DelayXms(500);
	
	LCD_Fill(0,0,LCD_W,LCD_H,WHITE); //清屏
	LCD_ShowString(5,10,(u8*)"Connect Tencent",RED,WHITE,16,0); //接入腾讯云平台成功
	LCD_ShowString(5,30,(u8*)"  Success!",RED,WHITE,16,0);
	
	led_on();
	DelayXms(250);
	led_off();
	
	OneNET_Subscribe();//订阅主题
	set_button_handler(h0,h1,h2);//按键中断回调函数
	DelayMs(2500);
	LCD_Fill(0,0,LCD_W,LCD_H,WHITE); //清屏
	
	while(1)//1
	{
		BH1750_Read_Data();								//获取光照强度
		/*********上发数据*******/
		if(++timeCount >= 20)									//发送间隔5s
		{
			temp = DS18B20_ReadTemp();						//读取温度
			water_quality = get_tds_value();				//读取水质
			//BH1750_Read_Data();								//获取光照强度
			UsartPrintf(USART1,"temp=%d,water_quality=%d\r\n",(int)temp,(int)water_quality);
			UsartPrintf(USART_DEBUG, "tengNet_SendData\r\n");
			OneNet_SendData();									//发送数据
			
			timeCount = 0;
			ESP8266_Clear();
		}
		/**********************/
		heat();							//阈值处理函数

		
		/******界面显示****/
		
		switch(lcd_page)
		{
			case 0:
				if(lcd_clean==0)
				{
					LCD_Fill(0,0,LCD_W,LCD_H,WHITE); //清屏
					lcd_clean = 1;
				}
				show1();			//温度 水质屏幕显示
				break;
			default:
				if(lcd_clean==1)
				{
					LCD_Fill(0,0,LCD_W,LCD_H,WHITE); //清屏
					lcd_clean = 0;
				}
				show2();
				break;
		}		
		/***********************/
		
		/******接收数据*********/
		dataPtr = ESP8266_GetIPD(0); //获取到数据
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);  //对数据进行解析

		DelayXms(5);
		/***********************/		
	}
}
void h0(void)	//中断调用 PC9 按键3 按下低电平
{
	DelayMs(5);
	if(PCIn(9)==0)
	{
		DelayMs(10);
		if(PCIn(9)==0)
		{
			while(PCIn(9)==0);
			switch(lcd_page)
			{
				case 1: temp_limit--; break;
				case 2: water_limit--; break;
				case 3: led_limit--; break;
			}
		}
	}	
}
void h1(void)   //PC8 	按键2 按下低电平
{
	DelayMs(5);
	if(PCIn(8)==0)
	{
		DelayMs(10);
		if(PCIn(8)==0)
		{
			while(PCIn(8)==0);
			switch(lcd_page)
			{
				case 1: temp_limit++; break;
				case 2: water_limit++; break;
				case 3: led_limit++; break;
			}
		}
	}
}
void h2(void)    //PA0  按键1 按下高电平
{
	DelayMs(5);
	if(PAIn(0)==1)
	{
		DelayMs(10);
		if(PAIn(0)==1)
		{
			while(PAIn(0)==1);
			lcd_page = (lcd_page+1)%4;

		}
	}
}

void show1(void)						//屏幕展示界面
{
	char arr_light[20] = {0};
	LCD_ShowChinese(37,3,(u8*)"主界面",BLACK,WHITE,16,0);
	if(ds18b20_flag==1)
	{
		//LCD_ShowString(10,1,(u8 *)"Temp=-",BLACK,WHITE,16,0);
		LCD_ShowChinese(5,21,(u8*)"水温",BLACK,WHITE,16,0);
		LCD_ShowChar(37,21,':',BLACK,WHITE,16,0);
		LCD_ShowChar(47,21,'-',BLACK,WHITE,16,0);
		LCD_ShowFloatNum1(58,21,temp,4,BLACK,WHITE,16);
		
	}
	else
	{
		//LCD_ShowString(10,1,(u8 *)"Temp=+",BLACK,WHITE,16,0);
		LCD_ShowChinese(5,21,(u8*)"水温",BLACK,WHITE,16,0);
		LCD_ShowChar(37,21,':',BLACK,WHITE,16,0);
		LCD_ShowChar(47,21,'+',BLACK,WHITE,16,0);
		LCD_ShowFloatNum1(58,21,temp,4,BLACK,WHITE,16);
		LCD_ShowChinese(100,21,(u8*)"℃",BLACK,WHITE,16,0);
	}
	
	//TDS
	LCD_ShowString(5,40,(u8 *)"TDS :",BLACK,WHITE,16,0);
	//LCD_ShowFloatNum1(47,40,water_quality,4,BLACK,WHITE,16);
	memset(arr_light,'\0',20);
	sprintf(arr_light,"%.1f",water_quality);
	LCD_Fill(80,40,90,59,WHITE);
	LCD_ShowString(47,40,(u8 *)arr_light,BLACK,WHITE,16,0);
	LCD_ShowString(90,38,(u8 *)"ppm",BLACK,WHITE,16,0);
	
	//光照强度
	LCD_ShowChinese(2,60,(u8*)"光照强度",BLACK,WHITE,16,0);
	LCD_ShowChar(64,60,':',BLACK,WHITE,16,0);
	memset(arr_light,'\0',20);
	sprintf(arr_light,"%.1f",BH1750_temp);
	LCD_Fill(105,60,110,78,WHITE);
	LCD_ShowString(72,60,(u8 *)arr_light,BLACK,WHITE,16,0);
	LCD_ShowString(110,60,(u8 *)"LX",BLACK,WHITE,16,0);
	
	//加热
	LCD_ShowChinese(5,80,(u8*)"加热",BLACK,WHITE,16,0);
	LCD_ShowChar(37,80,':',BLACK,WHITE,16,0);
	if(hot_state == 1)
		LCD_ShowChinese(45,80,(u8*)"开",RED,WHITE,16,0);
	else
		LCD_ShowChinese(45,80,(u8*)"关",BLACK,WHITE,16,0);
	
	//照明
	LCD_ShowChinese(65,80,(u8*)"照明",BLACK,WHITE,16,0);
	LCD_ShowChar(97,80,':',BLACK,WHITE,16,0);
	if(light_led)
		LCD_ShowChinese(105,80,(u8*)"开",RED,WHITE,16,0);
	else
		LCD_ShowChinese(105,80,(u8*)"关",BLACK,WHITE,16,0);
	
	//水质异常显示
	if(alarm_lamp)
		LCD_ShowChinese(5,105,(u8*)"水质异常",RED,WHITE,16,0);
	else
		LCD_Fill(5,105,128,128,WHITE);
}

void show2()		//阈值处理界面
{
	LCD_ShowChinese(15,10,(u8*)"阈值设置界面",BLACK,WHITE,16,0);
	LCD_ShowChinese(5,35,(u8*)"温度阈值",BLACK,WHITE,16,0);
	LCD_ShowIntNum(75,35,temp_limit,2,BLACK,WHITE,16);
	LCD_ShowChinese(5,60,(u8*)"水质阈值",BLACK,WHITE,16,0);
	LCD_ShowIntNum(75,60,water_limit,2,BLACK,WHITE,16);
	LCD_ShowChinese(5,85,(u8*)"灯光阈值",BLACK,WHITE,16,0);
	LCD_ShowIntNum(75,85,led_limit,2,BLACK,WHITE,16);
	_show2();
}

void _show2()		//显示<-
{
	switch(lcd_page)
	{
		case 1:
			LCD_ShowChar(95,35,'<',RED,WHITE,16,0);
			LCD_ShowChar(105,35,'-',RED,WHITE,16,0);
			break;
		case 2:
			LCD_Fill(95,35,128,60,WHITE);	//清除上一个<-
			LCD_ShowChar(95,60,'<',RED,WHITE,16,0);
			LCD_ShowChar(105,60,'-',RED,WHITE,16,0);
			break;
		case 3:
			LCD_Fill(95,60,128,85,WHITE);	//清除上一个<-
			LCD_ShowChar(95,85,'<',RED,WHITE,16,0);
			LCD_ShowChar(105,85,'-',RED,WHITE,16,0);
			break;
	}
}


void heat(void)//阈值处理函数
{
	u16 light = 0;

	if(temp < temp_limit)	//控制加热 
	{
		relay_on();    //打开加热
		hot_state = 1;
	}
	else
	{
		relay_off();	//关闭加热
		hot_state = 0;
	}		
	
	
	if(BH1750_temp <= (float)led_limit)		//光照处理
	{
		light_led = 1;	//记录灯光状态
		light = (u16)(100.0 - BH1750_temp/20.0 * 100.0);
		PWM_SetCompare2(light);
	}	
	else
	{
		light_led = 0;	//记录灯光状态
		PWM_SetCompare2(0);
	}
		
	if((int)water_quality >= water_limit)		//水质处理 大于阈值报警 提醒换水
	{
		alarm_lamp = 1; //记录报警灯状态
		led_on();
	}
	else
	{
		alarm_lamp = 0;	//记录报警灯状态
		led_off();
	}
		
}


