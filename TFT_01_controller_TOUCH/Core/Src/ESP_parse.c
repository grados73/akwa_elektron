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

void ESP_ChangeRelayState(uint8_t RelayNumber, uint8_t NewState);

BlynkServerData Blynk1;


void UART_ESP_ParseLine(UARTDMA_HandleTypeDef *huartdma)
{
	char BufferReceive[BUFFOR_SIZE];

		if(!UARTDMA_GetLineFromReceiveBuffer(huartdma, BufferReceive))
		{
			// Header
			char* ParsePointer = strtok(BufferReceive, "=");

			if (strcmp(ParsePointer, "CHRELAY") == 0) // Command to change state of Relays
			{
				ESP_ParseChangeRelayState();
			}
		}
}


//
// Get info from Blynk Server About change Relays state
// CHRELAY=7,1\n
void ESP_ParseChangeRelayState(void)
{
	uint8_t RelayNr = 0;
	uint8_t NewState = 0;
	char* ParsePointer = strtok(NULL, ",");
	if(strlen(ParsePointer) > 0) // If string exists
	{
		RelayNr = atoi(ParsePointer);
		if(strlen(ParsePointer) > 0)
		{
			char* ParsePointer = strtok(NULL, ","); // Look for next token or end of string
			NewState = atoi(ParsePointer); // If there are no chars, change string to integer
		}
	}
	ESP_ChangeRelayState(RelayNr, NewState);
}


void ESP_ChangeRelayState(uint8_t RelayNumber, uint8_t NewState)
{
	// First Relay (Switch)
	if(1 == RelayNumber)
	{
		if(0 == NewState) firstSwitchTurn(0); //Turn OFF
		else firstSwitchTurn(1); //Turn ON
	}
	else if(2 == RelayNumber)
	{
		if(0 == NewState) secondSwitchTurn(0); //Turn OFF
		else secondSwitchTurn(1); //Turn ON
	}
	else if(3 == RelayNumber)
	{
		if(0 == NewState) thirdSwitchTurn(0); //Turn OFF
		else thirdSwitchTurn(1); //Turn ON
	}
	else if(4 == RelayNumber)
	{
		if(0 == NewState) fourthSwitchTurn(0); //Turn OFF
		else fourthSwitchTurn(1); //Turn ON
	}

	// First Light
	else if(5 == RelayNumber)
	{
		if(0 == NewState) firstLightTurn(0); //Turn OFF
		else firstLightTurn(1); //Turn ON
	}
	else if(6 == RelayNumber)
	{
		if(0 == NewState) secondLightTurn(0); //Turn OFF
		else secondLightTurn(1); //Turn ON
	}
	else if(7 == RelayNumber)
	{
		if(0 == NewState) thirdLightTurn(0); //Turn OFF
		else thirdLightTurn(1); //Turn ON
	}
	else if(8 == RelayNumber)
	{
		if(0 == NewState) fourthLightTurn(0); //Turn OFF
		else fourthLightTurn(1); //Turn ON
	}
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

void ESP_SendCurrentWaterLvl(uint32_t CurrentWaterLvl)
{
	sprintf(Message, "WLVL=%ld\n", CurrentWaterLvl);
	UARTDMA_Print(&huartdma1, Message); // Print message
}


