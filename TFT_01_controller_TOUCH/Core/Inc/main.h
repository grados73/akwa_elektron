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
#define TIM2_ENC_DT_Pin GPIO_PIN_0
#define TIM2_ENC_DT_GPIO_Port GPIOA
#define TIM2_ENC_CLK_Pin GPIO_PIN_1
#define TIM2_ENC_CLK_GPIO_Port GPIOA
#define USART2_TX_2uC_Pin GPIO_PIN_2
#define USART2_TX_2uC_GPIO_Port GPIOA
#define USART2_RX_2uC_Pin GPIO_PIN_3
#define USART2_RX_2uC_GPIO_Port GPIOA
#define IRQ_DS3231_RTC_Pin GPIO_PIN_4
#define IRQ_DS3231_RTC_GPIO_Port GPIOA
#define IRQ_DS3231_RTC_EXTI_IRQn EXTI4_IRQn
#define SPI1_SCK_ILI9341_Pin GPIO_PIN_5
#define SPI1_SCK_ILI9341_GPIO_Port GPIOA
#define SPI1_MISO_ILI9341_Pin GPIO_PIN_6
#define SPI1_MISO_ILI9341_GPIO_Port GPIOA
#define SPI1_MOSI_ILI9341_Pin GPIO_PIN_7
#define SPI1_MOSI_ILI9341_GPIO_Port GPIOA
#define SPI5_SCK_nRF_Pin GPIO_PIN_0
#define SPI5_SCK_nRF_GPIO_Port GPIOB
#define nRF_CE_Pin GPIO_PIN_1
#define nRF_CE_GPIO_Port GPIOB
#define nRF_IRQ_Pin GPIO_PIN_2
#define nRF_IRQ_GPIO_Port GPIOB
#define nRF_IRQ_EXTI_IRQn EXTI2_IRQn
#define TOUCH_CS_Pin GPIO_PIN_10
#define TOUCH_CS_GPIO_Port GPIOB
#define TFT_CS_Pin GPIO_PIN_12
#define TFT_CS_GPIO_Port GPIOB
#define TFT_RST_Pin GPIO_PIN_13
#define TFT_RST_GPIO_Port GPIOB
#define TFT_DC_Pin GPIO_PIN_14
#define TFT_DC_GPIO_Port GPIOB
#define RST_DS3231_RTC_Pin GPIO_PIN_15
#define RST_DS3231_RTC_GPIO_Port GPIOB
#define I2C3_SCL_EEPROM_Pin GPIO_PIN_8
#define I2C3_SCL_EEPROM_GPIO_Port GPIOA
#define TOUCH_IRQ_Pin GPIO_PIN_9
#define TOUCH_IRQ_GPIO_Port GPIOA
#define TOUCH_IRQ_EXTI_IRQn EXTI9_5_IRQn
#define SPI5_MOSI_nRF_Pin GPIO_PIN_10
#define SPI5_MOSI_nRF_GPIO_Port GPIOA
#define CSN_nRF_Pin GPIO_PIN_11
#define CSN_nRF_GPIO_Port GPIOA
#define SPI5_MISO_nRF_Pin GPIO_PIN_12
#define SPI5_MISO_nRF_GPIO_Port GPIOA
#define USART1_TX_ESP_Pin GPIO_PIN_15
#define USART1_TX_ESP_GPIO_Port GPIOA
#define SPI3_SCK_XPT2046_Pin GPIO_PIN_3
#define SPI3_SCK_XPT2046_GPIO_Port GPIOB
#define SPI3_MISO_XPT_2046_Pin GPIO_PIN_4
#define SPI3_MISO_XPT_2046_GPIO_Port GPIOB
#define SPI3_MOSI_XPT2046_Pin GPIO_PIN_5
#define SPI3_MOSI_XPT2046_GPIO_Port GPIOB
#define I2C1_SCL_DS3231_RTC_Pin GPIO_PIN_6
#define I2C1_SCL_DS3231_RTC_GPIO_Port GPIOB
#define USART1_RX_ESP_Pin GPIO_PIN_7
#define USART1_RX_ESP_GPIO_Port GPIOB
#define I2C3_SDA_EEPROM_Pin GPIO_PIN_8
#define I2C3_SDA_EEPROM_GPIO_Port GPIOB
#define I2C1_SDA_DS3231_RTC_Pin GPIO_PIN_9
#define I2C1_SDA_DS3231_RTC_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define FEEDING_TIME_IN_S 5
//
// Place of display error messages
#define ERROR_FILE_POZ_X 10
#define ERROR_FILE_POZ_Y 20
#define ERROR_FUNC_AND_LINE_POZ_X 10
#define ERROR_FUNC_AND_LINE_POZ_Y 50
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
