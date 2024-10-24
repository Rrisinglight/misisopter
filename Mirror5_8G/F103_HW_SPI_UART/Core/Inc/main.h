/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef union _Pack_Typedef	/* Pack_Typedef */
{
	uint16_t 	RSSI;
	uint16_t 	RX_Freq;
	uint16_t 	SendIdx;
} Pack_Typedef;

typedef struct 	_FullPackTypeDef/* FullPackTypeDef */
{
	Pack_Typedef Packs[1024];
} FullPackTypeDef;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
static const uint16_t freqs[40] = {
  5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725,//A band
  5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866,//B band
  5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945,//E band
  5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880,//F band
  5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917,//r Band
  };

static const uint16_t Norm_freqs[40] = {
  5645,//E band
  5658,//r Band
  5665,//E band
  5685,//E band
  5695,//r Band
  5705,//E band
  5725,//A band
  5732,//r Band
  5733,//B band
  5740,//F band
  5745,//A band
  5752,//B band
  5760,//F band
  5765,//A band
  5769,//r Band
  5771,//B band
  5780,//F band
  5785,//A band
  5790,//B band
  5800,//F band
  5805,//A band
  5806,//r Band
  5809,//B band
  5820,//F band
  5825,//A band
  5828,//B band
  5840,//F band
  5843,//r Band
  5845,//A band
  5847,//B band
  5860,//F band
  5865,//A band
  5866,//B band
  5880,//F band
  5880,//r Band
  5885,//E band
  5905,//E band
  5917,//r Band
  5925,//E band
  5945,//E band
  };

static const uint16_t Full_Freqs_Lst[122] = {
	4867,
	4884,
	4921,
	4958,
	4990,
	4995,
	5020,
	5032,
	5050,
	5069,
	5080,
	5099,
	5110,
	5140,
	5170,
	5200,
	5325,
	5333,
	5348,
	5362,
	5366,
	5373,
	5384,
	5399,
	5402,
	5413,
	5420,
	5436,
	5438,
	5453,
	5456,
	5473,
	5474,
	5492,
	5493,
	5510,
	5528,
	5533,
	5546,
	5547,
	5564,
	5573,
	5582,
	5584,
	5600,
	5613,
	5621,
	5645,
	5653,
	5658,
	5660,
	5665,
	5669,
	5677,
	5685,
	5693,
	5695,
	5705,
	5725,
	5732,
	5733,
	5735,
	5740,
	5745,
	5752,
	5760,
	5765,
	5768,
	5769,
	5770,
	5771,
	5773,
	5780,
	5785,
	5790,
	5794,
	5800,
	5804,
	5805,
	5806,
	5809,
	5813,
	5820,
	5825,
	5828,
	5839,
	5840,
	5843,
	5845,
	5847,
	5853,
	5860,
	5865,
	5866,
	5876,
	5878,
	5880,
	5885,
	5893,
	5902,
	5905,
	5912,
	5914,
	5917,
	5925,
	5933,
	5945,
	5960,
	5980,
	6000,
	6002,
	6020,
	6028,
	6040,
	6054,
	6060,
	6080,
	6100,
	6106,
	6132,
	6158,
	6184,
	};	//


/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BlueLed_Pin GPIO_PIN_13
#define BlueLed_GPIO_Port GPIOC
#define VTX1_PWR_Pin GPIO_PIN_1
#define VTX1_PWR_GPIO_Port GPIOA
#define SPI1_SS_Pin GPIO_PIN_6
#define SPI1_SS_GPIO_Port GPIOA
#define SPI2_SS_Pin GPIO_PIN_14
#define SPI2_SS_GPIO_Port GPIOB
#define VTX2_PWR_Pin GPIO_PIN_8
#define VTX2_PWR_GPIO_Port GPIOA
#define I2C1_RST_Pin GPIO_PIN_4
#define I2C1_RST_GPIO_Port GPIOB
#define I2C1_LED_Pin GPIO_PIN_5
#define I2C1_LED_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define SPI1_SS_En HAL_GPIO_WritePin(SPI1_SS_GPIO_Port, SPI1_SS_Pin, GPIO_PIN_RESET)
#define SPI1_SS_Dis HAL_GPIO_WritePin(SPI1_SS_GPIO_Port, SPI1_SS_Pin, GPIO_PIN_SET)

#define SPI2_SS_En HAL_GPIO_WritePin(SPI2_SS_GPIO_Port, SPI2_SS_Pin, GPIO_PIN_RESET)
#define SPI2_SS_Dis HAL_GPIO_WritePin(SPI2_SS_GPIO_Port, SPI2_SS_Pin, GPIO_PIN_SET)

#define VTX1_PWR_ON HAL_GPIO_WritePin(VTX1_PWR_GPIO_Port, VTX1_PWR_Pin, GPIO_PIN_RESET)
#define VTX1_PWR_OFF HAL_GPIO_WritePin(VTX1_PWR_GPIO_Port, VTX1_PWR_Pin, GPIO_PIN_SET)

#define VTX2_PWR_ON HAL_GPIO_WritePin(VTX2_PWR_GPIO_Port, VTX2_PWR_Pin, GPIO_PIN_RESET)
#define VTX2_PWR_OFF HAL_GPIO_WritePin(VTX2_PWR_GPIO_Port, VTX2_PWR_Pin, GPIO_PIN_SET)

#define I2C1_LED_Set HAL_GPIO_WritePin(I2C1_LED_GPIO_Port, I2C1_LED_Pin, GPIO_PIN_SET)
#define I2C1_LED_Clr HAL_GPIO_WritePin(I2C1_LED_GPIO_Port, I2C1_LED_Pin, GPIO_PIN_RESET)
#define I2C1_LED_Xor HAL_GPIO_TogglePin(I2C1_LED_GPIO_Port, I2C1_LED_Pin)

#define Get_KN HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)
#define Get_PA0 HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)
#define Get_PA4 HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4)
#define Get_PA5 HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5)

#define PA1_Set HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET)
#define PA1_Clr HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET)
#define PA1_Xor HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1)

#define PA2_Set HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET)
#define PA2_Clr HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET)
#define PA2_Xor HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_2)

#define PA5_Set HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET)
#define PA5_Clr HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET)
#define PA5_Xor HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5)

#define PA6_Set HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET)
#define PA6_Clr HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET)
#define PA6_Xor HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6)

#define PB6_Set HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET)
#define PB6_Clr HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET)
#define PB6_Xor HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_6)

#define PB12_Set HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET)
#define PB12_Clr HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET)
#define PB12_Xor HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_12)

#define PB13_Set HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET)
#define PB13_Clr HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET)
#define PB13_Xor HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_13)

#define PB14_Set HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET)
#define PB14_Clr HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET)
#define PB14_Xor HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14)

#define PB15_Set HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET)
#define PB15_Clr HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET)
#define PB15_Xor HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_15)

#define PC13_Set HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET)
#define PC13_Clr HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET)
#define PC13_Xor HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13)

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
