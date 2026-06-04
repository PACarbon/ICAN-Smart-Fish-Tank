/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "user.h"
#include "OLED.h"
#include "DS18B20.h"
#include "PID.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RX_SIZE  256//最大缓冲数组
// #define RXBUFFERSIZE  256
//char RxBuffer[RXBUFFERSIZE]; 
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//uint8_t usernum = 0;//用户选择操作缓冲数
//volatile float target_tempture = 0;//目标温度
//volatile float tempture = 0;//当前温度
//extern uint8_t arxbuffer;//缓冲数组
static int kp,ki,target_temp,ntemp;//比例系数，积分系数,目标温度，当前温度

extern uint8_t RX_Buff[];//接收数据缓冲
extern uint8_t RX_Len;//接收数据长度
extern uint8_t RX_data[];//接收数据缓存
volatile int target;
uint8_t usernum = 0;//用户控制缓冲变量
volatile int tempture,setpwm;//当前温度,设置pwm

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	int temp=0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
	 PID_Init();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
//	HAL_UART_Receive_IT(&huart1, (uint8_t *)&arxbuffer, 1);
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);//开启串口空闲中断
//	USAR_UART_IDLECallback(&huart1,RX_SIZE);
	HAL_UART_Receive_DMA(&huart1,RX_Buff,RX_SIZE);//开启DMA接收
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);//开启pwm通道
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
		oled_init();
		HAL_Delay(500);
		user_Init();
		target = 28;
		
	 while(DS18B20_Init())
		{
		
		printf("mechine checked failed!!!\r\n");
		HAL_Delay(500);
		} 
		printf("mechine checked success!!!\r\n");

		
		
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */


		if(usernum)
		{	
			
			tempture = DS18B20_Get_Temperature()/10;
			setpwm = calculate_tempture(target,tempture);
			delay_ms_tim(10);
		__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_1,setpwm);
			user_show(target);
			delay_ms_tim(2000);		
		}

//		user_set();
//		switch(usernum)
//		{
//			case 1:  user_set();break;
//			case 2:  user_show(target_tempture);break;
//			case 3:	 user_show(target_tempture);break;
////		
//		}
		
//		if(usernum == 1)
//		{
//			user_set();
//		break;
//		}
//		else if(usernum == 2)
//		{
//			user_show(target_tempture);
////		float t=DS18B20_Get_Temperature()/10;
////		if(t<0)
////			{
////			t=-t;
////			}		
////		
////		printf("Tempture: %.2f C\r\n",t);
//   }
//		else
//		{
//			user_set2();
//		
//		}
	}

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

