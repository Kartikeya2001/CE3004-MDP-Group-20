/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim8;

UART_HandleTypeDef huart3;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = { .name = "defaultTask",
		.stack_size = 128 * 4, .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for ShowTask */
osThreadId_t ShowTaskHandle;
const osThreadAttr_t ShowTask_attributes = { .name = "ShowTask", .stack_size =
		128 * 4, .priority = (osPriority_t) osPriorityLow, };
/* Definitions for MotorTask */
osThreadId_t MotorTaskHandle;
const osThreadAttr_t MotorTask_attributes = { .name = "MotorTask", .stack_size =
		128 * 4, .priority = (osPriority_t) osPriorityLow, };
/* Definitions for EncoderTask */
osThreadId_t EncoderTaskHandle;
const osThreadAttr_t EncoderTask_attributes = { .name = "EncoderTask",
		.stack_size = 128 * 4, .priority = (osPriority_t) osPriorityLow, };
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM8_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM4_Init(void);
void StartDefaultTask(void *argument);
void show(void *argument);
void motors(void *argument);
void encoder_task(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t aRxBuffer[1];

uint8_t storeInstruc[256];
int count = 0;
uint16_t pwnVal = 0;
uint32_t local_time, sensor_time;
uint32_t distance_sensor;

uint32_t IC_Val1 = 0;
uint32_t IC_Val2 = 0;
uint32_t Difference = 0;
uint32_t Distance = 0;
uint8_t Is_First_Captured = 0;
/* USER CODE END 0 */
#define TRIG_PIN GPIO_PIN_13
#define TRIG_PORT GPIOD

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

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
	MX_TIM8_Init();
	MX_TIM2_Init();
	MX_TIM1_Init();
	MX_USART3_UART_Init();
	MX_TIM4_Init();
	/* USER CODE BEGIN 2 */
	OLED_Init();
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);

	HAL_UART_Receive_IT(&huart3, (uint8_t*) aRxBuffer, 1);

	/* USER CODE END 2 */

	/* Init scheduler */
	osKernelInitialize();

	/* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
	/* USER CODE END RTOS_MUTEX */

	/* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
	/* USER CODE END RTOS_SEMAPHORES */

	/* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
	/* USER CODE END RTOS_TIMERS */

	/* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	/* USER CODE END RTOS_QUEUES */

	/* Create the thread(s) */
	/* creation of defaultTask */
	defaultTaskHandle = osThreadNew(StartDefaultTask, NULL,
			&defaultTask_attributes);

	/* creation of ShowTask */
	ShowTaskHandle = osThreadNew(show, NULL, &ShowTask_attributes);

	/* creation of MotorTask */
	MotorTaskHandle = osThreadNew(motors, NULL, &MotorTask_attributes);

	/* creation of EncoderTask */
	EncoderTaskHandle = osThreadNew(encoder_task, NULL,
			&EncoderTask_attributes);

	/* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
	/* USER CODE END RTOS_THREADS */

	/* USER CODE BEGIN RTOS_EVENTS */
	/* add events, ... */
	/* USER CODE END RTOS_EVENTS */

	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */
	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief TIM1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM1_Init(void) {

	/* USER CODE BEGIN TIM1_Init 0 */

	/* USER CODE END TIM1_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };
	TIM_OC_InitTypeDef sConfigOC = { 0 };
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = { 0 };

	/* USER CODE BEGIN TIM1_Init 1 */

	/* USER CODE END TIM1_Init 1 */
	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 160;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 1000;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim1) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_TIM_PWM_Init(&htim1) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4)
			!= HAL_OK) {
		Error_Handler();
	}
	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.DeadTime = 0;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM1_Init 2 */

	/* USER CODE END TIM1_Init 2 */
	HAL_TIM_MspPostInit(&htim1);

}

/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void) {

	/* USER CODE BEGIN TIM2_Init 0 */

	/* USER CODE END TIM2_Init 0 */

	TIM_Encoder_InitTypeDef sConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM2_Init 1 */

	/* USER CODE END TIM2_Init 1 */
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 0;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 65535;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
	sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
	sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC1Filter = 10;
	sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
	sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC2Filter = 10;
	if (HAL_TIM_Encoder_Init(&htim2, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM2_Init 2 */

	/* USER CODE END TIM2_Init 2 */

}

/**
 * @brief TIM4 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM4_Init(void) {

	/* USER CODE BEGIN TIM4_Init 0 */

	/* USER CODE END TIM4_Init 0 */

	TIM_MasterConfigTypeDef sMasterConfig = { 0 };
	TIM_IC_InitTypeDef sConfigIC = { 0 };

	/* USER CODE BEGIN TIM4_Init 1 */

	/* USER CODE END TIM4_Init 1 */
	htim4.Instance = TIM4;
	htim4.Init.Prescaler = 8;
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Init.Period = 0xffff - 1;
	htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_IC_Init(&htim4) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	sConfigIC.ICFilter = 0;
	if (HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM4_Init 2 */

	/* USER CODE END TIM4_Init 2 */

}

/**
 * @brief TIM8 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM8_Init(void) {

	/* USER CODE BEGIN TIM8_Init 0 */

	/* USER CODE END TIM8_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };
	TIM_OC_InitTypeDef sConfigOC = { 0 };
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = { 0 };

	/* USER CODE BEGIN TIM8_Init 1 */

	/* USER CODE END TIM8_Init 1 */
	htim8.Instance = TIM8;
	htim8.Init.Prescaler = 0;
	htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim8.Init.Period = 7199;
	htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim8.Init.RepetitionCounter = 0;
	htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim8) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_TIM_PWM_Init(&htim8) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_1)
			!= HAL_OK) {
		Error_Handler();
	}
	if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_2)
			!= HAL_OK) {
		Error_Handler();
	}
	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.DeadTime = 0;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM8_Init 2 */

	/* USER CODE END TIM8_Init 2 */

}

/**
 * @brief USART3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART3_UART_Init(void) {

	/* USER CODE BEGIN USART3_Init 0 */

	/* USER CODE END USART3_Init 0 */

	/* USER CODE BEGIN USART3_Init 1 */

	/* USER CODE END USART3_Init 1 */
	huart3.Instance = USART3;
	huart3.Init.BaudRate = 115200;
	huart3.Init.WordLength = UART_WORDLENGTH_8B;
	huart3.Init.StopBits = UART_STOPBITS_1;
	huart3.Init.Parity = UART_PARITY_NONE;
	huart3.Init.Mode = UART_MODE_TX_RX;
	huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart3.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart3) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART3_Init 2 */

	/* USER CODE END USART3_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOE,
	OLED_SCL_Pin | OLED_SDA_Pin | OLED_RST_Pin | OLED_DC_Pin | LED3_Pin,
			GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, AIN2_Pin | AIN1_Pin | BIN1_Pin | BIN2_Pin,
			GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);

	/*Configure GPIO pins : OLED_SCL_Pin OLED_SDA_Pin OLED_RST_Pin OLED_DC_Pin
	 LED3_Pin */
	GPIO_InitStruct.Pin = OLED_SCL_Pin | OLED_SDA_Pin | OLED_RST_Pin
			| OLED_DC_Pin | LED3_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/*Configure GPIO pins : AIN2_Pin AIN1_Pin BIN1_Pin BIN2_Pin */
	GPIO_InitStruct.Pin = AIN2_Pin | AIN1_Pin | BIN1_Pin | BIN2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : PD13 */
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	/*Prevent unused argument(s) compilation warning */
	UNUSED(huart);
//
//	HAL_UART_Transmit(&huart3,(uint8_t *)aRxBuffer,10,0xFFFF);

}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument) {
	/* USER CODE BEGIN 5 */
	/* Infinite loop */
	//uint8_t ch = 'A';
	aRxBuffer[0] = '0';
	for (;;) {

		//HCSR04_Read();

		HAL_UART_Receive_IT(&huart3, (uint8_t*) aRxBuffer, 1);

		//HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);

		//    sensor_time = hcsr04_read();
		//    distance_sensor  = sensor_time * 0.034/2; //1 at (approx) 7cm, 2 and 14cm for 0.034; 1 at 5.5cm for 0.045/2

		if (aRxBuffer[0] != '0') {
			storeInstruc[count] = aRxBuffer[0];
			count++;
			aRxBuffer[0] = '0';
		}

		osDelay(100);

	}
	/* USER CODE END 5 */
}

/* USER CODE BEGIN Header_show */
/**
 * @brief Function implementing the ShowTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_show */
void show(void *argument) {
	/* USER CODE BEGIN show */
	uint8_t hello[20] = "Hello World!\0";
	/* Infinite loop */
	for (;;) {
		sprintf(hello, "%s\0", storeInstruc);
		OLED_ShowString(10, 40, hello);

		HCSR04_Read();
		osDelay(100);

		OLED_Refresh_Gram();
//	      osDelay(100);
	}
	/* USER CODE END show */
}

/* USER CODE BEGIN Header_motors */
/**
 * @brief Function implementing the MotorTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_motors */
void motors(void *argument) {
	/* USER CODE BEGIN motors */
	int leftCounter = 0;
	int rightCounter = 0;
	int backwardCounter = 0;
	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
	/* Infinite loop */
	for (;;) {
		//BACKWARD
//		switch ('q') {
		switch (storeInstruc[0]) {
		case 'f': //forward
			HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);

			HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);

			//      htim1.Instance->CCR4 = 85; // Set to Right
			//      osDelay(1000);
//			if(rightCounter <= 2){
//				htim1.Instance->CCR4 = 76; // Set to Center (Slightly Right)
//				osDelay(500);
//				rightCounter++;
//				if(rightCounter == 2){
//					leftCounter = 0;
//				}
//			} else if (leftCounter <= 1){
//				htim1.Instance->CCR4 = 75; // Set to Center (Slightly Right)
//				osDelay(500);
//				leftCounter++;
//				if(leftCounter == 1){
//					rightCounter = 0;
//				}
//			}
//
			htim1.Instance->CCR4 = 76; // Set to Center (Slightly Right)
			osDelay(500);

			pwnVal = 1200;
			// modify the comparison value for the duty cycle
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

			osDelay(650);

			pwnVal = 0;
			// modify the comparison value for the duty cycle
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

			HAL_UART_Transmit(&huart3, (uint8_t*) &storeInstruc, 1, 0xFFFF);
			memmove(storeInstruc, storeInstruc + 1,
					(sizeof(storeInstruc) - 1) / sizeof(storeInstruc[0]));
			count--;

			break;

			//    case '2': // Turn right 90
			//      HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
			//      HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);
			//
			//      HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
			//      HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);
			//
			//      htim1.Instance->CCR4 = 76; //Center
			//      osDelay(1000);
			//      htim1.Instance->CCR4 = 102; // Right
			//
			//      pwnVal = 1200;
			//      // modify the comparison value for the duty cycle
			//      __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
			//      __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);
			//      osDelay(3250); //Center
			//
			//      //    uint8_t seconds =
			//      //    while()
			//
			//      pwnVal = 0;
			//      __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
			//      __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);
			//
			//      memmove(storeInstruc, storeInstruc + 1,
			//          (sizeof(storeInstruc) - 1) / sizeof(storeInstruc[0]));
			//      count--;
			//      HAL_UART_Transmit(&huart3, (uint8_t*) &storeInstruc, 1, 0xFFFF);
			//
			//      break;
			//
			//    case '3': //Turn Left 90
			//
			//      HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
			//      HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);
			//
			//      HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
			//      HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);
			//
			//      htim1.Instance->CCR4 = 76; //Center
			//      osDelay(1000);
			//      htim1.Instance->CCR4 = 59; //Left
			//      pwnVal = 1200;
			//
			//      // modify the comparison value for the duty cycle
			//      __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
			//      __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);
			//
			//      osDelay(3750); //Center (Need to Test this delay value)
			//
			//      pwnVal = 0;
			//      __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
			//      __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);
			//
			//      memmove(storeInstruc, storeInstruc + 1,
			//          (sizeof(storeInstruc) - 1) / sizeof(storeInstruc[0]));
			//      count--;
			//      HAL_UART_Transmit(&huart3, (uint8_t*) &storeInstruc, 1, 0xFFFF);
			//
			//      break;
			//    case '4': // Turn right 360
			//      HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
			//      HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);
			//
			//      HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
			//      HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);
			//
			//      htim1.Instance->CCR4 = 76; //Center
			//      osDelay(1000);
			//      htim1.Instance->CCR4 = 100; // Right
			//      pwnVal = 1200;
			//
			//      // modify the comparison value for the duty cycle
			//      __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
			//      __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);
			//      osDelay(13000);
			//      //    uint8_t seconds =
			//      //    while()
			//      pwnVal = 0;
			//
			//      __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
			//      __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);
			//
			//      memmove(storeInstruc, storeInstruc + 1,
			//          (sizeof(storeInstruc) - 1) / sizeof(storeInstruc[0]));
			//      count--;
			//      HAL_UART_Transmit(&huart3, (uint8_t*) &storeInstruc, 1, 0xFFFF);
			//
			//      break;
			//    case '5': //Turn left 360
			//
			//      HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
			//      HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);
			//
			//      HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
			//      HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);
			//
			//      htim1.Instance->CCR4 = 76; //Center
			//      osDelay(1000);
			//      htim1.Instance->CCR4 = 59; // Left
			//      pwnVal = 1200;
			//      // modify the comparison value for the duty cycle
			//      __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
			//      __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);
			//      osDelay(14250);
			//      //    uint8_t seconds =
			//      //    while()
			//      pwnVal = 0;
			//      __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
			//      __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);
			//
			//      memmove(storeInstruc, storeInstruc + 1,
			//          (sizeof(storeInstruc) - 1) / sizeof(storeInstruc[0]));
			//      count--;
			//      HAL_UART_Transmit(&huart3, (uint8_t*) &storeInstruc, 1, 0xFFFF);
			//
			//      break;
		case 'r': //right on the spot
			turnRightLittle();

			HAL_UART_Transmit(&huart3, (uint8_t*) &storeInstruc, 1, 0xFFFF);
			memmove(storeInstruc, storeInstruc + 1,
					(sizeof(storeInstruc) - 1) / sizeof(storeInstruc[0]));
			count--;

			break;

		case 'l': // left on the spot
			turnLeftLittle();

			HAL_UART_Transmit(&huart3, (uint8_t*) &storeInstruc, 1, 0xFFFF);
			memmove(storeInstruc, storeInstruc + 1,
					(sizeof(storeInstruc) - 1) / sizeof(storeInstruc[0]));
			count--;
			break;
		case 'b': // backwards

			HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_RESET);

			HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_RESET);

			htim1.Instance->CCR4 = 76; // Set to Center (Slightly Right)
			osDelay(500);

			pwnVal = 1200;
			// modify the comparison value for the duty cycle
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

			osDelay(640);

			pwnVal = 0;
			// modify the comparison value for the duty cycle
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

			HAL_UART_Transmit(&huart3, (uint8_t*) &storeInstruc, 1, 0xFFFF);
			memmove(storeInstruc, storeInstruc + 1,
					(sizeof(storeInstruc) - 1) / sizeof(storeInstruc[0]));

			count--;

			break;

		case 'm': // Maneuver
			//START TURN RIGHT
			turnRight();
			//END TURN RIGHT ON THE SPOT

			//START GO FORWARD
			HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);

			HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);

			//      htim1.Instance->CCR4 = 85; // Set to Right
			//      osDelay(1000);
			htim1.Instance->CCR4 = 76; // Set to Center (Slightly Right)
			osDelay(500);

			pwnVal = 1200;
			// modify the comparison value for the duty cycle
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

			osDelay(2500);

			pwnVal = 0;
			// modify the comparison value for the duty cycle
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);
			//END GO FORWARD

			//START TURN LEFT
			turnLeft();
			//END TURN LEFT

			//START GO FORWARD 2
			HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);

			HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);
			//      htim1.Instance->CCR4 = 85; // Set to Right
			//      osDelay(1000);
			htim1.Instance->CCR4 = 76; // Set to Center (Slightly Right)
			osDelay(500);

			pwnVal = 1200;
			// modify the comparison value for the duty cycle
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

			osDelay(2250);

			pwnVal = 0;
			// modify the comparison value for the duty cycle
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);
			//END GO FORWARD 2

			//START TURN LEFT
			turnLeft();
			//END TURN LEFT

			// DONT TOUCH THIS
			HAL_UART_Transmit(&huart3, (uint8_t*) &storeInstruc, 1, 0xFFFF);
			memmove(storeInstruc, storeInstruc + 1,
					(sizeof(storeInstruc) - 1) / sizeof(storeInstruc[0]));
			count--;

			break;
		case 'M': // Maneuver
			HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);

			HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);

			//      htim1.Instance->CCR4 = 85; // Set to Right
			//      osDelay(1000);
			htim1.Instance->CCR4 = 76; // Set to Center (Slightly Right)
			osDelay(500);

			pwnVal = 1200;
			// modify the comparison value for the duty cycle
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

			osDelay(2600);

			pwnVal = 0;
			// modify the comparison value for the duty cycle
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

			//turn left
			turnLeft();

			//forward 2
			HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);

			HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);

			//      htim1.Instance->CCR4 = 85; // Set to Right
			//      osDelay(1000);
			htim1.Instance->CCR4 = 76; // Set to Center (Slightly Right)
			osDelay(500);

			pwnVal = 1200;
			// modify the comparison value for the duty cycle
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

			osDelay(2200);

			pwnVal = 0;
			// modify the comparison value for the duty cycle
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

			HAL_UART_Transmit(&huart3, (uint8_t*) &storeInstruc, 1, 0xFFFF);
			memmove(storeInstruc, storeInstruc + 1,
					(sizeof(storeInstruc) - 1) / sizeof(storeInstruc[0]));
			count--;

			break;
		case 'q':
			startInfinitely();

			turnLeft();
			turnRight();
			goStraight6m();

			returnRight();

			htim1.Instance->CCR4 = 76; // Right:100
			osDelay(300);
			HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);

			HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);

			pwnVal = 3000; //2000
			// modify the comparison value for the duty cycle
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

			osDelay(200); //150

			returnLeft();

			endInfinitely();

			memmove(storeInstruc, storeInstruc + 1,
					(sizeof(storeInstruc) - 1) / sizeof(storeInstruc[0]));
			count--;
			HAL_UART_Transmit(&huart3, (uint8_t*) &storeInstruc, 1, 0xFFFF);
			break;

		case 'Q':
//			startInfinitelyTwo();
//
//			turnLeftTwo();
//			turnRightTwo();
//			goStraight6mTwo();
//			turnRightTwo();
//			turnLeftTwo();
//
//			endInfinitelyTwo();
//
//			memmove(storeInstruc, storeInstruc + 1,
//					(sizeof(storeInstruc) - 1) / sizeof(storeInstruc[0]));
//			count--;
//			HAL_UART_Transmit(&huart3, (uint8_t*) &storeInstruc, 1, 0xFFFF);
//			break;

			startInfinitelyTwo();

			turnLeft();
			turnRight();
			goStraight6m();

			returnRight();

			htim1.Instance->CCR4 = 76; // Right:100
			osDelay(300);
			HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);

			HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);

			pwnVal = 3000; //2000
			// modify the comparison value for the duty cycle
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

			osDelay(225); //150

			returnLeft();

			endInfinitelyTwo();

			memmove(storeInstruc, storeInstruc + 1,
					(sizeof(storeInstruc) - 1) / sizeof(storeInstruc[0]));
			count--;
			HAL_UART_Transmit(&huart3, (uint8_t*) &storeInstruc, 1, 0xFFFF);
			break;
		case 'z':
			endInfinitely();
			break;
		default:
			printf("");
//			memmove(storeInstruc, storeInstruc + 1,
//								(sizeof(storeInstruc) - 1) / sizeof(storeInstruc[0]));
//			count--;
//			HAL_UART_Transmit(&huart3, (uint8_t*) &storeInstruc, 1, 0xFFFF);
			break;
		}
	}
	osDelay(100);
}

void startInfinitely() {
	htim1.Instance->CCR4 = 76;
	osDelay(300);

	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);

	while (Distance > 65) {
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 4000);
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 4000);
	}

	while (Distance > 50) {
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 2750);
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 2750);
	}

	while (Distance > 45) {
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 1500);
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 1500);
	}

	while (Distance > 35) {
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 1000);
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 1000);
	}
//
//	while (Distance > 30) {
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 1000);
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 1000);
//	}
//
//	while (Distance > 25) {
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 800);
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 800);
//	}
//
//	__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 0);
//	__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 0);
}

void endInfinitely() {
	htim1.Instance->CCR4 = 76;
	osDelay(300);

	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);

	while (Distance > 55) {
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 4500);
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 4500);
	}

	while (Distance > 45) {
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 2500);
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 2500);
	}

//	while (Distance > 35) {
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 2000);
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 2000);
//	}

//	while (Distance > 30) {
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 1000);
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 1000);
//	}

	while (Distance > 22) { //15
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 1500);
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 1500);
	}

	__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 0);
	__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 0);
}

void goStraight6m() {
	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);

	htim1.Instance->CCR4 = 76; // Set to Center (Slightly Right)
	osDelay(300);

	pwnVal = 3000; //2000
	// modify the comparison value for the duty cycle
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

	osDelay(1500);	//2000, 1850, 1700, 1600, 1500

	pwnVal = 0;
	// modify the comparison value for the duty cycle
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);
}
//void returnLeft(){
//	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
//		HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);
//
//		HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
//		HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);
//
//	//	htim1.Instance->CCR4 = 76; //Center
//	//	osDelay(500);
//		htim1.Instance->CCR4 = 58; //Left:58
//		osDelay(300);
//		pwnVal = 2000;
//
//		// modify the comparison value for the duty cycle
//		__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
//		__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);
//		osDelay(800);//2020, 1200
//		htim1.Instance->CCR4 = 76; //Center
//		osDelay(300);
//		pwnVal = 1700;
//
//		// modify the comparison value for the duty cycle
//		__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
//		__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);
//		osDelay(600);
//
//		pwnVal = 0;
//		__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
//		__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);
//}
void returnRight() {
	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);

//	htim1.Instance->CCR4 = 76; //Center
//	osDelay(500);
	htim1.Instance->CCR4 = 100; // Right:100
	osDelay(300);

	pwnVal = 2500; //2000
	// modify the comparison value for the duty cycle
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

//	osDelay(3120);
	osDelay(2450);	//2850,2650, 2550, 2450

	htim1.Instance->CCR4 = 76; //Center
	osDelay(300);

	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);
	osDelay(700);

	pwnVal = 0;
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);
}
void returnLeft() {
	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);

//	htim1.Instance->CCR4 = 76; //Center
//	osDelay(500);
	htim1.Instance->CCR4 = 58; //Left:58
	osDelay(300);
	pwnVal = 2500;

	// modify the comparison value for the duty cycle
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);
	osDelay(800); //2020, 1200, 1100, 1400, 1500, 1300, 950
//	htim1.Instance->CCR4 = 76; //Center
//	osDelay(300);
//	pwnVal = 1700;
//
//	// modify the comparison value for the duty cycle
//	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
//	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);
//	osDelay(700);
//
//	pwnVal = 0;
//	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
//	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);
}
void turnLeft() {
	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);

//	htim1.Instance->CCR4 = 76; //Center
//	osDelay(500);
	htim1.Instance->CCR4 = 58; //Left:58
	osDelay(300);
	pwnVal = 2000;

	// modify the comparison value for the duty cycle
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);
	osDelay(800); //2020, 1200
	htim1.Instance->CCR4 = 76; //Center
	osDelay(300);
	pwnVal = 2000;

	// modify the comparison value for the duty cycle
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);
	osDelay(1100);

	pwnVal = 0;
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);
}

void turnRight() {
	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);

//	htim1.Instance->CCR4 = 76; //Center
//	osDelay(500);
	htim1.Instance->CCR4 = 100; // Right:100
	osDelay(300);

	pwnVal = 2500;
	// modify the comparison value for the duty cycle
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

//	osDelay(3120);
	osDelay(2000); //2450 2100
	pwnVal = 0;
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

}

void startInfinitelyTwo() {
//	htim1.Instance->CCR4 = 76;
//	osDelay(300);
//
//	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);
//
//	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);
//
//	while (Distance > 55) {
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 4000);
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 4000);
//	}
//
//	while (Distance > 45) {
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 2500);
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 2500);
//	}
//
//	while (Distance > 35) {
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 1500);
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 1500);
//	}
//
//	while (Distance > 30) {
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 1000);
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 1000);
//	}
//
//	while (Distance > 25) {
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 800);
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 800);
//	}
//
//	__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 0);
//	__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 0);

	htim1.Instance->CCR4 = 76;
		osDelay(300);

		HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);

		HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);

		while (Distance > 75) {
			__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 5000);
			__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 5000);
		}

		while (Distance > 60) {
			__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 3050);
			__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 3050);
		}

		while (Distance > 45) {
			__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 1500);
			__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 1500);
		}

		while (Distance > 35) {
			__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 1000);
			__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 1000);
		}
	//
	//	while (Distance > 30) {
	//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 1000);
	//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 1000);
	//	}
	//
	//	while (Distance > 25) {
	//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 800);
	//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 800);
	//	}
	//
	//	__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 0);
	//	__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 0);
}

void endInfinitelyTwo() {
//	htim1.Instance->CCR4 = 76;
//	osDelay(300);
//
//	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);
//
//	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);
//
//	while (Distance > 55) {
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 4000);
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 4000);
//	}
//
//	while (Distance > 45) {
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 2500);
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 2500);
//	}
//
//	while (Distance > 35) {
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 1500);
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 1500);
//	}
//
//	while (Distance > 30) {
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 1000);
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 1000);
//	}
//
//	while (Distance > 15) {
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 800);
//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 800);
//	}
//
//	__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 0);
//	__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 0);
	htim1.Instance->CCR4 = 76;
		osDelay(300);

		HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);

		HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);

		while (Distance > 55) {
			__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 4500);
			__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 4500);
		}

		while (Distance > 45) {
			__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 2500);
			__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 2500);
		}

	//	while (Distance > 35) {
	//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 2000);
	//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 2000);
	//	}

	//	while (Distance > 30) {
	//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 1000);
	//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 1000);
	//	}

		while (Distance > 22) { //15
			__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 1500);
			__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 1500);
		}

		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 0);
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 0);
}

void goStraight6mTwo() {
	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);

	htim1.Instance->CCR4 = 76; // Set to Center (Slightly Right)
	osDelay(300);

	pwnVal = 2000;
	// modify the comparison value for the duty cycle
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

	osDelay(2000);

	pwnVal = 0;
	// modify the comparison value for the duty cycle
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);
}

void turnLeftTwo() {
	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);

//	htim1.Instance->CCR4 = 76; //Center
//	osDelay(500);
	htim1.Instance->CCR4 = 59; //Left
	osDelay(300);
	pwnVal = 1700;

	// modify the comparison value for the duty cycle
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

	osDelay(2020);

	pwnVal = 0;
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);
}

void turnRightTwo() {
	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);

//	htim1.Instance->CCR4 = 76; //Center
//	osDelay(500);
	htim1.Instance->CCR4 = 100; // Right
	osDelay(300);

	pwnVal = 2000;
	// modify the comparison value for the duty cycle
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

//	osDelay(3025);
	osDelay(3120);
	pwnVal = 0;
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

}

void turnLeftLittle() {
	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);

	htim1.Instance->CCR4 = 76; //Center
	osDelay(600);

	// forward 1
	htim1.Instance->CCR4 = 59; //left
	pwnVal = 1200;
	osDelay(600);

	// modify the comparison value for the duty cycle
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

	osDelay(800);

	//BACKWARD 1
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,0);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,0);
	htim1.Instance->CCR4 = 100; //right
	osDelay(600);

	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_RESET);

	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

	osDelay(400);

	//FORWARD 2
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,0);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,0);
	htim1.Instance->CCR4 = 76; //left
	osDelay(600);

	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);

	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

	osDelay(425);

	//BACKWARD 2
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,0);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,0);
	htim1.Instance->CCR4 = 100; // right
	osDelay(600);

	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_RESET);

	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

	osDelay(350);

	//FORWARD 3
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,0);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,0);
	htim1.Instance->CCR4 = 59; //left
	osDelay(600);

	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);

	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

	osDelay(400);

	//BACKWARD 3
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,0);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,0);
	htim1.Instance->CCR4 = 100; // right
	osDelay(600);

	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_RESET);

	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

	osDelay(475);

	//FORWARD 4
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,0);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,0);
	htim1.Instance->CCR4 = 59; //left
	osDelay(600);

	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);

	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

//		osDelay(480);
	osDelay(580); //550

	/*//BACKWARD 4
	 __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,0);
	 __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,0);
	 htim1.Instance->CCR4 = 59; // right
	 osDelay(600);

	 HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_SET);
	 HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_RESET);

	 HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_SET);
	 HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_RESET);

	 __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	 __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

	 osDelay(450);*/

	// last backward
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,0);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,0);

	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_RESET);

	htim1.Instance->CCR4 = 76; //center
	osDelay(600);

	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

	osDelay(890);
	pwnVal = 0;

	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);
}

void turnRightLittle() {
	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);

	htim1.Instance->CCR4 = 76; //Center
	osDelay(600);

	// forward 1
	htim1.Instance->CCR4 = 100; //left
	pwnVal = 1200;
	osDelay(600);

	// modify the comparison value for the duty cycle
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

	osDelay(800);

	//BACKWARD 1
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,0);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,0);
	htim1.Instance->CCR4 = 59; //right
	osDelay(600);

	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_RESET);

	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

	osDelay(400);

	//FORWARD 2
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,0);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,0);
	htim1.Instance->CCR4 = 76; //left
	osDelay(600);

	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);

	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

	osDelay(425);

	//BACKWARD 2
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,0);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,0);
	htim1.Instance->CCR4 = 59; // right
	osDelay(600);

	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_RESET);

	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

	osDelay(350);

	//FORWARD 3
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,0);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,0);
	htim1.Instance->CCR4 = 100; //left
	osDelay(600);

	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);

	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

	osDelay(400);

	//BACKWARD 3
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,0);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,0);
	htim1.Instance->CCR4 = 59; // right
	osDelay(600);

	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_RESET);

	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

	osDelay(480);

	//FORWARD 4
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,0);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,0);
	htim1.Instance->CCR4 = 100; //left
	osDelay(600);

	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);

	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

//		osDelay(480);
	osDelay(600); //550

	/*//BACKWARD 4
	 __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,0);
	 __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,0);
	 htim1.Instance->CCR4 = 59; // right
	 osDelay(600);

	 HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_SET);
	 HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_RESET);

	 HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_SET);
	 HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_RESET);

	 __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	 __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

	 osDelay(450);*/

	// last backward
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,0);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,0);

	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_RESET);

	htim1.Instance->CCR4 = 76; //center
	osDelay(600);

	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);

	osDelay(950);
	pwnVal = 0;

	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwnVal);
	__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_2,pwnVal);
}

void delay(uint16_t time) {
	__HAL_TIM_SET_COUNTER(&htim4, 0);
	while (__HAL_TIM_GET_COUNTER (&htim4) < time)
		;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) // if the interrupt source is channel1
			{
		if (Is_First_Captured == 0) // if the first value is not captured
				{
			IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read the first value
			Is_First_Captured = 1;  // set the first captured as true
			// Now change the polarity to falling edge
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1,
					TIM_INPUTCHANNELPOLARITY_FALLING);
		}

		else if (Is_First_Captured == 1)   // if the first is already captured
				{
			IC_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read second value
			__HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter

			if (IC_Val2 > IC_Val1) {
				Difference = IC_Val2 - IC_Val1;
			}

			else if (IC_Val1 > IC_Val2) {
				Difference = (0xffff - IC_Val1) + IC_Val2;
			}

			Distance = Difference * .034 / 2;
			Is_First_Captured = 0; // set it back to false

			// set polarity to rising edge
			uint8_t hello[20];
			sprintf(hello, "dist: %5d\0", Distance);
			OLED_ShowString(10, 10, hello);
			//OLED_Refresh_Gram();

			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1,
					TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(&htim4, TIM_IT_CC1);
		}
	}
}

void HCSR04_Read(void) {
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET);
	delay(10);
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);

	__HAL_TIM_ENABLE_IT(&htim4, TIM_IT_CC1);
}

/* USER CODE BEGIN Header_encoder_task */
/**
 * @brief Function implementing the EncoderTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_encoder_task */
void encoder_task(void *argument) {
	/* USER CODE BEGIN encoder_task */
	/* Infinite loop */
	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);

	int cnt1, cnt2, diff;
	uint32_t tick;

	cnt1 = __HAL_TIM_GET_COUNTER(&htim2);
	tick = HAL_GetTick();
	uint8_t hello[20];
	uint16_t dir;
	for (;;) {
		if (HAL_GetTick() - tick > 1000L) {
			cnt2 = __HAL_TIM_GET_COUNTER(&htim2);
			if (__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2)) {
				if (cnt2 < cnt1)
					diff = cnt1 - cnt2;
				else
					diff = (65535 - cnt2) + cnt1;
			} else {
				if (cnt2 > cnt1)
					diff = cnt2 - cnt1;
				else
					diff = (65535 - cnt1) + cnt2;
			}

			sprintf(hello, "Speed:%5d\0", diff);
			OLED_ShowString(10, 20, hello);
			dir = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2);
			sprintf(hello, "Dir:%5d\0", dir);
			OLED_ShowString(10, 30, hello);
			cnt1 = __HAL_TIM_GET_COUNTER(&htim2);
			tick = HAL_GetTick();
		}
	}
	/* USER CODE END encoder_task */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
