/**
  ******************************************************************************
  ******************************************************************************
  * @file           : menuTFT.c
  * @project		: free_michelle
  * @author			: grados73 - https://github.com/grados73
  * @purpose		: file to service menu on TFT display
  ******************************************************************************
  **/

#include "main.h"
#include "menuTFT.h"
#include "TFT_ILI9341.h"
#include "XPT2064.h"
#include "GFX_Color.h"
#include "GFX_EnhancedFonts.h"
#include "functions.h"
#include "parser.h"
#include "ds3231_for_stm32_hal.h"
#include "eeprom.h"

extern float CTemp;
extern float CPres;
extern uint8_t SwitchesButtonState[4];
extern uint8_t LightsButtonState[4];
extern uint8_t ActivityButtonState[2];
uint8_t StateChangeFlag = 0; // using to indicate change screen activity
uint8_t ClockChangeFlag = 0; // using to indicate change screen to Clock Set
uint8_t WSLedChangeFlag = 0; // using to indicate change screen to WS LEDs Set
uint8_t ScheduleChangeFlag = 0; // using to indicate change screen to Schedule
uint8_t DayOfWeek = 1;
uint8_t Hours = 15;
uint8_t Minutes = 5;
uint8_t NrOfLeds = 15;

uint8_t schedule1DayInWeekTab[7] = {0};	// schedule1DayInWeekTab[] = {Mon, Tue, Wed, Thu, Fri, Sat, Sun}
uint8_t schedule1RelayAndSwitchTab[9] = {0}; // schedule1RelayAndSwitchTab[] = { R1, R2, R3, R4, WS, L1, L2, L3, L4}
uint8_t schedule2DayInWeekTab[7] = {0};	// schedule2DayInWeekTab[] = {Mon, Tue, Wed, Thu, Fri, Sat, Sun}
uint8_t schedule2RelayAndSwitchTab[9] = {0}; // schedule2RelayAndSwitchTab[] = { R1, R2, R3, R4, WS, L1, L2, L3, L4}
uint8_t hourOnSchedule1 = 0;
uint8_t hourOffSchedule1 = 0;
uint8_t minuteOnSchedule1 = 0;
uint8_t minuteOffSchedule1 = 0;
uint8_t hourOnSchedule2 = 0;
uint8_t hourOffSchedule2 = 0;
uint8_t minuteOnSchedule2 = 0;
uint8_t minuteOffSchedule2 = 0;

extern UARTDMA_HandleTypeDef huartdma1;



uint32_t TimerTouch = 0; // Timer to debouncing function

MenuTFTState State = MENUTFT_INIT; // Initialization state for MenuTFT State Machine

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	MAIN FUNCTION TO HANDLING STAND MACHINE OF SCREEN
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MenuTFT(void)
{
	switch(State)
	{
	case MENUTFT_INIT:
		system_init();
		State = MENUTFT_PARAMETERS;
		StateChangeFlag = 1;
		break;
	case MENUTFT_PARAMETERS:
		if(StateChangeFlag == 1) // make only one time
		{
			showCurrentParameters(CTemp, 0, 0, 0, CPres);
			StateChangeFlag = 0;
		}
		TouchParametersActivity();
		break;
	case MENUTFT_SWITCH:
		if(StateChangeFlag == 1) // make only one time
		{
			showControlPanel();
			StateChangeFlag = 0;
		}
		TouchSwitchActivity();
		break;
	case MENUTFT_LIGHTS:
		if(StateChangeFlag == 1) // make only one time
		{
			showLightsControlPanel();
			StateChangeFlag = 0;
		}
		TouchLightsActivity();
		break;
	case MENUTFT_CLOCK:
		if(StateChangeFlag == 1) // make only one time
		{
			showClockSetPanel();
			ClockChangeFlag = 1;
			StateChangeFlag = 0;
		}
		TouchClockActivity();
		break;
	case MENUTFT_ACTIVITIES:
		if(StateChangeFlag == 1) // make only one time
		{
			showPreparedActivitiesPanel();
			StateChangeFlag = 0;
		}
		TouchPredefinedActivityActivity();
		break;
	case MENUTFT_WS_LED:
		if(StateChangeFlag == 1) // make only one time
		{
			showWSLedPanel();
			WSLedChangeFlag = 1;
			StateChangeFlag = 0;
		}
		TouchWSLedActivity();
		break;
	case MENUTFT_SCHEDULE_1:
		if(StateChangeFlag == 1) // make only one time
		{
			showSchedule1Panel();
			ScheduleChangeFlag = 1;
			StateChangeFlag = 0;
		}
		Schedule1Activity();
		break;
	case MENUTFT_SCHEDULE_2:
		if(StateChangeFlag == 1) // make only one time
		{
			showSchedule2Panel();
			ScheduleChangeFlag = 1;
			StateChangeFlag = 0;
		}
		Schedule2Activity();
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Function to handling touch on Parameters Screen
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TouchParametersActivity(void)
{
	// Check if screen was touched
	if(XPT2046_IsTouched())
	{
		if(HAL_GetTick() - TimerTouch >= SWITCH_DEBOUNCING_TIME_MS) // If pass 1000ms from last change State
		{
			uint16_t x, y; // Touch points
			XPT2046_GetTouchPoint(&x, &y); // Get the current couched point

			//
			// Check if it is button to change screen
			//
			// Check if that point is inside the LEFT Button
			if((x >= LEFT_BUTTON_X)&&(x <= (LEFT_BUTTON_X+LEFT_BUTTON_W))&&
					(y >= LEFT_BUTTON_Y)&&(y <= (LEFT_BUTTON_Y + LEFT_BUTTON_H)))
			{
				State = MENUTFT_LIGHTS;
				StateChangeFlag = 1;
			}

			// Check if that point is inside the RIGHT Button
			else if((x >= RIGHT_BUTTON_X)&&(x <= (RIGHT_BUTTON_X+RIGHT_BUTTON_W))&&
					(y >= RIGHT_BUTTON_Y)&&(y <= (RIGHT_BUTTON_Y + RIGHT_BUTTON_H)))
			{
				State = MENUTFT_SWITCH;
				StateChangeFlag = 1;
			}

			// Check if that point is inside the MEDIUM Button
			else if((x >= MEDIUM_BUTTON_X)&&(x <= (MEDIUM_BUTTON_X+MEDIUM_BUTTON_W))&&
					(y >= MEDIUM_BUTTON_Y)&&(y <= (MEDIUM_BUTTON_Y + MEDIUM_BUTTON_H)))
			{
				State = MENUTFT_CLOCK;
				StateChangeFlag = 1;
			}
			TimerTouch = HAL_GetTick();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Function to handling touch on Switches Screen
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TouchSwitchActivity(void)
{
	// Check if screen was touched
	if(XPT2046_IsTouched())
	{
		if(HAL_GetTick() - TimerTouch >= SWITCH_DEBOUNCING_TIME_MS) // If pass 1000ms from last change State
		{
			uint16_t x, y; // Touch points
			XPT2046_GetTouchPoint(&x, &y); // Get the current couched point

			//
			// Check if it is button to change screen
			//
			// Check if that point is inside the LEFT Button
			if((x >= LEFT_BUTTON_X)&&(x <= (LEFT_BUTTON_X+LEFT_BUTTON_W))&&
					(y >= LEFT_BUTTON_Y)&&(y <= (LEFT_BUTTON_Y + LEFT_BUTTON_H)))
			{
				State = MENUTFT_PARAMETERS;
				StateChangeFlag = 1;
			}

			// Check if that point is inside the RIGHT Button
			else if((x >= RIGHT_BUTTON_X)&&(x <= (RIGHT_BUTTON_X+RIGHT_BUTTON_W))&&
					(y >= RIGHT_BUTTON_Y)&&(y <= (RIGHT_BUTTON_Y + RIGHT_BUTTON_H)))
			{
				State = MENUTFT_LIGHTS;
				StateChangeFlag = 1;
			}

			// Check if that point is inside the MEDIUM Button
			else if((x >= MEDIUM_BUTTON_X)&&(x <= (MEDIUM_BUTTON_X+MEDIUM_BUTTON_W))&&
					(y >= MEDIUM_BUTTON_Y)&&(y <= (MEDIUM_BUTTON_Y + MEDIUM_BUTTON_H)))
			{
				State = MENUTFT_ACTIVITIES;
				StateChangeFlag = 1;
			}

			//
			// Check if it is button to change SWITCH status
			//
			//TODO: POTWIERDZENIE WYKONANIA POLECENIA!!!
			else if((x >= SWITCH_BUTTON_X)&&(x <= (SWITCH_BUTTON_X + SWITCH_BUTTON_W)))
			{
				EF_SetFont(&arial_11ptFontInfo);
				changeSwitchStatusTouch(y);
			}
			TimerTouch = HAL_GetTick();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Function to handling touch on Lights Screen
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TouchLightsActivity(void)
{
	// Check if screen was touched
	if(XPT2046_IsTouched())
	{
		if(HAL_GetTick() - TimerTouch >= SWITCH_DEBOUNCING_TIME_MS) // If pass 1000ms from last change State
		{
			uint16_t x, y; // Touch points
			XPT2046_GetTouchPoint(&x, &y); // Get the current couched point

			//
			// Check if it is button to change screen
			//
			// Check if that point is inside the LEFT Button
			if((x >= LEFT_BUTTON_X)&&(x <= (LEFT_BUTTON_X+LEFT_BUTTON_W))&&
					(y >= LEFT_BUTTON_Y)&&(y <= (LEFT_BUTTON_Y + LEFT_BUTTON_H)))
			{
				State = MENUTFT_SWITCH;
				StateChangeFlag = 1;
			}

			// Check if that point is inside the RIGHT Button
			else if((x >= RIGHT_BUTTON_X)&&(x <= (RIGHT_BUTTON_X+RIGHT_BUTTON_W))&&
					(y >= RIGHT_BUTTON_Y)&&(y <= (RIGHT_BUTTON_Y + RIGHT_BUTTON_H)))
			{
				State = MENUTFT_PARAMETERS;
				StateChangeFlag = 1;
			}

			// Check if that point is inside the MEDIUM Button
			else if((x >= MEDIUM_BUTTON_X)&&(x <= (MEDIUM_BUTTON_X+MEDIUM_BUTTON_W))&&
					(y >= MEDIUM_BUTTON_Y)&&(y <= (MEDIUM_BUTTON_Y + MEDIUM_BUTTON_H)))
			{
				State = MENUTFT_WS_LED;
				StateChangeFlag = 1;
			}

			//
			// Check if it is button to change LIGHTS status
			//
			//TODO: POWIERDZENIA WYKONANIA KOMENDY
			else if((x >= LIGHTS_BUTTON_X)&&(x <= (LIGHTS_BUTTON_X + LIGHTS_BUTTON_W)))
			{
				EF_SetFont(&arial_11ptFontInfo);
				changeLightsStatusTouch(y);
			}

			TimerTouch = HAL_GetTick();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Function to handling touch on Set CLOCK Screen
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TouchClockActivity(void)
{
	// Check if screen was touched
		if(XPT2046_IsTouched())
		{
			EF_SetFont(&arialBlack_20ptFontInfo);
			if(HAL_GetTick() - TimerTouch >= SWITCH_DEBOUNCING_TIME_MS) // If pass 1000ms from last change State
			{
				uint16_t x, y; // Touch points

				XPT2046_GetTouchPoint(&x, &y); // Get the current couched point

				if( 1 == ClockChangeFlag) // If we just get inside this screen we must get current Hour and Minutes to easier change them, but only once
				{
					Hours = DS3231_GetHour();
					Minutes = DS3231_GetMinute();
					DayOfWeek = DS3231_GetDayOfWeek();
					ClockChangeFlag = 0;
				}

				//
				// Check if it is button to change screen
				//
				// Check if that point is inside the LEFT Button - back to Parameters
				if((x >= LEFT_BUTTON_X)&&(x <= (LEFT_BUTTON_X+LEFT_BUTTON_W))&&
						(y >= LEFT_BUTTON_Y)&&(y <= (LEFT_BUTTON_Y + LEFT_BUTTON_H)))
				{
					State = MENUTFT_PARAMETERS;
					StateChangeFlag = 1;
				}

				// Check if that point is inside the RIGHT Button - Confirmed changed clock
				else if((x >= RIGHT_BUTTON_X)&&(x <= (RIGHT_BUTTON_X+RIGHT_BUTTON_W))&&
						(y >= RIGHT_BUTTON_Y)&&(y <= (RIGHT_BUTTON_Y + RIGHT_BUTTON_H)))
				{
					DS3231_SetDayOfWeek(DayOfWeek);
					DS3231_SetHour(Hours);
					DS3231_SetMinute(Minutes);
					DS3231_SetSecond(50);
					sprintf((char*)Msg, "-Time Changed-");
					EF_PutString(Msg, CLOCK_STRING_POZ_X, CLOCK_STRING_POZ_Y, ILI9341_GREEN, BG_COLOR, ILI9341_LIGHTGREY);
				}

				// Check if that point is inside the Medium Button - change screen to Schedule  screen
				else if((x >= MEDIUM_BUTTON_CLOCK_X)&&(x <= (MEDIUM_BUTTON_CLOCK_X + MEDIUM_BUTTON_CLOCK_W))&&
						(y >= MEDIUM_BUTTON_CLOCK_Y)&&(y <= (MEDIUM_BUTTON_CLOCK_Y + MEDIUM_BUTTON_H_CLOCK)))
				{
					State = MENUTFT_SCHEDULE_1;
					StateChangeFlag = 1;
				}


				//
				// Check if it is button to increase by an ONE (1) HOUR / MINUT - first ROW
				//
				else if((x >= CLOCK_BUTTON_X)&&(x <= (CLOCK_BUTTON_X + CLOCK_BUTTON_W)))
				{
					clockIncreaseOneHourAndMinute(x, y);
				}

				//
				// Check if it is button to increase by an 6 HOURS / 10 MINUTS - second ROW
				//
				else if((x >= (CLOCK_BUTTON2_X))&&(x <= (CLOCK_BUTTON2_X + CLOCK_BUTTON_W)))
				{
					clockIncreaseSixHoursTenMinutes(x,y);
				}
				TimerTouch = HAL_GetTick();
			}
		}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Function to handling touch on Predefined Activity Screen
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TouchPredefinedActivityActivity()
{
	if(XPT2046_IsTouched())
	{

		if(HAL_GetTick() - TimerTouch >= SWITCH_DEBOUNCING_TIME_MS) // If pass 1000ms from last change State
		{
			uint16_t x, y; // Touch points
			XPT2046_GetTouchPoint(&x, &y); // Get the current couched point

			EF_SetFont(&arial_11ptFontInfo);
			//
			// Check if it is button to change screen
			//
			// Check if that point is inside the LEFT Button
			if((x >= LEFT_BUTTON_X)&&(x <= (LEFT_BUTTON_X+LEFT_BUTTON_W))&&
					(y >= LEFT_BUTTON_Y)&&(y <= (LEFT_BUTTON_Y + LEFT_BUTTON_H)))
			{
				State = MENUTFT_SWITCH;
				StateChangeFlag = 1;
			}

			//
			// Check if that point is in row where are button to Predefined Activity
			//
			else if((x >= ACTIVITY_BUTTON_X)&&(x <= (ACTIVITY_BUTTON_X + ACTIVITY_BUTTON_W)))
			{
				predefinedActivityTouchAction(y);
			}
			TimerTouch = HAL_GetTick();
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Function to handling touch on LEDs WS2812b controlling screen
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TouchWSLedActivity(void)
{
	// Check if screen was touched
		if(XPT2046_IsTouched())
		{
			EF_SetFont(&arialBlack_20ptFontInfo);
			if(HAL_GetTick() - TimerTouch >= SWITCH_DEBOUNCING_TIME_MS) // If pass 1000ms from last change State
			{
				uint16_t x, y; // Touch points

				XPT2046_GetTouchPoint(&x, &y); // Get the current couched point

				if( 1 == ClockChangeFlag) // If we just get inside this screen we must get number of LEDs, but we do it only once
				{
					while(eeprom_read(EEPROM_ADR_NUMBER_WS_LEDS, &NrOfLeds, sizeof(NrOfLeds)) != HAL_OK); // read number of leds
					WSLedChangeFlag = 0;
				}

				//
				// Check if it is button to change screen
				//
				// Check if that point is inside the LEFT Button - back to Parameters
				if((x >= LEFT_BUTTON_X)&&(x <= (LEFT_BUTTON_X+LEFT_BUTTON_W))&&
						(y >= LEFT_BUTTON_Y)&&(y <= (LEFT_BUTTON_Y + LEFT_BUTTON_H)))
				{
					State = MENUTFT_LIGHTS;
					StateChangeFlag = 1;
				}


				//
				// Check if it is button to increase number of LEDs - first line
				//
				else if((y >= WS_B_1_POZ_Y)&&(y <= (WS_B_1_POZ_Y + WS_LED_BUTTON_H)))
				{
					increaseNumberOfWSLedOnTFT(x,y);
				}

				//
				// Check if it is second line button to change LEDs to WHITE // BLUE // ETC
				//
				else
				{
					changeTypeOfWSLights(x, y);
				}

				TimerTouch = HAL_GetTick();
			}
		}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Function to handling touch on Schedule 1 (ONE) screen
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Schedule1Activity()
{
	if(1 == ScheduleChangeFlag) // initialization variables from eeprom, only once per change screen
	{
		updateAllSchedulesInfo(1);
		ScheduleChangeFlag = 0;
	}

	// Check if screen was touched
	if(XPT2046_IsTouched())
	{
		if(HAL_GetTick() - TimerTouch >= SWITCH_DEBOUNCING_TIME_MS) // If pass 1000ms from last change State
		{
			uint16_t x, y; // Touch points
			XPT2046_GetTouchPoint(&x, &y); // Get the current couched point

			//
			// Check if it is button to change screen
			//
			// Check if that point is inside the LEFT Button
			if((x >= LEFT_BUTTON_X)&&(x <= (LEFT_BUTTON_X+LEFT_BUTTON_W))&&
					(y >= LEFT_BUTTON_Y)&&(y <= (LEFT_BUTTON_Y + LEFT_BUTTON_H)))
			{
				State = MENUTFT_CLOCK;
				StateChangeFlag = 1;
			}

			// Check if that point is inside the RIGHT Button
			else if((x >= RIGHT_BUTTON_X)&&(x <= (RIGHT_BUTTON_X + RIGHT_BUTTON_W))&&
					(y >= RIGHT_BUTTON_Y)&&(y <= (RIGHT_BUTTON_Y + RIGHT_BUTTON_H)))
			{
				State = MENUTFT_SCHEDULE_2;
				StateChangeFlag = 1;
			}

			//Check if touch is inside row with day of week
			else if((y >= DAY_CHECK_BUTTON_Y)&&(y <= (DAY_CHECK_BUTTON_Y + DAY_CHECK_BUTTON_H)))
			{
				MenuTFTSchedule1ActivityDayOfWeekRow(x,y);
			}

			// Check if touch is in row Add hour/minute to ON time
			else if((y >= HOUR_MINUTE_ON_ADD_SHEDULE_Y)&&(y <= (HOUR_MINUTE_ON_ADD_SHEDULE_Y + DAY_CHECK_BUTTON_H)))
			{
				MenuTFTSchedule1ActivityHourMinuteONAdd(x,y);
			}

			// Check if touch is in row Add hour/minute to OFF time
			else if((y >= HOUR_MINUTE_OFF_ADD_SHEDULE_Y)&&(y <= (HOUR_MINUTE_OFF_ADD_SHEDULE_Y + DAY_CHECK_BUTTON_H)))
			{
				MenuTFTSchedule1ActivityHourMinuteOFFAdd(x,y);
			}

			//Check if touch is inside row with rectangle of Relays and Lights
			else if((y >= DAY_CHECK_RELAY_BUTTON_Y)&&(y <= (DAY_CHECK_RELAY_BUTTON_Y + DAY_CHECK_BUTTON_H)))
			{
				MenuTFTSchedule1ActivityRelaysAndLightsRow(x,y);
			}

			// Check if that point is inside the MEDIUM Button - CONFIRM
			else if((x >= MEDIUM_BUTTON_X)&&(x <= (MEDIUM_BUTTON_X+MEDIUM_BUTTON_W))&&
					(y >= MEDIUM_BUTTON_Y)&&(y <= (MEDIUM_BUTTON_Y + MEDIUM_BUTTON_H)))
			{
				EEPROM_ScheduleHourOnUpdate(1, hourOnSchedule1);
				EEPROM_ScheduleMinuteOnUpdate(1, minuteOnSchedule1);
				EEPROM_ScheduleHourOffUpdate(1, hourOffSchedule1);
				EEPROM_ScheduleMinuteOffUpdate(1, minuteOffSchedule1);
				EEPROM_ScheduleDayInWeekUpdate(1,  schedule1DayInWeekTab);
				EEPROM_ScheduleRelayAndSwitchTabUpdate(1, schedule1RelayAndSwitchTab);

				EF_SetFont(&arialBlack_20ptFontInfo);
				sprintf((char*)Msg, " H1 ZMIENIONY      ");
				EF_PutString(Msg, CLOCK_STRING_POZ_X-7, SWIATLA_STRING_POZ_Y, ILI9341_ORANGE, BG_COLOR, ILI9341_LIGHTGREY);

				EF_SetFont(&arial_11ptFontInfo);
				GFX_DrawFillRoundRectangle(MEDIUM_BUTTON_X, MEDIUM_BUTTON_Y, MEDIUM_BUTTON_W, MEDIUM_BUTTON_H, RIGHT_LEFT_BUTTON_R,  ILI9341_GREEN);
				sprintf((char*)Msg, " >POTW<");
				EF_PutString(Msg, (MEDIUM_BUTTON_X + 6), (MEDIUM_BUTTON_Y + 2), ILI9341_BLACK, BG_TRANSPARENT, ILI9341_GREEN);
				//TODO! Day of week
			}
			TimerTouch = HAL_GetTick();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Function to handling touch on Schedule 2 (TWO) screen
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Schedule2Activity()
{

	if(1 == ScheduleChangeFlag) // initialization variables from eeprom, only once per change screen
	{
		updateAllSchedulesInfo(2);
		ScheduleChangeFlag = 0;
	}

	// Check if screen was touched
	if(XPT2046_IsTouched())
	{
		if(HAL_GetTick() - TimerTouch >= SWITCH_DEBOUNCING_TIME_MS) // If pass 1000ms from last change State
		{
			uint16_t x, y; // Touch points
			XPT2046_GetTouchPoint(&x, &y); // Get the current couched point

			//
			// Check if it is button to change screen
			//
			// Check if that point is inside the LEFT Button
			if((x >= LEFT_BUTTON_X)&&(x <= (LEFT_BUTTON_X+LEFT_BUTTON_W))&&
					(y >= LEFT_BUTTON_Y)&&(y <= (LEFT_BUTTON_Y + LEFT_BUTTON_H)))
			{
				State = MENUTFT_CLOCK;
				StateChangeFlag = 1;
			}

			//Check if touch is inside row with day of week
			else if((y >= DAY_CHECK_BUTTON_Y)&&(y <= (DAY_CHECK_BUTTON_Y + DAY_CHECK_BUTTON_H)))
			{
				MenuTFTSchedule2ActivityDayOfWeekRow(x,y);
			}

			// Check if touch is in row Add hour/minute to ON time
			else if((y >= HOUR_MINUTE_ON_ADD_SHEDULE_Y)&&(y <= (HOUR_MINUTE_ON_ADD_SHEDULE_Y + DAY_CHECK_BUTTON_H)))
			{
				MenuTFTSchedule2ActivityHourMinuteONAdd(x,y);
			}

			// Check if touch is in row Add hour/minute to OFF time
			else if((y >= HOUR_MINUTE_OFF_ADD_SHEDULE_Y)&&(y <= (HOUR_MINUTE_OFF_ADD_SHEDULE_Y + DAY_CHECK_BUTTON_H)))
			{
				MenuTFTSchedule2ActivityHourMinuteOFFAdd(x,y);
			}

			//Check if touch is inside row with rectangle of Relays and Lights
			else if((y >= DAY_CHECK_RELAY_BUTTON_Y)&&(y <= (DAY_CHECK_RELAY_BUTTON_Y + DAY_CHECK_BUTTON_H)))
			{
				MenuTFTSchedule2ActivityRelaysAndLightsRow(x,y);
			}

			// Check if that point is inside the MEDIUM Button - CONFIRM
			else if((x >= MEDIUM_BUTTON_X)&&(x <= (MEDIUM_BUTTON_X+MEDIUM_BUTTON_W))&&
					(y >= MEDIUM_BUTTON_Y)&&(y <= (MEDIUM_BUTTON_Y + MEDIUM_BUTTON_H)))
			{
				EEPROM_ScheduleHourOnUpdate(2, hourOnSchedule2);
				EEPROM_ScheduleMinuteOnUpdate(2, minuteOnSchedule2);
				EEPROM_ScheduleHourOffUpdate(2, hourOffSchedule2);
				EEPROM_ScheduleMinuteOffUpdate(2, minuteOffSchedule2);
				EEPROM_ScheduleDayInWeekUpdate(2,  schedule2DayInWeekTab);
				EEPROM_ScheduleRelayAndSwitchTabUpdate(2, schedule2RelayAndSwitchTab);

				EF_SetFont(&arialBlack_20ptFontInfo);
				sprintf((char*)Msg, " H2 ZMIENIONY       ");
				EF_PutString(Msg, CLOCK_STRING_POZ_X-7, SWIATLA_STRING_POZ_Y, ILI9341_ORANGE, BG_COLOR, ILI9341_LIGHTGREY);

				EF_SetFont(&arial_11ptFontInfo);
				GFX_DrawFillRoundRectangle(MEDIUM_BUTTON_X, MEDIUM_BUTTON_Y, MEDIUM_BUTTON_W, MEDIUM_BUTTON_H, RIGHT_LEFT_BUTTON_R,  ILI9341_GREEN);
				sprintf((char*)Msg, " >POTW<");
				EF_PutString(Msg, (MEDIUM_BUTTON_X + 6), (MEDIUM_BUTTON_Y + 2), ILI9341_BLACK, BG_TRANSPARENT, ILI9341_GREEN);
				// TODO! Add day of week
			}
			TimerTouch = HAL_GetTick();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Functions to Support for accurate touch case
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Functions to handle touch in Schedule1Activity()
//
// Functions to handle touch in change day section
void MenuTFTSchedule1ActivityDayOfWeekRow(uint16_t x, uint16_t y)
{
	//Monday - PN
	if((x >= DAY_CHECK_BUTTON_1_X)&&(x <= (DAY_CHECK_BUTTON_1_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule1DayInWeekTab[0]) changeTFTScheduleDayOfWeek(1, 0);//if is ON
		else changeTFTScheduleDayOfWeek(1, 1);//if is OFF
		schedule1DayInWeekTab[0] = !schedule1DayInWeekTab[0]; // update current state in tab holding state of day in week
	}
	//Tuesday - WT
	else if((x >= DAY_CHECK_BUTTON_2_X)&&(x <= (DAY_CHECK_BUTTON_2_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule1DayInWeekTab[1]) changeTFTScheduleDayOfWeek(2, 0);//if is ON
		else changeTFTScheduleDayOfWeek(2, 1);//if is OFF
		schedule1DayInWeekTab[1] = !schedule1DayInWeekTab[1];
	}
	//Wednesday - SR
	else if((x >= DAY_CHECK_BUTTON_3_X)&&(x <= (DAY_CHECK_BUTTON_3_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule1DayInWeekTab[2]) changeTFTScheduleDayOfWeek(3, 0);//if is ON
		else changeTFTScheduleDayOfWeek(3, 1);//if is OFF
		schedule1DayInWeekTab[2] = !schedule1DayInWeekTab[2];
	}
	//Thursday - CZ
	else if((x >= DAY_CHECK_BUTTON_4_X)&&(x <= (DAY_CHECK_BUTTON_4_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule1DayInWeekTab[3]) changeTFTScheduleDayOfWeek(4, 0);//if is ON
		else changeTFTScheduleDayOfWeek(4, 1);//if is OFF
		schedule1DayInWeekTab[3] = !schedule1DayInWeekTab[3];
	}
	//Friday - PT
	else if((x >= DAY_CHECK_BUTTON_5_X)&&(x <= (DAY_CHECK_BUTTON_5_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule1DayInWeekTab[4]) changeTFTScheduleDayOfWeek(5, 0);//if is ON
		else changeTFTScheduleDayOfWeek(5, 1);//if is OFF
		schedule1DayInWeekTab[4] = !schedule1DayInWeekTab[4];
	}
	//Saturday - SB
	else if((x >= DAY_CHECK_BUTTON_6_X)&&(x <= (DAY_CHECK_BUTTON_6_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule1DayInWeekTab[5]) changeTFTScheduleDayOfWeek(6, 0);//if is ON
		else changeTFTScheduleDayOfWeek(6, 1);//if is OFF
		schedule1DayInWeekTab[5] = !schedule1DayInWeekTab[5];
	}
	//Sunday - ND
	else if((x >= DAY_CHECK_BUTTON_7_X)&&(x <= (DAY_CHECK_BUTTON_7_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule1DayInWeekTab[6]) changeTFTScheduleDayOfWeek(7, 0);//if is ON
		else changeTFTScheduleDayOfWeek(7, 1);//if is OFF
		schedule1DayInWeekTab[6] = !schedule1DayInWeekTab[6];
	}
}

//
// Update rectangle with state of each day
//
void changeTFTScheduleDayOfWeek(uint8_t NrOfDay, uint8_t NewState)
{
	if (1 == NrOfDay)	// if Monday
	{
		if(0 == NewState)	// if turn off
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_BUTTON_1_X+1, DAY_CHECK_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_OFF);
		}
		else	// if turn on
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_BUTTON_1_X+1, DAY_CHECK_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_ON);
		}
	}
	else if (2== NrOfDay)	// if Tuesday
	{
		if(0 == NewState)	// if turn off
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_BUTTON_2_X+1, DAY_CHECK_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_OFF);
		}
		else	// if turn on
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_BUTTON_2_X+1, DAY_CHECK_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_ON);
		}
	}
	else if (3== NrOfDay)	// if Wednesday
	{
		if(0 == NewState)	// if turn off
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_BUTTON_3_X+1, DAY_CHECK_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_OFF);
		}
		else	// if turn on
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_BUTTON_3_X+1, DAY_CHECK_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_ON);
		}
	}
	else if (4== NrOfDay)	// if Thursday
	{
		if(0 == NewState)	// if turn off
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_BUTTON_4_X+1, DAY_CHECK_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_OFF);
		}
		else	// if turn on
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_BUTTON_4_X+1, DAY_CHECK_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_ON);
		}
	}
	else if (5== NrOfDay)	// if Friday
	{
		if(0 == NewState)	// if turn off
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_BUTTON_5_X+1, DAY_CHECK_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_OFF);
		}
		else	// if turn on
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_BUTTON_5_X+1, DAY_CHECK_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_ON);
		}
	}
	else if (6== NrOfDay)	// if Saturday
	{
		if(0 == NewState)	// if turn off
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_BUTTON_6_X+1, DAY_CHECK_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_OFF);
		}
		else	// if turn on
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_BUTTON_6_X+1, DAY_CHECK_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_ON);
		}
	}
	else if (7== NrOfDay)	// If Sunday
	{
		if(0 == NewState)	// if turn off
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_BUTTON_7_X+1, DAY_CHECK_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_OFF);
		}
		else	// if turn on
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_BUTTON_7_X+1, DAY_CHECK_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_ON);
		}
	}
}

//
// Update rectangle with state of each day
//
void changeTFTScheduleRelayLights(uint8_t NrOfRS, uint8_t NewState)
{
	if(1 == NrOfRS) // R1
	{
		if (0 == NewState) // if turn off
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_RELAY_BUTTON_1_X+1, DAY_CHECK_RELAY_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_OFF);
		}
		else // if turn on
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_RELAY_BUTTON_1_X+1, DAY_CHECK_RELAY_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_ON);
		}
	}
	else if(2 == NrOfRS) // R2
	{
		if (0 == NewState) // if turn off
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_RELAY_BUTTON_2_X+1, DAY_CHECK_RELAY_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_OFF);
		}
		else // if turn on
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_RELAY_BUTTON_2_X+1, DAY_CHECK_RELAY_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_ON);
		}
	}
	else if(3 == NrOfRS) // R3
	{
		if (0 == NewState) // if turn off
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_RELAY_BUTTON_3_X+1, DAY_CHECK_RELAY_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_OFF);
		}
		else // if turn on
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_RELAY_BUTTON_3_X+1, DAY_CHECK_RELAY_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_ON);
		}
	}
	else if(4 == NrOfRS) // R4
	{
		if (0 == NewState) // if turn off
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_RELAY_BUTTON_4_X+1, DAY_CHECK_RELAY_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_OFF);
		}
		else // if turn on
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_RELAY_BUTTON_4_X+1, DAY_CHECK_RELAY_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_ON);
		}
	}
	else if(5 == NrOfRS) // WS2812b
	{
		if (0 == NewState) // if turn off
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_RELAY_BUTTON_5_X+1, DAY_CHECK_RELAY_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_OFF);
		}
		else // if turn on
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_RELAY_BUTTON_5_X+1, DAY_CHECK_RELAY_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_ON);
		}
	}
	else if(6 == NrOfRS) // L1
	{
		if (0 == NewState) // if turn off
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_RELAY_BUTTON_6_X+1, DAY_CHECK_RELAY_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_OFF);
		}
		else // if turn on
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_RELAY_BUTTON_6_X+1, DAY_CHECK_RELAY_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_ON);
		}
	}
	else if(7 == NrOfRS) // L2
	{
		if (0 == NewState) // if turn off
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_RELAY_BUTTON_7_X+1, DAY_CHECK_RELAY_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_OFF);
		}
		else // if turn on
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_RELAY_BUTTON_7_X+1, DAY_CHECK_RELAY_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_ON);
		}
	}
	else if(8 == NrOfRS) // L3
	{
		if (0 == NewState) // if turn off
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_RELAY_BUTTON_8_X+1, DAY_CHECK_RELAY_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_OFF);
		}
		else // if turn on
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_RELAY_BUTTON_8_X+1, DAY_CHECK_RELAY_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_ON);
		}
	}
	else if(9 == NrOfRS) // L4
	{
		if (0 == NewState) // if turn off
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_RELAY_BUTTON_9_X+1, DAY_CHECK_RELAY_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_OFF);
		}
		else // if turn on
		{
			GFX_DrawFillRoundRectangle(DAY_CHECK_RELAY_BUTTON_9_X+1, DAY_CHECK_RELAY_BUTTON_Y+1, DAY_CHECK_BUTTON_W-2, DAY_CHECK_BUTTON_H-2, DAY_CHECK_BUTTON_R, DAY_CHECK_SWITCH_COLOR_ON);
		}
	}
}

//
// Handle touch in Hour And Minute ON section
void MenuTFTSchedule1ActivityHourMinuteONAdd(uint16_t x, uint16_t y)
{
	EF_SetFont(&arialBlack_20ptFontInfo);
	// Check if it is Hour to add +1H
	if((x >= ONE_HOUR_ADD_SHEDULE_X)&&(x <= (ONE_HOUR_ADD_SHEDULE_X + HOOUR_MINUTE_BUTTON_W)))
	{
		if(hourOnSchedule1 < 24)
		{
			hourOnSchedule1++;
		}
		else
		{
			hourOnSchedule1 = 1;
		}
		if(hourOnSchedule1 >= 10) sprintf((char*)Msg, " %d ", hourOnSchedule1);
		else sprintf((char*)Msg, " 0%d ", hourOnSchedule1);
		EF_PutString(Msg, STRING_ON_OFF_HOUR_X , STRING_ON_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);

	}

	// Check if it is Hour to add +1M
	else if((x >= ONE_MINUTE_ADD_SHEDULE_X)&&(x <= (ONE_MINUTE_ADD_SHEDULE_X + HOOUR_MINUTE_BUTTON_W)))
	{
		if(minuteOnSchedule1 < 59)
		{
			minuteOnSchedule1++;
		}
		else
		{
			minuteOnSchedule1 = 0;
		}
		if(minuteOnSchedule1 >= 10) sprintf((char*)Msg, " %d ", minuteOnSchedule1);
		else sprintf((char*)Msg, " 0%d ", minuteOnSchedule1);
		EF_PutString(Msg, STRING_ON_OFF_MINUTE_X , STRING_ON_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);
	}

	// Check if it is Hour to add +10M
	else if((x >= TEN_MINUTE_ADD_SHEDULE_X)&&(x <= (TEN_MINUTE_ADD_SHEDULE_X + HOOUR_MINUTE_BUTTON_W)))
	{
		if(minuteOnSchedule1 < 49)
		{
			minuteOnSchedule1 = minuteOnSchedule1 + 10;
		}
		else
		{
			minuteOnSchedule1 = (minuteOnSchedule1 + 10) % 60;
		}
		if(minuteOnSchedule1 >= 10) sprintf((char*)Msg, " %d ", minuteOnSchedule1);
		else sprintf((char*)Msg, " 0%d ", minuteOnSchedule1);
		EF_PutString(Msg, STRING_ON_OFF_MINUTE_X , STRING_ON_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);
	}
}

//
// Handle touch in Hour And Minute ON section
void MenuTFTSchedule1ActivityHourMinuteOFFAdd(uint16_t x, uint16_t y)
{
	EF_SetFont(&arialBlack_20ptFontInfo);
	// Check if it is Hour to add +1H
	if((x >= ONE_HOUR_ADD_SHEDULE_X)&&(x <= (ONE_HOUR_ADD_SHEDULE_X + HOOUR_MINUTE_BUTTON_W)))
	{
		if(hourOffSchedule1 < 24)
		{
			hourOffSchedule1++;
		}
		else
		{
			hourOffSchedule1 = 1;
		}
		if(hourOffSchedule1 >= 10) sprintf((char*)Msg, " %d ", hourOffSchedule1);
		else sprintf((char*)Msg, " 0%d ", hourOffSchedule1);
		EF_PutString(Msg, STRING_ON_OFF_HOUR_X-2 , STRING_OFF_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);

	}

	// Check if it is Hour to add +1M
	else if((x >= ONE_MINUTE_ADD_SHEDULE_X)&&(x <= (ONE_MINUTE_ADD_SHEDULE_X + HOOUR_MINUTE_BUTTON_W)))
	{
		if(minuteOffSchedule1 < 59)
		{
			minuteOffSchedule1++;
		}
		else
		{
			minuteOffSchedule1 = 0;
		}
		if(minuteOffSchedule1 >= 10) sprintf((char*)Msg, " %d ", minuteOffSchedule1);
		else sprintf((char*)Msg, " 0%d ", minuteOffSchedule1);
		EF_PutString(Msg, STRING_ON_OFF_MINUTE_X , STRING_OFF_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);
	}

	// Check if it is Hour to add +10M
	else if((x >= TEN_MINUTE_ADD_SHEDULE_X)&&(x <= (TEN_MINUTE_ADD_SHEDULE_X + HOOUR_MINUTE_BUTTON_W)))
	{
		if(minuteOffSchedule1 < 49)
		{
			minuteOffSchedule1 = minuteOffSchedule1 + 10;
		}
		else
		{
			minuteOffSchedule1 = (minuteOffSchedule1 + 10) % 60;
		}
		if(minuteOffSchedule1 >= 10) sprintf((char*)Msg, " %d ", minuteOffSchedule1);
		else sprintf((char*)Msg, " 0%d ", minuteOffSchedule1);
		EF_PutString(Msg, STRING_ON_OFF_MINUTE_X , STRING_OFF_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Functions to handle touch in Schedule2Activity()
//
// Functions to handle touch in change day section
void MenuTFTSchedule2ActivityDayOfWeekRow(uint16_t x, uint16_t y)
{
	//Monday - PN
	if((x >= DAY_CHECK_BUTTON_1_X)&&(x <= (DAY_CHECK_BUTTON_1_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule2DayInWeekTab[0]) changeTFTScheduleDayOfWeek(1, 0);//if is ON
		else changeTFTScheduleDayOfWeek(1, 1);//if is OFF
		schedule2DayInWeekTab[0] = !schedule2DayInWeekTab[0]; // update current state in tab holding state of day in week
	}
	//Tuesday - WT
	else if((x >= DAY_CHECK_BUTTON_2_X)&&(x <= (DAY_CHECK_BUTTON_2_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule2DayInWeekTab[1]) changeTFTScheduleDayOfWeek(2, 0);//if is ON
		else changeTFTScheduleDayOfWeek(2, 1);//if is OFF
		schedule2DayInWeekTab[1] = !schedule2DayInWeekTab[1]; // update current state in tab holding state of day in week
	}
	//Wednesday - SR
	else if((x >= DAY_CHECK_BUTTON_3_X)&&(x <= (DAY_CHECK_BUTTON_3_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule2DayInWeekTab[2]) changeTFTScheduleDayOfWeek(3, 0);//if is ON
		else changeTFTScheduleDayOfWeek(3, 1);//if is OFF
		schedule2DayInWeekTab[2] = !schedule2DayInWeekTab[2]; // update current state in tab holding state of day in week
	}
	//Thursday - CZ
	else if((x >= DAY_CHECK_BUTTON_4_X)&&(x <= (DAY_CHECK_BUTTON_4_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule2DayInWeekTab[3]) changeTFTScheduleDayOfWeek(4, 0);//if is ON
		else changeTFTScheduleDayOfWeek(4, 1);//if is OFF
		schedule2DayInWeekTab[3] = !schedule2DayInWeekTab[3]; // update current state in tab holding state of day in week
	}
	//Friday - PT
	else if((x >= DAY_CHECK_BUTTON_5_X)&&(x <= (DAY_CHECK_BUTTON_5_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule2DayInWeekTab[4]) changeTFTScheduleDayOfWeek(5, 0);//if is ON
		else changeTFTScheduleDayOfWeek(5, 1);//if is OFF
		schedule2DayInWeekTab[4] = !schedule2DayInWeekTab[4]; // update current state in tab holding state of day in week
	}
	//Saturday - SB
	else if((x >= DAY_CHECK_BUTTON_6_X)&&(x <= (DAY_CHECK_BUTTON_6_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule2DayInWeekTab[5]) changeTFTScheduleDayOfWeek(6, 0);//if is ON
		else changeTFTScheduleDayOfWeek(6, 1);//if is OFF
		schedule2DayInWeekTab[5] = !schedule2DayInWeekTab[5]; // update current state in tab holding state of day in week
	}
	//Sunday - ND
	else if((x >= DAY_CHECK_BUTTON_7_X)&&(x <= (DAY_CHECK_BUTTON_7_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule2DayInWeekTab[6]) changeTFTScheduleDayOfWeek(7, 0);//if is ON
		else changeTFTScheduleDayOfWeek(7, 1);//if is OFF
		schedule2DayInWeekTab[6] = !schedule2DayInWeekTab[6]; // update current state in tab holding state of day in week
	}
}

//
// Handle touch in Hour And Minute ON section
void MenuTFTSchedule2ActivityHourMinuteONAdd(uint16_t x, uint16_t y)
{
	EF_SetFont(&arialBlack_20ptFontInfo);
	// Check if it is Hour to add +1H
	if((x >= ONE_HOUR_ADD_SHEDULE_X)&&(x <= (ONE_HOUR_ADD_SHEDULE_X + HOOUR_MINUTE_BUTTON_W)))
	{
		if(hourOnSchedule2 < 24)
		{
			hourOnSchedule2++;
		}
		else
		{
			hourOnSchedule2 = 1;
		}
		if(hourOnSchedule2 >= 10) sprintf((char*)Msg, " %d ", hourOnSchedule2);
		else sprintf((char*)Msg, " 0%d ", hourOnSchedule2);
		EF_PutString(Msg, STRING_ON_OFF_HOUR_X-2 , STRING_ON_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);

	}

	// Check if it is Hour to add +1M
	else if((x >= ONE_MINUTE_ADD_SHEDULE_X)&&(x <= (ONE_MINUTE_ADD_SHEDULE_X + HOOUR_MINUTE_BUTTON_W)))
	{
		if(minuteOnSchedule2 < 59)
		{
			minuteOnSchedule2++;
		}
		else
		{
			minuteOnSchedule2 = 0;
		}
		if(minuteOnSchedule2 >= 10) sprintf((char*)Msg, " %d ", minuteOnSchedule2);
		else sprintf((char*)Msg, " 0%d ", minuteOnSchedule2);
		EF_PutString(Msg, STRING_ON_OFF_MINUTE_X , STRING_ON_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);
	}

	// Check if it is Hour to add +10M
	else if((x >= TEN_MINUTE_ADD_SHEDULE_X)&&(x <= (TEN_MINUTE_ADD_SHEDULE_X + HOOUR_MINUTE_BUTTON_W)))
	{
		if(minuteOnSchedule2 < 49)
		{
			minuteOnSchedule2 = minuteOnSchedule2 + 10;
		}
		else
		{
			minuteOnSchedule2 = (minuteOnSchedule2 + 10) % 60;
		}
		if(minuteOnSchedule2 >= 10) sprintf((char*)Msg, " %d ", minuteOnSchedule2);
		else sprintf((char*)Msg, " 0%d ", minuteOnSchedule2);
		EF_PutString(Msg, STRING_ON_OFF_MINUTE_X , STRING_ON_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);
	}
}

//
// Handle touch in Hour And Minute OFF section
void MenuTFTSchedule2ActivityHourMinuteOFFAdd(uint16_t x, uint16_t y)
{
	EF_SetFont(&arialBlack_20ptFontInfo);
	// Check if it is Hour to add +1H
	if((x >= ONE_HOUR_ADD_SHEDULE_X)&&(x <= (ONE_HOUR_ADD_SHEDULE_X + HOOUR_MINUTE_BUTTON_W)))
	{
		if(hourOffSchedule2 < 24)
		{
			hourOffSchedule2++;
		}
		else
		{
			hourOffSchedule2 = 1;
		}
		if(hourOffSchedule2 >= 10) sprintf((char*)Msg, " %d ", hourOffSchedule2);
		else sprintf((char*)Msg, " 0%d ", hourOffSchedule2);
		EF_PutString(Msg, STRING_ON_OFF_HOUR_X , STRING_OFF_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);

	}

	// Check if it is Hour to add +1M
	else if((x >= ONE_MINUTE_ADD_SHEDULE_X)&&(x <= (ONE_MINUTE_ADD_SHEDULE_X + HOOUR_MINUTE_BUTTON_W)))
	{
		if(minuteOffSchedule2 < 59)
		{
			minuteOffSchedule2++;
		}
		else
		{
			minuteOffSchedule2 = 0;
		}
		if(minuteOffSchedule2 >= 10) sprintf((char*)Msg, " %d ", minuteOffSchedule2);
		else sprintf((char*)Msg, " 0%d ", minuteOffSchedule2);
		EF_PutString(Msg, STRING_ON_OFF_MINUTE_X , STRING_OFF_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);
	}

	// Check if it is Hour to add +10M
	else if((x >= TEN_MINUTE_ADD_SHEDULE_X)&&(x <= (TEN_MINUTE_ADD_SHEDULE_X + HOOUR_MINUTE_BUTTON_W)))
	{
		if(minuteOffSchedule2 < 49)
		{
			minuteOffSchedule2 = minuteOffSchedule2 + 10;
		}
		else
		{
			minuteOffSchedule2 = (minuteOffSchedule2 + 10) % 60;
		}
		if(minuteOffSchedule2 >= 10) sprintf((char*)Msg, " %d ", minuteOffSchedule2);
		else sprintf((char*)Msg, " 0%d ", minuteOffSchedule2);
		EF_PutString(Msg, STRING_ON_OFF_MINUTE_X , STRING_OFF_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);
	}
}

//
// Handle touch in Relays and Lights in Schedule 1 section
//
void MenuTFTSchedule1ActivityRelaysAndLightsRow(uint16_t x, uint16_t y)
{
	// R1
	if((x >= DAY_CHECK_RELAY_BUTTON_1_X)&&(x <= (DAY_CHECK_RELAY_BUTTON_1_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule1RelayAndSwitchTab[0]) changeTFTScheduleRelayLights(1, 0);//if is ON -> Turn OFF
		else changeTFTScheduleRelayLights(1, 1);//if is OFF -> Turn ON
		schedule1RelayAndSwitchTab[0] = !schedule1RelayAndSwitchTab[0]; // update current state in tab holding state of day in week
	}
	// R2
	if((x >= DAY_CHECK_RELAY_BUTTON_2_X)&&(x <= (DAY_CHECK_RELAY_BUTTON_2_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule1RelayAndSwitchTab[1]) changeTFTScheduleRelayLights(2, 0);//if is ON -> Turn OFF
		else changeTFTScheduleRelayLights(2, 1);//if is OFF -> Turn ON
		schedule1RelayAndSwitchTab[1] = !schedule1RelayAndSwitchTab[1]; // update current state in tab holding state of day in week
	}
	// R3
	if((x >= DAY_CHECK_RELAY_BUTTON_3_X)&&(x <= (DAY_CHECK_RELAY_BUTTON_3_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule1RelayAndSwitchTab[2]) changeTFTScheduleRelayLights(3, 0);//if is ON -> Turn OFF
		else changeTFTScheduleRelayLights(3, 1);//if is OFF -> Turn ON
		schedule1RelayAndSwitchTab[2] = !schedule1RelayAndSwitchTab[2]; // update current state in tab holding state of day in week
	}
	// R4
	if((x >= DAY_CHECK_RELAY_BUTTON_4_X)&&(x <= (DAY_CHECK_RELAY_BUTTON_4_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule1RelayAndSwitchTab[3]) changeTFTScheduleRelayLights(4, 0);//if is ON -> Turn OFF
		else changeTFTScheduleRelayLights(4, 1);//if is OFF -> Turn ON
		schedule1RelayAndSwitchTab[3] = !schedule1RelayAndSwitchTab[3]; // update current state in tab holding state of day in week
	}
	// WS2812b
	if((x >= DAY_CHECK_RELAY_BUTTON_5_X)&&(x <= (DAY_CHECK_RELAY_BUTTON_5_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule1RelayAndSwitchTab[4]) changeTFTScheduleRelayLights(5, 0);//if is ON -> Turn OFF
		else changeTFTScheduleRelayLights(5, 1);//if is OFF -> Turn ON
		schedule1RelayAndSwitchTab[4] = !schedule1RelayAndSwitchTab[4]; // update current state in tab holding state of day in week
	}
	// L1
	if((x >= DAY_CHECK_RELAY_BUTTON_6_X)&&(x <= (DAY_CHECK_RELAY_BUTTON_6_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule1RelayAndSwitchTab[5]) changeTFTScheduleRelayLights(6, 0);//if is ON -> Turn OFF
		else changeTFTScheduleRelayLights(6, 1);//if is OFF -> Turn ON
		schedule1RelayAndSwitchTab[5] = !schedule1RelayAndSwitchTab[5]; // update current state in tab holding state of day in week
	}
	// L2
	if((x >= DAY_CHECK_RELAY_BUTTON_7_X)&&(x <= (DAY_CHECK_RELAY_BUTTON_7_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule1RelayAndSwitchTab[6]) changeTFTScheduleRelayLights(7, 0);//if is ON -> Turn OFF
		else changeTFTScheduleRelayLights(7, 1);//if is OFF -> Turn ON
		schedule1RelayAndSwitchTab[6] = !schedule1RelayAndSwitchTab[6]; // update current state in tab holding state of day in week
	}
	// L3
	if((x >= DAY_CHECK_RELAY_BUTTON_8_X)&&(x <= (DAY_CHECK_RELAY_BUTTON_8_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule1RelayAndSwitchTab[7]) changeTFTScheduleRelayLights(8, 0);//if is ON -> Turn OFF
		else changeTFTScheduleRelayLights(8, 1);//if is OFF -> Turn ON
		schedule1RelayAndSwitchTab[7] = !schedule1RelayAndSwitchTab[7]; // update current state in tab holding state of day in week
	}
	// L4
	if((x >= DAY_CHECK_RELAY_BUTTON_9_X)&&(x <= (DAY_CHECK_RELAY_BUTTON_9_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule1RelayAndSwitchTab[8]) changeTFTScheduleRelayLights(9, 0);//if is ON -> Turn OFF
		else changeTFTScheduleRelayLights(9, 1);//if is OFF -> Turn ON
		schedule1RelayAndSwitchTab[8] = !schedule1RelayAndSwitchTab[8]; // update current state in tab holding state of day in week
	}

}

//
// Handle touch in Relays and Lights in Schedule 2 section
//
void MenuTFTSchedule2ActivityRelaysAndLightsRow(uint16_t x, uint16_t y)
{
	// R1
	if((x >= DAY_CHECK_RELAY_BUTTON_1_X)&&(x <= (DAY_CHECK_RELAY_BUTTON_1_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule2RelayAndSwitchTab[0]) changeTFTScheduleRelayLights(1, 0);//if is ON -> Turn OFF
		else changeTFTScheduleRelayLights(1, 1);//if is OFF -> Turn ON
		schedule2RelayAndSwitchTab[0] = !schedule2RelayAndSwitchTab[0]; // update current state in tab holding state of day in week
	}
	// R2
	if((x >= DAY_CHECK_RELAY_BUTTON_2_X)&&(x <= (DAY_CHECK_RELAY_BUTTON_2_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule2RelayAndSwitchTab[1]) changeTFTScheduleRelayLights(2, 0);//if is ON -> Turn OFF
		else changeTFTScheduleRelayLights(2, 1);//if is OFF -> Turn ON
		schedule2RelayAndSwitchTab[1] = !schedule2RelayAndSwitchTab[1]; // update current state in tab holding state of day in week
	}
	// R3
	if((x >= DAY_CHECK_RELAY_BUTTON_3_X)&&(x <= (DAY_CHECK_RELAY_BUTTON_3_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule2RelayAndSwitchTab[2]) changeTFTScheduleRelayLights(3, 0);//if is ON -> Turn OFF
		else changeTFTScheduleRelayLights(3, 1);//if is OFF -> Turn ON
		schedule2RelayAndSwitchTab[2] = !schedule2RelayAndSwitchTab[2]; // update current state in tab holding state of day in week
	}
	// R4
	if((x >= DAY_CHECK_RELAY_BUTTON_4_X)&&(x <= (DAY_CHECK_RELAY_BUTTON_4_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule2RelayAndSwitchTab[3]) changeTFTScheduleRelayLights(4, 0);//if is ON -> Turn OFF
		else changeTFTScheduleRelayLights(4, 1);//if is OFF -> Turn ON
		schedule2RelayAndSwitchTab[3] = !schedule2RelayAndSwitchTab[3]; // update current state in tab holding state of day in week
	}
	// WS2812b
	if((x >= DAY_CHECK_RELAY_BUTTON_5_X)&&(x <= (DAY_CHECK_RELAY_BUTTON_5_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule2RelayAndSwitchTab[4]) changeTFTScheduleRelayLights(5, 0);//if is ON -> Turn OFF
		else changeTFTScheduleRelayLights(5, 1);//if is OFF -> Turn ON
		schedule2RelayAndSwitchTab[4] = !schedule2RelayAndSwitchTab[4]; // update current state in tab holding state of day in week
	}
	// L1
	if((x >= DAY_CHECK_RELAY_BUTTON_6_X)&&(x <= (DAY_CHECK_RELAY_BUTTON_6_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule2RelayAndSwitchTab[5]) changeTFTScheduleRelayLights(6, 0);//if is ON -> Turn OFF
		else changeTFTScheduleRelayLights(6, 1);//if is OFF -> Turn ON
		schedule2RelayAndSwitchTab[5] = !schedule2RelayAndSwitchTab[5]; // update current state in tab holding state of day in week
	}
	// L2
	if((x >= DAY_CHECK_RELAY_BUTTON_7_X)&&(x <= (DAY_CHECK_RELAY_BUTTON_7_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule2RelayAndSwitchTab[6]) changeTFTScheduleRelayLights(7, 0);//if is ON -> Turn OFF
		else changeTFTScheduleRelayLights(7, 1);//if is OFF -> Turn ON
		schedule2RelayAndSwitchTab[6] = !schedule2RelayAndSwitchTab[6]; // update current state in tab holding state of day in week
	}
	// L3
	if((x >= DAY_CHECK_RELAY_BUTTON_8_X)&&(x <= (DAY_CHECK_RELAY_BUTTON_8_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule2RelayAndSwitchTab[7]) changeTFTScheduleRelayLights(8, 0);//if is ON -> Turn OFF
		else changeTFTScheduleRelayLights(8, 1);//if is OFF -> Turn ON
		schedule2RelayAndSwitchTab[7] = !schedule2RelayAndSwitchTab[7]; // update current state in tab holding state of day in week
	}
	// L4
	if((x >= DAY_CHECK_RELAY_BUTTON_9_X)&&(x <= (DAY_CHECK_RELAY_BUTTON_9_X + DAY_CHECK_BUTTON_W)))
	{
		if(schedule2RelayAndSwitchTab[8]) changeTFTScheduleRelayLights(9, 0);//if is ON -> Turn OFF
		else changeTFTScheduleRelayLights(9, 1);//if is OFF -> Turn ON
		schedule2RelayAndSwitchTab[8] = !schedule2RelayAndSwitchTab[8]; // update current state in tab holding state of day in week
	}
}

//
//Read All info about schedules
void updateAllSchedulesInfo(uint8_t NrOfSchedule)
{
	if( 1 == NrOfSchedule)
	{
	  EEPROM_ScheduleHourOnRead(1, &hourOnSchedule1);
	  EEPROM_ScheduleMinuteOnRead(1, &minuteOnSchedule1);
	  EEPROM_ScheduleHourOffRead(1, &hourOffSchedule1);
	  EEPROM_ScheduleMinuteOffRead(1, &minuteOffSchedule1);
	  EEPROM_ScheduleDayInWeekRead(1, schedule1DayInWeekTab);
	  EEPROM_ScheduleRelayAndSwitchTabRead(1, schedule1RelayAndSwitchTab);
	}

	else if ( 2 == NrOfSchedule)
	{
	  EEPROM_ScheduleHourOnRead(2, &hourOnSchedule2);
	  EEPROM_ScheduleMinuteOnRead(2, &minuteOnSchedule2);
	  EEPROM_ScheduleHourOffRead(2, &hourOffSchedule2);
	  EEPROM_ScheduleMinuteOffRead(2, &minuteOffSchedule2);
	  EEPROM_ScheduleDayInWeekRead(2, schedule1DayInWeekTab);
	  EEPROM_ScheduleRelayAndSwitchTabRead(2, schedule1RelayAndSwitchTab);
	}

}

//
// Functions to update Hour and Minute after touch
//
void clockIncreaseOneHourAndMinute(uint16_t x, uint16_t y)
{
	if((y >= CLOCK_B_1_POZ_Y)&&(y <= (CLOCK_B_1_POZ_Y + CLOCK_BUTTON_H))) // Add 1 Hour
	{
		if(Hours < 24)
		{
			Hours++;
		}
		else
		{
			Hours = 1;
		}
		sprintf((char*)Msg, " %d  ", Hours);
		EF_PutString(Msg, STRING_H_M_NUMBER_POZ_X, STRING_HOUR_POZ_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);

	}
	else if((y >= CLOCK_B_2_POZ_Y)&&(y <= (CLOCK_B_2_POZ_Y + CLOCK_BUTTON_H))) // Add 1 Minute
	{
		if(Minutes < 59)
		{
			Minutes++;
		}
		else
		{
			Minutes = 0;
		}
		sprintf((char*)Msg, " %d  ", Minutes);
		EF_PutString(Msg, STRING_H_M_NUMBER_POZ_X, STRING_MINUTE_POZ_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);
	}
	else if((y >= CLOCK_B_3_POZ_Y)&&(y <= (CLOCK_B_3_POZ_Y + CLOCK_BUTTON_H))) // Add 1 Day
	{
		if(DayOfWeek < 7)
		{
			DayOfWeek++;
		}
		else
		{
			DayOfWeek = 1;
		}
		sprintf((char*)Msg, "DZIEŃ TYG:  %d ", DayOfWeek);
		EF_PutString(Msg, STRING_HOUR_MINUTE_POZ_X, STRING_DAY_OF_WEEK_POZ_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);
	}
}

void clockIncreaseSixHoursTenMinutes(uint16_t x, uint16_t y)
{
	if((y >= CLOCK_B_1_POZ_Y)&&(y <= (CLOCK_B_1_POZ_Y + CLOCK_BUTTON_H))) // Add 6 Hour
	{

		if(Hours < 19)
		{
			Hours = Hours + 6;
		}
		else
		{
			Hours = 1;
		}
		sprintf((char*)Msg, " %d  ", Hours);
		EF_PutString(Msg, (STRING_H_M_NUMBER_POZ_X-3), STRING_HOUR_POZ_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);

	}
	else if((y >= CLOCK_B_2_POZ_Y)&&(y <= (CLOCK_B_2_POZ_Y + CLOCK_BUTTON_H))) // Add 10 Minute
	{
		if(Minutes < 49)
		{
			Minutes = Minutes +10;
		}
		else
		{
			Minutes = (Minutes + 10) % 60;
		}
		sprintf((char*)Msg, " %d  ", Minutes);
		EF_PutString(Msg, (STRING_H_M_NUMBER_POZ_X-4), STRING_MINUTE_POZ_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);

	}
	EF_SetFont(&arialBlack_20ptFontInfo);
}

//
// Change status of Switch after touch them
//
void changeSwitchStatusTouch(uint8_t y)
{
	//
	// FIRST SWITCH
	if((y >= SWITCH_1_POZ_Y)&&(y <= (SWITCH_1_POZ_Y + SWITCH_BUTTON_H)))
	{
		if(SwitchesButtonState[0] >= 1) // if is ON
		{
			firstSwitchTurn(0);
		}
		else // if is OFF
		{
			firstSwitchTurn(1);
		}
	}
	//
	// SECOND SWITCH
	else if((y >= SWITCH_2_POZ_Y)&&(y <= (SWITCH_2_POZ_Y + SWITCH_BUTTON_H)))
	{
		if(SwitchesButtonState[1] >= 1) // if is ON
		{
			secondSwitchTurn(0);
		}
		else // if is OFF
		{
			secondSwitchTurn(1);
		}
	}
	//
	// THIRD SWITCH
	else if((y >= SWITCH_3_POZ_Y)&&(y <= (SWITCH_3_POZ_Y + SWITCH_BUTTON_H)))
	{
		if(SwitchesButtonState[2] >= 1) // if is ON
		{
			thirdSwitchTurn(0);
		}
		else // if is OFF
		{
			thirdSwitchTurn(1);
		}
	}
	//
	// FOURTH SWITCH
	else if((y >= SWITCH_4_POZ_Y)&&(y <= (SWITCH_4_POZ_Y + SWITCH_BUTTON_H)))
	{
		if(SwitchesButtonState[3] >= 1) // if is ON
		{
			fourthSwitchTurn(0);
		}
		else // if is OFF
		{
			fourthSwitchTurn(1);
		}
	}
	EF_SetFont(&arialBlack_20ptFontInfo);
}

//
// Change status of Lights after touch them
//
void changeLightsStatusTouch(uint8_t y)
{
	//
	// FIRST LIGHT
	if((y >= LIGHT_B_1_POZ_Y)&&(y <= (LIGHT_B_1_POZ_Y + LIGHTS_BUTTON_H)))
	{
		if(LightsButtonState[0] >= 1) // if is ON
		{
			firstLightTurn(0);
		}
		else // if is OFF
		{
			firstLightTurn(1);
		}
	}
	//
	// SECOND LIGHT
	else if((y >= LIGHT_B_2_POZ_Y)&&(y <= (LIGHT_B_2_POZ_Y + LIGHTS_BUTTON_H)))
	{
		if(LightsButtonState[1] >= 1) // if is ON
		{
			secondLightTurn(0);
		}
		else // if is OFF
		{
			secondLightTurn(1);
		}
	}

	//
	// THIRD LIGHT
	else if((y >= LIGHT_B_3_POZ_Y)&&(y <= (LIGHT_B_3_POZ_Y + LIGHTS_BUTTON_H)))
	{
		if(LightsButtonState[2] >= 1) // if is ON
		{
			thirdLightTurn(0);
		}
		else // if is OFF
		{
			thirdLightTurn(1);
		}
	}

	//
	// FOURTH LIGHT
	else if((y >= LIGHT_B_4_POZ_Y)&&(y <= (LIGHT_B_4_POZ_Y + LIGHTS_BUTTON_H)))
	{
		if(LightsButtonState[3] >= 1) // if is ON
		{
			fourthLightTurn(0);
		}
		else // if is OFF
		{
			fourthLightTurn(1);
		}
	}
	EF_SetFont(&arialBlack_20ptFontInfo);
}

//
// Handling touch button to ON/OFF predefined activity
//
void predefinedActivityTouchAction(uint8_t y)
{
	if((y >= ACTIVITY_BUTTON_1_Y)&&(y<= (ACTIVITY_BUTTON_1_Y + ACTIVITY_BUTTON_H))) // First predefined activity button
	{
		if(ActivityButtonState[0] >= 1) // if is ON
		{
			ActivityButtonState[0] = 0;
			GFX_DrawFillRoundRectangle(ACTIVITY_BUTTON_X, ACTIVITY_BUTTON_1_Y, ACTIVITY_BUTTON_W, ACTIVITY_BUTTON_H, ACTIVITY_BUTTON_R, SWITCH_OFF_BUTTON_COLOR);
			sprintf((char*)Msg, "KARMIENIE");
			EF_PutString(Msg, (ACTIVITY_BUTTON_X+STRING_ACTIVITIES_1_X_ERRATA), (ACTIVITY_BUTTON_1_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
			sprintf((char*)Msg, "OFF");
			EF_PutString(Msg, (ACTIVITY_BUTTON_X+STRING_ACTIVITIES_ON_OFF_X_ERRATA), (ACTIVITY_BUTTON_1_Y+STRING_ACTIVITIES_Y_INTER), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
			predefinedActivityKarmienie(0); // Turn Activity OFF
		}
		else // if is OFF
		{
			ActivityButtonState[0] = 1;
			GFX_DrawFillRoundRectangle(ACTIVITY_BUTTON_X, ACTIVITY_BUTTON_1_Y, ACTIVITY_BUTTON_W, ACTIVITY_BUTTON_H, ACTIVITY_BUTTON_R, SWITCH_ON_ACTIVITY_BUTTON_COLOR);
			sprintf((char*)Msg, "KARMIENIE");
			EF_PutString(Msg, (ACTIVITY_BUTTON_X+STRING_ACTIVITIES_1_X_ERRATA), (ACTIVITY_BUTTON_1_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_ON_ACTIVITY_BUTTON_COLOR);
			sprintf((char*)Msg, "ON");
			EF_PutString(Msg, (ACTIVITY_BUTTON_X+STRING_ACTIVITIES_ON_OFF_X_ERRATA), (ACTIVITY_BUTTON_1_Y+STRING_ACTIVITIES_Y_INTER), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_ON_ACTIVITY_BUTTON_COLOR);
			predefinedActivityKarmienie(1);
		}

	}

	else if((y >= ACTIVITY_BUTTON_2_Y)&&(y<= (ACTIVITY_BUTTON_2_Y + ACTIVITY_BUTTON_H))) // Second predefined activity button
	{
		if(ActivityButtonState[1] >= 1) // if is ON
		{
			ActivityButtonState[1] = 0;
			GFX_DrawFillRoundRectangle(ACTIVITY_BUTTON_X, ACTIVITY_BUTTON_2_Y, ACTIVITY_BUTTON_W, ACTIVITY_BUTTON_H, ACTIVITY_BUTTON_R, SWITCH_OFF_BUTTON_COLOR);
			sprintf((char*)Msg, "CZYSZCZENIE");
			EF_PutString(Msg, (ACTIVITY_BUTTON_X+STRING_ACTIVITIES_2_X_ERRATA), (ACTIVITY_BUTTON_2_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
			sprintf((char*)Msg, "OFF");
			EF_PutString(Msg, (ACTIVITY_BUTTON_X+STRING_ACTIVITIES_ON_OFF_X_ERRATA), (ACTIVITY_BUTTON_2_Y+STRING_ACTIVITIES_Y_INTER), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
			predefinedActivityCzyszczenie(0);
		}
		else // if is OFF
		{
			ActivityButtonState[1] = 1;
			GFX_DrawFillRoundRectangle(ACTIVITY_BUTTON_X, ACTIVITY_BUTTON_2_Y, ACTIVITY_BUTTON_W, ACTIVITY_BUTTON_H, ACTIVITY_BUTTON_R, SWITCH_ON_ACTIVITY_BUTTON_COLOR);
			sprintf((char*)Msg, "CZYSZCZENIE");
			EF_PutString(Msg, (ACTIVITY_BUTTON_X+STRING_ACTIVITIES_2_X_ERRATA), (ACTIVITY_BUTTON_2_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_ON_ACTIVITY_BUTTON_COLOR);
			sprintf((char*)Msg, "ON");
			EF_PutString(Msg, (ACTIVITY_BUTTON_X+STRING_ACTIVITIES_ON_OFF_X_ERRATA), (ACTIVITY_BUTTON_2_Y+STRING_ACTIVITIES_Y_INTER), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_ON_ACTIVITY_BUTTON_COLOR);
			predefinedActivityCzyszczenie(1);
		}
	}
}

//
// Handling Touch to increase number of WS LED
//
void increaseNumberOfWSLedOnTFT(uint8_t x, uint8_t y)
{
	if((x >= WS_LED_BUTTON_1_X)&&(x <= (WS_LED_BUTTON_1_X + WS_LED_BUTTON_W))) // Add +1 LED
	{
		if(NrOfLeds < 98)
		{
			NrOfLeds++;
		}
		else
		{
			NrOfLeds = 1;
		}
		if(NrOfLeds < 10)sprintf((char*)Msg, "  %d ", NrOfLeds);
		else sprintf((char*)Msg, " %d", NrOfLeds);
		EF_PutString(Msg, STRING_WS_LED_POZ_NUMBER_X, STRING_WS_LED_ILOSC_POZ_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);
	}
	else if((x >= WS_LED_BUTTON_2_X)&&(x <= (WS_LED_BUTTON_2_X + WS_LED_BUTTON_W))) // Add +10 LED
	{
		if(NrOfLeds < 90)
		{
			NrOfLeds = NrOfLeds + 10;
		}
		else
		{
			NrOfLeds = (NrOfLeds + 10) % 99;
		}
	// Display String
	if(NrOfLeds < 10)sprintf((char*)Msg, "  %d ", NrOfLeds);
	else sprintf((char*)Msg, " %d", NrOfLeds);
	EF_PutString(Msg, STRING_WS_LED_POZ_NUMBER_X, STRING_WS_LED_ILOSC_POZ_Y, ILI9341_BLACK, BG_COLOR, ILI9341_LIGHTGREY);
	}


	else if((x >= WS_LED_BUTTON_3_X)&&(x <= (WS_LED_BUTTON_3_X + WS_LED_BUTTON_W))) // OK - confirm and send number of LEDs
	{
		SendComand(UCMD_WS_NUMBER_LED);
		EF_SetFont(&arial_11ptFontInfo);
		GFX_DrawFillRoundRectangle(WS_LED_BUTTON_3_X, WS_B_1_POZ_Y, WS_LED_BUTTON_W, WS_LED_BUTTON_H, WS_LED_BUTTON_R, SWITCH_ON_BUTTON_COLOR);
		sprintf((char*)Msg, "OK");
		EF_PutString(Msg, (WS_LED_BUTTON_3_X+STRING_ERRATA_X+1), (WS_B_1_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_ON_BUTTON_COLOR);
		// Write chosen number of LEDs to EEPROM memory
		eeprom_write(EEPROM_ADR_NUMBER_WS_LEDS, &NrOfLeds, sizeof(NrOfLeds));
	}
}

void changeTypeOfWSLights(uint8_t x, uint8_t y)
{
	if((y >= WS_B_2_POZ_Y)&&(y <= (WS_B_2_POZ_Y + WS_LED_BUTTON_H)))
	{
		if((x >= WS_WHITE_BUTTON_1_X)&&(x <= (WS_WHITE_BUTTON_1_X + WS_COLOR_BUTTON_W))) // MAX
		{
			SendComand(UCMD_WS_LED_WHITE_MAX);
		}
		else if((x >= WS_WHITE_BUTTON_2_X)&&(x <= (WS_WHITE_BUTTON_2_X + WS_COLOR_BUTTON_W))) // MID
		{
			SendComand(UCMD_WS_LED_WHITE_MID);
		}
		else if((x >= WS_WHITE_BUTTON_3_X)&&(x <= (WS_WHITE_BUTTON_3_X + WS_COLOR_BUTTON_W))) // MIN
		{
			SendComand(UCMD_WS_LED_WHITE_MIN);
		}
	}

	//
	// Check if it is third line button to change LEDs to BLUE
	//
	else if((y >= WS_B_3_POZ_Y)&&(y <= (WS_B_3_POZ_Y + WS_LED_BUTTON_H)))
	{
		if((x >= WS_WHITE_BUTTON_1_X)&&(x <= (WS_WHITE_BUTTON_1_X + WS_COLOR_BUTTON_W))) // MAX
		{
			SendComand(UCMD_WS_LED_BLUE_MAX);
		}
		else if((x >= WS_WHITE_BUTTON_2_X)&&(x <= (WS_WHITE_BUTTON_2_X + WS_COLOR_BUTTON_W))) // MID
		{
			SendComand(UCMD_WS_LED_BLUE_MID);
		}
		else if((x >= WS_WHITE_BUTTON_3_X)&&(x <= (WS_WHITE_BUTTON_3_X + WS_COLOR_BUTTON_W))) // MIN
		{
			SendComand(UCMD_WS_LED_BLUE_MIN);
		}
	}

	//
	// Check if it is fourth line button to change LEDs to Predefined
	//
	else if((y >= WS_B_4_POZ_Y)&&(y <= (WS_B_4_POZ_Y + WS_LED_BUTTON_H)))
	{
		if((x >= WS_PREDEFINED_BUTTON_1_X)&&(x <= (WS_PREDEFINED_BUTTON_1_X + WS_PREDEFINED_BUTTON_W))) // MAX
		{
			SendComand(UCMD_WS_LED_DAILY);
		}
		else if((x >= WS_PREDEFINED_BUTTON_2_X)&&(x <= (WS_PREDEFINED_BUTTON_2_X + WS_PREDEFINED_BUTTON_W))) // MID
		{
			SendComand(UCMD_WS_LED_WHITE_OFF);
		}
		else if((x >= WS_PREDEFINED_BUTTON_3_X)&&(x <= (WS_PREDEFINED_BUTTON_3_X + WS_PREDEFINED_BUTTON_W))) // MIN
		{
			SendComand(UCMD_WS_LED_NIGHT);
		}
	}

	//
	// Check if it is fifth line button to change LEDs to Predefined animations
	//
	else if((y >= WS_B_5_POZ_Y)&&(y <= (WS_B_5_POZ_Y + WS_LED_BUTTON_H)))
	{
		if((x >= WS_PREDEFINED_ACT_BUTTON_1_X)&&(x <= (WS_PREDEFINED_ACT_BUTTON_1_X + WS_PREDEFINED_BUTTON_W))) // MAX
		{
			SendComand(UCMD_WS_ANIMATION_SUNRISE);
		}
		else if((x >= WS_PREDEFINED_ACT_BUTTON_2_X)&&(x <= (WS_PREDEFINED_ACT_BUTTON_2_X + WS_PREDEFINED_BUTTON_W))) // MID
		{
			SendComand(UCMD_WS_ANIMATION_SUNSET);
		}
	}
}

////////////////////////////////////////////////////////////////////
//
// Functions to change state of relays to manage switches and lights
//
void firstSwitchTurn(uint8_t NewState)
{
	if(0 == NewState) //Turn OFF
	{
		SendComand(UCMD_RELAY_1_OFF); // Send comannd to OFF
		UARTDMA_Print(&huartdma1, "RELAYOFF=1\n"); // Print message to ESP
		if(MENUTFT_SWITCH == State)
		{
			EF_SetFont(&arial_11ptFontInfo);
			GFX_DrawFillRoundRectangle(SWITCH_BUTTON_X, SWITCH_1_POZ_Y, SWITCH_BUTTON_W, SWITCH_BUTTON_H, SWITCH_BUTTON_R, SWITCH_OFF_BUTTON_COLOR);
			sprintf((char*)Msg, "OFF");
			EF_PutString(Msg, (SWITCH_BUTTON_X+STRING_ERRATA_X), (SWITCH_1_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
		}
		SwitchesButtonState[0] = 0;
		EEPROM_RelayStateUpdate(1, 0);
	}
	else //Turn ON
	{
		SendComand(UCMD_RELAY_1_ON); // Send comannd to ON
		UARTDMA_Print(&huartdma1, "RELAYON=1\n"); // Print message to ESP
		if(MENUTFT_SWITCH == State)
		{
			EF_SetFont(&arial_11ptFontInfo);
			GFX_DrawFillRoundRectangle(SWITCH_BUTTON_X, SWITCH_1_POZ_Y, SWITCH_BUTTON_W, SWITCH_BUTTON_H, SWITCH_BUTTON_R, SWITCH_ON_BUTTON_COLOR);
			sprintf((char*)Msg, "ON");
			EF_PutString(Msg, (SWITCH_BUTTON_X+STRING_ERRATA_X), (SWITCH_1_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_ON_BUTTON_COLOR);
		}
		SwitchesButtonState[0] = 1;
		EEPROM_RelayStateUpdate(1, 1);
	}
}
void secondSwitchTurn(uint8_t NewState)
{
	if(0 == NewState) //Turn OFF
	{
		SendComand(UCMD_RELAY_2_OFF); // Send comannd to OFF
		UARTDMA_Print(&huartdma1, "RELAYOFF=2\n"); // Print message to ESP
		if(MENUTFT_SWITCH == State)
		{
			EF_SetFont(&arial_11ptFontInfo);
			GFX_DrawFillRoundRectangle(SWITCH_BUTTON_X, SWITCH_2_POZ_Y, SWITCH_BUTTON_W, SWITCH_BUTTON_H, SWITCH_BUTTON_R, SWITCH_OFF_BUTTON_COLOR);
			sprintf((char*)Msg, "OFF");
			EF_PutString(Msg, (SWITCH_BUTTON_X+STRING_ERRATA_X), (SWITCH_2_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
		}
		SwitchesButtonState[1] = 0;
		EEPROM_RelayStateUpdate(2, 0);
	}
	else //Turn ON
	{
		SendComand(UCMD_RELAY_2_ON); // Send comannd to ON
		UARTDMA_Print(&huartdma1, "RELAYON=2\n"); // Print message to ESP
		if(MENUTFT_SWITCH == State)
		{
			EF_SetFont(&arial_11ptFontInfo);
			GFX_DrawFillRoundRectangle(SWITCH_BUTTON_X, SWITCH_2_POZ_Y, SWITCH_BUTTON_W, SWITCH_BUTTON_H, SWITCH_BUTTON_R, SWITCH_ON_BUTTON_COLOR);
			sprintf((char*)Msg, "ON");
			EF_PutString(Msg, (SWITCH_BUTTON_X+STRING_ERRATA_X), (SWITCH_2_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_ON_BUTTON_COLOR);
		}
		SwitchesButtonState[1] = 1;
		EEPROM_RelayStateUpdate(2, 1);
	}
}
void thirdSwitchTurn(uint8_t NewState)
{
	if(0 == NewState) //Turn OFF
	{
		SendComand(UCMD_RELAY_3_OFF); // Send comannd to OFF
		UARTDMA_Print(&huartdma1, "RELAYOFF=3\n"); // Print message to ESP
		if(MENUTFT_SWITCH == State)
		{
			EF_SetFont(&arial_11ptFontInfo);
			GFX_DrawFillRoundRectangle(SWITCH_BUTTON_X, SWITCH_3_POZ_Y, SWITCH_BUTTON_W, SWITCH_BUTTON_H, SWITCH_BUTTON_R, SWITCH_OFF_BUTTON_COLOR);
			sprintf((char*)Msg, "OFF");
			EF_PutString(Msg, (SWITCH_BUTTON_X+STRING_ERRATA_X), (SWITCH_3_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
		}
		SwitchesButtonState[2] = 0;
		EEPROM_RelayStateUpdate(3, 0);
	}
	else //Turn ON
	{
		SendComand(UCMD_RELAY_3_ON); // Send comannd to ON
		UARTDMA_Print(&huartdma1, "RELAYON=3\n"); // Print message to ESP
		if(MENUTFT_SWITCH == State)
		{
			EF_SetFont(&arial_11ptFontInfo);
			GFX_DrawFillRoundRectangle(SWITCH_BUTTON_X, SWITCH_3_POZ_Y, SWITCH_BUTTON_W, SWITCH_BUTTON_H, SWITCH_BUTTON_R, SWITCH_ON_BUTTON_COLOR);
			sprintf((char*)Msg, "ON");
			EF_PutString(Msg, (SWITCH_BUTTON_X+STRING_ERRATA_X), (SWITCH_3_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_ON_BUTTON_COLOR);
		}
		SwitchesButtonState[2] = 1;
		EEPROM_RelayStateUpdate(3, 1);
	}
}
void fourthSwitchTurn(uint8_t NewState)
{
	if(0 == NewState) //Turn OFF
	{
		SendComand(UCMD_RELAY_4_OFF); // Send comannd to OFF
		UARTDMA_Print(&huartdma1, "RELAYOFF=4\n"); // Print message to ESP
		if(MENUTFT_SWITCH == State)
		{
			EF_SetFont(&arial_11ptFontInfo);
			GFX_DrawFillRoundRectangle(SWITCH_BUTTON_X, SWITCH_4_POZ_Y, SWITCH_BUTTON_W, SWITCH_BUTTON_H, SWITCH_BUTTON_R, SWITCH_OFF_BUTTON_COLOR);
			sprintf((char*)Msg, "OFF");
			EF_PutString(Msg, (SWITCH_BUTTON_X+STRING_ERRATA_X), (SWITCH_4_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
		}
		SwitchesButtonState[3] = 0;
		EEPROM_RelayStateUpdate(4, 0);
	}
	else //Turn ON
	{
		SendComand(UCMD_RELAY_4_ON); // Send comannd to ON
		UARTDMA_Print(&huartdma1, "RELAYON=4\n"); // Print message to ESP
		if(MENUTFT_SWITCH == State)
		{
			EF_SetFont(&arial_11ptFontInfo);
			GFX_DrawFillRoundRectangle(SWITCH_BUTTON_X, SWITCH_4_POZ_Y, SWITCH_BUTTON_W, SWITCH_BUTTON_H, SWITCH_BUTTON_R, SWITCH_ON_BUTTON_COLOR);
			sprintf((char*)Msg, "ON");
			EF_PutString(Msg, (SWITCH_BUTTON_X+STRING_ERRATA_X), (SWITCH_4_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_ON_BUTTON_COLOR);
		}
		SwitchesButtonState[3] = 1;
		EEPROM_RelayStateUpdate(4, 1);
	}
}


void firstLightTurn(uint8_t NewState)
{
	if(0 == NewState) //Turn OFF
	{
		SendComand(UCMD_LIGHT_1_OFF); // Send comannd to OFF
		UARTDMA_Print(&huartdma1, "RELAYOFF=5\n"); // Print message to ESP
		if(MENUTFT_LIGHTS == State)
		{
			EF_SetFont(&arial_11ptFontInfo);
			GFX_DrawFillRoundRectangle(LIGHTS_BUTTON_X, LIGHT_B_1_POZ_Y, LIGHTS_BUTTON_W, LIGHTS_BUTTON_H, LIGHTS_BUTTON_R, SWITCH_OFF_BUTTON_COLOR);
			sprintf((char*)Msg, "OFF");
			EF_PutString(Msg, (LIGHTS_BUTTON_X+STRING_ERRATA_X), (LIGHT_B_1_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
		}
		LightsButtonState[0] = 0;
		EEPROM_LightStateUpdate(1, 0);
	}
	else //Turn ON
	{
		SendComand(UCMD_LIGHT_1_ON); // Send comannd to ON
		UARTDMA_Print(&huartdma1, "RELAYON=5\n"); // Print message to ESP
		if(MENUTFT_LIGHTS == State)
		{
			EF_SetFont(&arial_11ptFontInfo);
			GFX_DrawFillRoundRectangle(LIGHTS_BUTTON_X, LIGHT_B_1_POZ_Y, LIGHTS_BUTTON_W, LIGHTS_BUTTON_H, LIGHTS_BUTTON_R, SWITCH_ON_BUTTON_COLOR);
			sprintf((char*)Msg, "ON");
			EF_PutString(Msg, (LIGHTS_BUTTON_X+STRING_ERRATA_X), (LIGHT_B_1_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
		}
		LightsButtonState[0] = 1;
		EEPROM_LightStateUpdate(1, 1);
	}
}
void secondLightTurn(uint8_t NewState)
{
	if(0 == NewState) //Turn OFF
	{
		SendComand(UCMD_LIGHT_2_OFF); // Send comannd to OFF
		UARTDMA_Print(&huartdma1, "RELAYOFF=6\n"); // Print message to ESP
		if(MENUTFT_LIGHTS == State)
		{
			EF_SetFont(&arial_11ptFontInfo);
			GFX_DrawFillRoundRectangle(LIGHTS_BUTTON_X, LIGHT_B_2_POZ_Y, LIGHTS_BUTTON_W, LIGHTS_BUTTON_H, LIGHTS_BUTTON_R, SWITCH_OFF_BUTTON_COLOR);
			sprintf((char*)Msg, "OFF");
			EF_PutString(Msg, (LIGHTS_BUTTON_X+STRING_ERRATA_X), (LIGHT_B_2_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
		}
		LightsButtonState[1] = 0;
		EEPROM_LightStateUpdate(2, 0);
	}
	else //Turn ON
	{
		SendComand(UCMD_LIGHT_2_ON); // Send comannd to ON
		UARTDMA_Print(&huartdma1, "RELAYON=6\n"); // Print message to ESP
		if(MENUTFT_LIGHTS == State)
		{
			EF_SetFont(&arial_11ptFontInfo);
			GFX_DrawFillRoundRectangle(LIGHTS_BUTTON_X, LIGHT_B_2_POZ_Y, LIGHTS_BUTTON_W, LIGHTS_BUTTON_H, LIGHTS_BUTTON_R, SWITCH_ON_BUTTON_COLOR);
			sprintf((char*)Msg, "ON");
			EF_PutString(Msg, (LIGHTS_BUTTON_X+STRING_ERRATA_X), (LIGHT_B_2_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
		}
		LightsButtonState[1] = 1;
		EEPROM_LightStateUpdate(2, 1);
	}
}
void thirdLightTurn(uint8_t NewState)
{
	if(0 == NewState) //Turn OFF
	{
		SendComand(UCMD_LIGHT_3_OFF); // Send comannd to OFF
		UARTDMA_Print(&huartdma1, "RELAYOFF=7\n"); // Print message to ESP
		if(MENUTFT_LIGHTS == State)
		{
			EF_SetFont(&arial_11ptFontInfo);
			GFX_DrawFillRoundRectangle(LIGHTS_BUTTON_X, LIGHT_B_3_POZ_Y, LIGHTS_BUTTON_W, LIGHTS_BUTTON_H, LIGHTS_BUTTON_R, SWITCH_OFF_BUTTON_COLOR);
			sprintf((char*)Msg, "OFF");
			EF_PutString(Msg, (LIGHTS_BUTTON_X+STRING_ERRATA_X), (LIGHT_B_3_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
		}
		LightsButtonState[2] = 0;
		EEPROM_LightStateUpdate(3, 0);
	}
	else //Turn ON
	{
		SendComand(UCMD_LIGHT_3_ON); // Send comannd to ON
		UARTDMA_Print(&huartdma1, "RELAYON=7\n"); // Print message to ESP
		if(MENUTFT_LIGHTS == State)
		{
			EF_SetFont(&arial_11ptFontInfo);
			GFX_DrawFillRoundRectangle(LIGHTS_BUTTON_X, LIGHT_B_3_POZ_Y, LIGHTS_BUTTON_W, LIGHTS_BUTTON_H, LIGHTS_BUTTON_R, SWITCH_ON_BUTTON_COLOR);
			sprintf((char*)Msg, "ON");
			EF_PutString(Msg, (LIGHTS_BUTTON_X+STRING_ERRATA_X), (LIGHT_B_3_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
		}
		LightsButtonState[2] = 1;
		EEPROM_LightStateUpdate(3, 1);
	}
}
void fourthLightTurn(uint8_t NewState)
{
	if(0 == NewState) //Turn OFF
	{
		SendComand(UCMD_LIGHT_4_OFF); // Send comannd to OFF
		UARTDMA_Print(&huartdma1, "RELAYOFF=8\n"); // Print message to ESP
		if(MENUTFT_LIGHTS == State)
		{
			EF_SetFont(&arial_11ptFontInfo);
			GFX_DrawFillRoundRectangle(LIGHTS_BUTTON_X, LIGHT_B_4_POZ_Y, LIGHTS_BUTTON_W, LIGHTS_BUTTON_H, LIGHTS_BUTTON_R, SWITCH_OFF_BUTTON_COLOR);
			sprintf((char*)Msg, "OFF");
			EF_PutString(Msg, (LIGHTS_BUTTON_X+STRING_ERRATA_X), (LIGHT_B_4_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
		}
		LightsButtonState[3] = 0;
		EEPROM_LightStateUpdate(4, 0);
	}
	else //Turn ON
	{
		SendComand(UCMD_LIGHT_4_ON); // Send comannd to ON
		UARTDMA_Print(&huartdma1, "RELAYON=8\n"); // Print message to ESP
		if(MENUTFT_LIGHTS == State)
		{
			EF_SetFont(&arial_11ptFontInfo);
			GFX_DrawFillRoundRectangle(LIGHTS_BUTTON_X, LIGHT_B_4_POZ_Y, LIGHTS_BUTTON_W, LIGHTS_BUTTON_H, LIGHTS_BUTTON_R, SWITCH_ON_BUTTON_COLOR);
			sprintf((char*)Msg, "ON");
			EF_PutString(Msg, (LIGHTS_BUTTON_X+STRING_ERRATA_X), (LIGHT_B_4_POZ_Y+STRING_ERRATA_Y), ILI9341_BLACK, BG_TRANSPARENT, SWITCH_OFF_BUTTON_COLOR);
		}
		LightsButtonState[3] = 1;
		EEPROM_LightStateUpdate(4, 1);
	}
}
