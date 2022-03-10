/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  ******************************************************************************
  * @file           : main.c
  * @project		: free_michelle
  * @author			: grados73 - https://github.com/grados73
  * @purpose		: main file of GPIO controller
  * @TODO			: delete global variables , mappery, consty in functions arguments, typedefy, makra ifdef
  * 				: watchdog with define to disable watchdog during debug
  ******************************************************************************
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "functions.h"
#include "uartdma.h"
#include "parser.h"
#include "ds18b20.h"
#include "ws2812b.h"

#include "string.h"
#include "stdlib.h"
#include "stdio.h"
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
//BMP280_t Bmp280;
struct Measurements BMPResults;
float CTemp, CPressure;

UARTDMA_HandleTypeDef huartdma2;
UARTDMA_HandleTypeDef huartdma6;

uint8_t BufferReceive[64];
uint8_t RefreshMeasurementDataCounter = 0;

// PHYSICAL ADRESS OF DS18B20 SENSORS
const uint8_t ds1[] = { 0x28, 0x5b, 0x9d, 0x96, 0x3a, 0x19, 0x1, 0xcf }; // zew on wire
const uint8_t ds2[] = { 0x28, 0xff, 0x8e, 0xfc, 0x71, 0x15, 0x3, 0xf7 }; // wew

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
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
  MX_I2C1_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_TIM10_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  MX_USART6_UART_Init();
  MX_TIM2_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
   bmp280_init(); // Pressure and Temperature Bosh sensor Initialization

   HAL_TIM_Base_Start_IT(&htim10); // Start TIMER 10 to

   // Initialization of UART in DMA mode
   UARTDMA_Init(&huartdma2, &huart2); // init UART to connect with second uC
   UARTDMA_Init(&huartdma6, &huart6); // init UART to connect with distance sensor to measure water LVL

   // Initialization of DS18B20 Temperature sensor
   //if (ds18b20_init() != HAL_OK)  Error_Handler();
   ds18b20_init(); // TODO! it is version for developing. In future handle errors ^^

   // Initialization of WS2812B led by default mode
   ws2812b_init();
   ws2812b_LightDaily(WS2812B_BRIGHTNESS_MIN_PWR);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  //
	  // RECEIVE
	  //
	  if(UARTDMA_IsDataReceivedReady(&huartdma2))
	  {
		  UART_ParseLine(&huartdma2); // Parsing function
	  }

	  //
	  // TRANSMIT
	  //
	  UARTDMA_TransmitEvent(&huartdma2);

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
  /* TIM1_UP_TIM10_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM10)
	{

		BMPResults = temp_pressure_measurement(); // Read Temperature and Pressure from sensor
		CTemp = BMPResults.Temp;
		CPressure = BMPResults.Pressure;
		HAL_GPIO_TogglePin(USER_LED_GPIO_Port, USER_LED_Pin); // Connect! It must stay.

		if (RefreshMeasurementDataCounter == (DATA_RELEASE_TIME - 1)) // one second before sending current parameters
		{
			ds18b20_start_measure(ds1); // Start measure of temperature in outside sensor
			UART_DistanceSensorParseLine(&huartdma6); // Distance Parsing function
		}
		if(RefreshMeasurementDataCounter >= DATA_RELEASE_TIME)
		{
			PodajTemperatureRoutine(1);
			PodajCisnienieRoutine(1);
			PodajTemperatureRoutine(2);
			RefreshMeasurementDataCounter = 0;
		}
		RefreshMeasurementDataCounter++;
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

