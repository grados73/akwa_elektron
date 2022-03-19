/*
 * menuTFT.h
 *
 *  Created on: Nov 29, 2021
 *      Author: grad_
 */

#ifndef INC_MENUTFT_H_
#define INC_MENUTFT_H_

#define SWITCH_DEBOUNCING_TIME_MS 1000

//
// Define position, color and size of button
//
#define RIGHT_LEFT_BUTTON_R 5 //Rounding of button
#define SWITCH_ON_BUTTON_COLOR ILI9341_GREENYELLOW
#define SWITCH_OFF_BUTTON_COLOR ILI9341_RED
#define SWITCH_CLOCK_BUTTON_COLOR ILI9341_ORANGE
#define LED_WS_WHITE_BUTTON_COLOR ILI9341_WHITE
#define LED_WS_BLUE_BUTTON_COLOR ILI9341_BLUE
#define LED_WS_DAILY_BUTTON_COLOR ILI9341_YELLOW
#define LED_WS_NIGHT_BUTTON_COLOR ILI9341_MAGENTA
#define LED_WS_SUN_BUTTON_COLOR ILI9341_CYAN
#define SWITCH_ON_ACTIVITY_BUTTON_COLOR ILI9341_GREEN
#define DAY_CHECK_SWITCH_COLOR ILI9341_RED
#define DAY_CHECK_SWITCH_COLOR_OFF ILI9341_RED
#define DAY_CHECK_SWITCH_COLOR_ON ILI9341_GREEN
#define SHEDULE_ADD_HOUR_MINUTE_BUTTON_COLOR ILI9341_YELLOW

//define position and size of button to change menu screen to RIGHT
#define RIGHT_BUTTON_X 220
#define RIGHT_BUTTON_Y 216
#define RIGHT_BUTTON_W 95
#define RIGHT_BUTTON_H 20
//define position and size of button to change menu screen to LEFT
#define LEFT_BUTTON_X 10
#define LEFT_BUTTON_Y 216
#define LEFT_BUTTON_W 100
#define LEFT_BUTTON_H 20
//define position and size of button to change menu screen to CLOCK
#define MEDIUM_BUTTON_X 120
#define MEDIUM_BUTTON_Y 216
#define MEDIUM_BUTTON_W 90
#define MEDIUM_BUTTON_H 20

//
//define position and size of button to change state of EVERY switch in showControlPanel()
#define SWITCH_BUTTON_X 270
#define SWITCH_BUTTON_W 33
#define SWITCH_BUTTON_H 21
#define SWITCH_BUTTON_R 5
//define position of EACH ONE switch in showControlPanel()
#define SWITCH_1_POZ_Y (STRING_SWITCH_1_POZ_Y+5)
#define SWITCH_2_POZ_Y (STRING_SWITCH_2_POZ_Y+5)
#define SWITCH_3_POZ_Y (STRING_SWITCH_3_POZ_Y+5)
#define SWITCH_4_POZ_Y (STRING_SWITCH_4_POZ_Y+5)

//
//define position and size of button to change state of EVERY switch in showLightsControlPanel()
#define LIGHTS_BUTTON_X 200
#define LIGHTS_BUTTON_W 33
#define LIGHTS_BUTTON_H 21
#define LIGHTS_BUTTON_R 5
//define position of EACH ONE switch in showLightsControlPanel()
#define LIGHT_B_1_POZ_Y (STRING_LIGHTS_1_POZ_Y+5)
#define LIGHT_B_2_POZ_Y (STRING_LIGHTS_2_POZ_Y+5)
#define LIGHT_B_3_POZ_Y (STRING_LIGHTS_3_POZ_Y+5)
#define LIGHT_B_4_POZ_Y (STRING_LIGHTS_4_POZ_Y+5)

//define position and size of button to change current time
#define CLOCK_BUTTON_X 240
#define CLOCK_BUTTON2_X (CLOCK_BUTTON_X+CLOCK_BUTTON_W+10)

#define CLOCK_BUTTON_W 30
#define CLOCK_BUTTON_H 21
#define CLOCK_BUTTON_R 5

#define MEDIUM_BUTTON_CLOCK_X 110
#define MEDIUM_BUTTON_CLOCK_Y 190
#define MEDIUM_BUTTON_CLOCK_W 110
#define MEDIUM_BUTTON_H_CLOCK 22

#define CLOCK_B_1_POZ_Y (STRING_HOUR_POZ_Y+5)
#define CLOCK_B_2_POZ_Y (STRING_MINUTE_POZ_Y+5)
#define CLOCK_B_3_POZ_Y (STRING_DAY_OF_WEEK_POZ_Y+5)

//define position and size of button to change activity state
#define ACTIVITY_BUTTON_X 100
#define ACTIVITY_BUTTON_W 120
#define ACTIVITY_BUTTON_H 40
#define ACTIVITY_BUTTON_R 10
#define ACTIVITY_BUTTON_1_Y 60
#define ACTIVITY_BUTTON_2_Y 120

//define position and size of button to change WS2812b LEDs
#define WS_LED_BUTTON_W 30
#define WS_LED_BUTTON_H 21
#define WS_LED_BUTTON_R 5
#define WS_LED_BUTTON_SPACE 5

#define WS_LED_BUTTON_1_X 217
#define WS_LED_BUTTON_2_X (WS_LED_BUTTON_1_X+WS_LED_BUTTON_W+WS_LED_BUTTON_SPACE)
#define WS_LED_BUTTON_3_X (WS_LED_BUTTON_2_X+WS_LED_BUTTON_W+WS_LED_BUTTON_SPACE)

#define WS_WHITE_BUTTON_1_X 150
#define WS_WHITE_BUTTON_2_X (WS_WHITE_BUTTON_1_X+WS_COLOR_BUTTON_W+WS_COLOR_BUTTON_SPACE)
#define WS_WHITE_BUTTON_3_X (WS_WHITE_BUTTON_2_X+WS_COLOR_BUTTON_W+WS_COLOR_BUTTON_SPACE)

#define WS_COLOR_BUTTON_SPACE 12
#define WS_COLOR_BUTTON_W 40

#define WS_PREDEFINED_BUTTON_1_X 20
#define WS_PREDEFINED_BUTTON_2_X (WS_PREDEFINED_BUTTON_1_X+WS_PREDEFINED_BUTTON_W+WS_PREDEFINED_BUTTON_SPACE)
#define WS_PREDEFINED_BUTTON_3_X (WS_PREDEFINED_BUTTON_2_X+WS_PREDEFINED_BUTTON_W+WS_PREDEFINED_BUTTON_SPACE)

#define WS_PREDEFINED_ACT_BUTTON_1_X 70
#define WS_PREDEFINED_ACT_BUTTON_2_X (WS_PREDEFINED_ACT_BUTTON_1_X+WS_PREDEFINED_BUTTON_W+WS_PREDEFINED_BUTTON_SPACE)

#define WS_PREDEFINED_BUTTON_W 80
#define WS_PREDEFINED_BUTTON_H 21
#define WS_PREDEFINED_BUTTON_SPACE 20


#define WS_B_1_POZ_Y (STRING_WS_LED_ILOSC_POZ_Y+5)
#define WS_B_2_POZ_Y (STRING_WS_LED_WHITE_POZ_Y+5)
#define WS_B_3_POZ_Y (STRING_WS_LED_BLUE_POZ_Y+5)
#define WS_B_4_POZ_Y (WS_B_3_POZ_Y + WS_LED_BUTTON_H + 10)
#define WS_B_5_POZ_Y (WS_B_4_POZ_Y + WS_LED_BUTTON_H + 10)

//
// Schedule panel
// define position of check button to define day of week
#define DAY_CHECK_BUTTON_Y 50
#define DAY_CHECK_BUTTON_W 20
#define DAY_CHECK_BUTTON_H 20
#define DAY_CHECK_BUTTON_R 5
#define DAY_CHECK_BUTTON_SPACE 10

#define DAY_CHECK_BUTTON_1_X 60
#define DAY_CHECK_BUTTON_2_X (DAY_CHECK_BUTTON_1_X + DAY_CHECK_BUTTON_W + DAY_CHECK_BUTTON_SPACE)
#define DAY_CHECK_BUTTON_3_X (DAY_CHECK_BUTTON_2_X + DAY_CHECK_BUTTON_W + DAY_CHECK_BUTTON_SPACE)
#define DAY_CHECK_BUTTON_4_X (DAY_CHECK_BUTTON_3_X + DAY_CHECK_BUTTON_W + DAY_CHECK_BUTTON_SPACE)
#define DAY_CHECK_BUTTON_5_X (DAY_CHECK_BUTTON_4_X + DAY_CHECK_BUTTON_W + DAY_CHECK_BUTTON_SPACE)
#define DAY_CHECK_BUTTON_6_X (DAY_CHECK_BUTTON_5_X + DAY_CHECK_BUTTON_W + DAY_CHECK_BUTTON_SPACE)
#define DAY_CHECK_BUTTON_7_X (DAY_CHECK_BUTTON_6_X + DAY_CHECK_BUTTON_W + DAY_CHECK_BUTTON_SPACE)

#define DAY_CHECK_RELAY_BUTTON_SPACE 12
#define DAY_CHECK_RELAY_BUTTON_Y 170

#define DAY_CHECK_RELAY_BUTTON_1_X 21
#define DAY_CHECK_RELAY_BUTTON_2_X (DAY_CHECK_RELAY_BUTTON_1_X + DAY_CHECK_BUTTON_W + DAY_CHECK_RELAY_BUTTON_SPACE)
#define DAY_CHECK_RELAY_BUTTON_3_X (DAY_CHECK_RELAY_BUTTON_2_X + DAY_CHECK_BUTTON_W + DAY_CHECK_RELAY_BUTTON_SPACE)
#define DAY_CHECK_RELAY_BUTTON_4_X (DAY_CHECK_RELAY_BUTTON_3_X + DAY_CHECK_BUTTON_W + DAY_CHECK_RELAY_BUTTON_SPACE)
#define DAY_CHECK_RELAY_BUTTON_5_X (DAY_CHECK_RELAY_BUTTON_4_X + DAY_CHECK_BUTTON_W + DAY_CHECK_RELAY_BUTTON_SPACE)
#define DAY_CHECK_RELAY_BUTTON_6_X (DAY_CHECK_RELAY_BUTTON_5_X + DAY_CHECK_BUTTON_W + DAY_CHECK_RELAY_BUTTON_SPACE)
#define DAY_CHECK_RELAY_BUTTON_7_X (DAY_CHECK_RELAY_BUTTON_6_X + DAY_CHECK_BUTTON_W + DAY_CHECK_RELAY_BUTTON_SPACE)
#define DAY_CHECK_RELAY_BUTTON_8_X (DAY_CHECK_RELAY_BUTTON_7_X + DAY_CHECK_BUTTON_W + DAY_CHECK_RELAY_BUTTON_SPACE)
#define DAY_CHECK_RELAY_BUTTON_9_X (DAY_CHECK_RELAY_BUTTON_8_X + DAY_CHECK_BUTTON_W + DAY_CHECK_RELAY_BUTTON_SPACE)

#define HOOUR_MINUTE_BUTTON_W 34
#define HOUR_MINUTE_ON_ADD_SHEDULE_Y (STRING_ON_Y+1)
#define HOUR_MINUTE_OFF_ADD_SHEDULE_Y (STRING_OFF_Y+1)
#define ONE_HOUR_ADD_SHEDULE_X  187
#define ONE_MINUTE_ADD_SHEDULE_X (ONE_HOUR_ADD_SHEDULE_X + HOOUR_MINUTE_BUTTON_W + DAY_CHECK_RELAY_BUTTON_SPACE)
#define TEN_MINUTE_ADD_SHEDULE_X (ONE_MINUTE_ADD_SHEDULE_X + HOOUR_MINUTE_BUTTON_W + DAY_CHECK_RELAY_BUTTON_SPACE)

//
// Variables
//
// Structure of MenuTFT current state
typedef enum
{
	MENUTFT_INIT, 			// Build GUI
	MENUTFT_PARAMETERS, 	// Read parameters
	MENUTFT_SWITCH,			// Show current switch state, and possibility to change them
	MENUTFT_CLOCK,			// Set current Hour and Minute
	MENUTFT_ACTIVITIES,		// Show possible Activities connected with pre-prepared actions
	MENUTFT_LIGHTS, 		// Show current lights state, and possibility to change them
	MENUTFT_WS_LED,			// Show panel to control WS2812b LEDs
	MENUTFT_SCHEDULE_1,		// Show panel to set shedule 1
	MENUTFT_SCHEDULE_2		// Show panel to set shedule 2
} MenuTFTState;

typedef enum
{
	ENCODER_IDLE, 			// Build GUI
	ENCODER_CLOCK_HOUR,		// Clock screen
	ENCODER_CLOCK_MINUTE,
	ENCODER_CLOCK_DAY


} EncoderRotateState;

//
// Declaration of function to handling touch in each screen
void MenuTFT(void);
void TouchParametersActivity(void);
void TouchSwitchActivity(void);
void TouchLightsActivity(void);
void TouchClockActivity(void);
void TouchPredefinedActivityActivity();
void TouchWSLedActivity(void);
void showSchedule1Panel(void);
void showSchedule2Panel(void);
void Schedule1Activity(void);
void Schedule2Activity(void);

void MenuTFTSchedule1ActivityDayOfWeekRow(uint16_t x, uint16_t y);
void MenuTFTSchedule1ActivityHourMinuteONAdd(uint16_t x, uint16_t y);
void MenuTFTSchedule1ActivityHourMinuteOFFAdd(uint16_t x, uint16_t y);
void MenuTFTSchedule1ActivityRelaysAndLightsRow(uint16_t x, uint16_t y);

void MenuTFTSchedule2ActivityDayOfWeekRow(uint16_t x, uint16_t y);
void MenuTFTSchedule2ActivityHourMinuteONAdd(uint16_t x, uint16_t y);
void MenuTFTSchedule2ActivityHourMinuteOFFAdd(uint16_t x, uint16_t y);
void MenuTFTSchedule2ActivityRelaysAndLightsRow(uint16_t x, uint16_t y);

void changeTFTScheduleDayOfWeek(uint8_t NrOfDay, uint8_t NewState);
void changeTFTScheduleRelayLights(uint8_t NrOfRS, uint8_t NewState);

void updateAllSchedulesInfo(uint8_t NrOfSchedule);

void clockIncreaseOneHourAndMinute(uint16_t x, uint16_t y);
void clockIncreaseSixHoursTenMinutes(uint16_t x, uint16_t y);

void changeSwitchStatusTouch(uint8_t y);
void changeLightsStatusTouch(uint8_t y);

void predefinedActivityTouchAction(uint8_t y);

void increaseNumberOfWSLedOnTFT(uint8_t x, uint8_t y);
void changeTypeOfWSLights(uint8_t x, uint8_t y);

void firstSwitchTurn(uint8_t NewState);
void secondSwitchTurn(uint8_t NewState);
void thirdSwitchTurn(uint8_t NewState);
void fourthSwitchTurn(uint8_t NewState);

void firstLightTurn(uint8_t NewState);
void secondLightTurn(uint8_t NewState);
void thirdLightTurn(uint8_t NewState);
void fourthLightTurn(uint8_t NewState);

#endif /* INC_MENUTFT_H_ */
