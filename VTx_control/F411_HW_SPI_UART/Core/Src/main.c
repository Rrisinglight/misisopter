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
#include "adc.h"
#include "crc.h"
#include "dma.h"
#include "fatfs.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "RX5808.h"
//#include "ssd1306.h"
#include "math.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
struct Pack_Fild	/* Pack_Typedef */
{
	uint16_t 	RSSI;
	uint16_t 	RX_Freq;
	uint16_t 	SendIdx;
};

//struct Pack_Fild Packs[1024];
struct Pack_Fild Packs[122];

#define ADC_REFERENCE_VOLTAGE        1.2
#define ADC_MAX                    0xFFF

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define Command_lengh	16
#define laser_lengh	9
#define BUFFER_SIZE   27
#define TRAMP_FRAME_LENGTH 16

#define TRAMP_SYNC_START            0x0F
#define TRAMP_SYNC_STOP             0x00
#define TRAMP_COMMAND_SET_FREQ      'F' // 0x46, command. not require response, down't wait a response
#define TRAMP_COMMAND_SET_POWER     'P' // 0x50, command. not require response, down't wait a response
#define TRAMP_COMMAND_ACTIVE_STATE  'I' // 0x49, command, according to ardupilot code, thats set pitmode
#define TRAMP_COMMAND_GET_CONFIG    'v' // 0x76, request(require response, wait for a response)
#define TRAMP_COMMAND_CMD_RF        'r' // 0x72, request(require response, wait for a response), activate/deactivate Tramp protocol
#define TRAMP_COMMAND_CMD_SENSOR    's' // 0x73, request(require response, wait for a response), gets current temperature of VTX

//#define VTX_Scan	// сканирование VTX 5500...6000МГц, 100х5МГц

//#define Line_Scan	// линейное сканирование 5500...6000МГц, 100х5МГц
#define Fix_Scan	// сканирование по каналом 5645...5945МГц, 40 каналов
//#define Full_Scan	// сканирование по каналом 4867...6184МГц, 122 канала

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t SPI1_OUT[8] = "UUUUUUUU";
uint8_t SPI1_RSV[8] = "AAAAAAAA";
uint8_t SPI2_OUT[8] = "UUUUUUUU";
uint8_t SPI2_RSV[8] = "AAAAAAAA";
uint8_t UART1_OUT[8] = "UUUUUUUU";
uint8_t UART1_RSV[8] = "AAAAAAAA";
uint8_t UART2_OUT[8] = "UUUUUUUU";
uint8_t UART2_RSV[8] = "AAAAAAAA";

uint8_t CLK1_CNT = 0;

uint16_t i,j,k,l = 0;

float mcuVoltage = 0;
float batteryVoltage = 0;
uint16_t adcData[100];
uint32_t TmpADC = 0;
uint16_t RSSI[100];
uint16_t minRSSI = 0;
uint16_t Fnd_RSSI = 0;
uint16_t Fnd_Freq[122];
uint8_t  Fnd_Idx = 0;
uint8_t  Fnd_Flag = 0;
uint16_t maxRSSI = 0;
uint16_t AvgRSSI = 400;
uint16_t LvlRSSI = 200;
uint16_t Noise_Lvl = 0;
uint32_t SummRSSI = 0;
uint16_t RX_Freq = 0;
uint16_t Fix_freqs[40];
uint16_t Fix_Norm_freqs[40];
uint16_t Full_Freqs[122];

uint8_t command_word1[Command_lengh]={0x0F,0x72,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x72,0x00};
uint8_t command_word2[Command_lengh]={0x0F,0x76,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x76,0x00};
uint8_t command_start_freq[Command_lengh]={0x0F,0x46,0x3F,0x16,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x9B,0x00};
uint8_t command_start_freq1[Command_lengh]={0x0F,0x46,0x3D,0x16,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x99,0x00};
uint8_t command_start_freq2[Command_lengh]={0x0F,0x46,0x39,0x17,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x96,0x00};
uint8_t command_start_freq3[Command_lengh]={0x0F,0x46,0xe4,0x16,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00};
uint8_t Laser_on[laser_lengh]={0xAE,0xA7,0x05,0x00,0x40,0x01,0x46,0xBC,0xBE};
uint8_t Laser_off[laser_lengh]={0xAE,0xA7,0x05,0x00,0x40,0x00,0x45,0xBC,0xBE};

uint8_t receiveBuffer[BUFFER_SIZE];

volatile uint32_t temp[10],temp1[10];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// ТХ1 pin INPUT, PB6 -> USART1
void InputTx1Pin(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

   GPIO_InitStruct.Pin = GPIO_PIN_6;
   GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
   GPIO_InitStruct.Pull = GPIO_PULLUP;
   HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
   PA2_Set;
} //
//--------------------------------------------------------------------------------------------------------------------
// ТХ1 pin UART, PB6 -> USART1
void UART_Tx1Pin(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

   GPIO_InitStruct.Pin = GPIO_PIN_6;
   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
//   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
   GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
   HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//   PB6_Set;
} //
//--------------------------------------------------------------------------------------------------------------------
// ТХ1 pin OUT, PB6 -> USART1
void OutTx1Pin(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

   GPIO_InitStruct.Pin = GPIO_PIN_6;
   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
   PB6_Set;
} //

// ТХ2 pin INPUT PA2 -> USART2
void InputTx2Pin(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

   GPIO_InitStruct.Pin = GPIO_PIN_2;
   GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
   GPIO_InitStruct.Pull = GPIO_PULLUP;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
   PA2_Set;
} //
//--------------------------------------------------------------------------------------------------------------------
// ТХ2 pin UART, PA2 -> USART2
void UART_Tx2Pin(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

   GPIO_InitStruct.Pin = GPIO_PIN_2;
   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
//   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
   GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//   PA2_Set;
} //
//--------------------------------------------------------------------------------------------------------------------
// ТХ2 pin OUT, PA2 -> USART2
void OutTx2Pin(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

   GPIO_InitStruct.Pin = GPIO_PIN_2;
   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
   PA2_Set;
} //

uint8_t trampCrc(const uint8_t* frame)//const trampFrame_t* frame)
{
  //---------this is BetaFlight version, this version doesn't work
  //uint8_t crc = 0;
  //const uint8_t* p = frame;// (const uint8_t*)frame;
  //const uint8_t* pEnd = p + (TRAMP_HEADER_LENGTH + TRAMP_PAYLOAD_LENGTH);
  //for (; p != pEnd; p++)
  //{
  //  crc += *p;
  //}
  //return crc;
  //---------this is arduPilot version, this version works
  uint8_t cksum = 0;
  for (int i = 1; i < TRAMP_FRAME_LENGTH - 2; i++)
  {
    cksum += frame[i];
  }

  return cksum;
}

void Fill_Cmd_freq(uint16_t freq)
{
	uint16_t i = 0;
	for(i = 0; i < 16; i++){command_start_freq[i] = 0x00;}
	command_start_freq[0] = TRAMP_SYNC_START;
	command_start_freq[1] = 0x46;
	command_start_freq[2] = freq%256;
	command_start_freq[3] = freq/256;
	command_start_freq[14] = command_start_freq[1] + command_start_freq[2] + command_start_freq[3];
	command_start_freq[15] = TRAMP_SYNC_STOP;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
//	uint8_t data=0;
//	volatile uint32_t i=0;
//  volatile uint32_t count = 0;
//  volatile uint32_t freq = 5000;
//  volatile uint16_t Start_Full_freq = Full_Freqs[0];
//  volatile uint16_t Start_Fix_freq = Norm_freqs[0];

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
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  MX_USB_DEVICE_Init();
  MX_CRC_Init();
  MX_FATFS_Init();
  MX_I2C2_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_SPI2_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(2000);	// задержка для старта USB

//  HAL_ADCEx_Calibration_Start(&hadc1);
//  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcData, 2);
  i = 0;
  j = 0;
  k = 0;
  l = 0;

  //Packs[0].RX_Freq = Start_Fix_freq;

/* Формируем таблицы частот */
  for(l = 0; l < 40; l++)
  {
	  Fix_freqs[l] = freqs[l];
	  Fix_Norm_freqs[l] = Norm_freqs[l];
  }
  for(l = 0; l < 122; l++){Full_Freqs[l] = Full_Freqs_Lst[l];}

  VTX1_PWR_ON;		// Включаем передатчик
  HAL_Delay(500);	// задержка

  RX5808_Init();

  SPI1_SS_En;
  CLK1_CNT = 0;
  HAL_SPI_Transmit(&hspi1, SPI1_OUT, 4, 0x01);
  SPI1_SS_Dis;

  HAL_UART_MspInit(&huart2);
//	UART_Tx2Pin();
  HAL_Delay(1);	// задержка
  HAL_UART_Transmit(&huart2,command_word1,16,20);
  HAL_Delay(1);	// задержка
  InputTx2Pin();
  HAL_Delay(1);	// задержка
  HAL_UART_Receive(&huart2, receiveBuffer, 16, 30);
  HAL_Delay(50);	// задержка

//  VTX1_PWR_OFF;		// Выключаем передатчик

#ifdef VTX_Scan

  VTX1_PWR_ON;		// Включаем передатчик

  for(k = 0; k < 40; k++)	// 40 - FixChanals, 122 - FullChanals
//  if(k < 40)	// 40 - FixChanals, 122 - FullChanals
  {
/*
	  HAL_UART_MspInit(&huart2);
	//	UART_Tx2Pin();
	  HAL_Delay(1);	// задержка
//	  HAL_UART_Transmit(&huart2,command_word1,16,20);
	  HAL_UART_Transmit(&huart2,command_word2,16,20);
	  HAL_Delay(1);	// задержка
	  InputTx2Pin();
	  HAL_Delay(1);	// задержка
	  HAL_UART_Receive(&huart2, receiveBuffer, 16, 30);
*/
	  Fill_Cmd_freq(Fix_Norm_freqs[k]);
//	  Fill_Cmd_freq(Full_Freqs[i]);

	  HAL_UART_MspInit(&huart2);
//	  UART_Tx2Pin();
	  HAL_Delay(1);	// задержка
	  HAL_UART_Transmit(&huart2,command_start_freq,16,30);
	  HAL_Delay(1);	// задержка
/*	  HAL_UART_Transmit(&huart2,command_word2,16,20);
	  HAL_Delay(1);	// задержка
	  InputTx2Pin();
	  HAL_Delay(1);	// задержка
	  HAL_UART_Receive(&huart2, receiveBuffer, 16, 30);
*/
	  HAL_Delay(30000);	// задержка

  }

  VTX1_PWR_OFF;		// Выключаем передатчик

 #endif
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	//	I2C1_LED_Xor;
//	PC13_Xor;

#ifdef Line_Scan
//	if(i < 500)
	if(i < 100)
	{
//		prog_freg(5500+1*i);       // диапазон сканирования 5500 - 6000 МГц
		prog_freg(5500+5*i);       // диапазон сканирования 5500 - 6000 МГц
		Packs[i].RX_Freq = (5500+5*i);
		Packs[i].SendIdx = i;

		i++;
//		prog_freg(5823);       // диапазон сканирования 5500 - 6000 МГц

		HAL_SPI_MspInit(&hspi1);
		HAL_Delay(1);	// задержка

		SPI1_SS_En;
		CLK1_CNT = 0;
		HAL_SPI_Transmit(&hspi1, SPI1_OUT, 4, 0x01);
		SPI1_SS_Dis;

		HAL_ADC_Start_IT(&hadc1);
		HAL_Delay(5);	// задержка

		adcData[i] = TmpADC;
	}	// scaning
#endif
#ifdef Fix_Scan
//	if(i < 500)
	if(i < 40)
	{
//		prog_freg(5500+1*i);       // диапазон сканирования 5500 - 6000 МГц
		prog_freg(Fix_Norm_freqs[i]);       // диапазон сканирования 5500 - 6000 МГц
		Packs[i].RX_Freq = (Fix_Norm_freqs[i]);
		Packs[i].SendIdx = i;

		i++;
//		prog_freg(5823);       // диапазон сканирования 5500 - 6000 МГц

		HAL_SPI_MspInit(&hspi1);
		HAL_Delay(1);	// задержка

		SPI1_SS_En;
		CLK1_CNT = 0;
		HAL_SPI_Transmit(&hspi1, SPI1_OUT, 4, 0x01);
		SPI1_SS_Dis;

		HAL_ADC_Start_IT(&hadc1);
		HAL_Delay(1);	// задержка

		adcData[i] = TmpADC;
	}	// scaning
#endif
#ifdef Full_Scan
//	if(i < 500)
	if(i < 122)
	{
//		prog_freg(5500+1*i);       // диапазон сканирования 5500 - 6000 МГц
		prog_freg(Full_Freqs[i]);       // диапазон сканирования 5500 - 6000 МГц
		Packs[i].RX_Freq = (Full_Freqs[i]);
		Packs[i].SendIdx = i;

		i++;
//		prog_freg(5823);       // диапазон сканирования 5500 - 6000 МГц

		HAL_SPI_MspInit(&hspi1);
		HAL_Delay(1);	// задержка

		SPI1_SS_En;
		CLK1_CNT = 0;
		HAL_SPI_Transmit(&hspi1, SPI1_OUT, 4, 0x01);
		SPI1_SS_Dis;

		HAL_ADC_Start_IT(&hadc1);
		HAL_Delay(1);	// задержка

		adcData[i] = TmpADC;
	}	// scaning
#endif
	else
	{
		for(j = 0; j < i; j++)
		{
			Packs[j].RSSI = adcData[j];
			SummRSSI += Packs[j].RSSI;

//			if(Packs[j].RSSI > maxRSSI + LvlRSSI)
			if(Packs[j].RSSI > AvgRSSI + LvlRSSI)
			{

//				if(Fnd_Flag && (Packs[j].RSSI > maxRSSI))
//				{
//					maxRSSI = Packs[j].RSSI;
//					RX_Freq = Packs[j].RX_Freq;
//					Fnd_Idx = j;
//					Fnd_RSSI = maxRSSI;
//				}
//				else
//				{
					maxRSSI = Packs[j].RSSI;
					RX_Freq = Packs[j].RX_Freq;
					Fnd_Idx = j;
					Fnd_Flag = 1;
					Fnd_RSSI = maxRSSI;
//				}
			}
		}

		AvgRSSI = SummRSSI/(i + 1);
		SummRSSI = AvgRSSI;
/*
	Тут имеем частоту RX_Freq которую надо подавить.
*/
		if(Fnd_Flag)
		{
			VTX1_PWR_ON;		// Включаем передатчик

			PC13_Clr;

			HAL_Delay(750);	// задержка

//		HAL_Delay(1);	// задержка
// Это для SA, 4800
//		HAL_UART_Transmit(&huart1, UART1_OUT, 2, 0x01);
/* Тут надо вставить отключение выхода */
/* Тут надо еще буфер и размет ответа исправить */
//	   HAL_UART_Receive(&huart1, RX_buffer, 1024, 0x03);

// Это для Trump, 9600
//		HAL_UART_Transmit(&huart2, UART2_OUT, 2, 0x01);
/* Тут надо вставить отключение выхода */
/* Тут надо еще буфер и размер ответа исправить */
//	    HAL_UART_Receive(&huart2, RX_buffer, 1024, 0x03);
/*
		if(k < 3)
		{
			switch (k)
			{
				case 0:
				{
					HAL_UART_Transmit(&huart2,command_start_freq,16,5);
				}
				break;
				case 1:
				{
					HAL_UART_Transmit(&huart2,command_start_freq1,16,5);
				}
				break;
				case 2:
				{
					HAL_UART_Transmit(&huart2,command_start_freq2,16,5);
				}
				break;
				case 3:
				{
					HAL_UART_Transmit(&huart2,command_start_freq3,16,5);
				}
				break;
			}
		}
		else k = 0;
*/

			HAL_UART_MspInit(&huart2);
//		UART_Tx2Pin();
			HAL_Delay(1);	// задержка
/*
		//		HAL_UART_Transmit(&huart2,command_word1,16,20);
		HAL_UART_Transmit(&huart2,command_word2,16,20);
		HAL_Delay(1);	// задержка
		InputTx2Pin();
		HAL_Delay(1);	// задержка
		HAL_UART_Receive(&huart2, receiveBuffer, 16, 30);
		HAL_Delay(10);	// задержка
*/
			HAL_UART_MspInit(&huart2);

//	    UART_Tx2Pin();
			HAL_Delay(1);	// задержка
//		HAL_UART_Transmit(&huart2,New_freq,16,5);
#ifdef Line_Scan
			Fill_Cmd_freq(RX_Freq);
#endif
#ifdef Fix_Scan
			Fill_Cmd_freq(Fix_Norm_freqs[Fnd_Idx]);
#endif
#ifdef Full_Scan
			Fill_Cmd_freq(Full_Freqs[Fnd_Idx]);
#endif

			HAL_UART_Transmit(&huart2,command_start_freq,16,30);

// Тут влепить запуск сканера, оптрон на кнопку

			HAL_Delay(5000);	// задержка на работу глушилки
//			HAL_Delay(18000);	// задержка на работу глушилки



			PC13_Set;

			VTX1_PWR_OFF;		// Выключаем передатчик

			Fnd_Flag = 0;
		}

		HAL_Delay(1);	// задержка

		maxRSSI = AvgRSSI;
		i = 0;

	}	// end scaning

/*
	SPI2_SS_En;
	HAL_SPI_Transmit(&hspi2, SPI2_OUT, 4, 0x01);
	HAL_SPI_Receive(&hspi2, SPI2_RSV, 4, 0X01);
	SPI2_SS_Dis;
*/
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
/*
	HAL_UART_MspInit(&huart2);
//	UART_Tx2Pin();
	HAL_Delay(1);	// задержка
	HAL_UART_Transmit(&huart2,command_word1,16,20);
	HAL_Delay(1);	// задержка
	InputTx2Pin();
	HAL_Delay(1);	// задержка
	HAL_UART_Receive(&huart2, receiveBuffer, 16, 30);
	HAL_Delay(100);	// задержка

	HAL_UART_MspInit(&huart2);
//	UART_Tx2Pin();
	HAL_Delay(1);	// задержка
	HAL_UART_Transmit(&huart2,command_start_freq,16,20);
	HAL_Delay(1);	// задержка
	InputTx2Pin();
	HAL_Delay(1);	// задержка
	HAL_UART_Receive(&huart2, receiveBuffer, 16, 30);
	HAL_Delay(100);	// задержка

	HAL_UART_MspInit(&huart2);
//	UART_Tx2Pin();
	HAL_Delay(1);	// задержка
	HAL_UART_Transmit(&huart2,command_start_freq1,16,20);
	HAL_Delay(1);	// задержка
	InputTx2Pin();
	HAL_Delay(1);	// задержка
	HAL_UART_Receive(&huart2, receiveBuffer, 16, 30);
	HAL_Delay(100);	// задержка

	HAL_UART_MspInit(&huart2);
//	UART_Tx2Pin();
	HAL_Delay(1);	// задержка
	HAL_UART_Transmit(&huart2,command_start_freq2,16,20);
	HAL_Delay(1);	// задержка
	InputTx2Pin();
	HAL_Delay(1);	// задержка
	HAL_UART_Receive(&huart2, receiveBuffer, 16, 30);
	HAL_Delay(100);	// задержка
	HAL_Delay(1000);	// задержка
*/
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV4;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
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
