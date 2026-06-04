/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
//#define RXBUFFER_SIZE  256//最大接收字节
//char RXBUFFER[RXBUFFER_SIZE] = {0};//接收数据
//uint8_t arxbuffer = 0;//接收数据缓冲
//uint8_t rxbuffer_cnt = 0;//接收缓冲计数
#define RX_SIZE  256//最大缓冲数组

uint8_t RX_Buff[RX_SIZE];//接收数据缓冲
uint8_t RX_Len;//接收数据长度
uint8_t RX_data[RX_SIZE];//接收数据缓存
extern volatile int target;
extern uint8_t usernum;

extern uint8_t tempture;

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
  /* USER CODE END USART1_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* USART1_RX Init */
    hdma_usart1_rx.Instance = DMA1_Channel5;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_NORMAL;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);

    /* USART1_TX Init */
    hdma_usart1_tx.Instance = DMA1_Channel4;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart1_tx);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */
		
  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/**
  * 函数功能: 重定向c库函数printf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}
 
/**
  * 函数功能: 重定向c库函数getchar,scanf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int fgetc(FILE *f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&huart1, &ch, 1, 0xffff);
  return ch;
}

void USAR_UART_IDLECallback(UART_HandleTypeDef *huart,uint8_t rxlen)
{
	uint8_t  i = 0,j = 0;
	if(huart == &huart1)//判断是否为串口1发生中断
	memcpy(RX_data,RX_Buff,rxlen);//将缓冲区数据复制到接收数据数组中
	HAL_UART_Transmit_DMA(&huart1,RX_Buff,rxlen);//将接收到的数据发送到上位机
	rxlen = 0;
	HAL_UART_Receive_DMA(&huart1,RX_Buff,RX_SIZE);//重新打开DMA中断
	
	//在回调函数中进行，用户输入判断
	i = atoi(RX_data);
	HAL_UART_Transmit(&huart1, &i, 1, HAL_MAX_DELAY);
	 if(i == 2)
	{
				if(j == 0)
				{
					user_show(target);				
					j++;
				}
				else if(j == 1)
				{
					user_show(target);
					j = 0;
				}
				usernum = 1;
	}
	else if(i == 3)
	{
			target++;
			user_set();
			usernum = 0;
	
	}
	
	else if(i == 4)
	{
		target--;
		user_set();
		usernum = 0;
	}
	else if(i == 5)
	{
		user_set();
		usernum = 0;
	}
	else if(i == 6)
	{
		user_Init();
		usernum = 0;		
	}
	
	

	
//	if(i == 1)
//	{
//		if(j == 1)
//		{
//			oled_clear();
//			oled_show_string(40,0,"welcome",2);
//			j = 3;
//		}
//		if(j == 0)
//		{
//			user_set();
//			j++;
//		}
//		if(j == 3)
//		{
//			j = 0 ;
//		}
//			
//	}
//	
//	
//	else if(i == 2)
//	{
//		if(j == 1)
//		{
//			j = 0;
//		}
//		if(j == 0)
//		{
//			user_show(target);
//			j++;
//		} 
//	}
//	
//	else
//	{
//		oled_clear();
//		oled_show_string(40,0,"error",2);
//		j++;
//	}	
}

void Serial_SendByte(uint8_t Byte)
{
    // 使用 HAL_UART_Transmit 函数发送数据
    HAL_UART_Transmit(&huart1, &Byte, 1, HAL_MAX_DELAY);
}

void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(String[i]);
	}
}

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *uartHandle)
//{
//		UNUSED(huart1);
//	extern volatile uint8_t usernum;
//	extern volatile float target_tempture;
//	//判断接收数据是否溢出
//	if(rxbuffer_cnt >=255)
//	{
//		rxbuffer_cnt = 0;
//		memset(RXBUFFER,0x00,sizeof(RXBUFFER));//溢出清零
//		HAL_UART_Transmit(&huart1,(uint8_t*)"数据溢出",10,0xFFFF);//向上位机发送数据溢出
//	}
//	else
//	{
//		
//		RXBUFFER[rxbuffer_cnt++] = arxbuffer;
////		int i = atoi(RXBUFFER);
//		if(arxbuffer == '1')
//		{	
//			usernum =1;
//		
//		}
//		else if(arxbuffer == '2')
//		{
//			usernum =2;
//		}
//		else 
//		{
//			 
//			usernum = 3;
//		}
// 
//		
//		if((RXBUFFER[rxbuffer_cnt-1] == 0x00)&&(RXBUFFER[rxbuffer_cnt-2] == 0x0D))//判断结束标志
//		{
//			while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX);//检测UART发送结束
//			rxbuffer_cnt=0;
//			memset(RXBUFFER,0x00,sizeof(RXBUFFER));//清空数组
//			
//		}
//			
//	}
//		HAL_UART_Receive_IT(&huart1, (uint8_t *)&arxbuffer, 1);   //因为接收中断使用了一次即关闭，所以在最后加入这行代码即可实现无限使用
//}

/* USER CODE END 1 */
