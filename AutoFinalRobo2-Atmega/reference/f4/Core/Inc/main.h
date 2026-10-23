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
#define State_LED_Pin GPIO_PIN_13
#define State_LED_GPIO_Port GPIOC
#define IR_Objects_Pin GPIO_PIN_14
#define IR_Objects_GPIO_Port GPIOC
#define IR_Line_Pin GPIO_PIN_15
#define IR_Line_GPIO_Port GPIOC
#define MotorOut4_Pin GPIO_PIN_3
#define MotorOut4_GPIO_Port GPIOA
#define EncoderSW_Pin GPIO_PIN_0
#define EncoderSW_GPIO_Port GPIOB
#define nRF24_CE_Pin GPIO_PIN_2
#define nRF24_CE_GPIO_Port GPIOB
#define MotorOut3_Pin GPIO_PIN_10
#define MotorOut3_GPIO_Port GPIOB
#define nRF24_CSN_Pin GPIO_PIN_12
#define nRF24_CSN_GPIO_Port GPIOB
#define nRF24_IRQ_Pin GPIO_PIN_8
#define nRF24_IRQ_GPIO_Port GPIOA
#define nRF24_IRQ_EXTI_IRQn EXTI9_5_IRQn
#define MotorOut1_Pin GPIO_PIN_15
#define MotorOut1_GPIO_Port GPIOA
#define MotorOut2_Pin GPIO_PIN_3
#define MotorOut2_GPIO_Port GPIOB
#define Luces_IR_Pin GPIO_PIN_4
#define Luces_IR_GPIO_Port GPIOB
#define User_BTN_Pin GPIO_PIN_5
#define User_BTN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
