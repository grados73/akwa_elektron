/*
 * ESP_parse.h
 *
 *  Created on: Feb 26, 2022
 *      Author: grad_
 */

#ifndef INC_ESP_PARSE_H_
#define INC_ESP_PARSE_H_

#include "uartdma.h"
#include "TFT_ILI9341.h"
#include "GFX_Color.h"
#include "GFX_EnhancedFonts.h"
#include "EnhancedFonts/arialBlack_20ptFontInfo.h"
#include "EnhancedFonts/ArialBlack_28pts_bold.h"
#include "string.h"
#include "stdio.h"
#include "functions.h"
#include "stdbool.h"

#define BUFFOR_SIZE 65

typedef struct
{
	bool Relay1State;
	bool Relay2State;
	bool Relay3State;
	bool Relay4State;

	bool Light1State;
	bool Light2State;
	bool Light3State;
	bool Light4State;


}BlynkServerData;

void UART_ESP_ParseLine(UARTDMA_HandleTypeDef *huartdma);
void ESP_ParseChangeRelayState(void);

void ESP_SendCurrentTempInside(float CurrentTemp);
void ESP_SendCurrentTempOutside(float CurrentTemp);
void ESP_SendCurrentPressure(float CurrentPress);
void ESP_SendCurrentWaterLvl(uint32_t CurrentWaterLvl);


#endif /* INC_ESP_PARSE_H_ */
