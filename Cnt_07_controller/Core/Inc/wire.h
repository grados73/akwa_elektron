/*
 * wire.h
 *
 *  Created on: Dec 13, 2021
 *      Author: grad_
 */

#ifndef INC_WIRE_H_
#define INC_WIRE_H_

#include "stm32f4xx.h"

// Inicjalizacja 1-wire
// Start licznika używanego do opóźnień
// return - HAL_OK/HAL_ERROR zależnie od stanu licznika
HAL_StatusTypeDef wire_init(void);

// Przesłanie sekwencji reset przez 1-wire
// return - HAL_OK/HAL_ERROR zależnie od odp. czujnika
HAL_StatusTypeDef wire_reset(void);

// Odczytanie danych przez 1-wire
// Czyta 8 bitów i połącz w bajt
// return - odczytany bajt
uint8_t wire_read(void);

// Wyślij dane przez 1-wire
// byte - bajt do wysłania
void wire_write(uint8_t byte);

// Policz sumę kontrolną
// data - blok danych
// len - długość bloku danych
uint8_t wire_crc(const uint8_t* data, int len);

#endif /* INC_WIRE_H_ */
