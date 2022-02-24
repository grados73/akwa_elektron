/*
 * functions.h
 *
 *  	Author: grados73
 *
 *
 */



#ifndef INC_FUNCTIONS_H_
#define INC_FUNCTIONS_H_

#include "main.h"
#include "TFT_ILI9341.h"
#include "GFX_Color.h"
#include "GFX_EnhancedFonts.h"
#include "EnhancedFonts/arialBlack_20ptFontInfo.h"
#include "EnhancedFonts/ArialBlack_28pts_bold.h"
#include "EnhancedFonts/arialBlack_11ptFontInfo.h"
#include "string.h"
#include "stdio.h"
#include "functions.h"
#include "uartdma.h"

#define INIT_TIME_IN_MS 2000
#define ONE_MODULE_INIT_TIME_IN_MS (INIT_TIME_IN_MS/10)

//
// Define position string in showCurrentParameters function
//
#define PARAMETRY_STRING_POZ_X 47
#define PARAMETRY_STRING_POZ_Y 10
#define CZAS_POZ_X 20
#define CZAS_POZ_Y 50
#define TEMP_ZEW_POZ_X 20
#define TEMP_ZEW_POZ_Y 80
#define TEMP_WEW_POZ_X 20
#define TEMP_WEW_POZ_Y 110
#define POZ_WODY_POZ_X 20
#define POZ_WODY_POZ_Y 140
#define CISN_POZ_X 20
#define CISN_POZ_Y 170

//
// Define position string in showControlPanel funcion
//
#define PRZELACZNIKI_STRING_POZ_X 35
#define PRZELACZNIKI_STRING_POZ_Y 10
#define STRING_SWITCH_POZ_X 20
#define STRING_SWITCH_1_POZ_Y 60
#define STRING_SWITCH_2_POZ_Y 90
#define STRING_SWITCH_3_POZ_Y 120
#define STRING_SWITCH_4_POZ_Y 150

//
// Define position string in showLightsControlPanel function
//
#define SWIATLA_STRING_POZ_X 70
#define SWIATLA_STRING_POZ_Y 10
#define STRING_LIGHTS_POZ_X 20
#define STRING_LIGHTS_1_POZ_Y 60
#define STRING_LIGHTS_2_POZ_Y 90
#define STRING_LIGHTS_3_POZ_Y 120
#define STRING_LIGHTS_4_POZ_Y 150

//
// Define position string in showClockSetPanel function
//
#define CLOCK_STRING_POZ_X 47
#define CLOCK_STRING_POZ_Y 10
#define STRING_HOUR_MINUTE_POZ_X 20
#define STRING_HOUR_POZ_Y 70
#define STRING_MINUTE_POZ_Y 110
#define STRING_DAY_OF_WEEK_POZ_Y 150
#define STRING_H_M_NUMBER_POZ_X 160

//
// Define position string in showWSLedPanel function
//
#define WS_LED_STRING_POZ_X 70
#define WS_LED_STRING_POZ_Y 10
#define STRING_WS_LED_POZ_X 20
#define STRING_WS_LED_POZ_NUMBER_X 172
#define STRING_WS_LED_ILOSC_POZ_Y 60
#define STRING_WS_LED_WHITE_POZ_Y 90
#define STRING_WS_LED_BLUE_POZ_Y 120

//
// Define position string in showSchedulePanel function
//
#define STRING_ON_OFF_X 10
#define STRING_ON_Y 100
#define STRING_OFF_Y 135
#define STRING_ON_OFF_HOUR_X 80
#define STRING_ON_OFF_MINUTE_X 138

//
// Define errata begin of string in connection to begin of button
//
#define STRING_ERRATA_X 3
#define STRING_ERRATA_Y 2
#define STRING_ACTIVITIES_1_X_ERRATA 19
#define STRING_ACTIVITIES_2_X_ERRATA 5
#define STRING_ACTIVITIES_ON_OFF_X_ERRATA 47
#define STRING_ACTIVITIES_Y_INTER 20

#define HEADER_UNDERLINE_POZ_Y 41
#define HEADER_UNDERLINE_COLOR ILI9341_ORANGE

uint8_t Msg[64]; // Message to receive/send UART

//
// Prototype of functions
//
void initWait(uint32_t TimeInMs);
uint8_t system_init();
void showCurrentParameters(float temp_zew, float temp_wew, uint8_t * TimeTab, uint8_t water_lvl, float presure);
void showPreparedActivitiesPanel();
void showControlPanel();
void showLightsControlPanel();
void showClockSetPanel();
void showWSLedPanel();
void showSchedule1Panel();
void showSchedule2Panel();

void ChangeHourOnScreen();
void predefinedActivityKarmienie(uint8_t State);
void predefinedActivityCzyszczenie(uint8_t State);

void drawCurrentStateOfLights();
void drawCurrentStateOfSwitches();
void drawSchedulePanel(uint8_t NrOfSchedule);

void restoreTFTScheduleDayOfWeek(uint8_t NrOfSchedule);
void restoreTFTScheduleRelayLights(uint8_t NrOfSchedule);


#endif /* INC_FUNCTIONS_H_ */
