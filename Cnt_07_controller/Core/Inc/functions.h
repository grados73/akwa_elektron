/*
 * functions.h
 *
 *  Created on: Sep 15, 2021
 *      Author: grad_
 */



#ifndef INC_FUNCTIONS_H_
#define INC_FUNCTIONS_H_

#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "bmp280.h"


float Temp, Pressure;

struct Measurements{
	float Temp;
	float Pressure;
};

BMP280_t Bmp280;

void bmp280_init(void);
struct Measurements temp_pressure_measurement(void);

#endif /* INC_FUNCTIONS_H_ */
