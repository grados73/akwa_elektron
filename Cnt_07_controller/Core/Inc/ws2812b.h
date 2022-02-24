/*
 * ws2812b.h
 *
 *  Created on: Dec 17, 2021
 *      Author: grad_
 */

#ifndef INC_WS2812B_H_
#define INC_WS2812B_H_

#include "stm32f4xx.h"

void ws2812b_init(void);

void ws2812b_SetColor(uint32_t led, uint8_t red, uint8_t green, uint8_t blue);

void ws2812b_update(void);

void ws2812b_wait(void);



#define WS2812B_BRIGHTNESS_MAX_PWR 255
#define WS2812B_BRIGHTNESS_MID_PWR 150
#define WS2812B_BRIGHTNESS_MIN_PWR 100

void ws2812b_LightTurnOff(void);
void ws2812b_LightWhite(uint8_t Brightness);
void ws2812b_LightBlue(uint8_t Brightness);
void ws2812b_LightDaily(uint8_t Brightness);
void ws2812b_LightNight(uint8_t Brightness);

#endif /* INC_WS2812B_H_ */
