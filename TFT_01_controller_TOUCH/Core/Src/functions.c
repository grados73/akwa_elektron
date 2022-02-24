/**
  ******************************************************************************
  ******************************************************************************
  * @file           : functions.c
  * @project		: free_michelle
  * @author			: grados73 - https://github.com/grados73
  * @purpose		: file with functions to operate on screen etc.

  ******************************************************************************
  ******************************************************************************
  **/

#include "functions.h"
#include "logo.h"
#include "parser.h"
#include "menuTFT.h"
#include "ds3231_for_stm32_hal.h"
#include "tim.h"
#include "eeprom.h"
#include "uartdma.h"

extern float CTemp;
extern float CPres;
extern float CTempWew;
extern uint32_t CDist2water ;
extern uint8_t Time[3];
extern uint8_t SwitchesButtonState[4];
extern uint8_t LightsButtonState[4];
extern uint8_t ActivityButtonState[2];
extern MenuTFTState State;
extern uint8_t NrOfLeds;
extern UARTDMA_HandleTypeDef huartdma2;

uint8_t OldHours = 0;
uint8_t OldMinutes = 0;
static uint32_t LastTime = 0;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	FUNCTIONS TO DISPLAY EACH KIND OF SCREEN
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Display INITIALIZATION screen
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t system_init(){
	  ILI9341_ClearDisplay(ILI9341_LIGHTGREY);
	  //
	  // Draw Image with LOGO
	  ILI9341_DrawImage(30, 50, logo, 240, 140);

	  EF_SetFont(&arialBlack_20ptFontInfo);
	  sprintf((char*)Msg, "Inicjalizacja...");
	  EF_PutString(Msg, 60, 10, ILI9341_BLACK, BG_TRANSPARENT, ILI9341_LIGHTGREY);

	  //
	  // Draw Rectangle to indicate progress of INITIALIZATION
	  LastTime = HAL_GetTick();
	  GFX_DrawRoundRectangle(60, 190, 20, 20, 5, ILI9341_RED);
	  GFX_DrawRoundRectangle(90, 190, 20, 20, 5, ILI9341_RED);
	  GFX_DrawRoundRectangle(120, 190, 20, 20, 5, ILI9341_RED);
	  GFX_DrawRoundRectangle(150, 190, 20, 20, 5, ILI9341_RED);
	  GFX_DrawRoundRectangle(180, 190, 20, 20, 5, ILI9341_RED);
	  GFX_DrawRoundRectangle(210, 190, 20, 20, 5, ILI9341_RED);
	  GFX_DrawRoundRectangle(240, 190, 20, 20, 5, ILI9341_RED);
	  LastTime = HAL_GetTick();
	  EF_SetFont(&arial_11ptFontInfo);
	  	  initWait(ONE_MODULE_INIT_TIME_IN_MS);
	  	  LastTime = HAL_GetTick();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//First step of initialization
//
	  GFX_DrawFillRoundRectangle(60, 190, 20, 20, 5, ILI9341_GREEN);
	  	  eeprom_read(EEPROM_ADR_NUMBER_WS_LEDS, &NrOfLeds, sizeof(NrOfLeds)); // read number of leds
	  	  SendComand(UCMD_WS_NUMBER_LED); // Send number of LEDs
	  	  UARTDMA_TransmitEvent(&huartdma2); // TRANSMIT

		  sprintf((char*)Msg, "Akt. inf. o ledach...    ");
		  EF_PutString(Msg, 20, 220, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);
	  	  	  initWait(ONE_MODULE_INIT_TIME_IN_MS);
	  	  	  LastTime = HAL_GetTick();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Second step of initialization
//
	  GFX_DrawFillRoundRectangle(90, 190, 20, 20, 5, ILI9341_GREEN);
	  	  SendComand(UCMD_TEMP_2);	// ASK for current temperature inside
  	  	  EEPROM_RelayStateRestore(); // Restore state of relay to state before power off, from EEPROM memory
  	  	  UARTDMA_TransmitEvent(&huartdma2); // TRANSMIT

		  sprintf((char*)Msg, "Akt. inf. o przekaźnikach...  ");
		  EF_PutString(Msg, 20, 220, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);
  	  	  	  initWait(ONE_MODULE_INIT_TIME_IN_MS);
  	  	  	  LastTime = HAL_GetTick();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Third step of initialization
//
	  GFX_DrawFillRoundRectangle(120, 190, 20, 20, 5, ILI9341_GREEN);
	  	  SendComand(UCMD_PRES_1);	// ASK for current pressure
	  	  updateAllSchedulesInfo(1); // Read all info about schedule 1

			  initWait(ONE_MODULE_INIT_TIME_IN_MS);
			  LastTime = HAL_GetTick();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Fourth step of initialization
//
	  GFX_DrawFillRoundRectangle(150, 190, 20, 20, 5, ILI9341_GREEN);
  	  	  EEPROM_LightStateRestore(); // Restore state of lights to state before power off, from EEPROM memory
  	  	  UARTDMA_TransmitEvent(&huartdma2); // TRANSMIT

		  sprintf((char*)Msg, "Akt. inf. o światlach...    ");
		  EF_PutString(Msg, 20, 220, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);
			  initWait(ONE_MODULE_INIT_TIME_IN_MS);
			  LastTime = HAL_GetTick();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Fifth step of initialization
//
	  GFX_DrawFillRoundRectangle(180, 190, 20, 20, 5, ILI9341_GREEN);
	  	  	SendComand(UCMD_TEMP_1);	// ASK for current temperature outside
	  	  	updateAllSchedulesInfo(2); // Read all info about schedule 2

			  initWait(ONE_MODULE_INIT_TIME_IN_MS);
			  LastTime = HAL_GetTick();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Sixth step of initialization
//
	  GFX_DrawFillRoundRectangle(210, 190, 20, 20, 5, ILI9341_GREEN);
	  	  SendComand(UCMD_RELAY_SCHOW_ALL); // ASK for current relay state
			  initWait(ONE_MODULE_INIT_TIME_IN_MS);
			  LastTime = HAL_GetTick();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Fifth step of initialization
//
	  GFX_DrawFillRoundRectangle(240, 190, 20, 20, 5, ILI9341_GREEN);
	  	  SendComand(UCMD_LIGHT_SCHOW_ALL); // ASK for current lights state
	  	  UARTDMA_TransmitEvent(&huartdma2); // TRANSMIT
			  initWait(2*ONE_MODULE_INIT_TIME_IN_MS);
			  LastTime = HAL_GetTick();

	  return 1; // TODO! DODAĆ SPRAWDZENIE POPRAWNOŚCI INICJALIZACJI I ZWRÓCENIE 1 / 0
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Display current PARAMETERS screen
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void showCurrentParameters(float temp_zew, float temp_wew, uint8_t * TimeTab, uint8_t water_lvl, float presure)
{
	  ILI9341_ClearDisplay(ILI9341_LIGHTGREY);
	  SendComand(UCMD_TEMP_1);
	  SendComand(UCMD_PRES_1);
	  SendComand(UCMD_TEMP_2);

	  uint8_t CHour = DS3231_GetHour();
	  uint8_t CMinute = DS3231_GetMinute();


	  EF_SetFont(&arialBlack_20ptFontInfo);

	  //
	  // Display HEADER
	  sprintf((char*)Msg, "-=PARAMETRY=-");
	  EF_PutString(Msg, PARAMETRY_STRING_POZ_X, PARAMETRY_STRING_POZ_Y, ILI9341_DARKCYAN, BG_TRANSPARENT, ILI9341_LIGHTGREY);
	  GFX_DrawLine(0, HEADER_UNDERLINE_POZ_Y, 320, HEADER_UNDERLINE_POZ_Y, HEADER_UNDERLINE_COLOR);
	  GFX_DrawLine(0, HEADER_UNDERLINE_POZ_Y+2, 320, HEADER_UNDERLINE_POZ_Y+2, HEADER_UNDERLINE_COLOR);

	  //
	  // Display PARAMETERS
	  if ((CHour<10)&&(CMinute<10)) sprintf((char*)Msg, "Czas: 0%d : 0%d", CHour, CMinute);
	  else if(CHour<10)	sprintf((char*)Msg, "Czas: 0%d : %d", CHour, CMinute);
	  else if(CMinute<10) sprintf((char*)Msg, "Czas: %d : 0%d", CHour, CMinute);
	  else sprintf((char*)Msg, "Czas: %d : %d", CHour, CMinute);
	  EF_PutString(Msg, CZAS_POZ_X, CZAS_POZ_Y, ILI9341_BLACK, BG_TRANSPARENT, ILI9341_LIGHTGREY);

	  sprintf((char*)Msg, "Temp. zewn: %.2f`C", CTemp);
	  EF_PutString(Msg, TEMP_ZEW_POZ_X, TEMP_ZEW_POZ_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);

	  sprintf((char*)Msg, "Temp. wewn: %.2f`C", CTempWew);
	  EF_PutString(Msg, TEMP_WEW_POZ_X, TEMP_WEW_POZ_Y, ILI9341_BLACK, BG_TRANSPARENT, ILI9341_LIGHTGREY);

	  sprintf((char*)Msg, "Poz. wody: -%ldmm", CDist2water);
	  EF_PutString(Msg, POZ_WODY_POZ_X, POZ_WODY_POZ_Y, ILI9341_BLACK, BG_TRANSPARENT, ILI9341_LIGHTGREY);

	  sprintf((char*)Msg, "Ciśnienie: %.1fhPa ", CPres);
	  EF_PutString(Msg, CISN_POZ_X, CISN_POZ_Y, ILI9341_BLACK, BG_TRANSPARENT, ILI9341_LIGHTGREY);

	  //
	  // Display button to change screen
	  EF_SetFont(&arial_11ptFontInfo);
	  // Right button
	  GFX_DrawFillRoundRectangle(RIGHT_BUTTON_X, RIGHT_BUTTON_Y, RIGHT_BUTTON_W, RIGHT_BUTTON_H, RIGHT_LEFT_BUTTON_R,  ILI9341_GREEN);
	  sprintf((char*)Msg, "CONTROL=>");
	  EF_PutString(Msg, (RIGHT_BUTTON_X + 3), (RIGHT_BUTTON_Y + 2), ILI9341_BLACK, BG_TRANSPARENT, ILI9341_GREEN);
	  // Left button
	  GFX_DrawFillRoundRectangle(LEFT_BUTTON_X, LEFT_BUTTON_Y, LEFT_BUTTON_W, LEFT_BUTTON_H, RIGHT_LEFT_BUTTON_R,  ILI9341_GREEN);
	  sprintf((char*)Msg, "<=LIGHTS");
	  EF_PutString(Msg, (LEFT_BUTTON_X + 10), (LEFT_BUTTON_Y + 2), ILI9341_BLACK, BG_TRANSPARENT, ILI9341_GREEN);
	  // Medium button
	  GFX_DrawFillRoundRectangle(MEDIUM_BUTTON_X, MEDIUM_BUTTON_Y, MEDIUM_BUTTON_W, MEDIUM_BUTTON_H, RIGHT_LEFT_BUTTON_R,  ILI9341_ORANGE);
	  sprintf((char*)Msg, ">CLOCK<");
	  EF_PutString(Msg, (MEDIUM_BUTTON_X + 12), (MEDIUM_BUTTON_Y + 2), ILI9341_BLACK, BG_TRANSPARENT, ILI9341_GREEN);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Display control panel to change SWITCH STATE
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void showControlPanel()
{
	  SendComand(UCMD_RELAY_SCHOW_ALL); // ASK for current relay state

	  ILI9341_ClearDisplay(ILI9341_LIGHTGREY);
	  EF_SetFont(&arialBlack_20ptFontInfo);

	  // Display HEADER
	  sprintf((char*)Msg, "-=PRZEŁĄCZNIKI=-");
	  EF_PutString(Msg, PRZELACZNIKI_STRING_POZ_X, PRZELACZNIKI_STRING_POZ_Y, ILI9341_BLUE, BG_TRANSPARENT, ILI9341_LIGHTGREY);
	  GFX_DrawLine(0, HEADER_UNDERLINE_POZ_Y, 320, HEADER_UNDERLINE_POZ_Y, HEADER_UNDERLINE_COLOR);
	  GFX_DrawLine(0, HEADER_UNDERLINE_POZ_Y+2, 320, HEADER_UNDERLINE_POZ_Y+2, HEADER_UNDERLINE_COLOR);

	  //
	  // Display Strings with name of SWITCH
	  sprintf((char*)Msg, "PRZEŁĄCZNIK 1:");
	  EF_PutString(Msg, STRING_SWITCH_POZ_X, STRING_SWITCH_1_POZ_Y, ILI9341_BLACK, BG_TRANSPARENT, ILI9341_LIGHTGREY);

	  sprintf((char*)Msg, "PRZEŁĄCZNIK 2:");
	  EF_PutString(Msg, STRING_SWITCH_POZ_X, STRING_SWITCH_2_POZ_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);

	  sprintf((char*)Msg, "PRZEŁĄCZNIK 3:");
	  EF_PutString(Msg, STRING_SWITCH_POZ_X, STRING_SWITCH_3_POZ_Y, ILI9341_BLACK, BG_TRANSPARENT, ILI9341_LIGHTGREY);

	  sprintf((char*)Msg, "PRZEŁĄCZNIK 4:");
	  EF_PutString(Msg, STRING_SWITCH_POZ_X, STRING_SWITCH_4_POZ_Y, ILI9341_BLACK, BG_TRANSPARENT, ILI9341_LIGHTGREY);

	  //
	  // Display button to change screen
	  EF_SetFont(&arial_11ptFontInfo);
	  GFX_DrawFillRoundRectangle(RIGHT_BUTTON_X, RIGHT_BUTTON_Y, RIGHT_BUTTON_W, RIGHT_BUTTON_H, RIGHT_LEFT_BUTTON_R,  ILI9341_GREEN);
	  sprintf((char*)Msg, "LIGHTS=>");
	  EF_PutString(Msg, (RIGHT_BUTTON_X + 3), (RIGHT_BUTTON_Y + 2), ILI9341_BLACK, BG_TRANSPARENT, ILI9341_GREEN);

	  GFX_DrawFillRoundRectangle(LEFT_BUTTON_X, LEFT_BUTTON_Y, LEFT_BUTTON_W, LEFT_BUTTON_H, RIGHT_LEFT_BUTTON_R,  ILI9341_GREEN);
	  sprintf((char*)Msg, "<=PARAM");
	  EF_PutString(Msg, (LEFT_BUTTON_X + 10), (LEFT_BUTTON_Y + 2), ILI9341_BLACK, BG_TRANSPARENT, ILI9341_GREEN);

	  GFX_DrawFillRoundRectangle(MEDIUM_BUTTON_X, MEDIUM_BUTTON_Y, MEDIUM_BUTTON_W, MEDIUM_BUTTON_H, RIGHT_LEFT_BUTTON_R,  ILI9341_ORANGE);
	  sprintf((char*)Msg, "ACTIVITIES");
	  EF_PutString(Msg, (MEDIUM_BUTTON_X + 6), (MEDIUM_BUTTON_Y + 2), ILI9341_BLACK, BG_TRANSPARENT, ILI9341_GREEN);
	  EF_SetFont(&arialBlack_20ptFontInfo);


}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Display screen to change LIGHTS
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void showLightsControlPanel()
{
	  SendComand(UCMD_LIGHT_SCHOW_ALL); // ASK for current lights state

	  ILI9341_ClearDisplay(ILI9341_LIGHTGREY);
	  EF_SetFont(&arialBlack_20ptFontInfo);

	  // Display Header
	  sprintf((char*)Msg, "-=ŚWIATŁA=-");
	  EF_PutString(Msg, SWIATLA_STRING_POZ_X, SWIATLA_STRING_POZ_Y, ILI9341_ORANGE, BG_TRANSPARENT, ILI9341_LIGHTGREY);
	  GFX_DrawLine(0, HEADER_UNDERLINE_POZ_Y, 320, HEADER_UNDERLINE_POZ_Y, HEADER_UNDERLINE_COLOR);
	  GFX_DrawLine(0, HEADER_UNDERLINE_POZ_Y+2, 320, HEADER_UNDERLINE_POZ_Y+2, HEADER_UNDERLINE_COLOR);

	  // Display String with name of LIGHT
	  sprintf((char*)Msg, "ŚWIATŁO 1:");
	  EF_PutString(Msg, STRING_LIGHTS_POZ_X, STRING_LIGHTS_1_POZ_Y, ILI9341_BLACK, BG_TRANSPARENT, ILI9341_LIGHTGREY);

	  sprintf((char*)Msg, "ŚWIATŁO 2:");
	  EF_PutString(Msg, STRING_LIGHTS_POZ_X, STRING_LIGHTS_2_POZ_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);

	  sprintf((char*)Msg, "ŚWIATŁO 3:");
	  EF_PutString(Msg, STRING_LIGHTS_POZ_X, STRING_LIGHTS_3_POZ_Y, ILI9341_BLACK, BG_TRANSPARENT, ILI9341_LIGHTGREY);

	  sprintf((char*)Msg, "ŚWIATŁO 4:");
	  EF_PutString(Msg, STRING_LIGHTS_POZ_X, STRING_LIGHTS_4_POZ_Y, ILI9341_BLACK, BG_TRANSPARENT, ILI9341_LIGHTGREY);

	  //
	  // Display button to change screen
	  EF_SetFont(&arial_11ptFontInfo);
	  GFX_DrawFillRoundRectangle(RIGHT_BUTTON_X, RIGHT_BUTTON_Y, RIGHT_BUTTON_W, RIGHT_BUTTON_H, RIGHT_LEFT_BUTTON_R,  ILI9341_GREEN);
	  sprintf((char*)Msg, "PARAM=>");
	  EF_PutString(Msg, (RIGHT_BUTTON_X + 10), (RIGHT_BUTTON_Y + 2), ILI9341_BLACK, BG_TRANSPARENT, ILI9341_GREEN);

	  // Medium button
	  GFX_DrawFillRoundRectangle(MEDIUM_BUTTON_X, MEDIUM_BUTTON_Y, MEDIUM_BUTTON_W, MEDIUM_BUTTON_H, RIGHT_LEFT_BUTTON_R,  ILI9341_ORANGE);
	  sprintf((char*)Msg, ">WS2812b<");
	  EF_PutString(Msg, (MEDIUM_BUTTON_X + 6), (MEDIUM_BUTTON_Y + 2), ILI9341_BLACK, BG_TRANSPARENT, ILI9341_GREEN);

	  GFX_DrawFillRoundRectangle(LEFT_BUTTON_X, LEFT_BUTTON_Y, LEFT_BUTTON_W, LEFT_BUTTON_H, RIGHT_LEFT_BUTTON_R,  ILI9341_GREEN);
	  sprintf((char*)Msg, "<=CONTROL");
	  EF_PutString(Msg, (LEFT_BUTTON_X + 3), (LEFT_BUTTON_Y + 2), ILI9341_BLACK, BG_TRANSPARENT, ILI9341_GREEN);


}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Display screen to change current CLOCK
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void showClockSetPanel()
{
	ILI9341_ClearDisplay(ILI9341_LIGHTGREY);
	EF_SetFont(&arialBlack_20ptFontInfo);

	uint8_t CHour = DS3231_GetHour();
	uint8_t CMinute = DS3231_GetMinute();
	uint8_t CDayOfWeek = DS3231_GetDayOfWeek();

	//
	// Display HEADER string
	sprintf((char*)Msg, "-=SET CLOCK=-");
	EF_PutString(Msg, CLOCK_STRING_POZ_X, CLOCK_STRING_POZ_Y, ILI9341_ORANGE, BG_TRANSPARENT, ILI9341_LIGHTGREY);
	GFX_DrawLine(0, HEADER_UNDERLINE_POZ_Y, 320, HEADER_UNDERLINE_POZ_Y, HEADER_UNDERLINE_COLOR);
	GFX_DrawLine(0, HEADER_UNDERLINE_POZ_Y+2, 320, HEADER_UNDERLINE_POZ_Y+2, HEADER_UNDERLINE_COLOR);

	sprintf((char*)Msg, "GODZINA: %d", CHour);
	EF_PutString(Msg, STRING_HOUR_MINUTE_POZ_X, STRING_HOUR_POZ_Y, ILI9341_BLACK, BG_TRANSPARENT, ILI9341_LIGHTGREY);

	sprintf((char*)Msg, "MINUTA:   %d", CMinute);
	EF_PutString(Msg, STRING_HOUR_MINUTE_POZ_X, STRING_MINUTE_POZ_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);

	sprintf((char*)Msg, "DZIEŃ TYG:  %d", CDayOfWeek);
	EF_PutString(Msg, STRING_HOUR_MINUTE_POZ_X, STRING_DAY_OF_WEEK_POZ_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);

	// Display string on button
	EF_SetFont(&arial_11ptFontInfo);

	GFX_DrawFillRoundRectangle(CLOCK_BUTTON_X, CLOCK_B_1_POZ_Y, CLOCK_BUTTON_W, CLOCK_BUTTON_H, CLOCK_BUTTON_R, SWITCH_CLOCK_BUTTON_COLOR);
	sprintf((char*)Msg, "+1");
	EF_PutString(Msg, (CLOCK_BUTTON_X+STRING_ERRATA_X), (CLOCK_B_1_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_CLOCK_BUTTON_COLOR);

	GFX_DrawFillRoundRectangle(CLOCK_BUTTON2_X, CLOCK_B_1_POZ_Y, CLOCK_BUTTON_W, CLOCK_BUTTON_H, CLOCK_BUTTON_R, SWITCH_CLOCK_BUTTON_COLOR);
	sprintf((char*)Msg, "+6");
	EF_PutString(Msg, (CLOCK_BUTTON2_X+STRING_ERRATA_X+2), (CLOCK_B_1_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_CLOCK_BUTTON_COLOR);

	GFX_DrawFillRoundRectangle(CLOCK_BUTTON_X, CLOCK_B_2_POZ_Y, CLOCK_BUTTON_W, CLOCK_BUTTON_H, CLOCK_BUTTON_R, SWITCH_CLOCK_BUTTON_COLOR);
	sprintf((char*)Msg, "+1");
	EF_PutString(Msg, (CLOCK_BUTTON_X+STRING_ERRATA_X+2), (CLOCK_B_2_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_CLOCK_BUTTON_COLOR);

	GFX_DrawFillRoundRectangle(CLOCK_BUTTON2_X, CLOCK_B_2_POZ_Y, CLOCK_BUTTON_W, CLOCK_BUTTON_H, CLOCK_BUTTON_R, SWITCH_CLOCK_BUTTON_COLOR);
	sprintf((char*)Msg, "+10");
	EF_PutString(Msg, (CLOCK_BUTTON2_X+STRING_ERRATA_X), (CLOCK_B_2_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_CLOCK_BUTTON_COLOR);

	GFX_DrawFillRoundRectangle(CLOCK_BUTTON_X, CLOCK_B_3_POZ_Y, CLOCK_BUTTON_W, CLOCK_BUTTON_H, CLOCK_BUTTON_R, SWITCH_CLOCK_BUTTON_COLOR);
	sprintf((char*)Msg, "+1D");
	EF_PutString(Msg, (CLOCK_BUTTON_X+STRING_ERRATA_X), (CLOCK_B_3_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_CLOCK_BUTTON_COLOR);

	//
	// Display button to change screen
	EF_SetFont(&arial_11ptFontInfo);
	GFX_DrawFillRoundRectangle(RIGHT_BUTTON_X, RIGHT_BUTTON_Y, RIGHT_BUTTON_W, RIGHT_BUTTON_H, RIGHT_LEFT_BUTTON_R,  ILI9341_GREEN);
	sprintf((char*)Msg, "CONFIRM");
	EF_PutString(Msg, (RIGHT_BUTTON_X + 10), (RIGHT_BUTTON_Y + 2), ILI9341_BLACK, BG_TRANSPARENT, ILI9341_GREEN);

	// Medium button
	GFX_DrawFillRoundRectangle(MEDIUM_BUTTON_CLOCK_X-30, MEDIUM_BUTTON_CLOCK_Y, MEDIUM_BUTTON_CLOCK_W+60, MEDIUM_BUTTON_H_CLOCK, RIGHT_LEFT_BUTTON_R,  ILI9341_ORANGE);
	sprintf((char*)Msg, "HARMONOGRAMY");
	EF_PutString(Msg, (MEDIUM_BUTTON_CLOCK_X -10), (MEDIUM_BUTTON_CLOCK_Y + 3), ILI9341_BLACK, BG_TRANSPARENT, ILI9341_GREEN);

	GFX_DrawFillRoundRectangle(LEFT_BUTTON_X, LEFT_BUTTON_Y, LEFT_BUTTON_W, LEFT_BUTTON_H, RIGHT_LEFT_BUTTON_R,  ILI9341_GREEN);
	sprintf((char*)Msg, "<=BACK");
	EF_PutString(Msg, (LEFT_BUTTON_X + 3), (LEFT_BUTTON_Y + 2), ILI9341_BLACK, BG_TRANSPARENT, ILI9341_GREEN);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Display screen with Activities
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void showPreparedActivitiesPanel()
{
	ILI9341_ClearDisplay(ILI9341_LIGHTGREY);
	EF_SetFont(&arialBlack_20ptFontInfo);

	// Display Header
	sprintf((char*)Msg, "-=ACTIVITIES=-");
	EF_PutString(Msg, CLOCK_STRING_POZ_X, CLOCK_STRING_POZ_Y, ILI9341_BLUE, BG_TRANSPARENT, ILI9341_LIGHTGREY);
	GFX_DrawLine(0, HEADER_UNDERLINE_POZ_Y, 320, HEADER_UNDERLINE_POZ_Y, HEADER_UNDERLINE_COLOR);
	GFX_DrawLine(0, HEADER_UNDERLINE_POZ_Y+2, 320, HEADER_UNDERLINE_POZ_Y+2, HEADER_UNDERLINE_COLOR);

	//
	// Display button to change screen
	EF_SetFont(&arial_11ptFontInfo);
	GFX_DrawFillRoundRectangle(LEFT_BUTTON_X, LEFT_BUTTON_Y, LEFT_BUTTON_W, LEFT_BUTTON_H, RIGHT_LEFT_BUTTON_R,  ILI9341_GREEN);
	sprintf((char*)Msg, "<=BACK");
	EF_PutString(Msg, (LEFT_BUTTON_X + 3), (LEFT_BUTTON_Y + 2), ILI9341_BLACK, BG_TRANSPARENT, ILI9341_GREEN);

	//
	// Draw current state of Activities button
	//
	// First Button
	//
	if(ActivityButtonState[0] >= 1)
	{
		GFX_DrawFillRoundRectangle(ACTIVITY_BUTTON_X, ACTIVITY_BUTTON_1_Y, ACTIVITY_BUTTON_W, ACTIVITY_BUTTON_H, ACTIVITY_BUTTON_R, SWITCH_ON_ACTIVITY_BUTTON_COLOR);
		sprintf((char*)Msg, "KARMIENIE");
		EF_PutString(Msg, (ACTIVITY_BUTTON_X+STRING_ACTIVITIES_1_X_ERRATA), (ACTIVITY_BUTTON_1_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_ON_ACTIVITY_BUTTON_COLOR);
		sprintf((char*)Msg, "ON");
		EF_PutString(Msg, (ACTIVITY_BUTTON_X+STRING_ACTIVITIES_ON_OFF_X_ERRATA), (ACTIVITY_BUTTON_1_Y+STRING_ACTIVITIES_Y_INTER), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_ON_ACTIVITY_BUTTON_COLOR);
	}
	else
	{
		GFX_DrawFillRoundRectangle(ACTIVITY_BUTTON_X, ACTIVITY_BUTTON_1_Y, ACTIVITY_BUTTON_W, ACTIVITY_BUTTON_H, ACTIVITY_BUTTON_R, SWITCH_OFF_BUTTON_COLOR);
		sprintf((char*)Msg, "KARMIENIE");
		EF_PutString(Msg, (ACTIVITY_BUTTON_X+STRING_ACTIVITIES_1_X_ERRATA), (ACTIVITY_BUTTON_1_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
		sprintf((char*)Msg, "OFF");
		EF_PutString(Msg, (ACTIVITY_BUTTON_X+STRING_ACTIVITIES_ON_OFF_X_ERRATA), (ACTIVITY_BUTTON_1_Y+STRING_ACTIVITIES_Y_INTER), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
	}
	 //
	 // Second Button
	 //
	if(ActivityButtonState[1] >= 1)
	{
		GFX_DrawFillRoundRectangle(ACTIVITY_BUTTON_X, ACTIVITY_BUTTON_2_Y, ACTIVITY_BUTTON_W, ACTIVITY_BUTTON_H, ACTIVITY_BUTTON_R, SWITCH_ON_ACTIVITY_BUTTON_COLOR);
		sprintf((char*)Msg, "CZYSZCZENIE");
		EF_PutString(Msg, (ACTIVITY_BUTTON_X+STRING_ACTIVITIES_2_X_ERRATA), (ACTIVITY_BUTTON_2_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_ON_ACTIVITY_BUTTON_COLOR);
		sprintf((char*)Msg, "ON");
		EF_PutString(Msg, (ACTIVITY_BUTTON_X+STRING_ACTIVITIES_ON_OFF_X_ERRATA), (ACTIVITY_BUTTON_2_Y+STRING_ACTIVITIES_Y_INTER), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_ON_ACTIVITY_BUTTON_COLOR);
	}
	else
	{
		GFX_DrawFillRoundRectangle(ACTIVITY_BUTTON_X, ACTIVITY_BUTTON_2_Y, ACTIVITY_BUTTON_W, ACTIVITY_BUTTON_H, ACTIVITY_BUTTON_R, SWITCH_OFF_BUTTON_COLOR);
		sprintf((char*)Msg, "CZYSZCZENIE");
		EF_PutString(Msg, (ACTIVITY_BUTTON_X+STRING_ACTIVITIES_2_X_ERRATA), (ACTIVITY_BUTTON_2_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
		sprintf((char*)Msg, "OFF");
		EF_PutString(Msg, (ACTIVITY_BUTTON_X+STRING_ACTIVITIES_ON_OFF_X_ERRATA), (ACTIVITY_BUTTON_2_Y+STRING_ACTIVITIES_Y_INTER), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Display screen to controlling WS2812b LEDs
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void showWSLedPanel()
{
	ILI9341_ClearDisplay(ILI9341_LIGHTGREY);
	EF_SetFont(&arialBlack_20ptFontInfo);


	// Display Header
	sprintf((char*)Msg, "-=WS2812b=-");
	EF_PutString(Msg, WS_LED_STRING_POZ_X, WS_LED_STRING_POZ_Y, ILI9341_BLUE, BG_TRANSPARENT, ILI9341_LIGHTGREY);
	GFX_DrawLine(0, HEADER_UNDERLINE_POZ_Y, 320, HEADER_UNDERLINE_POZ_Y, HEADER_UNDERLINE_COLOR);
	GFX_DrawLine(0, HEADER_UNDERLINE_POZ_Y+2, 320, HEADER_UNDERLINE_POZ_Y+2, HEADER_UNDERLINE_COLOR);

	// Display String
	sprintf((char*)Msg, "ILOŚĆ LED: %d", NrOfLeds);
	EF_PutString(Msg, STRING_WS_LED_POZ_X, STRING_WS_LED_ILOSC_POZ_Y, ILI9341_BLACK, BG_TRANSPARENT, ILI9341_LIGHTGREY);

	sprintf((char*)Msg, "WHITE:");
	EF_PutString(Msg, STRING_WS_LED_POZ_X, STRING_WS_LED_WHITE_POZ_Y, ILI9341_BLACK, BG_TRANSPARENT, ILI9341_LIGHTGREY);

	sprintf((char*)Msg, "BLUE:");
	EF_PutString(Msg, STRING_WS_LED_POZ_X, STRING_WS_LED_BLUE_POZ_Y, ILI9341_BLACK, BG_TRANSPARENT, ILI9341_LIGHTGREY);

	//
	// Display first row of buttons - number of LEDs
	EF_SetFont(&arial_11ptFontInfo);
	GFX_DrawFillRoundRectangle(WS_LED_BUTTON_1_X, WS_B_1_POZ_Y, WS_LED_BUTTON_W, WS_LED_BUTTON_H, WS_LED_BUTTON_R, SWITCH_CLOCK_BUTTON_COLOR);
	sprintf((char*)Msg, "+1");
	EF_PutString(Msg, (WS_LED_BUTTON_1_X+STRING_ERRATA_X + 3), (WS_B_1_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_CLOCK_BUTTON_COLOR);

	GFX_DrawFillRoundRectangle(WS_LED_BUTTON_2_X, WS_B_1_POZ_Y, WS_LED_BUTTON_W, WS_LED_BUTTON_H, WS_LED_BUTTON_R, SWITCH_CLOCK_BUTTON_COLOR);
	sprintf((char*)Msg, "+10");
	EF_PutString(Msg, (WS_LED_BUTTON_2_X+STRING_ERRATA_X+2), (WS_B_1_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_CLOCK_BUTTON_COLOR);

	GFX_DrawFillRoundRectangle(WS_LED_BUTTON_3_X, WS_B_1_POZ_Y, WS_LED_BUTTON_W, WS_LED_BUTTON_H, WS_LED_BUTTON_R, SWITCH_OFF_BUTTON_COLOR);
	sprintf((char*)Msg, "OK");
	EF_PutString(Msg, (WS_LED_BUTTON_3_X+STRING_ERRATA_X+2), (WS_B_1_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);

	//
	// Display second row of buttons - WHITE
	GFX_DrawFillRoundRectangle(WS_WHITE_BUTTON_1_X, WS_B_2_POZ_Y, WS_COLOR_BUTTON_W, WS_LED_BUTTON_H, WS_LED_BUTTON_R, LED_WS_WHITE_BUTTON_COLOR);
	sprintf((char*)Msg, "MAX");
	EF_PutString(Msg, (WS_WHITE_BUTTON_1_X+STRING_ERRATA_X+2), (WS_B_2_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, LED_WS_WHITE_BUTTON_COLOR);

	GFX_DrawFillRoundRectangle(WS_WHITE_BUTTON_2_X, WS_B_2_POZ_Y, WS_COLOR_BUTTON_W, WS_LED_BUTTON_H, WS_LED_BUTTON_R, LED_WS_WHITE_BUTTON_COLOR);
	sprintf((char*)Msg, "MID");
	EF_PutString(Msg, (WS_WHITE_BUTTON_2_X+STRING_ERRATA_X+5), (WS_B_2_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, LED_WS_WHITE_BUTTON_COLOR);

	GFX_DrawFillRoundRectangle(WS_WHITE_BUTTON_3_X, WS_B_2_POZ_Y, WS_COLOR_BUTTON_W, WS_LED_BUTTON_H, WS_LED_BUTTON_R, LED_WS_WHITE_BUTTON_COLOR);
	sprintf((char*)Msg, "MIN");
	EF_PutString(Msg, (WS_WHITE_BUTTON_3_X+STRING_ERRATA_X+5), (WS_B_2_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, LED_WS_WHITE_BUTTON_COLOR);

	//
	// Display third row of buttons - BLUE
	GFX_DrawFillRoundRectangle(WS_WHITE_BUTTON_1_X, WS_B_3_POZ_Y, WS_COLOR_BUTTON_W, WS_LED_BUTTON_H, WS_LED_BUTTON_R, LED_WS_BLUE_BUTTON_COLOR);
	sprintf((char*)Msg, "MAX");
	EF_PutString(Msg, (WS_WHITE_BUTTON_1_X+STRING_ERRATA_X+2), (WS_B_3_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, LED_WS_BLUE_BUTTON_COLOR);

	GFX_DrawFillRoundRectangle(WS_WHITE_BUTTON_2_X, WS_B_3_POZ_Y, WS_COLOR_BUTTON_W, WS_LED_BUTTON_H, WS_LED_BUTTON_R, LED_WS_BLUE_BUTTON_COLOR);
	sprintf((char*)Msg, "MID");
	EF_PutString(Msg, (WS_WHITE_BUTTON_2_X+STRING_ERRATA_X+5), (WS_B_3_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, LED_WS_BLUE_BUTTON_COLOR);

	GFX_DrawFillRoundRectangle(WS_WHITE_BUTTON_3_X, WS_B_3_POZ_Y, WS_COLOR_BUTTON_W, WS_LED_BUTTON_H, WS_LED_BUTTON_R, LED_WS_BLUE_BUTTON_COLOR);
	sprintf((char*)Msg, "MIN");
	EF_PutString(Msg, (WS_WHITE_BUTTON_3_X+STRING_ERRATA_X+5), (WS_B_3_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, LED_WS_BLUE_BUTTON_COLOR);

	//
	// Display fourth row of buttons
	GFX_DrawFillRoundRectangle(WS_PREDEFINED_BUTTON_1_X, WS_B_4_POZ_Y, WS_PREDEFINED_BUTTON_W, WS_PREDEFINED_BUTTON_H, WS_LED_BUTTON_R, LED_WS_DAILY_BUTTON_COLOR);
	sprintf((char*)Msg, "DAILY");
	EF_PutString(Msg, (WS_PREDEFINED_BUTTON_1_X+STRING_ERRATA_X+5), (WS_B_4_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, LED_WS_DAILY_BUTTON_COLOR);

	GFX_DrawFillRoundRectangle(WS_PREDEFINED_BUTTON_2_X, WS_B_4_POZ_Y, WS_PREDEFINED_BUTTON_W, WS_PREDEFINED_BUTTON_H, WS_LED_BUTTON_R, SWITCH_OFF_BUTTON_COLOR);
	sprintf((char*)Msg, "OFF");
	EF_PutString(Msg, (WS_PREDEFINED_BUTTON_2_X+STRING_ERRATA_X+12), (WS_B_4_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);

	GFX_DrawFillRoundRectangle(WS_PREDEFINED_BUTTON_3_X, WS_B_4_POZ_Y, WS_PREDEFINED_BUTTON_W, WS_PREDEFINED_BUTTON_H, WS_LED_BUTTON_R, LED_WS_NIGHT_BUTTON_COLOR);
	sprintf((char*)Msg, "NIGHT");
	EF_PutString(Msg, (WS_PREDEFINED_BUTTON_3_X+STRING_ERRATA_X+10), (WS_B_4_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, LED_WS_NIGHT_BUTTON_COLOR);

	//
	// Display fifth row of buttons
	GFX_DrawFillRoundRectangle(WS_PREDEFINED_ACT_BUTTON_1_X, WS_B_5_POZ_Y, WS_PREDEFINED_BUTTON_W, WS_PREDEFINED_BUTTON_H, WS_LED_BUTTON_R, LED_WS_SUN_BUTTON_COLOR);
	sprintf((char*)Msg, "SUNRISE");
	EF_PutString(Msg, (WS_PREDEFINED_ACT_BUTTON_1_X+STRING_ERRATA_X+5), (WS_B_5_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, LED_WS_SUN_BUTTON_COLOR);

	GFX_DrawFillRoundRectangle(WS_PREDEFINED_ACT_BUTTON_2_X, WS_B_5_POZ_Y, WS_PREDEFINED_BUTTON_W, WS_PREDEFINED_BUTTON_H, WS_LED_BUTTON_R, LED_WS_SUN_BUTTON_COLOR);
	sprintf((char*)Msg, "SUNSET");
	EF_PutString(Msg, (WS_PREDEFINED_ACT_BUTTON_2_X+STRING_ERRATA_X+8), (WS_B_5_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, LED_WS_SUN_BUTTON_COLOR);


	//
	// Display button to change screen
	EF_SetFont(&arial_11ptFontInfo);
	GFX_DrawFillRoundRectangle(LEFT_BUTTON_X, LEFT_BUTTON_Y, LEFT_BUTTON_W, LEFT_BUTTON_H, RIGHT_LEFT_BUTTON_R,  ILI9341_GREEN);
	sprintf((char*)Msg, "<=BACK");
	EF_PutString(Msg, (LEFT_BUTTON_X + 3), (LEFT_BUTTON_Y + 2), ILI9341_BLACK, BG_TRANSPARENT, ILI9341_GREEN);


}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Display screen to setting Schedule Panel
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void drawschedulePanel(uint8_t NrOfschedule)
{
	uint8_t hourOnschedule1 = 0;
	uint8_t hourOffschedule1 = 0;
	uint8_t minuteOnschedule1 = 0;
	uint8_t minuteOffschedule1 = 0;
	uint8_t hourOnschedule2 = 0;
	uint8_t hourOffschedule2 = 0;
	uint8_t minuteOnschedule2 = 0;
	uint8_t minuteOffschedule2 = 0;

	uint8_t schedule1DayInWeekTab[7] = {0};
	uint8_t schedule1RelayAndSwitchTab[9] = {0};
	uint8_t schedule2DayInWeekTab[7] = {0};
	uint8_t schedule2RelayAndSwitchTab[9] = {0};
	  //
	  // Draw empty Rectangle to indicate day of week
	  GFX_DrawRoundRectangle(DAY_CHECK_BUTTON_1_X, DAY_CHECK_BUTTON_Y, DAY_CHECK_BUTTON_W, DAY_CHECK_BUTTON_H, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR);
	  GFX_DrawRoundRectangle(DAY_CHECK_BUTTON_2_X, DAY_CHECK_BUTTON_Y, DAY_CHECK_BUTTON_W, DAY_CHECK_BUTTON_H, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR);
	  GFX_DrawRoundRectangle(DAY_CHECK_BUTTON_3_X, DAY_CHECK_BUTTON_Y, DAY_CHECK_BUTTON_W, DAY_CHECK_BUTTON_H, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR);
	  GFX_DrawRoundRectangle(DAY_CHECK_BUTTON_4_X, DAY_CHECK_BUTTON_Y, DAY_CHECK_BUTTON_W, DAY_CHECK_BUTTON_H, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR);
	  GFX_DrawRoundRectangle(DAY_CHECK_BUTTON_5_X, DAY_CHECK_BUTTON_Y, DAY_CHECK_BUTTON_W, DAY_CHECK_BUTTON_H, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR);
	  GFX_DrawRoundRectangle(DAY_CHECK_BUTTON_6_X, DAY_CHECK_BUTTON_Y, DAY_CHECK_BUTTON_W, DAY_CHECK_BUTTON_H, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR);
	  GFX_DrawRoundRectangle(DAY_CHECK_BUTTON_7_X, DAY_CHECK_BUTTON_Y, DAY_CHECK_BUTTON_W, DAY_CHECK_BUTTON_H, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR);

	  // Restore from EEPROM chosen day for this schedule
	  restoreTFTScheduleDayOfWeek(NrOfschedule);
	  restoreTFTScheduleRelayLights(NrOfschedule);

	  EF_SetFont(&arial_11ptFontInfo);
	  sprintf((char*)Msg, "PN WT ŚR CZ PT SB ND");
	  EF_PutString(Msg, DAY_CHECK_BUTTON_1_X, (DAY_CHECK_BUTTON_Y + 25), ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);

	  //
	  // Draw empty Rectangle to indicate relay and lights
	  GFX_DrawRoundRectangle(DAY_CHECK_RELAY_BUTTON_1_X, DAY_CHECK_RELAY_BUTTON_Y, DAY_CHECK_BUTTON_W, DAY_CHECK_BUTTON_H, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR);
	  GFX_DrawRoundRectangle(DAY_CHECK_RELAY_BUTTON_2_X, DAY_CHECK_RELAY_BUTTON_Y, DAY_CHECK_BUTTON_W, DAY_CHECK_BUTTON_H, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR);
	  GFX_DrawRoundRectangle(DAY_CHECK_RELAY_BUTTON_3_X, DAY_CHECK_RELAY_BUTTON_Y, DAY_CHECK_BUTTON_W, DAY_CHECK_BUTTON_H, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR);
	  GFX_DrawRoundRectangle(DAY_CHECK_RELAY_BUTTON_4_X, DAY_CHECK_RELAY_BUTTON_Y, DAY_CHECK_BUTTON_W, DAY_CHECK_BUTTON_H, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR);
	  GFX_DrawRoundRectangle(DAY_CHECK_RELAY_BUTTON_5_X, DAY_CHECK_RELAY_BUTTON_Y, DAY_CHECK_BUTTON_W, DAY_CHECK_BUTTON_H, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR);
	  GFX_DrawRoundRectangle(DAY_CHECK_RELAY_BUTTON_6_X, DAY_CHECK_RELAY_BUTTON_Y, DAY_CHECK_BUTTON_W, DAY_CHECK_BUTTON_H, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR);
	  GFX_DrawRoundRectangle(DAY_CHECK_RELAY_BUTTON_7_X, DAY_CHECK_RELAY_BUTTON_Y, DAY_CHECK_BUTTON_W, DAY_CHECK_BUTTON_H, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR);
	  GFX_DrawRoundRectangle(DAY_CHECK_RELAY_BUTTON_8_X, DAY_CHECK_RELAY_BUTTON_Y, DAY_CHECK_BUTTON_W, DAY_CHECK_BUTTON_H, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR);
	  GFX_DrawRoundRectangle(DAY_CHECK_RELAY_BUTTON_9_X, DAY_CHECK_RELAY_BUTTON_Y, DAY_CHECK_BUTTON_W, DAY_CHECK_BUTTON_H, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR);

	  sprintf((char*)Msg, "R1  R2  R3  R4 WS L1  L2  L3  L4");
	  EF_PutString(Msg, (DAY_CHECK_RELAY_BUTTON_1_X), (DAY_CHECK_RELAY_BUTTON_Y + 25), ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);

	  GFX_DrawFillRoundRectangle(ONE_HOUR_ADD_SHEDULE_X, HOUR_MINUTE_ON_ADD_SHEDULE_Y, HOOUR_MINUTE_BUTTON_W, DAY_CHECK_BUTTON_H, DAY_CHECK_BUTTON_R, SHEDULE_ADD_HOUR_MINUTE_BUTTON_COLOR);
	  sprintf((char*)Msg, "+1H");
	  EF_PutString(Msg, (ONE_HOUR_ADD_SHEDULE_X+6), (HOUR_MINUTE_ON_ADD_SHEDULE_Y + 3), ILI9341_BLACK, BG_TRANSPARENT, SHEDULE_ADD_HOUR_MINUTE_BUTTON_COLOR);

	  GFX_DrawFillRoundRectangle(ONE_MINUTE_ADD_SHEDULE_X, HOUR_MINUTE_ON_ADD_SHEDULE_Y, HOOUR_MINUTE_BUTTON_W, DAY_CHECK_BUTTON_H, DAY_CHECK_BUTTON_R, SHEDULE_ADD_HOUR_MINUTE_BUTTON_COLOR);
	  sprintf((char*)Msg, "+1M");
	  EF_PutString(Msg, (ONE_MINUTE_ADD_SHEDULE_X+3), (HOUR_MINUTE_ON_ADD_SHEDULE_Y + 3), ILI9341_BLACK, BG_TRANSPARENT, SHEDULE_ADD_HOUR_MINUTE_BUTTON_COLOR);

	  GFX_DrawFillRoundRectangle(TEN_MINUTE_ADD_SHEDULE_X, HOUR_MINUTE_ON_ADD_SHEDULE_Y, HOOUR_MINUTE_BUTTON_W, DAY_CHECK_BUTTON_H, DAY_CHECK_BUTTON_R, SHEDULE_ADD_HOUR_MINUTE_BUTTON_COLOR);
	  sprintf((char*)Msg, "+10M");
	  EF_PutString(Msg, (TEN_MINUTE_ADD_SHEDULE_X+1), (HOUR_MINUTE_ON_ADD_SHEDULE_Y + 3), ILI9341_BLACK, BG_TRANSPARENT, SHEDULE_ADD_HOUR_MINUTE_BUTTON_COLOR);


	  GFX_DrawFillRoundRectangle(ONE_HOUR_ADD_SHEDULE_X, HOUR_MINUTE_OFF_ADD_SHEDULE_Y, HOOUR_MINUTE_BUTTON_W, DAY_CHECK_BUTTON_H, DAY_CHECK_BUTTON_R, SHEDULE_ADD_HOUR_MINUTE_BUTTON_COLOR);
	  sprintf((char*)Msg, "+1H");
	  EF_PutString(Msg, (ONE_HOUR_ADD_SHEDULE_X+6), (HOUR_MINUTE_OFF_ADD_SHEDULE_Y + 3), ILI9341_BLACK, BG_TRANSPARENT, SHEDULE_ADD_HOUR_MINUTE_BUTTON_COLOR);

	  GFX_DrawFillRoundRectangle(ONE_MINUTE_ADD_SHEDULE_X, HOUR_MINUTE_OFF_ADD_SHEDULE_Y, HOOUR_MINUTE_BUTTON_W, DAY_CHECK_BUTTON_H, DAY_CHECK_BUTTON_R, SHEDULE_ADD_HOUR_MINUTE_BUTTON_COLOR);
	  sprintf((char*)Msg, "+1M");
	  EF_PutString(Msg, (ONE_MINUTE_ADD_SHEDULE_X+3), (HOUR_MINUTE_OFF_ADD_SHEDULE_Y + 3), ILI9341_BLACK, BG_TRANSPARENT, SHEDULE_ADD_HOUR_MINUTE_BUTTON_COLOR);

	  GFX_DrawFillRoundRectangle(TEN_MINUTE_ADD_SHEDULE_X, HOUR_MINUTE_OFF_ADD_SHEDULE_Y, HOOUR_MINUTE_BUTTON_W, DAY_CHECK_BUTTON_H, DAY_CHECK_BUTTON_R, SHEDULE_ADD_HOUR_MINUTE_BUTTON_COLOR);
	  sprintf((char*)Msg, "+10M");
	  EF_PutString(Msg, (TEN_MINUTE_ADD_SHEDULE_X+1), (HOUR_MINUTE_OFF_ADD_SHEDULE_Y + 3), ILI9341_BLACK, BG_TRANSPARENT, SHEDULE_ADD_HOUR_MINUTE_BUTTON_COLOR);

	  //
	  // Display hour and minute ON and OFF



	  EF_SetFont(&arialBlack_20ptFontInfo);
	  if(NrOfschedule == 1)
	  {
		  // Read hour and minute of ON and OFF
		  EEPROM_ScheduleHourOnRead(1, &hourOnschedule1);
		  EEPROM_ScheduleMinuteOnRead(1, &minuteOnschedule1);
		  EEPROM_ScheduleHourOffRead(1, &hourOffschedule1);
		  EEPROM_ScheduleMinuteOffRead(1, &minuteOffschedule1);
		  // Read day of week and relay / lights in current schedule
		  EEPROM_ScheduleDayInWeekRead(1, schedule1DayInWeekTab);
		  EEPROM_ScheduleRelayAndSwitchTabRead(1, schedule1RelayAndSwitchTab);

		  if(hourOnschedule1 > 23) hourOnschedule1 = 23;
		  if(minuteOnschedule1 > 59) minuteOnschedule1 = 59;
		  if(hourOffschedule1 > 23) hourOffschedule1 = 23;
		  if(minuteOffschedule1 > 59) minuteOffschedule1 = 59;

		  if((hourOnschedule1 < 10)&&(minuteOnschedule1 < 10))sprintf((char*)Msg, "ON:   0%d  :0%d", hourOnschedule1, minuteOnschedule1);
		  else if(hourOnschedule1 < 10) sprintf((char*)Msg, "ON:   0%d  : %d", hourOnschedule1, minuteOnschedule1);
		  else if (minuteOnschedule1 < 10) sprintf((char*)Msg, "ON:    %d  :0%d", hourOnschedule1, minuteOnschedule1);
		  else sprintf((char*)Msg, "ON:    %d  : %d", hourOnschedule1, minuteOnschedule1);
		  EF_PutString(Msg, STRING_ON_OFF_X, STRING_ON_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);

		  if((hourOffschedule1 < 10)&&(minuteOffschedule1 < 10))sprintf((char*)Msg, "OFF: 0%d  :0%d", hourOffschedule1, minuteOffschedule1);
		  else if(hourOffschedule1 < 10) sprintf((char*)Msg, "OFF: 0%d  : %d", hourOffschedule1, minuteOffschedule1);
		  else if (minuteOffschedule1 < 10) sprintf((char*)Msg, "OFF:  %d  :0%d", hourOffschedule1, minuteOffschedule1);
		  else sprintf((char*)Msg, "OFF:  %d  : %d", hourOffschedule1, minuteOffschedule1);
		  EF_PutString(Msg, STRING_ON_OFF_X, STRING_OFF_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);
	  }
	  else if(NrOfschedule == 2)
	  {
		  // Read hour and minute of ON and OFF
		  EEPROM_ScheduleHourOnRead(2, &hourOnschedule2);
		  EEPROM_ScheduleMinuteOnRead(2, &minuteOnschedule2);
		  EEPROM_ScheduleHourOffRead(2, &hourOffschedule2);
		  EEPROM_ScheduleMinuteOffRead(2, &minuteOffschedule2);
		  // Read day of week and relay / lights in current schedule
		  EEPROM_ScheduleDayInWeekRead(2, schedule2DayInWeekTab);
		  EEPROM_ScheduleRelayAndSwitchTabRead(2, schedule2RelayAndSwitchTab);

		  if(hourOnschedule2 > 23) hourOnschedule2 = 23;
		  if(minuteOnschedule2 > 59) minuteOnschedule2 = 59;
		  if(hourOffschedule2 > 23) hourOffschedule2 = 23;
		  if(minuteOffschedule2 > 59) minuteOffschedule2 = 59;

		  if((hourOnschedule2 < 10)&&(minuteOnschedule2 < 10))sprintf((char*)Msg, "ON:    0%d :0%d", hourOnschedule2, minuteOnschedule2);
		  else if(hourOnschedule2 < 10) sprintf((char*)Msg, "ON:    0%d : %d", hourOnschedule2, minuteOnschedule2);
		  else if (minuteOnschedule2 < 10) sprintf((char*)Msg, "ON:     %d :0%d", hourOnschedule2, minuteOnschedule2);
		  else sprintf((char*)Msg, "ON:     %d : %d", hourOnschedule2, minuteOnschedule2);
		  EF_PutString(Msg, STRING_ON_OFF_X, STRING_ON_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);

		  if((hourOffschedule2 < 10)&&(minuteOffschedule2 < 10))sprintf((char*)Msg, "OFF:  0%d :0%d", hourOffschedule2, minuteOffschedule2);
		  else if(hourOffschedule2 < 10) sprintf((char*)Msg, "OFF:  0%d : %d", hourOffschedule2, minuteOffschedule2);
		  else if (minuteOffschedule2 < 10) sprintf((char*)Msg, "OFF:   %d :0%d", hourOffschedule2, minuteOffschedule2);
		  else sprintf((char*)Msg, "OFF:   %d : %d", hourOffschedule2, minuteOffschedule2);
		  EF_PutString(Msg, STRING_ON_OFF_X+2, STRING_OFF_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);
	  }
}

void showSchedule1Panel()
{
	  ILI9341_ClearDisplay(ILI9341_LIGHTGREY);
	  EF_SetFont(&arialBlack_20ptFontInfo);

	  // Display Header
	  sprintf((char*)Msg, "HARMONOGRAM 1");
	  EF_PutString(Msg, CLOCK_STRING_POZ_X-5, SWIATLA_STRING_POZ_Y, ILI9341_ORANGE, BG_TRANSPARENT, ILI9341_LIGHTGREY);
	  GFX_DrawLine(0, HEADER_UNDERLINE_POZ_Y, 320, HEADER_UNDERLINE_POZ_Y, HEADER_UNDERLINE_COLOR);
	  GFX_DrawLine(0, HEADER_UNDERLINE_POZ_Y+2, 320, HEADER_UNDERLINE_POZ_Y+2, HEADER_UNDERLINE_COLOR);

	  drawschedulePanel(1);

	  //
	  // Display button to change screen
	  EF_SetFont(&arial_11ptFontInfo);
	  GFX_DrawFillRoundRectangle(RIGHT_BUTTON_X, RIGHT_BUTTON_Y, RIGHT_BUTTON_W, RIGHT_BUTTON_H, RIGHT_LEFT_BUTTON_R,  ILI9341_GREEN);
	  sprintf((char*)Msg, "HARM. 2=>");
	  EF_PutString(Msg, (RIGHT_BUTTON_X + 10), (RIGHT_BUTTON_Y + 2), ILI9341_BLACK, BG_TRANSPARENT, ILI9341_GREEN);

	  GFX_DrawFillRoundRectangle(MEDIUM_BUTTON_X, MEDIUM_BUTTON_Y, MEDIUM_BUTTON_W, MEDIUM_BUTTON_H, RIGHT_LEFT_BUTTON_R,  ILI9341_ORANGE);
	  sprintf((char*)Msg, " >POTW<");
	  EF_PutString(Msg, (MEDIUM_BUTTON_X + 6), (MEDIUM_BUTTON_Y + 2), ILI9341_BLACK, BG_TRANSPARENT, ILI9341_GREEN);

	  GFX_DrawFillRoundRectangle(LEFT_BUTTON_X, LEFT_BUTTON_Y, LEFT_BUTTON_W, LEFT_BUTTON_H, RIGHT_LEFT_BUTTON_R,  ILI9341_GREEN);
	  sprintf((char*)Msg, "<= CLOCK");
	  EF_PutString(Msg, (LEFT_BUTTON_X + 3), (LEFT_BUTTON_Y + 2), ILI9341_BLACK, BG_TRANSPARENT, ILI9341_GREEN);
}

void showSchedule2Panel()
{
	  ILI9341_ClearDisplay(ILI9341_LIGHTGREY);
	  EF_SetFont(&arialBlack_20ptFontInfo);
	  // Display Header
	  sprintf((char*)Msg, "HARMONOGRAM 2");
	  EF_PutString(Msg, CLOCK_STRING_POZ_X-5, SWIATLA_STRING_POZ_Y, ILI9341_ORANGE, BG_TRANSPARENT, ILI9341_LIGHTGREY);
	  GFX_DrawLine(0, HEADER_UNDERLINE_POZ_Y, 320, HEADER_UNDERLINE_POZ_Y, HEADER_UNDERLINE_COLOR);
	  GFX_DrawLine(0, HEADER_UNDERLINE_POZ_Y+2, 320, HEADER_UNDERLINE_POZ_Y+2, HEADER_UNDERLINE_COLOR);

	  drawschedulePanel(2);

	  //
	  // Display button to change screen
	  EF_SetFont(&arial_11ptFontInfo);
	  GFX_DrawFillRoundRectangle(MEDIUM_BUTTON_X, MEDIUM_BUTTON_Y, MEDIUM_BUTTON_W, MEDIUM_BUTTON_H, RIGHT_LEFT_BUTTON_R,  ILI9341_ORANGE);
	  sprintf((char*)Msg, " >POTW<");
	  EF_PutString(Msg, (MEDIUM_BUTTON_X + 6), (MEDIUM_BUTTON_Y + 2), ILI9341_BLACK, BG_TRANSPARENT, ILI9341_GREEN);

	  GFX_DrawFillRoundRectangle(LEFT_BUTTON_X, LEFT_BUTTON_Y, LEFT_BUTTON_W, LEFT_BUTTON_H, RIGHT_LEFT_BUTTON_R,  ILI9341_GREEN);
	  sprintf((char*)Msg, "<= CLOCK");
	  EF_PutString(Msg, (LEFT_BUTTON_X + 3), (LEFT_BUTTON_Y + 2), ILI9341_BLACK, BG_TRANSPARENT, ILI9341_GREEN);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Another functions connected with screen
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Change displayed time, if is other than current time
//
void ChangeHourOnScreen()
{
	  uint8_t CHour = DS3231_GetHour();
	  uint8_t CMinute = DS3231_GetMinute();
	  EF_SetFont(&arialBlack_20ptFontInfo);

	  //
	  // Change HOURES
	  if(CHour != OldHours)
	  {
		  if(State == MENUTFT_PARAMETERS)
		  {
			  if(CHour < 10)
			  {
				  sprintf((char*)Msg, " 0%d", CHour);
			  }
			  else
			  {
				  sprintf((char*)Msg, " %d", CHour);
			  }
			  EF_PutString(Msg, CZAS_POZ_X + 77 , CZAS_POZ_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);
			  OldHours = CHour;
		  }
	  }
	  //
	  // Change MINUTES
	  if(CMinute != OldMinutes)
	  {
		  if(State == MENUTFT_PARAMETERS)
		  {
			  if(CMinute < 10)
			  {
				  sprintf((char*)Msg, " 0%d  ", CMinute);
			  }
			  else
			  {
				  sprintf((char*)Msg, " %d  ", CMinute);
			  }

			  EF_PutString(Msg, CZAS_POZ_X + 130 , CZAS_POZ_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);
			  OldMinutes = CMinute;
		  }
	  }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Action make during predefined activity - first activity
//
void predefinedActivityCzyszczenie(uint8_t State)
{
	uint8_t NowyStan = State;
	if(NowyStan == 0) //If turn Activity OFF - Filters ON
	{
		// Read which Relay was ON before turn on activity and restore state before activity
		EEPROM_RelayStateRestore();
		EEPROM_LightStateRestore();

		//TODO: To samo zrobic swiatlami, sprawdzic ktore byly wylaczone przed aktywnoscia i je spowrotem wylaczyc

	}
	else 	//If turn Activity ON - Filters OFF, Lights - ON
	{
		SendComand(UCMD_RELAY_ALL_OFF);

		SendComand(UCMD_LIGHT_ALL_ON);

	}
}

//
// Action make during predefined activity - first activity
//
void predefinedActivityKarmienie(uint8_t State)
{
	uint8_t NowyStan = State;
	if(NowyStan == 0) //If turn Activity OFF - Filters ON
	{
		EEPROM_RelayStateRestore();

		EF_SetFont(&arial_11ptFontInfo);
		GFX_DrawFillRoundRectangle(ACTIVITY_BUTTON_X, ACTIVITY_BUTTON_1_Y, ACTIVITY_BUTTON_W, ACTIVITY_BUTTON_H, ACTIVITY_BUTTON_R, SWITCH_OFF_BUTTON_COLOR);
		sprintf((char*)Msg, "KARMIENIE");
		EF_PutString(Msg, (ACTIVITY_BUTTON_X+STRING_ACTIVITIES_1_X_ERRATA), (ACTIVITY_BUTTON_1_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
		sprintf((char*)Msg, "OFF");
		EF_PutString(Msg, (ACTIVITY_BUTTON_X+STRING_ACTIVITIES_ON_OFF_X_ERRATA), (ACTIVITY_BUTTON_1_Y+STRING_ACTIVITIES_Y_INTER), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);

	}
	else 	//If turn Activity ON - Filters OFF, Lights - ON
	{
		SendComand(UCMD_RELAY_ALL_OFF);
		HAL_TIM_Base_Start_IT(&htim10); // start Timer to count time of feeding
	}

}

//
// Draw current state of switches button
//
void drawCurrentStateOfSwitches()
{
	  //
	  // First Switch
	  //
	  EF_SetFont(&arial_11ptFontInfo);
	  if(SwitchesButtonState[0] >= 1)
	  {
		  GFX_DrawFillRoundRectangle(SWITCH_BUTTON_X, SWITCH_1_POZ_Y, SWITCH_BUTTON_W, SWITCH_BUTTON_H, SWITCH_BUTTON_R, SWITCH_ON_BUTTON_COLOR);
		  sprintf((char*)Msg, "ON");
		  EF_PutString(Msg, (SWITCH_BUTTON_X+STRING_ERRATA_X), (SWITCH_1_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_ON_BUTTON_COLOR);
	  }
	  else
	  {
		  GFX_DrawFillRoundRectangle(SWITCH_BUTTON_X, SWITCH_1_POZ_Y, SWITCH_BUTTON_W, SWITCH_BUTTON_H, SWITCH_BUTTON_R, SWITCH_OFF_BUTTON_COLOR);
		  sprintf((char*)Msg, "OFF");
		  EF_PutString(Msg, (SWITCH_BUTTON_X+STRING_ERRATA_X), (SWITCH_1_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
	  }
	  //
	  // Second Switch
	  //
	  if(SwitchesButtonState[1] >= 1)
	  {
		  GFX_DrawFillRoundRectangle(SWITCH_BUTTON_X, SWITCH_2_POZ_Y, SWITCH_BUTTON_W, SWITCH_BUTTON_H, SWITCH_BUTTON_R, SWITCH_ON_BUTTON_COLOR);
		  sprintf((char*)Msg, "ON");
		  EF_PutString(Msg, (SWITCH_BUTTON_X+STRING_ERRATA_X), (SWITCH_2_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_ON_BUTTON_COLOR);
	  }
	  else
	  {
		  GFX_DrawFillRoundRectangle(SWITCH_BUTTON_X, SWITCH_2_POZ_Y, SWITCH_BUTTON_W, SWITCH_BUTTON_H, SWITCH_BUTTON_R, SWITCH_OFF_BUTTON_COLOR);
		  sprintf((char*)Msg, "OFF");
		  EF_PutString(Msg, (SWITCH_BUTTON_X+STRING_ERRATA_X), (SWITCH_2_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
	  }
	  //
	  // Third Switch
	  //
	  if(SwitchesButtonState[2] >= 1)
	  {
		  GFX_DrawFillRoundRectangle(SWITCH_BUTTON_X, SWITCH_3_POZ_Y, SWITCH_BUTTON_W, SWITCH_BUTTON_H, SWITCH_BUTTON_R, SWITCH_ON_BUTTON_COLOR);
		  sprintf((char*)Msg, "ON");
		  EF_PutString(Msg, (SWITCH_BUTTON_X+STRING_ERRATA_X), (SWITCH_3_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_ON_BUTTON_COLOR);
	  }
	  else
	  {
		  GFX_DrawFillRoundRectangle(SWITCH_BUTTON_X, SWITCH_3_POZ_Y, SWITCH_BUTTON_W, SWITCH_BUTTON_H, SWITCH_BUTTON_R, SWITCH_OFF_BUTTON_COLOR);
		  sprintf((char*)Msg, "OFF");
		  EF_PutString(Msg, (SWITCH_BUTTON_X+STRING_ERRATA_X), (SWITCH_3_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
	  }
	  //
	  // Fourth Switch
	  //
	  if(SwitchesButtonState[3] >= 1)
	  {
		  GFX_DrawFillRoundRectangle(SWITCH_BUTTON_X, SWITCH_4_POZ_Y, SWITCH_BUTTON_W, SWITCH_BUTTON_H, SWITCH_BUTTON_R, SWITCH_ON_BUTTON_COLOR);
		  sprintf((char*)Msg, "ON");
		  EF_PutString(Msg, (SWITCH_BUTTON_X+STRING_ERRATA_X), (SWITCH_4_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_ON_BUTTON_COLOR);
	  }
	  else
	  {
		  GFX_DrawFillRoundRectangle(SWITCH_BUTTON_X, SWITCH_4_POZ_Y, SWITCH_BUTTON_W, SWITCH_BUTTON_H, SWITCH_BUTTON_R, SWITCH_OFF_BUTTON_COLOR);
		  sprintf((char*)Msg, "OFF");
		  EF_PutString(Msg, (SWITCH_BUTTON_X+STRING_ERRATA_X), (SWITCH_4_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
	  }
}

//
//
// Draw current state of lights button
//
void drawCurrentStateOfLights()
{
	  //
	  // First Light
	  //
	  EF_SetFont(&arial_11ptFontInfo);
	  if(LightsButtonState[0] >= 1)
	  {
		  GFX_DrawFillRoundRectangle(LIGHTS_BUTTON_X, LIGHT_B_1_POZ_Y, LIGHTS_BUTTON_W, LIGHTS_BUTTON_H, LIGHTS_BUTTON_R, SWITCH_ON_BUTTON_COLOR);
		  sprintf((char*)Msg, "ON");
		  EF_PutString(Msg, (LIGHTS_BUTTON_X+STRING_ERRATA_X), (LIGHT_B_1_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
	  }
	  else
	  {
		  GFX_DrawFillRoundRectangle(LIGHTS_BUTTON_X, LIGHT_B_1_POZ_Y, LIGHTS_BUTTON_W, LIGHTS_BUTTON_H, LIGHTS_BUTTON_R, SWITCH_OFF_BUTTON_COLOR);
		  sprintf((char*)Msg, "OFF");
		  EF_PutString(Msg, (LIGHTS_BUTTON_X+STRING_ERRATA_X), (LIGHT_B_1_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
	  }
	  //
	  // Second Light
	  //
	  if(LightsButtonState[1] >= 1)
	  {
		  GFX_DrawFillRoundRectangle(LIGHTS_BUTTON_X, LIGHT_B_2_POZ_Y, LIGHTS_BUTTON_W, LIGHTS_BUTTON_H, LIGHTS_BUTTON_R, SWITCH_ON_BUTTON_COLOR);
		  sprintf((char*)Msg, "ON");
		  EF_PutString(Msg, (LIGHTS_BUTTON_X+STRING_ERRATA_X), (LIGHT_B_2_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
	  }
	  else
	  {
		  GFX_DrawFillRoundRectangle(LIGHTS_BUTTON_X, LIGHT_B_2_POZ_Y, LIGHTS_BUTTON_W, LIGHTS_BUTTON_H, LIGHTS_BUTTON_R, SWITCH_OFF_BUTTON_COLOR);
		  sprintf((char*)Msg, "OFF");
		  EF_PutString(Msg, (LIGHTS_BUTTON_X+STRING_ERRATA_X), (LIGHT_B_2_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
	  }
	  //
	  // Third Light
	  //
	  if(LightsButtonState[2] >= 1)
	  {
		  GFX_DrawFillRoundRectangle(LIGHTS_BUTTON_X, LIGHT_B_3_POZ_Y, LIGHTS_BUTTON_W, LIGHTS_BUTTON_H, LIGHTS_BUTTON_R, SWITCH_ON_BUTTON_COLOR);
		  sprintf((char*)Msg, "ON");
		  EF_PutString(Msg, (LIGHTS_BUTTON_X+STRING_ERRATA_X), (LIGHT_B_3_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
	  }
	  else
	  {
		  GFX_DrawFillRoundRectangle(LIGHTS_BUTTON_X, LIGHT_B_3_POZ_Y, LIGHTS_BUTTON_W, LIGHTS_BUTTON_H, LIGHTS_BUTTON_R, SWITCH_OFF_BUTTON_COLOR);
		  sprintf((char*)Msg, "OFF");
		  EF_PutString(Msg, (LIGHTS_BUTTON_X+STRING_ERRATA_X), (LIGHT_B_3_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
	  }
	  //
	  // Fourth Light
	  //
	  if(LightsButtonState[3] >= 1)
	  {
		  GFX_DrawFillRoundRectangle(LIGHTS_BUTTON_X, LIGHT_B_4_POZ_Y, LIGHTS_BUTTON_W, LIGHTS_BUTTON_H, LIGHTS_BUTTON_R, SWITCH_ON_BUTTON_COLOR);
		  sprintf((char*)Msg, "ON");
		  EF_PutString(Msg, (LIGHTS_BUTTON_X+STRING_ERRATA_X), (LIGHT_B_4_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
	  }
	  else
	  {
		  GFX_DrawFillRoundRectangle(LIGHTS_BUTTON_X, LIGHT_B_4_POZ_Y, LIGHTS_BUTTON_W, LIGHTS_BUTTON_H, LIGHTS_BUTTON_R, SWITCH_OFF_BUTTON_COLOR);
		  sprintf((char*)Msg, "OFF");
		  EF_PutString(Msg, (LIGHTS_BUTTON_X+STRING_ERRATA_X), (LIGHT_B_4_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
	  }
}


//
// Restore on TFT screen rectangle indicate each day of week, from EEPROM
void restoreTFTScheduleDayOfWeek(uint8_t NrOfSchedule)
{
	uint8_t TempDayTab[7] = {0};
	EEPROM_ScheduleDayInWeekRead(NrOfSchedule, TempDayTab);

	if(TempDayTab[0]) changeTFTScheduleDayOfWeek(1, 1);
	else changeTFTScheduleDayOfWeek(1, 0);
	if(TempDayTab[1]) changeTFTScheduleDayOfWeek(2, 1);
	else changeTFTScheduleDayOfWeek(2, 0);
	if(TempDayTab[2]) changeTFTScheduleDayOfWeek(3, 1);
	else changeTFTScheduleDayOfWeek(3, 0);
	if(TempDayTab[3]) changeTFTScheduleDayOfWeek(4, 1);
	else changeTFTScheduleDayOfWeek(4, 0);
	if(TempDayTab[4]) changeTFTScheduleDayOfWeek(5, 1);
	else changeTFTScheduleDayOfWeek(5, 0);
	if(TempDayTab[5]) changeTFTScheduleDayOfWeek(6, 1);
	else changeTFTScheduleDayOfWeek(6, 0);
	if(TempDayTab[6]) changeTFTScheduleDayOfWeek(7, 1);
	else changeTFTScheduleDayOfWeek(7, 0);

}

//
// Restore on TFT screen rectangle indicate each Relay and Light, from EEPROM
void restoreTFTScheduleRelayLights(uint8_t NrOfSchedule)
{
	uint8_t TempRelayLIghtsTab[9] = {0};
	EEPROM_ScheduleRelayAndSwitchTabRead(NrOfSchedule, TempRelayLIghtsTab);

	if(TempRelayLIghtsTab[0]) changeTFTScheduleRelayLights (1, 1);
	else changeTFTScheduleRelayLights(1, 0);
	if(TempRelayLIghtsTab[1]) changeTFTScheduleRelayLights(2, 1);
	else changeTFTScheduleRelayLights(2, 0);
	if(TempRelayLIghtsTab[2]) changeTFTScheduleRelayLights(3, 1);
	else changeTFTScheduleRelayLights(3, 0);
	if(TempRelayLIghtsTab[3]) changeTFTScheduleRelayLights(4, 1);
	else changeTFTScheduleRelayLights(4, 0);
	if(TempRelayLIghtsTab[4]) changeTFTScheduleRelayLights(5, 1);
	else changeTFTScheduleRelayLights(5, 0);
	if(TempRelayLIghtsTab[5]) changeTFTScheduleRelayLights(6, 1);
	else changeTFTScheduleRelayLights(6, 0);
	if(TempRelayLIghtsTab[6]) changeTFTScheduleRelayLights(7, 1);
	else changeTFTScheduleRelayLights(7, 0);
	if(TempRelayLIghtsTab[7]) changeTFTScheduleRelayLights(8, 1);
	else changeTFTScheduleRelayLights(8, 0);
	if(TempRelayLIghtsTab[8]) changeTFTScheduleRelayLights(9, 1);
	else changeTFTScheduleRelayLights(9, 0);

}

//
// Function to wait but only if it is necessary
void initWait(uint32_t TimeInMs)
{
    while (HAL_GetTick() - LastTime <= TimeInMs)
    {
    }
}
