/*
 * functions.c
 *
 *  Created on: Sep 15, 2021
 *      Author: grad_
 */
#include "functions.h"


#define BMP280_ADDRESS 0x76
struct Measurements BMPResults;


void bmp280_init(void)
{
	BMP280_Init(&Bmp280, &hi2c1, BMP280_ADDRESS);
}
struct Measurements temp_pressure_measurement(void)
{
	BMP280_ReadPressureAndTemperature(&Bmp280, &Pressure, &Temp);
	BMPResults.Pressure = Pressure;
	BMPResults.Temp = Temp;

	return BMPResults;

}



