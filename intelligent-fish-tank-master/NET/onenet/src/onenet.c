//单片机头文件
#include "stm32f10x.h"

//网络设备
#include "esp8266.h"

//协议文件
#include "onenet.h"
#include "mqttkit.h"

//算法
#include "utils_hmac.h"

//硬件驱动
#include "usart.h"
#include "delay.h"
#include "bitband.h"
#include "led.h"
#include "relay.h"
#include "servo.h"
#include "BH1750.h"

//C库
#include <string.h>
#include <stdio.h>



#define PROID			"RHEB0REAN2d11;12010126;c2d11;1733849386"   //UserName  ;前面产品id   最后一段是时间戳

#define ACCESS_KEY		"cpxnEPqBJmCn5IZ+eHQxOg=="   //设备密钥

#define DEVICE_NAME		"RHEB0REAN2d11"				//ClientId		产品id + 设备id

//#define PASSWORD		"178256be0cd9a544d334fdde93ba79274c494a604ee9ce7871504e248f468599;hmacsha256" //PassWord

/********************************************/
	


char devid[16];

char key[48];


extern unsigned char esp8266_buf[512];





//==========================================================
//	函数名称：	OneNet_DevLink
//
//	函数功能：	与onenet创建连接
//
//	入口参数：	无
//
//	返回参数：	1-成功	0-失败
//
//	说明：		与onenet平台建立连接
//==========================================================
_Bool OneNet_DevLink(void)
{
	char buf123[128];//临时缓冲区
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//协议包

	unsigned char *dataPtr;
	
	char authorization_buf[160];
	
	_Bool status = 1;
	
	/****给password(authorization_buf) 赋值 *******/
	memset(buf123,128,0);                                 									 //清除临时缓冲区
	base64_decode(ACCESS_KEY,(unsigned char*)buf123);                                         //对DEVICESECRE进行base64解码
	utils_hmac_sha1(PROID,strlen(PROID),authorization_buf,buf123,strlen(buf123));                   //以上一步的结果为秘钥对Username中的明文，进行hmacsha1加密，结果就是密码，并保存到缓冲区中
	strcat(authorization_buf ,";hmacsha1");                                                           //末尾追加字符串
	
	UsartPrintf(USART_DEBUG, "tengxun_DevLink\r\n"
							"NAME: %s,	PROID: %s,	KEY:%s\r\n"
                        , DEVICE_NAME, PROID, authorization_buf);
	/**********************************************/
	
//	memset(authorization_buf,'\0',sizeof(authorization_buf));
//	sprintf(authorization_buf,"%s",PASSWORD);	//PASSWORD
	
	//256是连接云平台的时间
	if(MQTT_PacketConnect(PROID, authorization_buf, DEVICE_NAME, 256, 1, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//上传平台
		
		dataPtr = ESP8266_GetIPD(250);									//等待平台响应
		if(dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0:UsartPrintf(USART_DEBUG, "Tips:	连接成功\r\n");status = 0;break;
					
					case 1:UsartPrintf(USART_DEBUG, "WARN:	连接失败：协议错误\r\n");break;
					case 2:UsartPrintf(USART_DEBUG, "WARN:	连接失败：非法的clientid\r\n");break;
					case 3:UsartPrintf(USART_DEBUG, "WARN:	连接失败：服务器失败\r\n");break;
					case 4:UsartPrintf(USART_DEBUG, "WARN:	连接失败：用户名或密码错误\r\n");break;
					case 5:UsartPrintf(USART_DEBUG, "WARN:	连接失败：非法链接(比如token非法)\r\n");break;
					
					default:UsartPrintf(USART_DEBUG, "ERR:	连接失败：未知错误\r\n");break;
				}
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//删包
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	MQTT_PacketConnect Failed\r\n");
	
	return status;
	
}
extern float temp,water_quality; //水温 水质
extern u8 temp_limit ;  //温度阈值
extern u8 water_limit ; //水质阈值
extern u8 led_limit ;	//灯光阈值
extern u8 light_led ;	//存储灯光是否开启
extern u8 alarm_lamp ;	//存储水质报警灯是否开启
extern u8 hot_state; //记录加热状态
//u8 feed;
unsigned char OneNet_FillBuf(char *buf)
{
	
	char text[48];
	
	memset(text, 0, sizeof(text));
	
	strcpy(buf, "{\"method\":\"report\",");
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"clientToken\":\"123\",\"params\":{");
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"temp\":%.2f,", temp); //"temp":3,
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"water_quality\":%.2f,", water_quality);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"light\":%.2f,", BH1750_temp);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));			//报警灯
	sprintf(text, "\"LED\":%d,", alarm_lamp);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));			//照明灯
	sprintf(text, "\"LED2\":%d,", light_led);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"switch_hot\":%d,", hot_state);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"led_limit\":%d,", led_limit);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"temp_limit\":%d,", temp_limit);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"water_limit\":%d", water_limit);
	strcat(buf, text);
	
	
	
	strcat(buf, "}}");
	
	return strlen(buf);

}

//==========================================================
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_SendData(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//协议包
	
	char buf[256];
	
	short body_len = 0, i = 0;
	
	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-MQTT\r\n");
	
	memset(buf, 0, sizeof(buf));
	
	body_len = OneNet_FillBuf(buf);																	//获取当前需要发送的数据流的总长度
	
	if(body_len)
	{
		if(MQTT_PacketSaveData(PROID, DEVICE_NAME, body_len, NULL, &mqttPacket) == 0)				//封包
		{
			for(; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = buf[i];
			
			ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//上传数据到平台
			UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", mqttPacket._len);
			
			MQTT_DeleteBuffer(&mqttPacket);															//删包
		}
		else
			UsartPrintf(USART_DEBUG, "WARN:	EDP_NewBuffer Failed\r\n");
	}
	
}

//==========================================================
//	函数名称：	OneNET_Publish
//
//	函数功能：	发布消息
//
//	入口参数：	topic：发布的主题
//				msg：消息内容
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNET_Publish(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};						//协议包
	
	UsartPrintf(USART_DEBUG, "Publish Topic: %s, Msg: %s\r\n", topic, msg);
	
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqtt_packet) == 0)
	{
		ESP8266_SendData(mqtt_packet._data, mqtt_packet._len);					//向平台发送订阅请求
		
		MQTT_DeleteBuffer(&mqtt_packet);										//删包
	}

}

//==========================================================
//	函数名称：	OneNET_Subscribe
//
//	函数功能：	订阅
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNET_Subscribe(void)
{
	
	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};						//协议包
	
	char topic_buf[56];
	const char *topic = topic_buf;
	
	snprintf(topic_buf, sizeof(topic_buf), "$thing/down/property/RHEB0REAN2/d11");
	
	UsartPrintf(USART_DEBUG, "Subscribe Topic: %s\r\n", topic_buf);
	
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, &topic, 1, &mqtt_packet) == 0)
	{
		ESP8266_SendData(mqtt_packet._data, mqtt_packet._len);					//向平台发送订阅请求
		
		MQTT_DeleteBuffer(&mqtt_packet);										//删包
	}

}

//==========================================================
//	函数名称：	OneNet_RevPro
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据
//
//	返回参数：	无
//
//	说明：		
//==========================================================
//int temp_up,temp_down; //接收数据上限和数据下限
void OneNet_RevPro(unsigned char *cmd)
{
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short topic_len = 0;
	unsigned short req_len = 0;
	
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;
	
	unsigned char type = 0;
	
	short result = 0;

	char *dataPtr = NULL;
	char numBuf[10];	
	int num = 0;
	char *p = NULL;//指向要处理的字符串
	
	
	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{
		case MQTT_PKT_PUBLISH:																//接收的Publish消息
		
			result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);//对消息进行解包 req_payload是消息主体
			
			if(result == 0)
			{
				char *data_ptr = NULL;
				
				UsartPrintf(USART_DEBUG, "topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
																	cmdid_topic, topic_len, req_payload, req_len);
				
				data_ptr = strstr(cmdid_topic, "request/");									//查找cmdid
				if(data_ptr)
				{
					char topic_buf[80], cmdid[40];
					
					data_ptr = strchr(data_ptr, '/');
					data_ptr++;
					
					memcpy(cmdid, data_ptr, 36);											//复制cmdid
					cmdid[36] = 0;
					
					snprintf(topic_buf, sizeof(topic_buf), "$thing/up/action/RHEB0REAN2/d11");
					OneNET_Publish(topic_buf, "ojbk");										//回复命令
				}
			}
			
		case MQTT_PKT_PUBACK:														//发送Publish消息，平台回复的Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				UsartPrintf(USART_DEBUG, "Tips:	MQTT Publish Send OK\r\n");
			
		break;
		
		case MQTT_PKT_SUBACK:																//发送Subscribe消息的Ack
		
			if(MQTT_UnPacketSubscribe(cmd) == 0)
				UsartPrintf(USART_DEBUG, "Tips:	MQTT Subscribe OK\r\n");
			else
				UsartPrintf(USART_DEBUG, "Tips:	MQTT Subscribe Err\r\n");
		
		break;
		
		default:
			result = -1;
		break;
	}
	
	ESP8266_Clear();									//清空缓存
	
	if(result == -1)
		return;
	//对接收到的数据进行处理  "params":{"LED":1} 或者 "params":{"temp":0,"humi":0,"LED":1,"LED2":1}

	dataPtr = strstr((char *)req_payload, "params");
	dataPtr = strchr(req_payload, ':');					//搜索':'
	if(dataPtr != NULL && result != -1)					//如果找到了
	{
		if(strstr((char *)req_payload, "\"LED\":1"))		//搜索"redled"
		{
			led_on();
		}else if(strstr((char *)req_payload, "\"LED\":0"))
		{
			led_off();
		}

		
		if((p=strstr((char *)req_payload, "temp_limit"))!=NULL)		//搜索"temp_limit":12
		{
			//设置温度上限
			if((p=strstr(p,":"))!=NULL)
			{
				memset(numBuf,'\0',10);
				num = 0;				
				p++;
				while(*p>='0' && *p<='9')
				{
					numBuf[num++] = *p++;
				}
				numBuf[num] = '\0';
				num = atoi(numBuf);
				temp_limit = num;
			}
		}
		if((p=strstr((char *)req_payload, "water_limit"))!=NULL)		//搜索"water_limit":12
		{
			//设置水质阈值
			if((p=strstr(p,":"))!=NULL)
			{
				memset(numBuf,'\0',10);
				num = 0;				
				p++;
				while(*p>='0' && *p<='9')
				{
					numBuf[num++] = *p++;
				}
				numBuf[num] = '\0';
				num = atoi(numBuf);
				water_limit = num;
			}
		}
		if((p=strstr((char *)req_payload, "led_limit"))!=NULL)		//搜索"led_limit":12
		{
			//设置光照阈值
			if((p=strstr(p,":"))!=NULL)
			{
				memset(numBuf,'\0',10);
				num = 0;				
				p++;
				while(*p>='0' && *p<='9')
				{
					numBuf[num++] = *p++;
				}
				numBuf[num] = '\0';
				num = atoi(numBuf);
				led_limit = num;
			}
		}
		
		/*
		if((p=strstr((char *)req_payload, "temp_up"))!=NULL)		//搜索"temp_up":12
		{
			//设置温度上限
			if((p=strstr(p,":"))!=NULL)
			{
				memset(numBuf,'\0',10);
				num = 0;				
				p++;
				while(*p>='0' && *p<='9')
				{
					numBuf[num++] = *p++;
				}
				numBuf[num] = '\0';
				num = atoi(numBuf);
				temp_up = num;
			}
		}
		if((p=strstr((char *)req_payload, "temp_down"))!=NULL)		//搜索"temp_down":20
		{
			//设置温度下限
			if((p=strstr(p,":"))!=NULL)
			{
				memset(numBuf,'\0',10);
				num = 0;				
				p++;
				while(*p>='0' && *p<='9')
				{
					numBuf[num++] = *p++;
				}
				numBuf[num] = '\0';
				num = atoi(numBuf);
				temp_down = num;
			}
		}
		*/
		
	}

	
	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}
