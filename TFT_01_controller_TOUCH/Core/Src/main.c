/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @project		: free_michelle
  * @author			: grados73 - https://github.com/grados73
  * @purpose		: TFT touch display controller for communication interface with
  * 				  user for aquarium controller project named "FREE MICHELLE"
  * @todo			: Use confirmation of make orders
  * 				: Read PH
  * 				: Schedule of activity (need EEPROM memory)
  * 				: Online access by ESP with push messages
  * 				: UPS
  * 				: Watchdog
  * 				: Add on screen: printf("Compiled on %s, %s\n" __DATE__, __TIME__); uC gosu l 5.7
  *					: and printf("Error in file: %s, Function: %s, Line: %d\n", __FILE__, __func__, __LINE__);
  *					: pragma once or HeaderGuards in every .h file
  ******************************************************************************
  **/
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* STANDARD LIBRARIES */
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
/* PRRIVATE FILES */
#include "TFT_ILI9341.h"	//TFT
#include "GFX_Color.h"
#include "GFX_EnhancedFonts.h"
#include "XPT2064.h"	//touch
#include "functions.h"
#include "uartdma.h"
#include "parser.h"
#include "menuTFT.h"
#include "ds3231_for_stm32_hal.h"	//RTC
#include "eeprom.h"
#include "ESP_parse.h"
/* PRRIVATE FONTS*/
#include "EnhancedFonts/arialBlack_20ptFontInfo.h"
#include "EnhancedFonts/arialBlack_11ptFontInfo.h"
#include "EnhancedFonts/ArialBlack_28pts_bold.h" // fonts without polish char! Warning!

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//
// Private Variables
//
UARTDMA_HandleTypeDef huartdma2;	// to second uC
UARTDMA_HandleTypeDef huartdma1;	// to ESP
//I2C_HandleTypeDef hi2c1;

uint8_t MsgMain[64]= {0};
//
// To update current displayed clock - current TFT screen displayed
extern MenuTFTState State;
extern uint32_t activitiesDurationTimeInSeconds;
//
// To count time duration Activity
uint8_t FeedingCounter = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_SPI1_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_SPI3_Init();
  MX_I2C1_Init();
  MX_TIM11_Init();
  MX_TIM10_Init();
  MX_I2C3_Init();
  MX_SPI5_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
  //
  // TFT controller INIT
  ILI9341_Init(&hspi1);
  // UART in DMA mode with use RingBuffer INIT
  UARTDMA_Init(&huartdma2, &huart2);
  UARTDMA_Init(&huartdma1, &huart1);
  // TFT touch controller INIT
  XPT2046_Init(&hspi3, EXTI9_5_IRQn);
  // RTC Initialization - I2C1
  DS3231_Init(&hi2c1);
  DS3231_SetInterruptMode(DS3231_ALARM_INTERRUPT);
  DS3231_SetRateSelect(DS3231_1HZ);
  DS3231_EnableOscillator(DS3231_ENABLED);
  // EEPROM CHECK TODO! Add check of eeprom and informing on screen about results
  uint8_t result = 0;
  eeprom_read(0x01, &result, sizeof(result));

  //Timer START
  HAL_TIM_Base_Start_IT(&htim11);

  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);

  int16_t EncoderValue = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  //////////////////////////////////////////////////////
	  // RECEIVE DATA FROM SECUND uC
	  //
	  if(UARTDMA_IsDataReceivedReady(&huartdma2))
	  {
		  UART_ParseLine(&huartdma2); // Parsing function
	  }

	  //
	  // TRANSMIT DATA AND COMAND TO SECOND uC
	  //
	  UARTDMA_TransmitEvent(&huartdma2);

	  //////////////////////////////////////////////////////
	  // RECEIVE DATA FROM ESP
	  //
	  if(UARTDMA_IsDataReceivedReady(&huartdma1))
	  {
		  UART_ESP_ParseLine(&huartdma1); // Parsing function
	  }

	  //
	  // TRANSMIT DATA AND COMAND TO ESP
	  //
	  UARTDMA_TransmitEvent(&huartdma1);

	  ////////////////////////////////////////////////////////
	  // TOUCH SCREEN
	  //
	  XPT2046_Task();

	  //
	  // CURRENT DISPLAYED SCREEN
	  //
	  MenuTFT();
	  ////////////////////////////////////////////////////////

	  EncoderValue = __HAL_TIM_GET_COUNTER(&htim2);
	  encoderUpgrade(&EncoderValue);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_OscInitStruct.PLL.PLLM = 12;
  RCC_OscInitStruct.PLL.PLLN = 96;
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
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* EXTI4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);
  /* I2C1_EV_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
  /* USART2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* SPI3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SPI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(SPI3_IRQn);
}

/* USER CODE BEGIN 4 */
// Periodic interrupt from TIMERS Callback
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM11) // Update event one per 8 seconds
	{
		ChangeHourOnScreen(); // Check if current showing hour are different from this from RTC and change it
	}
	if(htim->Instance == TIM10) // Update even each second - one per second 1/s
	{
		//TODO! Make possibility to change FEEDING_TIME_IN_S
		if(FeedingCounter >= activitiesDurationTimeInSeconds) // timer to count seconds from start feeding to turn off this activity
		{
			HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
			predefinedActivityKarmienie(0);
			FeedingCounter = 0;
			HAL_TIM_Base_Stop_IT(&htim10);

		}
		FeedingCounter++;
	}
}

// Output interrupt from GPIO etc. Callback
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == TOUCH_IRQ_Pin) // Interrupt from XPT2046 - touch screen controler
	{
		XPT2046_IRQ();
	}

	if(GPIO_Pin == IRQ_DS3231_RTC_Pin) // Interrupt from RTC - alarm one per minute
	{
		DS3231_ClearAlarm2Flag();
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

	ILI9341_ClearDisplay(ILI9341_RED);
	EF_SetFont(&arial_11ptFontInfo);
	sprintf((char*)MsgMain, "Error in Function: %s\n", __FILE__);
	EF_PutString(MsgMain, ERROR_FILE_POZ_X, ERROR_FILE_POZ_Y, ILI9341_BLACK, BG_COLOR, ILI9341_RED);
	sprintf((char*)MsgMain, "Function: %s, Line: %d\n",__func__, (int)__LINE__);
	EF_PutString(MsgMain, ERROR_FUNC_AND_LINE_POZ_X, ERROR_FUNC_AND_LINE_POZ_Y, ILI9341_BLACK, BG_COLOR, ILI9341_RED);

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

