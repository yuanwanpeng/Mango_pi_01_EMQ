
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "fatfs.h"

/* USER CODE BEGIN Includes */
#include "LED.h"
#include "button.h"
#include "BUZZER.h"
#include "LCD12864.h"
#include "Strategy.h"
#include <string.h>
#include "Check_Stroking_Cycle.h"
#include "HUMI.h"
#include "24c0x.h"
#include "relay.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_tx;
DMA_HandleTypeDef hdma_usart2_rx;

osThreadId defaultTaskHandle;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
osThreadId Start_LED_TaskHandle;		//LED������
osThreadId Start_HUMI_TaskHandle;		//�¶�������
osThreadId Start_RTC_TaskHandle;		//ʵʱʱ��������
osThreadId Start_Delete_TaskHandle;		//ɾ��������
osThreadId Start_Strategy_TaskHandle;
osThreadId Start_LCD12864_TaskHandle;	//12864���
osThreadId Start_BUTTON_TaskHandle;		//ɨ�谴��������
Delete_Task_struct G_Delete_Task_struct;//ɾ������Ľṹ��
SemaphoreHandle_t xSemaphore;

#ifdef SIM800C_VERSION
osThreadId Start_SIM800C_TaskHandle;	//800c���
#elif ESP8266_VERSION
osThreadId Start_ESP8266_TaskHandle;	//8266���
#endif
//Ӳ���汾��Ϣ�ṹ��

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_RTC_Init(void);
static void MX_USART1_UART_Init(void);
void StartDefaultTask(void const * argument);
static void MX_NVIC_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
 {
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  MX_RTC_Init();
  MX_USART1_UART_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
	
  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
 

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* USART1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
}

/* RTC init function */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef DateToUpdate;

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

    /**Initialize RTC Only 
    */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initialize RTC and set the Time and Date 
    */
  sTime.Hours = 0;
  sTime.Minutes = 0;
  sTime.Seconds = 0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
  DateToUpdate.Month = RTC_MONTH_JANUARY;
  DateToUpdate.Date = 1;
  DateToUpdate.Year = 0;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* SPI1 init function */
static void MX_SPI1_Init(void)
{

  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

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
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART2 init function */
static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, BACKLIGHT_Pin|SDA_24C0X_Pin|OUTC3_Pin|OUTC2_Pin 
                          |OUTC1_Pin|SCL_LCD_Pin|A0_LCD_Pin|RST_LCD_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SDA_AM2320_Pin|SCL_AM2320_Pin|SCL_24C0X_Pin|LED_REV_Pin 
                          |LED_FWD_Pin|MCU_RUN_LED_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, PWKEY_CTL_Pin|Buzzer_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SDA_LCD_GPIO_Port, SDA_LCD_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_LCD_GPIO_Port, CS_LCD_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : BACKLIGHT_Pin OUTC3_Pin OUTC2_Pin OUTC1_Pin */
  GPIO_InitStruct.Pin = BACKLIGHT_Pin|OUTC3_Pin|OUTC2_Pin|OUTC1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : SDA_AM2320_Pin SCL_AM2320_Pin SCL_24C0X_Pin */
  GPIO_InitStruct.Pin = SDA_AM2320_Pin|SCL_AM2320_Pin|SCL_24C0X_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PWKEY_CTL_Pin Buzzer_Pin LED_REV_Pin LED_FWD_Pin 
                           MCU_RUN_LED_Pin */
  GPIO_InitStruct.Pin = PWKEY_CTL_Pin|Buzzer_Pin|LED_REV_Pin|LED_FWD_Pin 
                          |MCU_RUN_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : SDA_24C0X_Pin SCL_LCD_Pin A0_LCD_Pin RST_LCD_Pin */
  GPIO_InitStruct.Pin = SDA_24C0X_Pin|SCL_LCD_Pin|A0_LCD_Pin|RST_LCD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : IN1_Pin B_IN4_Pin B_IN3_Pin */
  GPIO_InitStruct.Pin = IN1_Pin|B_IN4_Pin|B_IN3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SDA_LCD_Pin */
  GPIO_InitStruct.Pin = SDA_LCD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SDA_LCD_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CS_LCD_Pin */
  GPIO_InitStruct.Pin = CS_LCD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(CS_LCD_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : IN2_Pin B_IN2_Pin B_IN1_Pin */
  GPIO_InitStruct.Pin = IN2_Pin|B_IN2_Pin|B_IN1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
PUTCHAR_PROTOTYPE
{
	uint8_t c = ch;
	taskENTER_CRITICAL();			//�����ٽ���
	HAL_UART_Transmit(&huart1, &c, 1, 0xff);
	taskEXIT_CRITICAL();//�˳��ٽ���
	//HAL_UART_Transmit_DMA(&huart1, &c, 1);
	return 0;
}

/*
 * ����ڲ�������Ϣ
 */
void Check_Strategy(void)
{
	uint8_t i;
	Strategy_struct pBuffer[8];
	//Init_Strategy(0);//�����ڹ�������Ҫ����Ҫ��д��0��ʼд
	for(i = 0;i < 8;i++)
	{
		AT24CXX_Read(i*STRATEGY_ADDR,((uint8_t*)pBuffer)+(sizeof(pBuffer[i])*i),sizeof(pBuffer[i]));
		//printf("pBuffer[%d].Strategy_Num = %d\r\n",i+1,pBuffer[i].Strategy_Num);
		if(pBuffer[i].Strategy_Num != (i+1)){
			Init_Strategy(0);//�����ڹ�������Ҫ����Ҫ��д ��0��ʼд
			return;
		}
	}
	for(i = 0;i < 8;i++)
	{
		AT24CXX_Read(i*STRATEGY_ADDR,((uint8_t*)G_Strategy)+(sizeof(G_Strategy[i])*i),sizeof(G_Strategy[i]));
//		printf("[%d]Hours=%d\r\n",i,G_Strategy[i].Time.Hours);
//		printf("[%d]Minutes=%d\r\n",i,G_Strategy[i].Time.Minutes);
	}
	//printf("sizeof(S_Strategy) = %d,sizeof(S_Strategy[0]) = %d\r\n",sizeof(S_Strategy),sizeof(S_Strategy[0]));
	memcpy((uint8_t*)S_Strategy,(uint8_t*)G_Strategy,sizeof(S_Strategy));
	return;
}

/*
 * ɾ�����������
 * ��������⣬���������Ѿ�ִ��ò��û������������ɾ���Լ��������ܻ�����Դ��������ʹ�������ɾ�������ѵķ�ʽ
 * �ص�����������ô����
 * */
void Start_Delete_Task(void const * argument)
{
	while(1)
	{
		if(G_Delete_Task_struct.sign){
			osThreadTerminate(G_Delete_Task_struct.D_Task);
			G_Delete_Task_struct.sign = DISABLE;
		}
		osDelay(5);
	}
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for FATFS */
  MX_FATFS_Init();

  /* USER CODE BEGIN 5 */
	xSemaphore = xSemaphoreCreateBinary();
	if( xSemaphore == NULL )
	{
		printf("xSemaphore = NULL\r\n");
	}
	else
	{
	//	printf("xSemaphore = success\r\n");
	}
	
	osDelay(100);
	Check_Strategy();				//�������Ƿ�����
	Init_Humi_Comp();				//��ʼ���¶Ȳ���
	Check_Max_And_Now_Route();		//�������г̺͵�ǰ�г�
	Check_Delay_Inspection();		//��ʱ���ʱ��
	Check_Stroking_Cycle();			//��ǰ�ɻɹ�����
	Check_Board_Info();				//��������Ϣ
	taskENTER_CRITICAL();			//�����ٽ���


	osThreadDef(LCD12864_Task, Start_LCD12864_Task, osPriorityNormal, 0, 128);	//����LCD12864ģ�鿪ʼ
	Start_LCD12864_TaskHandle = osThreadCreate(osThread(LCD12864_Task), NULL);	//

	osThreadDef(Led_Task, Start_LED_Task, osPriorityNormal, 0, 64);				//����Led��ʼ����
	Start_LED_TaskHandle = osThreadCreate(osThread(Led_Task), NULL);

	osThreadDef(Button_Task, Start_Button_Task, osPriorityNormal, 0, 64);	//����������ʼ����
	Start_BUTTON_TaskHandle = osThreadCreate(osThread(Button_Task), NULL);

#ifdef SIM800C_VERSION
	osThreadDef(Sim800c_Task, Start_Sim800c_Task, osPriorityNormal, 0, 256);		//����sim800c��ʼ����
	Start_SIM800C_TaskHandle = osThreadCreate(osThread(Sim800c_Task), NULL);
#endif
	osThreadDef(HUMI_Task, Start_HUMI_Task, osPriorityNormal, 0, 64);			//�����¶Ȳɼ�ģ��
	Start_HUMI_TaskHandle = osThreadCreate(osThread(HUMI_Task), NULL);

	osThreadDef(Relay_Task, Start_Relay_Task, osPriorityHigh, 0, 64);			//�����̵���ģ��
	Start_Relay_TaskHandle = osThreadCreate(osThread(Relay_Task), NULL);

	osThreadDef(Delete_Task, Start_Delete_Task, osPriorityNormal, 0, 64);		//ɾ������ģ��  �Ҹ�����������
	Start_Delete_TaskHandle = osThreadCreate(osThread(Delete_Task), NULL);

	osThreadDef(Strategy_Task, Start_Strategy_Task, osPriorityNormal, 0, 128);	//��������ģ��
	Start_Strategy_TaskHandle = osThreadCreate(osThread(Strategy_Task), NULL);

	taskEXIT_CRITICAL();//�˳��ٽ���
	
  /* Infinite loop */
  for(;;)
  {	
		osDelay(100);
  }
  /* USER CODE END 5 */ 
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
		printf("_Error_Handler");
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
