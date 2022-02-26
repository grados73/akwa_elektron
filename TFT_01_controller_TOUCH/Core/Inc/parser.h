/*
 * parser.h
 *
 *  Created on: Jul 7, 2021
 *      Author: grad_
 */

#ifndef INC_PARSER_H_
#define INC_PARSER_H_

#include "uartdma.h"
#include "TFT_ILI9341.h"
#include "GFX_Color.h"
#include "GFX_EnhancedFonts.h"
#include "EnhancedFonts/arialBlack_20ptFontInfo.h"
#include "EnhancedFonts/ArialBlack_28pts_bold.h"
#include "string.h"
#include "stdio.h"
#include "functions.h"
#include "ESP_parse.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ERRORS
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define LED_FORMAT_ERROR "LED=ERR_FORMAT\n"
#define TEMPERATURE_FORMAT_ERROR "ATEMP=ERR_FORMAT\n"
#define PRESURE_FORMAT_ERROR "APRES=ERR_FORMAT\n"
#define CHANGE_RELAY_STATE_FORMAT_ERROR "ACHSTATE=ERR_FORMAT\n"
#define CHANGE_RELAY_STATE_NUMBER_ERROR "ACHSTATE=ERR_NUMBER\n"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// DECLARATION OF FUNCTION
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SwitchControllerState();
void IdleRoutine(uint8_t * AqaParameters);
void InitializationRoutine(uint8_t * AqaParameters);
void ParametersRoutine(uint8_t * AqaParameters);
void SwitchControlingRoutine(uint8_t * AqaParameters);
void LightControllingRoutine(uint8_t * AqaParameters);
void LightTimersRoutine(uint8_t * AqaParameters);
void ConnectionErrorRoutine(uint8_t * AqaParameters);



#define BUFFOR_SIZE 65
#define FRAME_SIZE 16
#define SOURCE_STATE_LENGTH 10
#define DATA_SIZE_FR 4
#define MY_ADRESS 2
#define NUMBER_OF_RELAY 4

//
// Declaration of function to parse USART
void UART_ParseLine(UARTDMA_HandleTypeDef *huartdma);
void UART_ParseLED(void);
void UART_ParseAnswStatus(void);
void UART_ParseAnswTemp(void);
void UART_ParseAnswPres(void);
void UART_ParseAnswDist(void);
void UART_ParseAnswChangeRelayState(void);
void UART_ParseAnswChangeLightState(void);
void UART_ParseAnswRelayStateStatus(void);
void UART_ParseAnswLightsStateStatus(void);

void PodajStatusRoutine();
void PodajTemperatureRoutine(uint8_t NrCzujnika);
void PodajCisnienieRoutine(uint8_t NrCzujnika);
void ZmienStanPrzekRoutine(uint8_t NrPrzekaznika, uint8_t Stan);

uint8_t SendComand(uint8_t Command);

/*
 * SENDING COMAND ID NUMBER
 *	0 	=>	LED=1\n 			// LED On
 * 	1	=>	LED=0\n 			// LED Off
 * 	2	=>	STATE=?\n			// Jaki jest stan uC
 * 	3	=>	TEMP=1\n			// Jaka jest temperatura 1 czujnika
 * 	4	=>	PRES=1\n			// Jakie jest cisnienie 1 czujnika
 * 	5	=>	CHSTATE=1,0\n		// Zmien stan przekaznika 1 na wylaczony
 * 	6	=>	CHSTATE=2,1\n		// Zmien stan przekaznika 2 na wlaczony
 * 	23	=>	CHSTATE=0,1\n		// Zmien stan wszystkich przekaznikow na wlaczony
 * 	24	=>	CHSTATE=0,0\n		// Zmien stan wszystkich przekaznikow na wylaczony
 * 	21	=>	STATESTATUS=?\n		// Podaj stan wszystkich przekaznikow
 * 	13  =>	CHLIGHT=1,0\n		// Zmien stan swiatla 1 na wylaczony
 * 	15  =>  CHLIGHT=2,1\n		// Zmien stan swiatla 2 na wlaczony
 *  25  =>	CHLIGHT=0,1\n		// Zmien stan wszystkich swiatel na wlaczony
 *  26  =>	CHLIGHT=0,0\n		// Zmien stan wszystkich swiatel na wylaczony
 * 	22	=>	LIGHTSSTATUS=?\n	// Podaj stan wszytskich swiatel
 * 	28	=>	CHLIGHT=9,0\n		// Wylacz LEDy WS2812b
 * 	36	=>	CHLIGHT=9,9\n		// Ustaw ilosc diod, wszystko wieksze nic 9 oznacza ilosc diod
 *
 */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// DEFINE of command sending to second uC (1-39)
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define UCMD_LED_ON 0
#define UCMD_LED_OFF 1
#define UCMD_STATE_ASK 2
#define UCMD_TEMP_1 3
#define UCMD_TEMP_2 27
#define UCMD_PRES_1 4
//***********************//
#define UCMD_RELAY_1_ON 5
#define UCMD_RELAY_1_OFF 6
#define UCMD_RELAY_2_ON 7
#define UCMD_RELAY_2_OFF 8
#define UCMD_RELAY_3_ON 9
#define UCMD_RELAY_3_OFF 10
#define UCMD_RELAY_4_ON 11
#define UCMD_RELAY_4_OFF 12
#define UCMD_RELAY_SCHOW_ALL 21
#define UCMD_RELAY_ALL_ON 23
#define UCMD_RELAY_ALL_OFF 24
//***********************//
#define UCMD_LIGHT_1_ON 13
#define UCMD_LIGHT_1_OFF 14
#define UCMD_LIGHT_2_ON 15
#define UCMD_LIGHT_2_OFF 16
#define UCMD_LIGHT_3_ON 17
#define UCMD_LIGHT_3_OFF 18
#define UCMD_LIGHT_4_ON 19
#define UCMD_LIGHT_4_OFF 20
#define UCMD_LIGHT_ALL_ON 25
#define UCMD_LIGHT_ALL_OFF 26
#define UCMD_LIGHT_SCHOW_ALL 22
//***********************//
#define UCMD_WS_LED_WHITE_OFF 28
#define UCMD_WS_LED_WHITE_MAX 29
#define UCMD_WS_LED_WHITE_MID 30
#define UCMD_WS_LED_WHITE_MIN 31
#define UCMD_WS_LED_BLUE_MAX 32
#define UCMD_WS_LED_BLUE_MID 33
#define UCMD_WS_LED_BLUE_MIN 34
#define UCMD_WS_LED_DAILY 35
#define UCMD_WS_LED_NIGHT 36
#define UCMD_WS_NUMBER_LED 37
#define UCMD_WS_ANIMATION_SUNRISE 38
#define UCMD_WS_ANIMATION_SUNSET 39


#endif /* INC_PARSER_H_ */
