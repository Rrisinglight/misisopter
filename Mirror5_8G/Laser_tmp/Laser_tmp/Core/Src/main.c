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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PA2_Set HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET)
#define PA2_Clr HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET)
#define PA2_Xor HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_2)
#define Command_lengh	16
#define laser_lengh	9
#define BUFFER_SIZE   27
#define TRAMP_FRAME_LENGTH 16
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t command_word1[Command_lengh]={0x0F,0x72,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x72,0x00};
uint8_t command_start_freq[Command_lengh]={0x0F,0x46,0x3F,0x16,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x9B,0x00};
uint8_t command_start_freq1[Command_lengh]={0x0F,0x46,0x3D,0x16,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x99,0x00};
uint8_t command_start_freq2[Command_lengh]={0x0F,0x46,0x39,0x17,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x96,0x00};
uint8_t command_start_freq3[Command_lengh]={0x0F,0x46,0xe4,0x16,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00};
uint8_t Laser_on[laser_lengh]={0xAE,0xA7,0x05,0x00,0x40,0x01,0x46,0xBC,0xBE};
uint8_t Laser_off[laser_lengh]={0xAE,0xA7,0x05,0x00,0x40,0x00,0x45,0xBC,0xBE};
uint8_t receiveBuffer[BUFFER_SIZE];
const uint16_t freqs[40] = {
  5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725,//A band
  5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866,//B band
  5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945,//E band
  5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880,//F band
  5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917,//r Band
};
volatile uint32_t temp[10],temp1[10];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// ?? ??? ?? ????, PA2
void InputTxPin(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

   GPIO_InitStruct.Pin = GPIO_PIN_2;
   GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
   GPIO_InitStruct.Pull = GPIO_PULLUP;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
   PA2_Set;
} //
//--------------------------------------------------------------------------------------------------------------------
// ?? ??? ?? ?????, PA2 -> USART
void UART_TxPin(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

   GPIO_InitStruct.Pin = GPIO_PIN_2;
   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
//   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//   GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
   PA2_Set;
} //
//--------------------------------------------------------------------------------------------------------------------
// ?? ??? ?? ?????, PA2
void OutTxPin(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

   GPIO_InitStruct.Pin = GPIO_PIN_2;
   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
   PA2_Set;
} //
//static uint8_t trampCrc(const uint8_t* frame)//const trampFrame_t* frame)
//{
//  //---------this is BetaFlight version, this version doesn't work
//  //uint8_t crc = 0;
//  //const uint8_t* p = frame;// (const uint8_t*)frame;
//  //const uint8_t* pEnd = p + (TRAMP_HEADER_LENGTH + TRAMP_PAYLOAD_LENGTH);
//  //for (; p != pEnd; p++) 
//  //{
//  //  crc += *p;
//  //}
//  //return crc;
//  //---------this is arduPilot version, this version works
//  uint8_t cksum = 0;
//  for (int i = 1; i < TRAMP_FRAME_LENGTH - 2; i++) 
//  {
//    cksum += frame[i];
//  }
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	uint8_t data=0;
	volatile uint32_t i=0;
	volatile uint32_t count=0;
	volatile uint32_t freq=5000;

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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		
//		while(!(USART1->SR&0x20))
//		{
//		temp[0]=USART1->SR;
//		count++;	
//		//temp[1]=USART1->CR1;
//		//temp[2]=USART1->CR2;
//		//temp[3]=USART1->CR3;
////		if((temp[0])&0x08)
////		{
////			temp[6]=USART1->SR;
////			data=USART1->DR;
////		}
//		//temp[4]=USART1->DR;
//		//temp[5]=USART1->GTPR;
//		//temp[6]=USART1->SR;
//		}
//		data=USART1->DR;
//		while(!(USART2->SR&0x80));
//	  USART2->DR=data;
//		
//		
//		if(count>=52000)
//		{
//			count=0;
//			HAL_Delay(250);
//		}
//		 
//		UART_TxPin();
//		MX_GPIO_Init();
//		MX_USART2_UART_Init();
		HAL_UART_Transmit(&huart2,command_word1,16,20);
		HAL_Delay(500);
//		HAL_UART_Transmit(&huart2,command_start_freq,16,20);
//		HAL_Delay(5000);
//		HAL_UART_Transmit(&huart2,command_start_freq1,16,20);
//		HAL_Delay(5000);
//		HAL_UART_Transmit(&huart2,command_start_freq2,16,20);
//		HAL_Delay(5000);
//		HAL_UART_Transmit(&huart2,command_start_freq3,16,20);
//		

		freq=freqs[i++];
		if(i>40)
		{		i=0;		}
//		freq=freq+50;
//		if(freq>6000)
//		{
//			freq=5000;
//		}
		command_start_freq[1]=0x46;
		command_start_freq[2]=freq%256;
		command_start_freq[3]=freq/256;
		
		command_start_freq[14]=command_start_freq[1]+command_start_freq[2]+command_start_freq[3];
		HAL_UART_Transmit(&huart2,command_start_freq,16,20);
		
//		InputTxPin();
//		if(HAL_UART_Receive(&huart2, &receiveBuffer[count],1,4)==0)
//		{
//			count++;
//			if(count>=26)
//			{
//				while(count>0)
//				{
//				HAL_UART_Transmit(&huart2, &receiveBuffer[count],1,4);
//				count--;
//				}
//			}
//			//data++;
//		}
//		else
//		{
//			HAL_UART_Transmit(&huart2, &data,1,1);
//		}
		HAL_Delay(2000);
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
