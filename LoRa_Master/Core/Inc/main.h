/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#define MCU_LED_Pin GPIO_PIN_13
#define MCU_LED_GPIO_Port GPIOC
#define SX1276_DIO0_Pin GPIO_PIN_1
#define SX1276_DIO0_GPIO_Port GPIOA
#define SX1276_DIO0_EXTI_IRQn EXTI1_IRQn
#define SX1276_SPI_NSS_Pin GPIO_PIN_4
#define SX1276_SPI_NSS_GPIO_Port GPIOA
#define SX1276_SPI_SCK_Pin GPIO_PIN_5
#define SX1276_SPI_SCK_GPIO_Port GPIOA
#define SX1276_SPI_MISO_Pin GPIO_PIN_6
#define SX1276_SPI_MISO_GPIO_Port GPIOA
#define SX1276_SPI_MOSI_Pin GPIO_PIN_7
#define SX1276_SPI_MOSI_GPIO_Port GPIOA
#define SX1276_RESET_Pin GPIO_PIN_0
#define SX1276_RESET_GPIO_Port GPIOB
#define SX1276_TXRX_Pin GPIO_PIN_2
#define SX1276_TXRX_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define LED_GREEN_TOG()		HAL_GPIO_TogglePin(MCU_LED_GPIO_Port,MCU_LED_Pin)
#define LED_GREEN_ON()		HAL_GPIO_WritePin(MCU_LED_GPIO_Port,MCU_LED_Pin,GPIO_PIN_RESET)
#define LED_GREEN_OFF()		HAL_GPIO_WritePin(MCU_LED_GPIO_Port,MCU_LED_Pin,GPIO_PIN_SET)
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
