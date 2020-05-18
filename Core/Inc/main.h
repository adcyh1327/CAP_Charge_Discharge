/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "stm32f2xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "variable.h"
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
#define RELAY_1_Pin GPIO_PIN_13
#define RELAY_1_GPIO_Port GPIOC
#define RELAY_2_Pin GPIO_PIN_0
#define RELAY_2_GPIO_Port GPIOC
#define RELAY_3_Pin GPIO_PIN_1
#define RELAY_3_GPIO_Port GPIOC
#define RELAY_4_Pin GPIO_PIN_2
#define RELAY_4_GPIO_Port GPIOC
#define RELAY_5_Pin GPIO_PIN_3
#define RELAY_5_GPIO_Port GPIOC
#define CS1_5541_Pin GPIO_PIN_7
#define CS1_5541_GPIO_Port GPIOA
#define DIN_5541_Pin GPIO_PIN_5
#define DIN_5541_GPIO_Port GPIOC
#define CLK_5541_Pin GPIO_PIN_0
#define CLK_5541_GPIO_Port GPIOB
#define LED3_Pin GPIO_PIN_10
#define LED3_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_11
#define LED2_GPIO_Port GPIOB
#define SPI2_NSS_Pin GPIO_PIN_12
#define SPI2_NSS_GPIO_Port GPIOB
#define INPUT_4_Pin GPIO_PIN_12
#define INPUT_4_GPIO_Port GPIOD
#define INPUT_3_Pin GPIO_PIN_13
#define INPUT_3_GPIO_Port GPIOD
#define INPUT_2_Pin GPIO_PIN_14
#define INPUT_2_GPIO_Port GPIOD
#define INPUT_1_Pin GPIO_PIN_15
#define INPUT_1_GPIO_Port GPIOD
#define LCD_485_EN_Pin GPIO_PIN_0
#define LCD_485_EN_GPIO_Port GPIOD
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
