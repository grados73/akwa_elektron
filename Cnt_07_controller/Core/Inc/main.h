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
#define LED_BLUE_Pin GPIO_PIN_13
#define LED_BLUE_GPIO_Port GPIOC
#define Buzzer_Pin GPIO_PIN_1
#define Buzzer_GPIO_Port GPIOA
#define USART2_TX_2uC_Pin GPIO_PIN_2
#define USART2_TX_2uC_GPIO_Port GPIOA
#define USART2_RX_2uC_Pin GPIO_PIN_3
#define USART2_RX_2uC_GPIO_Port GPIOA
#define RELAY_4_Pin GPIO_PIN_12
#define RELAY_4_GPIO_Port GPIOB
#define RELAY_3_Pin GPIO_PIN_13
#define RELAY_3_GPIO_Port GPIOB
#define RELAY_2_Pin GPIO_PIN_14
#define RELAY_2_GPIO_Port GPIOB
#define RELAY_1_Pin GPIO_PIN_15
#define RELAY_1_GPIO_Port GPIOB
#define TIM1_CH2_ws2812b_Pin GPIO_PIN_9
#define TIM1_CH2_ws2812b_GPIO_Port GPIOA
#define USER_LED_Pin GPIO_PIN_10
#define USER_LED_GPIO_Port GPIOA
#define USART6_TX_A02YYUW_Pin GPIO_PIN_11
#define USART6_TX_A02YYUW_GPIO_Port GPIOA
#define USART6_RX_A02YYUW_Pin GPIO_PIN_12
#define USART6_RX_A02YYUW_GPIO_Port GPIOA
#define USART_1_WIRE_DS_Pin GPIO_PIN_15
#define USART_1_WIRE_DS_GPIO_Port GPIOA
#define LIGHT_4_Pin GPIO_PIN_4
#define LIGHT_4_GPIO_Port GPIOB
#define LIGHT_3_Pin GPIO_PIN_5
#define LIGHT_3_GPIO_Port GPIOB
#define LIGHT_2_Pin GPIO_PIN_6
#define LIGHT_2_GPIO_Port GPIOB
#define LIGHT_1_Pin GPIO_PIN_7
#define LIGHT_1_GPIO_Port GPIOB
#define I2C1_SCL_BMP280_Pin GPIO_PIN_8
#define I2C1_SCL_BMP280_GPIO_Port GPIOB
#define I2C1_SDA_BMP280_Pin GPIO_PIN_9
#define I2C1_SDA_BMP280_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define DATA_RELEASE_TIME 10 // time in seconds every time the refresh of the environmental data will be do
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
