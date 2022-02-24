/**
  ******************************************************************************
  * @file           : bmp280.c
  * @project		: free_michelle
  * @author			: grados73 - https://github.com/grados73
  * @purpose		: plik z funkcjami do odczytu cisnienia i temperatury po I2C z czujnika BMP280
  ******************************************************************************
  **/

#include "main.h"
#include "bmp280.h"

//
// Read 8 bits from BMP280 from Register
//
uint8_t Read8(BMP280_t *bmp, uint8_t Register)
{
	uint8_t Value;

	HAL_I2C_Mem_Read(bmp->bmp_i2c, ((bmp->Address)<<1), Register, 1, &Value, 1, BMP280_I2C_TIMEOUT);

	return Value;
}

//
// Write 8 bits to BMP280 to Register
//
void Write8(BMP280_t *bmp, uint8_t Register, uint8_t Value)
{
	HAL_I2C_Mem_Write(bmp->bmp_i2c, ((bmp->Address)<<1), Register, 1, &Value, 1, BMP280_I2C_TIMEOUT);
}

//
// Read 16 bits from BMP280 from Register
//
uint16_t Read16(BMP280_t *bmp, uint8_t Register)
{
	uint8_t Value[2];

	HAL_I2C_Mem_Read(bmp->bmp_i2c, ((bmp->Address)<<1), Register, 1, Value, 2, BMP280_I2C_TIMEOUT);

	return ((Value[1] << 8) | Value[0]);
}

//
// Read 24 bits from BMP280 from Register
//
uint32_t Read24(BMP280_t *bmp, uint8_t Register)
{
	uint8_t Value[3];

	HAL_I2C_Mem_Read(bmp->bmp_i2c, ((bmp->Address)<<1), Register, 1, Value, 3, BMP280_I2C_TIMEOUT);

	return ((Value[0] << 16) | (Value[1] << 8) | Value[2]);
}

//
// Set mode - Forced or Normal - in Control register
//
void BMP280_SetMode(BMP280_t *bmp, uint8_t Mode)
{
	uint8_t Tmp;

	if(Mode > 3) Mode = 3;

	Tmp = Read8(bmp, BMP280_CONTROL);

	Tmp = Tmp & 0xFC; // Tmp (xxxx xx00)
	Tmp |= Mode & 0x03;

	Write8(bmp, BMP280_CONTROL, Tmp);
}

//
// Set pressure oversampling in Control register
//
void BMP280_SetPressureOversampling(BMP280_t *bmp, uint8_t POversampling)
{
	uint8_t Tmp;

	if(POversampling > 5) POversampling = 5;

	Tmp = Read8(bmp, BMP280_CONTROL);

	Tmp = Tmp & 0xE3; // Tmp (xxx0 00xx)
	Tmp |= ((POversampling << 2) & 0x1C);  // (0001 1100)

	Write8(bmp, BMP280_CONTROL, Tmp);
}

//
// Set temperature oversampling in Control register
//
void BMP280_SetTemperatureOversampling(BMP280_t *bmp, uint8_t TOversampling)
{
	uint8_t Tmp;

	if(TOversampling > 5) TOversampling = 5;

	Tmp = Read8(bmp, BMP280_CONTROL);

	Tmp = Tmp & 0x1F; // Tmp (000x xxxx)
	Tmp |= ((TOversampling << 5) & 0xE0) ;  // (1110 0000)

	Write8(bmp, BMP280_CONTROL, Tmp);
}

//
// Read Raw temperature data from BMP280
//
int32_t BMP280_ReadTemperatureRaw(BMP280_t *bmp)
{
	int32_t Tmp;

	Tmp = (int32_t)Read24(bmp, BMP280_TEMPDATA);

	Tmp >>= 4; // Move 4 left due to such storing (Datasheet).

	return Tmp;
}

//
// Read Raw pressure data from BMP280
//
int32_t BMP280_ReadPressureRaw(BMP280_t *bmp)
{
	int32_t Tmp;

	Tmp = (int32_t)Read24(bmp, BMP280_PRESSUREDATA);

	Tmp >>= 4; // Move 4 left due to such storing (Datasheet).

	return Tmp;
}

//
// Read and calculate temperature
// Whole procedure is taken from BMP280 Datasheet
//
float BMP280_ReadTemperature(BMP280_t *bmp)
{
	int32_t var1, var2, T;

	int32_t adc_T;

	adc_T = BMP280_ReadTemperatureRaw(bmp);

	var1 = ((((adc_T>>3) - ((int32_t)(bmp->t1)<<1))) * ((int32_t)(bmp->t2))) >> 11;

	var2 = (((((adc_T>>4) - ((int32_t)(bmp->t1))) * ((adc_T>>4) - ((int32_t)(bmp->t1)))) >> 12) *
	((int32_t)(bmp->t3))) >> 14;

	bmp->t_fine = var1 + var2;

	T = ((bmp->t_fine) * 5 + 128) >> 8;

	return (float)(T/100.0);
}

//
// Read and calculate temperature & pressure
// Whole procedure is taken from BMP280 Datasheet
//
uint8_t BMP280_ReadPressureAndTemperature(BMP280_t *bmp, float *Pressure, float *Temperature)
{
	// Have to read temperature first
	*Temperature = BMP280_ReadTemperature(bmp);

	int32_t var1, var2;
	uint32_t p;

	var1 = (((int32_t)(bmp->t_fine))>>1) - (int32_t)64000;
	var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((int32_t)(bmp->p6));
	var2 = var2 + ((var1*((int32_t)(bmp->p5)))<<1);
	var2 = (var2>>2)+(((int32_t)(bmp->p4))<<16);
	var1 = ((((bmp->p3) * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((int32_t)(bmp->p2)) * var1)>>1))>>18;
	var1 =((((32768+var1))*((int32_t)(bmp->p1)))>>15);

	if (var1 == 0)
	{
		return 1; // avoid exception caused by division by zero
	}

	// Raw pressure from BMP's ADC
	int32_t adc_P = BMP280_ReadPressureRaw(bmp);

	p = (((int32_t)(((int32_t)1048576)-adc_P)-(var2>>12)))*3125;
	if (p < 0x80000000)
	{
		p = (p << 1) / ((uint32_t)var1);
	}
	else
	{
		p = (p / (uint32_t)var1) * 2;
	}
	var1 = (((int32_t)(bmp->p9)) * ((int32_t)(((p>>3) * (p>>3))>>13)))>>12;
	var2 = (((int32_t)(p>>2)) * ((int32_t)(bmp->p8)))>>13;

	p = (uint32_t)((int32_t)p + ((var1 + var2 + (bmp->p7)) >> 4));

	// Convert Pa to hPa
	*Pressure = (float)(p/100.0);

	return 0;
}

//
// Init
//
uint8_t BMP280_Init(BMP280_t *bmp, I2C_HandleTypeDef *i2c, uint8_t Address)
{
	uint8_t ChipID;

	// Save I2C handler and address
	bmp->bmp_i2c = i2c;
	bmp->Address = Address;

	//Read Chip ID
	ChipID = Read8(bmp, BMP280_CHIPID);

	// Check if ID is correct
	if(ChipID != 0x58)
	{
		return 1;
	}

	// Read all coefficients
	bmp->t1 = Read16(bmp, BMP280_DIG_T1);
	bmp->t2 = Read16(bmp, BMP280_DIG_T2);
	bmp->t3 = Read16(bmp, BMP280_DIG_T3);

	bmp->p1 = Read16(bmp, BMP280_DIG_P1);
	bmp->p2 = Read16(bmp, BMP280_DIG_P2);
	bmp->p3 = Read16(bmp, BMP280_DIG_P3);
	bmp->p4 = Read16(bmp, BMP280_DIG_P4);
	bmp->p5 = Read16(bmp, BMP280_DIG_P5);
	bmp->p6 = Read16(bmp, BMP280_DIG_P6);
	bmp->p7 = Read16(bmp, BMP280_DIG_P7);
	bmp->p8 = Read16(bmp, BMP280_DIG_P8);
	bmp->p9 = Read16(bmp, BMP280_DIG_P9);

	// Set base settings
	BMP280_SetTemperatureOversampling(bmp, BMP280_TEMPERATURE_20BIT);
	BMP280_SetPressureOversampling(bmp, BMP280_ULTRAHIGHRES);
	BMP280_SetMode(bmp, BMP280_NORMALMODE);

	return 0;
}
