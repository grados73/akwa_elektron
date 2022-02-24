/*
 * ws2812b.c
 *
 *  Created on: Dec 17, 2021
 *      Author: grad_
 */
#include "ws2812b.h"
#include "tim.h"

#define BIT_0_TIME		40
#define BIT_1_TIME		80

#define RESET_LEN		50
#define LED_N			16
#define LED_N_MAX		100

uint8_t NumberOfLedsWS2812b = LED_N;

const uint8_t gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

static uint16_t led_buffer[RESET_LEN + 24 * LED_N_MAX + 1];

static void set_byte(uint32_t pos, uint8_t value)
{
  int i;
  for (i = 0; i < 8; i++) {
    if (value & 0x80) {
      led_buffer[pos + i] = BIT_1_TIME;
    } else {
      led_buffer[pos + i] = BIT_0_TIME;
    }

    value <<= 1;
  }
}

void ws2812b_init(void)
{
  int i;
  for (i = 0; i < RESET_LEN; i++)
    led_buffer[i] = 0;

  for (i = 0; i < 24 * NumberOfLedsWS2812b; i++)
    led_buffer[RESET_LEN + i] = BIT_0_TIME;

  led_buffer[RESET_LEN + 24 * NumberOfLedsWS2812b] = 100;

  HAL_TIM_Base_Start(&htim1);
  ws2812b_update();
}

void ws2812b_update(void)
{
	HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_2, (uint32_t*)led_buffer, sizeof(led_buffer));
//	HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_2, (uint32_t*)led_buffer, sizeof(led_buffer));
	//HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t*)led_buffer, sizeof(led_buffer));
}

void ws2812b_wait(void)
{
	while (HAL_TIM_GetChannelState(&htim1, TIM_CHANNEL_2) == HAL_TIM_CHANNEL_STATE_BUSY)
		{}
}


void ws2812b_SetColor(uint32_t led, uint8_t red, uint8_t green, uint8_t blue)
{
	if (led < NumberOfLedsWS2812b)
	{
		set_byte(RESET_LEN + 24 * led, gamma8[green % 256]);
		set_byte(RESET_LEN + 24 * led + 8, gamma8[red % 256]);
		set_byte(RESET_LEN + 24 * led + 16, gamma8[blue % 256]);
	}
}

//
//
//
void ws2812b_LightTurnOff(void)
{
	uint8_t r = 0;	// RED
	uint8_t g = 0;	// GREEN
	uint8_t b = 0;	// BLUE

	for (uint32_t led = 0; led < NumberOfLedsWS2812b; led++)
	{
		ws2812b_SetColor(led, r, g, b);
	}
	ws2812b_update();
}

//
// Turn On Lights with WHITE Color
//
void ws2812b_LightWhite(uint8_t Brightness)
{
	uint8_t r = 0;	// RED
	uint8_t g = 0;	// GREEN
	uint8_t b = 0;	// BLUE

	r = g = b = Brightness;

	for (uint32_t led = 0; led < NumberOfLedsWS2812b; led++)
	{
		ws2812b_SetColor(led, r, g, b);
	}
		ws2812b_update();
}

//
// Turn On Lights with Blue Color
//
void ws2812b_LightBlue(uint8_t Brightness)
{
	uint8_t r = 0;	// RED
	uint8_t g = 0;	// GREEN
	uint8_t b = Brightness;	// BLUE

	for (uint32_t led = 0; led < NumberOfLedsWS2812b; led++)
	{
		ws2812b_SetColor(led, r, g, b);
	}
		ws2812b_update();
}

//
// Turn On Lights with Warm Daily Yellow Color
//
void ws2812b_LightDaily(uint8_t Brightness)
{
	uint8_t r = ((220*Brightness)/255);
	uint8_t g = ((200*Brightness)/255);
	uint8_t b = ((100*Brightness)/255);

	for (uint32_t led = 0; led < NumberOfLedsWS2812b; led++)
	{
		ws2812b_SetColor(led, r, g, b);
	}
		ws2812b_update();
}

//
// Turn On Lights with Night Lights
//
void ws2812b_LightNight(uint8_t Brightness)
{
	uint8_t r = 0;
	uint8_t g = ((150*Brightness)/255);
	uint8_t b = ((250*Brightness)/255);

	for (uint32_t led = 0; led < NumberOfLedsWS2812b; led++)
	{
		ws2812b_SetColor(led, r, g, b);
	}
		ws2812b_update();
}



