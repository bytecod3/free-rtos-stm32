/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>

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
UART_HandleTypeDef huart1;

osThreadId defaultTaskHandle;
/* USER CODE BEGIN PV */

osThreadId senderTaskHandle;
osThreadId receiverTaskHandle;
osThreadId receiveFromStructTaskHandle;
osThreadId sendStructTaskHandle;

osThreadId receiveFromMultipleTaskHandle;

// queue handlers
xQueueHandle simpleQueue;
xQueueHandle structQueueHandle;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
void StartDefaultTask(void const * argument);

/* USER CODE BEGIN PFP */
void sendingTask(void const* argument);
void receivingTask(void const* argument);
void receiveFromStructTask(void const* argument);
void sendStructTask(void const * argument);
void receiveFromMultipleTask(void const* argument);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// structured queue
  typedef struct {
	  char* str;
	  int counter;
	  uint16_t large_value;
  }my_struct;


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
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
  MX_USART1_UART_Init();
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

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */

  //  simple queue
  simpleQueue = xQueueCreate(5, sizeof(int));

  if(simpleQueue == 0) {
	  char* str = "unable to create queue\r\n";
	  HAL_UART_Transmit(&huart1, (uint8_t*) str, strlen(str), HAL_MAX_DELAY);
  } else {
	  char* str = "Integer queue created successfully\r\n";
	  HAL_UART_Transmit(&huart1, (uint8_t*) str, strlen(str), HAL_MAX_DELAY);
  }


  structQueueHandle = xQueueCreate(2, sizeof(my_struct));

  if(structQueueHandle == pdFALSE) {
	  char* str = "unable to create structure queue\r\n";
	  HAL_UART_Transmit(&huart1, (uint8_t*) str, strlen(str), HAL_MAX_DELAY);
  }  else {
	  char* str = "Structure queue created successfully\r\n";
	  HAL_UART_Transmit(&huart1, (uint8_t*) str, strlen(str), HAL_MAX_DELAY);
  }


  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */

  // sender thread
  osThreadDef(senderTask, sendingTask, osPriorityNormal, 0, 128);
  senderTaskHandle = osThreadCreate(osThread(senderTask), NULL);

  // receiver thread
  osThreadDef(receiverTask, receivingTask, osPriorityNormal, 0, 128);
  receiverTaskHandle = osThreadCreate(osThread(receiverTask), NULL);

  // sender to structured queue
//  osThreadDef(recvStructTask, receiveFromStructTask, osPriorityNormal, 0, 128);
//  receiveFromStructTaskHandle = osThreadCreate(osThread(recvStructTask), NULL);
//
//  osThreadDef(sendStrTask, sendStructTask, osPriorityNormal, 0, 128);
//  sendStructTaskHandle = osThreadCreate(osThread(sendStrTask), NULL);

  osThreadDef(receivefromMultipleTsk, receiveFromMultipleTask, osPriorityNormal, 0, 128);
  receiveFromMultipleTaskHandle = osThreadCreate(osThread(receivefromMultipleTsk), NULL);


  /* USER CODE END RTOS_THREADS */

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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
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

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

// sending task
void sendingTask(void const * argument){

	int y = 130;
	uint32_t tickDelay = pdMS_TO_TICKS(2000);

	while(1) {
		if(xQueueSend(simpleQueue, &y, portMAX_DELAY) == pdPASS){
			char *str2 = " Successfully sent the number to the queue\nLeaving SENDER_HPT Task\n\n\n";
			//HAL_UART_Transmit(&huart1, (uint8_t*)str2, strlen(str2), HAL_MAX_DELAY);
		}
		vTaskDelay(tickDelay);
	}
}

// receiving task
void receivingTask(void const * argument){
	int received = 0;
	uint32_t tickDelay = pdMS_TO_TICKS(3000);
	while(1) {
		if(xQueueReceive(simpleQueue, &received, portMAX_DELAY) != pdTRUE) {
			HAL_UART_Transmit(&huart1, (uint8_t *)"Error in Receiving from Queue\n\n", 31, 1000);
		} else {
			char str[20];
			sprintf(str, "Received %d \r\n", received);
			HAL_UART_Transmit(&huart1, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
		}
		vTaskDelay(tickDelay);
	}
}

void sendStructTask(void const * argument) {

	my_struct* ptr_struct;
	uint32_t tickDelay = pdMS_TO_TICKS(2000);
	uint16_t indx = 1;

	while(1) {
		// allocate data
		ptr_struct = pvPortMalloc(sizeof(my_struct));

		// load the data
		ptr_struct->counter = 1 + indx;
		ptr_struct->large_value = 1000 + indx*199;
		ptr_struct->str = "Hello from sender 1\r\n";

		// send to queue
		if(xQueueSend(structQueueHandle, &ptr_struct, portMAX_DELAY) == pdPASS) {
			char* str = "Sent struct success";
		}

		indx = indx + 1;

		vTaskDelay(tickDelay);
	}

}

void receiveFromStructTask(void const * argument) {
	my_struct* ptrRcvStruct;
	uint32_t tickDelay = pdMS_TO_TICKS(3000);
	char *ptr;

	while(1) {
		char *str2 = "receiving from struct queue\r\n";
		HAL_UART_Transmit(&huart1, (uint8_t*)str2, strlen(str2), HAL_MAX_DELAY);

		if(xQueueReceive(structQueueHandle, &ptrRcvStruct, portMAX_DELAY) == pdPASS) {
			ptr = pvPortMalloc(100 * sizeof(char)); // memory for the string
			sprintf(ptr, "Recvd\r\n: counter = %d\r\n large value = %u\r\n string = %s\r\n\n",
					ptrRcvStruct->counter,
					ptrRcvStruct->large_value,
					ptrRcvStruct->str);

			HAL_UART_Transmit(&huart1, (uint8_t *)ptr, strlen(ptr), HAL_MAX_DELAY);

			vPortFree(ptr); // free the string memory
		}

		vPortFree(ptrRcvStruct);; // free the structure memory
		vTaskDelay(tickDelay);
	}
}

/**
 * Task to receive from multiple senders
 *
 */
void receiveFromMultipleTask(void const* argument) {

	int32_t lReceivedValue;
	BaseType_t xStatus;
	const TickType_t xTicksToWait = pdMS_TO_TICKS(100);

	while (1) {
		xStatus = xQueueReceive(simpleQueue, &lReceivedValue, xTicksToWait);
		if(xStatus == pdPASS) {
			char d[30];
			sprintf(d, "Received: %d\r\n", lReceivedValue);
			HAL_UART_Transmit(&huart1, (uint8_t*)d, strlen(d) , xTicksToWait);
		}

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
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

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
