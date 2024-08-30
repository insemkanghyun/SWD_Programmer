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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_RED_Pin GPIO_PIN_2
#define LED_RED_GPIO_Port GPIOE
#define LED_ORANGE_Pin GPIO_PIN_3
#define LED_ORANGE_GPIO_Port GPIOE
#define LED_GREEN_Pin GPIO_PIN_4
#define LED_GREEN_GPIO_Port GPIOE
#define SPI_CS_Pin GPIO_PIN_4
#define SPI_CS_GPIO_Port GPIOA
#define PROGRAM_BTN_Pin GPIO_PIN_0
#define PROGRAM_BTN_GPIO_Port GPIOB
#define PROGRAM_BTN_EXTI_IRQn EXTI0_IRQn
#define OPTION_PROG_DIP_SW_Pin GPIO_PIN_1
#define OPTION_PROG_DIP_SW_GPIO_Port GPIOB
#define BEEP_Pin GPIO_PIN_13
#define BEEP_GPIO_Port GPIOE
#define TARGET_RST_Pin GPIO_PIN_8
#define TARGET_RST_GPIO_Port GPIOA
#define SWD_CLK_Pin GPIO_PIN_6
#define SWD_CLK_GPIO_Port GPIOB
#define SWD_IO_Pin GPIO_PIN_8
#define SWD_IO_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
