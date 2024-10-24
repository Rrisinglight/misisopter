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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

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
#define SPI1_SS_Pin GPIO_PIN_6
#define SPI1_SS_GPIO_Port GPIOA
#define SPI2_SS_Pin GPIO_PIN_14
#define SPI2_SS_GPIO_Port GPIOB
#define I2C1_RST_Pin GPIO_PIN_4
#define I2C1_RST_GPIO_Port GPIOB
#define I2C1_LED_Pin GPIO_PIN_5
#define I2C1_LED_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define SPI1_SS_En HAL_GPIO_WritePin(SPI1_SS_GPIO_Port, SPI1_SS_Pin, GPIO_PIN_RESET)
#define SPI1_SS_Dis HAL_GPIO_WritePin(SPI1_SS_GPIO_Port, SPI1_SS_Pin, GPIO_PIN_SET)

#define SPI2_SS_En HAL_GPIO_WritePin(SPI2_SS_GPIO_Port, SPI2_SS_Pin, GPIO_PIN_RESET)
#define SPI2_SS_Dis HAL_GPIO_WritePin(SPI2_SS_GPIO_Port, SPI2_SS_Pin, GPIO_PIN_SET)

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

#define PA6_Set HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET)
#define PA6_Clr HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET)
#define PA6_Xor HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6)

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
