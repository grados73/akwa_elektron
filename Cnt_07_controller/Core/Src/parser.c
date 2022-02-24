/**
  ******************************************************************************
  * @file           : parser.c
  * @project		: free_michelle
  * @author			: grados73 - https://github.com/grados73
  * @purpose		: plik z funkcjami parsowania wiadomosci przychodzacych z UART
  ******************************************************************************
  **/


#include "main.h"
#include "parser.h"
#include "string.h"
#include "uartdma.h"
#include "stdlib.h"
#include "stdio.h"
#include "functions.h"
#include "ds18b20.h"
#include "stm32f4xx_hal.h"
#include "ws2812b.h"


extern UARTDMA_HandleTypeDef huartdma2;
extern UARTDMA_HandleTypeDef huartdma6;
char Message[BUFFOR_SIZE]; // Transmit buffer
char MyName[32] = {"SLAVE1"}; // Name string
uint8_t ChangingStateFlag;
uint8_t RelayState[4]= {0};
uint8_t LightState[4]= {0};
extern struct Measurements BMPResults;
extern const uint8_t ds1[]; // zew
extern const uint8_t ds2[]; // wew
extern uint8_t NumberOfLedsWS2812b;
uint32_t Liczba[5];


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Main USART parsing function
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * Parsing headers:
 * 		LED=1\n 		// LED On
 * 		LED=0\n 		// LED Off
 * 		STATE=?\n		// Jaki jest stan uC
 * 		TEMP=1\n		// Jaka jest temperatura 1 czujnika
 * 		PRES=1\n		// Jakie jest cisnienie 1 czujnika
 * 		CHSTATE=1,0\n	// Zmien stan przekaznika 1 na wylaczony
 * 		CHSTATE=2,1\n	// Zmien stan przekaznika 2 na wlaczony
 */

void UART_ParseLine(UARTDMA_HandleTypeDef *huartdma)
{
	char BufferReceive[BUFFOR_SIZE];

	if(!UARTDMA_GetLineFromReceiveBuffer(huartdma, BufferReceive))
	{
		// Header
		char* ParsePointer = strtok(BufferReceive, "="); // LED\0   1\0
		// ParsePointer == LED\0

	  if(strcmp(ParsePointer, "LED") == 0)
	  {
		  UART_ParseLED();
	  }
	  else if(strcmp(ParsePointer, "STATE") == 0)
	  {
		  UART_ParseStatus();
	  }
	  else if(strcmp(ParsePointer, "TEMP") == 0)
	  {
		  UART_ParseTemp();
	  }
	  else if(strcmp(ParsePointer, "PRES") == 0)
	  {
	  	  UART_ParsePres();
	  }
	  else if(strcmp(ParsePointer, "CHSTATE") == 0)
	  {
	  	  UART_ParseChangeRelayState();
	  }
	  else if(strcmp(ParsePointer, "CHLIGHT") == 0)
	  {
	  	  UART_ParseChangeLightState();
	  }
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// USART Distance parsing function
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UART_DistanceSensorParseLine(UARTDMA_HandleTypeDef *huartdma)
{
		char BufferReceive[BUFFOR_SIZE];

		uint8_t Tab[128] = {0};
		for(uint8_t i = 0 ; i < 127 ; i++)
		{
			Tab[i] = huartdma6.DMA_RX_Buffer[i];
		}

		if(!UARTDMA_GetLineFromReceiveBuffer(huartdma, BufferReceive))
		{
			UartSendErrorInfo(1);
		}

		UART_CountDistance(Tab);

		HAL_UART_Receive_DMA(huartdma6.huart, huartdma6.DMA_RX_Buffer, DMA_RX_BUFFER_SIZE);

}

void UART_CountDistance(uint8_t * Tab)
{
	uint32_t Amount = 0;
	uint32_t Average = 0;
	uint8_t Counter = 0;

	for(uint8_t i = 0; i < 128 ; i++)
	{
		if(Tab[i] == 0xff)
		{
			uint8_t DATA_H = Tab[i+1];
			uint8_t DATA_L = Tab[i+2];
			uint8_t SUM = Tab[i+3];
			if(1 == CheckSum(DATA_H, DATA_L, SUM))
			{
				Amount = Amount + RecalculateDistance(DATA_H, DATA_L, SUM);
				Counter++;
				i=i+2;
			}
		}
	}

	Average = Amount / Counter;
	sprintf(Message, "DIST=%lu\n", Average); // If not, Error message
	UARTDMA_Print(&huartdma2, Message); // Print message
}

uint8_t CheckSum(uint8_t DATA_H, uint8_t DATA_L, uint8_t SUM)
{
	uint32_t CountSum = 0;
	CountSum = (0xff+DATA_H+DATA_L)&0x00ff;

	if(CountSum == SUM) return 1;
	else return 0;
}
uint32_t RecalculateDistance(uint8_t DATA_H, uint8_t DATA_L, uint8_t SUM)
{
	uint32_t Distance = 0;

	Distance = DATA_H * 256 + DATA_L;

	return Distance;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// User LED parsing function - testing function
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UART_ParseLED()
{
	uint8_t LedState; // Received state variable

	char* ParsePointer = strtok(NULL, ","); // Look for next token or end of string
	// Should be now: ParsePointer == 1'\0'

	if(strlen(ParsePointer) > 0) // If string exists
	{
		if(ParsePointer[0] < '0' || ParsePointer[0] > '9') // Chceck if there are only numbers
		{
			UARTDMA_Print(&huartdma2, LED_FORMAT_ERROR); // Print message
			return;	// And exit parsing
		}

		LedState = atoi(ParsePointer); // If there are no chars, change string to integer

		if(LedState == 1) // LED ON
		{
			HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_RESET);
			UARTDMA_Print(&huartdma2, "LED=On\r\n");

		}
		else if(LedState == 0) // LED OFF
		{
			HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_SET);
			UARTDMA_Print(&huartdma2, "LED=Off\r\n");
		}
		else // Wrong state number
		{
			UARTDMA_Print(&huartdma2, LED_FORMAT_ERROR);
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// STATE parsing function
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UART_ParseStatus()
{
	char* ParsePointer = strtok(NULL, ",");
	char SourceState[SOURCE_STATE_LENGTH] = {0};

	if(strlen(ParsePointer) > 0) // If string exists
		{
			if(ParsePointer[0] == '?') // jesli to zapytanie o stan
			{
				PodajStatusRoutine();
			}
			else //jesli nie ma '?' tzn, ze podaje swoj stan
			{
				for(uint8_t i = 0 ; i < SOURCE_STATE_LENGTH ; i++)
				{
					SourceState[i] = ParsePointer[i];
				}
			}
			SourceState[1] = SourceState[1];
		}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TEMPERATURE parsing function
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UART_ParseTemp()
{
	uint8_t NrCzujnika; // Received state variable

	char* ParsePointer = strtok(NULL, ","); // Look for next token or end of string
	// Should be now: ParsePointer == 1'\0'

	if(strlen(ParsePointer) > 0) // If string exists
	{
		if(ParsePointer[0] < '0' || ParsePointer[0] > '9') // Chceck if there are only numbers
		{
			UARTDMA_Print(&huartdma2, TEMPERATURE_FORMAT_ERROR); // Print message
			return;	// And exit parsing
		}

		NrCzujnika = atoi(ParsePointer); // If there are no chars, change string to integer
		PodajTemperatureRoutine(NrCzujnika);

	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// PRESSURE parsing function
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UART_ParsePres()
{

	uint8_t NrCzujnika; // Received state variable

	char* ParsePointer = strtok(NULL, ","); // Look for next token or end of string
	// Should be now: ParsePointer == 1'\0'

	if(strlen(ParsePointer) > 0) // If string exists
	{
		if(ParsePointer[0] < '0' || ParsePointer[0] > '9') // Chceck if there are only numbers
		{
			UARTDMA_Print(&huartdma2, PRESURE_FORMAT_ERROR); // Print message
			return;	// And exit parsing
		}

		NrCzujnika = atoi(ParsePointer); // If there are no chars, change string to integer
		PodajCisnienieRoutine(NrCzujnika);


	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// RELAY parsing function
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UART_ParseChangeRelayState()
{
		uint8_t i,j; // Iterators
		uint8_t RelayParameters[2]; // Numer Przekaznika, Stan

		for(i = 0; i<2; i++) // 2 parametry sa oczekiwane - numer przekaznika i stan
		{
			char* ParsePointer = strtok(NULL, ","); // Look for next token or end of string

			if(strlen(ParsePointer) > 0) // If string exists
			{
				for(j=0; ParsePointer[j] != 0; j++) // Loop over all chars in current strong-block
				{
					if((ParsePointer[j] < '0' || ParsePointer[j] > '9') && ParsePointer[j] != '.' ) // Check if there are only numbers or dot sign
					{
						sprintf(Message, CHANGE_RELAY_STATE_FORMAT_ERROR); // If not, Error message
						UARTDMA_Print(&huartdma2, Message); // Print message
						return;	// And exit parsing
					}

					RelayParameters[i] = atoi(ParsePointer); // If there are no chars, change string to integer // before atof
				}
			}
			else
			{
				sprintf(Message, CHANGE_RELAY_STATE_FORMAT_ERROR); // If not, Error message
				UARTDMA_Print(&huartdma2, Message); // Print message
				return;	// And exit parsing
			}
		}
		ZmienStanPrzekRoutine(RelayParameters[0], RelayParameters[1]);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// LIGHTS parsing function
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UART_ParseChangeLightState()
{
			uint8_t i,j; // Iterators
			uint8_t LightParameters[2]; // Numer Przekaznika, Stan

			for(i = 0; i<2; i++) // 2 parametry sa oczekiwane - numer przekaznika i stan
			{
				char* ParsePointer = strtok(NULL, ","); // Look for next token or end of string

				if(strlen(ParsePointer) > 0) // If string exists
				{
					for(j=0; ParsePointer[j] != 0; j++) // Loop over all chars in current strong-block
					{
						if((ParsePointer[j] < '0' || ParsePointer[j] > '9') && ParsePointer[j] != '.' ) // Check if there are only numbers or dot sign
						{
							sprintf(Message, CHANGE_LIGHT_STATE_FORMAT_ERROR); // If not, Error message
							UARTDMA_Print(&huartdma2, Message); // Print message
							return;	// And exit parsing
						}

						LightParameters[i] = atoi(ParsePointer); // If there are no chars, change string to integer // before atof
					}
				}
				else
				{
					sprintf(Message, CHANGE_LIGHT_STATE_FORMAT_ERROR); // If not, Error message
					UARTDMA_Print(&huartdma2, Message); // Print message
					return;	// And exit parsing
				}
			}
			ZmienStanSwiatlaRoutine(LightParameters[0], LightParameters[1]);
//			sprintf(Message, CHANGE_LIGHT_STATE_CONFIRMATION); // If CHANGED
//			UARTDMA_Print(&huartdma2, Message); // Print message
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// STATUS parsing function
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// ORDERS EXECUTIVE FUNCTIONS ///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Answer about current uC state
//
void PodajStatusRoutine()
{
	UARTDMA_Print(&huartdma2, "ASTATE=READY\n");	// TODO: dodac kontrole wszystkich peryferiow i raportowanie o gotowosci
}

//
// Answer about current Temperature
//
void PodajTemperatureRoutine(uint8_t NrCzujnika)
{
	float aktualna_temperatura = 0;
	if(NrCzujnika == 1)
	{
		aktualna_temperatura = BMPResults.Temp;
	}
	else if (NrCzujnika == 2)
	{
		aktualna_temperatura = ds18b20_get_temp(ds1);
		if( aktualna_temperatura >= 84)
		{
			aktualna_temperatura = 0.0;
		}
	}
		sprintf(Message, "ATEMP=%d,%f\n", NrCzujnika, aktualna_temperatura);
		UARTDMA_Print(&huartdma2, Message);
}

//
// Answer about current Pressure
//
void PodajCisnienieRoutine(uint8_t NrCzujnika)
{
	float aktualne_cisnienie = 0;

	if(NrCzujnika == 1)
	{
		aktualne_cisnienie = BMPResults.Pressure;
	}
	sprintf(Message, "APRES=%f\n", aktualne_cisnienie);
	UARTDMA_Print(&huartdma2, Message);
}

//
// Executive change RELAY state
//
void ZmienStanPrzekRoutine(uint8_t NrPrzekaznika, uint8_t Stan)
{
	uint8_t Przekaznik;
	uint8_t NowyStan;
	Przekaznik = NrPrzekaznika;
	NowyStan = Stan;

	if(NrPrzekaznika == 0)
	{
		if(Stan == 1)
			{
			HAL_GPIO_WritePin(RELAY_1_GPIO_Port, RELAY_1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(RELAY_2_GPIO_Port, RELAY_2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(RELAY_3_GPIO_Port, RELAY_3_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(RELAY_4_GPIO_Port, RELAY_4_Pin, GPIO_PIN_RESET);
			}
		else
			{
			HAL_GPIO_WritePin(RELAY_1_GPIO_Port, RELAY_1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(RELAY_2_GPIO_Port, RELAY_2_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(RELAY_3_GPIO_Port, RELAY_3_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(RELAY_4_GPIO_Port, RELAY_4_Pin, GPIO_PIN_SET);
			}
			RelayState[0] = RelayState[1] = RelayState[2] = RelayState[3] = Stan;
	}

	else if(NrPrzekaznika == 1)
	{
		if(Stan == 1) HAL_GPIO_WritePin(RELAY_1_GPIO_Port, RELAY_1_Pin, GPIO_PIN_RESET);
		else HAL_GPIO_WritePin(RELAY_1_GPIO_Port, RELAY_1_Pin, GPIO_PIN_SET);
		RelayState[0] = Stan;
	}
	else if(NrPrzekaznika == 2)
	{
		if(Stan == 1) HAL_GPIO_WritePin(RELAY_2_GPIO_Port, RELAY_2_Pin, GPIO_PIN_RESET);
		else HAL_GPIO_WritePin(RELAY_2_GPIO_Port, RELAY_2_Pin, GPIO_PIN_SET);
		RelayState[1] = Stan;
	}
	else if(NrPrzekaznika == 3)
	{
		if(Stan == 1) HAL_GPIO_WritePin(RELAY_3_GPIO_Port, RELAY_3_Pin, GPIO_PIN_RESET);
		else HAL_GPIO_WritePin(RELAY_3_GPIO_Port, RELAY_3_Pin, GPIO_PIN_SET);
		RelayState[2] = Stan;
	}
	else if(NrPrzekaznika == 4)
	{
		if(Stan == 1) HAL_GPIO_WritePin(RELAY_4_GPIO_Port, RELAY_4_Pin, GPIO_PIN_RESET);
		else HAL_GPIO_WritePin(RELAY_4_GPIO_Port, RELAY_4_Pin, GPIO_PIN_SET);
		RelayState[3] = Stan;
	}
	else if(NrPrzekaznika == 7) // Show status of all relay
	{
		sprintf(Message, "ASSTATUS=%d,%d,%d,%d\n",RelayState[0],RelayState[1],RelayState[2],RelayState[3]);
		UARTDMA_Print(&huartdma2, Message); // Print message
		return;
	}
	else
	{
		sprintf(Message, CHANGE_RELAY_STATE_NUMBER_ERROR); // Zly numer przekaznika,
		UARTDMA_Print(&huartdma2, Message); // Print message
		return;	// And exit parsing
	}

	sprintf(Message, "ACHSTATE=%d,%d,DONE\n", Przekaznik, NowyStan); // Potwierdzenie wykonania polecenia
	UARTDMA_Print(&huartdma2, Message); // Print message
}

//
// Executive change LIGHT state
//
void ZmienStanSwiatlaRoutine(uint8_t NrSwiatla, uint8_t Stan)
{
	uint8_t LightNumber;
	uint8_t NowyStan;
	LightNumber = NrSwiatla;
	NowyStan = Stan;

	if(LightNumber == 0)
	{
			if(Stan == 1)
			{
				HAL_GPIO_WritePin(LIGHT_1_GPIO_Port, LIGHT_1_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(LIGHT_2_GPIO_Port, LIGHT_2_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(LIGHT_3_GPIO_Port, LIGHT_3_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(LIGHT_4_GPIO_Port, LIGHT_4_Pin, GPIO_PIN_RESET);
			}
			else
			{
				HAL_GPIO_WritePin(LIGHT_1_GPIO_Port, LIGHT_1_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(LIGHT_1_GPIO_Port, LIGHT_2_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(LIGHT_1_GPIO_Port, LIGHT_3_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(LIGHT_1_GPIO_Port, LIGHT_4_Pin, GPIO_PIN_SET);
			}
				LightState[0] = LightState[1] = LightState[2] = LightState[3] =	Stan;
	}

	else if(LightNumber == 1)
	{
		if(Stan == 1) HAL_GPIO_WritePin(LIGHT_1_GPIO_Port, LIGHT_1_Pin, GPIO_PIN_RESET);
		else HAL_GPIO_WritePin(LIGHT_1_GPIO_Port, LIGHT_1_Pin, GPIO_PIN_SET);
		LightState[0] = Stan;
	}
	else if(LightNumber == 2)
	{
		if(Stan == 1) HAL_GPIO_WritePin(LIGHT_2_GPIO_Port, LIGHT_2_Pin, GPIO_PIN_RESET);
		else HAL_GPIO_WritePin(LIGHT_2_GPIO_Port, LIGHT_2_Pin, GPIO_PIN_SET);
		LightState[1] = Stan;
	}
	else if(LightNumber == 3)
	{
		if(Stan == 1) HAL_GPIO_WritePin(LIGHT_3_GPIO_Port, LIGHT_3_Pin, GPIO_PIN_RESET);
		else HAL_GPIO_WritePin(LIGHT_3_GPIO_Port, LIGHT_3_Pin, GPIO_PIN_SET);
		LightState[2] = Stan;
	}


	else if(LightNumber == 4)
	{
		if(Stan == 1) HAL_GPIO_WritePin(LIGHT_4_GPIO_Port, LIGHT_4_Pin, GPIO_PIN_RESET);
		else HAL_GPIO_WritePin(LIGHT_4_GPIO_Port, LIGHT_4_Pin, GPIO_PIN_SET);
		LightState[3] = Stan;
	}
	else if(LightNumber == 7) //show status of all lights
	{
		sprintf(Message, "ALSTATUS=%d,%d,%d,%d\n", LightState[0], LightState[1], LightState[2], LightState[3]);
		UARTDMA_Print(&huartdma2, Message); // Print message
		return;
	}

	// controling of ws2812b
	else if(LightNumber == 9)
	{

		if (Stan == 0) // TURN WS LED OFF
		{
			ws2812b_LightTurnOff();
		}
		else if(Stan == 1) // Turn ON - WHITE - HIGH BRIGHTENESS
		{
			ws2812b_LightWhite(WS2812B_BRIGHTNESS_MAX_PWR);
		}
		else if(Stan == 2) // Turn ON - WHITE - MIDDING BRIGHTENESS
		{
			ws2812b_LightWhite(WS2812B_BRIGHTNESS_MID_PWR);
		}
		else if(Stan == 3) // Turn ON - WHITE - LOW BRIGHTENESS
		{
			ws2812b_LightWhite(WS2812B_BRIGHTNESS_MIN_PWR);
		}
		else if(Stan == 4) // Turn ON - BLUE - HIGH BRIGHTENESS
		{
			ws2812b_LightBlue(WS2812B_BRIGHTNESS_MAX_PWR);
		}
		else if(Stan == 5) // Turn ON - BLUE - MIDDING BRIGHTENESS
		{
			ws2812b_LightBlue(WS2812B_BRIGHTNESS_MID_PWR);
		}
		else if(Stan == 6) // Turn ON - BLUE - LOW BRIGHTENESS
		{
			ws2812b_LightBlue(WS2812B_BRIGHTNESS_MIN_PWR);
		}
		else if(Stan == 7) // Turn ON - WHITE - WARM DALLY BRIGHTENESS
		{
			ws2812b_LightDaily(WS2812B_BRIGHTNESS_MID_PWR);
		}
		else if(Stan == 8) // Turn ON - NIGHT LIGHTS
		{
			ws2812b_LightNight(WS2812B_BRIGHTNESS_MID_PWR);
		}
		else if(Stan > 10)
		{
			NumberOfLedsWS2812b = Stan;
		}
	}

	else
	{
		sprintf(Message, CHANGE_LIGHT_STATE_NUMBER_ERROR); // Zly numer swiatla,
		UARTDMA_Print(&huartdma2, Message); // Print message
		return;	// And exit parsing
	}

	sprintf(Message, "ACHLIGHT=%d,%d,DONE\n", LightNumber, NowyStan); // Potwierdzenie wykonania polecenia
	UARTDMA_Print(&huartdma2, Message); // Print message
}

//
// SENDING INFORMATION ABOUT ERRORs
//
void UartSendErrorInfo(uint8_t NrOfSection)
{
	if( 1 == NrOfSection)
	{
	sprintf(Message, "ERROR=DISTMESURE\n"); // Potwierdzenie wykonania polecenia
	UARTDMA_Print(&huartdma2, Message); // Print message
	}

}
