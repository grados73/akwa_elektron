/*
 * parser.h
 *
 *  Created on: Jan 25, 2021
 *      Author: grad_
 */


#ifndef INC_PARSER_H_
#define INC_PARSER_H_
#include "uartdma.h"

#define BUFFOR_SIZE 128
#define FRAME_SIZE 16
#define SOURCE_STATE_LENGTH 10

#define DATA_SIZE_FR 4

#define MY_ADRESS 2 //Adres danego urządzenia


//
//////////////////////////////////////// ERRORS //////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define LED_FORMAT_ERROR "LED=ERR_FORMAT\n"
#define TEMPERATURE_FORMAT_ERROR "ATEMP=ERR_FORMAT\n"
#define PRESURE_FORMAT_ERROR "APRES=ERR_FORMAT\n"
#define CHANGE_RELAY_STATE_FORMAT_ERROR "ACHSTATE=ERR_FORMAT\n"
#define CHANGE_RELAY_STATE_NUMBER_ERROR "ACHSTATE=ERR_NUMBER\n"
#define CHANGE_LIGHT_STATE_FORMAT_ERROR "ACHLIGHT=ERR_FORMAT\n"
#define CHANGE_LIGHT_STATE_NUMBER_ERROR "ACHLIGHT=ERR_NUMBER\n"

//////////////////////////////////////ORDERS CONFIRMATIONS///////////////////////////////////////////////////////////////////////////////////////////////////
#define CHANGE_RELAY_STATE_CONFIRMATION "ACHSTATE=OK\n"
#define CHANGE_LIGHT_STATE_CONFIRMATION "ACHSTATE=OK\n"

void UART_ParseLine(UARTDMA_HandleTypeDef *huartdma);
void UART_ParseLED();
void UART_ParseStatus();
void UART_ParseTemp();
void UART_ParsePres();
void UART_ParseChangeRelayState();
void UART_ParseChangeLightState();
void UART_ParseAnsStateStatus();
void UART_ParseAnsLightStatus();

void UART_DistanceSensorParseLine(UARTDMA_HandleTypeDef *huartdma);

void SwitchMotorRegular();

void PodajStatusRoutine();
void PodajTemperatureRoutine(uint8_t NrCzujnika);
void PodajCisnienieRoutine(uint8_t NrCzujnika);
void ZmienStanPrzekRoutine(uint8_t NrPrzekaznika, uint8_t Stan);
void ZmienStanSwiatlaRoutine(uint8_t NrSwiatla, uint8_t Stan);
void UART_CountDistance(uint8_t * Tab);
uint8_t CheckSum(uint8_t DATA_H, uint8_t DATA_L, uint8_t SUM);
uint32_t RecalculateDistance(uint8_t DATA_H, uint8_t DATA_L, uint8_t SUM);

void UartSendErrorInfo(uint8_t NrOfSection);



#endif /* INC_PARSER_H_ */
