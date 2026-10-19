/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32g4xx_hal.h"

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
#define TEM_ADC_Pin GPIO_PIN_0
#define TEM_ADC_GPIO_Port GPIOA
#define IMONOUT_Pin GPIO_PIN_1
#define IMONOUT_GPIO_Port GPIOA
#define LED_BLUE_Pin GPIO_PIN_2
#define LED_BLUE_GPIO_Port GPIOA
#define VIN_ADC_Pin GPIO_PIN_3
#define VIN_ADC_GPIO_Port GPIOA
#define SS_ATRK_Pin GPIO_PIN_4
#define SS_ATRK_GPIO_Port GPIOA
#define ENCODER_A_Pin GPIO_PIN_5
#define ENCODER_A_GPIO_Port GPIOA
#define FAN_Pin GPIO_PIN_6
#define FAN_GPIO_Port GPIOA
#define VOUT_Pin GPIO_PIN_7
#define VOUT_GPIO_Port GPIOA
#define DTRK_Pin GPIO_PIN_0
#define DTRK_GPIO_Port GPIOB
#define OLED_SDA_Pin GPIO_PIN_8
#define OLED_SDA_GPIO_Port GPIOA
#define OLED_SCL_Pin GPIO_PIN_9
#define OLED_SCL_GPIO_Port GPIOA
#define SYNC_Pin GPIO_PIN_10
#define SYNC_GPIO_Port GPIOA
#define FLT_Pin GPIO_PIN_11
#define FLT_GPIO_Port GPIOA
#define EN_UVLO_Pin GPIO_PIN_12
#define EN_UVLO_GPIO_Port GPIOA
#define MODE_Pin GPIO_PIN_15
#define MODE_GPIO_Port GPIOA
#define ENCODER_B_Pin GPIO_PIN_3
#define ENCODER_B_GPIO_Port GPIOB
#define LED_RED_Pin GPIO_PIN_4
#define LED_RED_GPIO_Port GPIOB
#define LED_YELLOW_Pin GPIO_PIN_5
#define LED_YELLOW_GPIO_Port GPIOB
#define BUTTON_Pin GPIO_PIN_8
#define BUTTON_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
