/*
 * eeprom.h
 *
 *  Created on: Dec 21, 2021
 *      Author: grad_
 */

#pragma once

#ifndef INC_EEPROM_H_
#define INC_EEPROM_H_
#include "stm32f4xx.h"

// Opóźnienie niezbędne do zakończenie zapisu
// Funkcja wprowadza automatycznie opóźnienie tylko, gdy jest ono potrzebne
void eeprom_wait(void);

// Odczyt danych z pamięci EEPROM
// addr - początkowy adres danych
// data - bufor na odczytane dane
// size - liczba bajtów do odczytania
// return - HAL_OK jeśli odczyt zakończony sukcesem, wpp. błąd
HAL_StatusTypeDef eeprom_read(uint32_t addr, void* data, uint32_t size);


// Zapis danych do pamięci EEPROM
// addr - początkowy adres danych
// data - bufor na dane zapisu
// size - liczba bajtów do zapisania
// return - HAL_OK jeśli zapis zakończony sukcesem, wpp. błąd
HAL_StatusTypeDef eeprom_write(uint32_t addr, const void* data, uint32_t size);


/*
 *  EEPROM 128B
* 	1	-	0x01	-	NUMBER OF WS2812b LEDs
*	2	-	0x02	-
*	3	-	0x03	-
*	4	-	0x04	-	WS2812b LEDs
*	5	-	0x05	-	SHEDULE 1 DAYS IN WEEK
*	6	-	0x06	-	SHEDULE 1 RELAYS
*	7	-	0x07	-	SHEDULE 1 LIGHTS
*	8	-	0x08	-	SHEDULE 1 HOUR ON
*	9	-	0x09	-	SHEDULE 1 MINUTE ON
*	10	-	0x0A	-	SHEDULE 1 HOUR OFF
*	11	-	0x0B	-	SHEDULE 1 MINUTE OFF
*	12	-	0x0C	-	SHEDULE 2 DAYS IN WEEK
*	13	-	0x0D	-	SHEDULE 2 RELAYS
*	14	-	0x0E	-	SHEDULE 2 LIGHTS
*	15	-	0x0F	-	SHEDULE 2 HOUR ON
*	16	-	0x10	-	SHEDULE 2 MINUTE ON
*	17	-	0x11	-	SHEDULE 2 HOUR OFF
*	18	-	0x12	-	SHEDULE 2 MINUTE OFF
*	19	-	0x13	-
*	20	-	0x14	-
*	21	-	0x15	-	ALARM TEMP WEW MAX
*	22	-	0x16	-	ALARM TEMP WEW MIN
*	23	-	0x17	-	NORMAL WATER LVL
*	24	-	0x18	-	MAX DIFFERENCE IN WATER LVL
*	25	-	0x19	-
*	26	-	0x1A	-
*	27	-	0x1B	-	TEMP WEW -12
*	28	-	0x1C	-	TEMP WEW -11
*	29	-	0x1D	-	TEMP WEW -10
*	30	-	0x1E	-	TEMP WEW -9
*	31	-	0x1F	-	TEMP WEW -8
*	32	-	0x20	-	TEMP WEW -7
*	33	-	0x21	-	TEMP WEW -6
*	34	-	0x22	-	TEMP WEW -5
*	35	-	0x23	-	TEMP WEW -4
*	36	-	0x24	-	TEMP WEW -3
*	37	-	0x25	-	TEMP WEW -2
*	38	-	0x26	-	TEMP WEW -1
*	39	-	0x27	-	TEMP WEW MEDIUM OF LAST 12H
*	40	-	0x28	-
*	41	-	0x29	-	SHEDULE 1 WS2812b DAY STATE
*	42	-	0x2A	-	SHEDULE 1 WS2812b NIGHT STATE
*	43	-	0x2B	-	SHEDULE 2 WS2812b DAY STATE
*	44	-	0x2C	-	SHEDULE 2 WS2812b NIGHT STATE
*	45	-	0x2D	-
*	46	-	0x2E	-	RELAY 1 STATE
*	47	-	0x2F	-	RELAY 2 STATE
*	48	-	0x30	-	RELAY 3 STATE
*	49	-	0x31	-	RELAY 4 STATE
*	50	-	0x32	-
*	51	-	0x33	-	LIGHT 1 STATE
*	52	-	0x34	-	LIGHT 2 STATE
*	53	-	0x35	-	LIGHT 3 STATE
*	54	-	0x36	-	LIGHT 4 STATE
*	55	-	0x37	-
*	56	-	0x38	-
*	57	-	0x39	-
*	58	-	0x3A	-
*	59	-	0x3B	-
*	60	-	0x3C	-
*	61	-	0x3D	-
*	62	-	0x3E	-
*	63	-	0x3F	-
*	64	-	0x40	-
*	65	-	0x41	-
*	66	-	0x42	-
*	67	-	0x43	-
*	68	-	0x44	-
*	69	-	0x45	-
*	70	-	0x46	-
*	71	-	0x47	-
*	72	-	0x48	-
*	73	-	0x49	-
*	74	-	0x4A	-
*	75	-	0x4B	-
*	76	-	0x4C	-
*	77	-	0x4D	-
*	78	-	0x4E	-
*	79	-	0x4F	-
*	80	-	0x50	-
*	81	-	0x51	-
*	82	-	0x52	-
*	83	-	0x53	-
*	84	-	0x54	-
*	85	-	0x55	-
*	86	-	0x56	-
*	87	-	0x57	-
*	88	-	0x58	-
*	89	-	0x59	-
*	90	-	0x5A	-
*	91	-	0x5B	-
*	92	-	0x5C	-
*	93	-	0x5D	-
*	94	-	0x5E	-
*	95	-	0x5F	-
*	96	-	0x60	-
*	97	-	0x61	-
*	98	-	0x62	-
*	99	-	0x63	-
*	100	-	0x64	-
*	101	-	0x65	-
*	102	-	0x66	-
*	103	-	0x67	-
*	104	-	0x68	-
*	105	-	0x69	-
*	106	-	0x6A	-
*	107	-	0x6B	-
*	108	-	0x6C	-
*	109	-	0x6D	-
*	110	-	0x6E	-
*	111	-	0x6F	-
*	112	-	0x70	-
*	113	-	0x71	-
*	114	-	0x72	-
*	115	-	0x73	-
*	116	-	0x74	-
*	117	-	0x75	-
*	118	-	0x76	-
*	119	-	0x77	-
*	120	-	0x78	-
*	121	-	0x79	-
*	122	-	0x7A	-
*	123	-	0x7B	-
*	124	-	0x7C	-
*	125	-	0x7D	-
*	126	-	0x7E	-
*	127	-	0x7F	-
*	128	-	0x80	-

 */

#define EEPROM_ADR_NUMBER_WS_LEDS 0x01
#define EEPROM_ADR_WS2812B_STATE 0x04

#define EEPROM_ADR_SHEDULE_1_DAYS 0x05
#define EEPROM_ADR_SHEDULE_1_RELAYS 0x06
#define EEPROM_ADR_SHEDULE_1_LIGHTS 0x07
#define EEPROM_ADR_SHEDULE_1_HOUR_ON 0x08
#define EEPROM_ADR_SHEDULE_1_MINUTE_ON 0x09
#define EEPROM_ADR_SHEDULE_1_HOUR_OFF 0x0A
#define EEPROM_ADR_SHEDULE_1_MINUTE_OFF 0x0B
#define EEPROM_ADR_SHEDULE_1_DAY_WS_LED_STATE 0x29
#define EEPROM_ADR_SHEDULE_1_NIGHT_WS_LED_STATE 0x2A

#define EEPROM_ADR_SHEDULE_2_DAYS 0x0C
#define EEPROM_ADR_SHEDULE_2_RELAYS 0x0D
#define EEPROM_ADR_SHEDULE_2_LIGHTS 0x0E
#define EEPROM_ADR_SHEDULE_2_HOUR_ON 0x0F
#define EEPROM_ADR_SHEDULE_2_MINUTE_ON 0x10
#define EEPROM_ADR_SHEDULE_2_HOUR_OFF 0x11
#define EEPROM_ADR_SHEDULE_2_MINUTE_OFF 0x12
#define EEPROM_ADR_SHEDULE_2_DAY_WS_LED_STATE 0x2B
#define EEPROM_ADR_SHEDULE_2_NIGHT_WS_LED_STATE 0x2C

#define EEPROM_ADR_ALARM_TEMP_WEW_MAX 0x15
#define EEPROM_ADR_ALARM_TEMP_WEW_MIN 0x16
#define EEPROM_ADR_NORMAL_WATER_LVL 0x17
#define EEPROM_ADR_MAXDIFFERENCE_IN_WATER_LVL 0x18

#define EEPROM_ADR_TEMP_WEW_12H_AGO 0x1B
#define EEPROM_ADR_TEMP_WEW_11H_AGO 0x1C
#define EEPROM_ADR_TEMP_WEW_10H_AGO 0x1D
#define EEPROM_ADR_TEMP_WEW_9H_AGO 0x1E
#define EEPROM_ADR_TEMP_WEW_8H_AGO 0x1F
#define EEPROM_ADR_TEMP_WEW_7H_AGO 0x20
#define EEPROM_ADR_TEMP_WEW_6H_AGO 0x21
#define EEPROM_ADR_TEMP_WEW_5H_AGO 0x22
#define EEPROM_ADR_TEMP_WEW_4H_AGO 0x23
#define EEPROM_ADR_TEMP_WEW_3H_AGO 0x24
#define EEPROM_ADR_TEMP_WEW_2H_AGO 0x25
#define EEPROM_ADR_TEMP_WEW_1H_AGO 0x26
#define EEPROM_ADR_TEMP_WEW_MED_LAST_12H 0x27

#define EEPROM_ADR_RELAY_1_STATE 0x2E
#define EEPROM_ADR_RELAY_2_STATE 0x2F
#define EEPROM_ADR_RELAY_3_STATE 0x30
#define EEPROM_ADR_RELAY_4_STATE 0x31

#define EEPROM_ADR_LIGHT_1_STATE 0x33
#define EEPROM_ADR_LIGHT_2_STATE 0x34
#define EEPROM_ADR_LIGHT_3_STATE 0x35
#define EEPROM_ADR_LIGHT_4_STATE 0x36


//
// Functions initializations
void EEPROM_RelayStateUpdate(uint8_t RelayNumber, uint8_t NewState);
uint8_t EEPROM_RelayStateRead(uint8_t RelayNumber);
void EEPROM_RelayStateRestore(void);

void EEPROM_LightStateUpdate(uint8_t LightNumber, uint8_t NewState);
uint8_t EEPROM_LightStateRead(uint8_t LightNumber);
void EEPROM_LightStateRestore(void);

//
// Read which day of week this schedule apply
void EEPROM_ScheduleDayInWeekRead(uint8_t NrOfSchedule, uint8_t * scheduleDayInWeekTab);
// Update which day of week this schedule apply
void EEPROM_ScheduleDayInWeekUpdate(uint8_t NrOfSchedule, const uint8_t * scheduleDayInWeekTab);

//
// Read which Relay and Lights this schedule apply
void EEPROM_ScheduleRelayAndSwitchTabRead(uint8_t NrOfSchedule, uint8_t * scheduleRelayAndSwitchTab);
// Update which Relay and Lights this schedule apply
void EEPROM_ScheduleRelayAndSwitchTabUpdate(uint8_t NrOfSchedule, const uint8_t * scheduleRelayAndSwitchTab);



//
// Read Hour and Minute of ON and OFF in schedules
void EEPROM_ScheduleHourOnRead(uint8_t NrOfSchedule, uint8_t * hourOn);
void EEPROM_ScheduleMinuteOnRead(uint8_t NrOfSchedule, uint8_t * minuteOn);
void EEPROM_ScheduleHourOffRead(uint8_t NrOfSchedule, uint8_t * hourOff);
void EEPROM_ScheduleMinuteOffRead(uint8_t NrOfSchedule, uint8_t * minuteOff);

//
// Set (Update) Hour and Minute of ON and OFF in schedules
void EEPROM_ScheduleHourOnUpdate(uint8_t NrOfSchedule, uint8_t  hourOn);
void EEPROM_ScheduleMinuteOnUpdate(uint8_t NrOfSchedule, uint8_t  minuteOn);
void EEPROM_ScheduleHourOffUpdate(uint8_t NrOfSchedule, uint8_t  hourOff);
void EEPROM_ScheduleMinuteOffUpdate(uint8_t NrOfSchedule, uint8_t  minuteOff);



#endif /* INC_EEPROM_H_ */
