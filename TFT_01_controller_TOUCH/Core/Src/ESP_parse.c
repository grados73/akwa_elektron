/**
  *********************************************************************************
  * @file           : ESP_parse.c
  * @project		: free_michelle
  * @author			: grados73 - https://github.com/grados73
  * @purpose		: files with functions to parse messages and commands from ESP
  *********************************************************************************
  **/
#include "main.h"
#include "ESP_parse.h"
#include "uartdma.h"

extern UARTDMA_HandleTypeDef huartdma1;
char Message[BUFFOR_SIZE]; // Transmit buffer

BlynkServerData Blynk1;


void UART_ESP_ParseLine(UARTDMA_HandleTypeDef *huartdma)
{
	char BufferReceive[BUFFOR_SIZE];

		if(!UARTDMA_GetLineFromReceiveBuffer(huartdma, BufferReceive))
		{
			// Header
			char* ParsePointer = strtok(BufferReceive, "=");

			if (strcmp(ParsePointer, "CHRELAY") == 0) // Answear about current Temperature
			{
				ESP_ParseChangeRelayState();
			}

		}

}


//
// Get info from Blynk Server
//
void ESP_ParseChangeRelayState(void)
{

}

//
// Send info to Blynk Server
//
void ESP_SendCurrentTempInside(float CurrentTemp)
{
	sprintf(Message, "TEMPINS=%f\n", CurrentTemp);
	UARTDMA_Print(&huartdma1, Message); // Print message
}

void ESP_SendCurrentTempOutside(float CurrentTemp)
{
	sprintf(Message, "TEMPOUT=%f\n", CurrentTemp);
	UARTDMA_Print(&huartdma1, Message); // Print message
}

void ESP_SendCurrentPressure(float CurrentPress)
{
	sprintf(Message, "PRES=%f\n", CurrentPress);
	UARTDMA_Print(&huartdma1, Message); // Print message
}


