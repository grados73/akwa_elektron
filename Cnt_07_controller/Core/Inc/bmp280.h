#ifndef INC_BMP280_H_
#define INC_BMP280_H_

#define BMP280_I2C_TIMEOUT	1000

// Temperature resolution
#define BMP280_TEMPERATURE_16BIT 1
#define BMP280_TEMPERATURE_17BIT 2
#define BMP280_TEMPERATURE_18BIT 3
#define BMP280_TEMPERATURE_19BIT 4
#define BMP280_TEMPERATURE_20BIT 5

// Pressure oversampling
#define BMP280_ULTRALOWPOWER	1
#define BMP280_LOWPOWER			2
#define BMP280_STANDARD			3
#define BMP280_HIGHRES			4
#define BMP280_ULTRAHIGHRES		5

// t_standby time
#define BME280_STANDBY_MS_0_5	0
#define BME280_STANDBY_MS_10	6
#define BME280_STANDBY_MS_20	7
#define BME280_STANDBY_MS_62_5	1
#define BME280_STANDBY_MS_125	2
#define BME280_STANDBY_MS_250 	3
#define BME280_STANDBY_MS_500	4
#define BME280_STANDBY_MS_1000	5

// IIR filter
#define BME280_FILTER_OFF	0
#define BME280_FILTER_X2 	1
#define BME280_FILTER_X4 	2
#define BME280_FILTER_X8	3
#define BME280_FILTER_X16 	4

// Mode
#define BMP280_SLEEPMODE		0
#define BMP280_FORCEDMODE		1
#define BMP280_NORMALMODE		3

//
//	Coeffs registers
//
#define	BMP280_DIG_T1		0x88
#define	BMP280_DIG_T2		0x8A
#define	BMP280_DIG_T3		0x8C

#define	BMP280_DIG_P1		0x8E
#define	BMP280_DIG_P2		0x90
#define	BMP280_DIG_P3		0x92
#define	BMP280_DIG_P4		0x94
#define	BMP280_DIG_P5		0x96
#define	BMP280_DIG_P6		0x98
#define	BMP280_DIG_P7		0x9A
#define	BMP280_DIG_P8		0x9C
#define	BMP280_DIG_P9		0x9E

//
//	Registers
//
#define	BMP280_CHIPID			0xD0
#define	BMP280_VERSION			0xD1
#define	BMP280_SOFTRESET		0xE0
#define	BMP280_CAL26			0xE1  // R calibration stored in 0xE1-0xF0
#define	BMP280_STATUS			0xF3
#define	BMP280_CONTROL			0xF4
#define	BMP280_CONFIG			0xF5
#define	BMP280_PRESSUREDATA		0xF7
#define	BMP280_TEMPDATA			0xFA

//
//	Control bits
//
#define	BMP280_MEASURING			(1<<3) // Conversion in progress

typedef struct
{
	I2C_HandleTypeDef 	*bmp_i2c;
	uint8_t				Address;

	// Coefficients for calculations
	int16_t t2, t3, p2, p3, p4, p5, p6, p7, p8, p9;
	uint16_t t1, p1;
	int32_t t_fine;

}BMP280_t;

uint8_t BMP280_Init(BMP280_t *bmp, I2C_HandleTypeDef *i2c, uint8_t Address);

void BMP280_SetMode(BMP280_t *bmp, uint8_t Mode);
void BMP280_SetTemperatureOversampling(BMP280_t *bmp, uint8_t TOversampling);
void BMP280_SetPressureOversampling(BMP280_t *bmp, uint8_t POversampling);

float BMP280_ReadTemperature(BMP280_t *bmp);
uint8_t BMP280_ReadPressureAndTemperature(BMP280_t *bmp, float *Pressure, float *Temperature);

#endif /* INC_BMP280_H_ */
