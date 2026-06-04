/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */
#include "stm32f1xx_hal.h"
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);

  /*Configure GPIO pin : PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */
//uint8_t DS18B20_ReadByte(uint8_t reg)  
//{
//	uint8_t data;
//	datawire(1);
//	HAL_Delay(4);
//	datawire(0);
//	HAL_Delay(60);
//	
//	for(int i = 0; i<8;i++)
//	{
//		datawire(1);
//		HAL_Delay(1);
//		data>>=1;
//		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)== SET)
//		{
//			data |=0x01;
//		}
//		datawire(0);
//		HAL_Delay(60);
//	}
//	return data;
//}

//float DS18B20_Read_Tempture()
//{
//		uint8_t tempL,tempH;
//		float temp;
//		tempL = DS18B20_ReadByte(DS18B20_REG_TEMP);
//		tempH = DS18B20_ReadByte(DS18B20_REG_TEMP+1);
//		temp = tempH<<8 | tempL;
//		temp *=0.0625;
//return 	temp;
//}

/* USER CODE END 2 */
