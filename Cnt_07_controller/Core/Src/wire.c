/*
 * wire.c
 *
 *  Created on: Dec 13, 2021
 *      Author: grad_
 */


#include "wire.h"
#include "gpio.h"
#include "tim.h"
#include "usart.h"

static void set_baudrate(uint32_t baudrate)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = baudrate;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_HalfDuplex_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
}

HAL_StatusTypeDef wire_init(void)
{
  return HAL_OK;
}


HAL_StatusTypeDef wire_reset(void)
{
	  uint8_t data_out = 0xF0;
	  uint8_t data_in = 0;

	  set_baudrate(9600);
	  HAL_UART_Transmit(&huart1, &data_out, 1, HAL_MAX_DELAY);
	  HAL_UART_Receive(&huart1, &data_in, 1, HAL_MAX_DELAY);
	  set_baudrate(115200);

	  if (data_in != 0xF0)
	    return HAL_OK;
	  else
	    return HAL_ERROR;
}

static void write_bit(int value)
{
  if (value) {
      uint8_t data_out = 0xff;
    HAL_UART_Transmit(&huart1, &data_out, 1, HAL_MAX_DELAY);
  } else {
      uint8_t data_out = 0x0;
    HAL_UART_Transmit(&huart1, &data_out, 1, HAL_MAX_DELAY);
  }
}

static int read_bit(void)
{
  uint8_t data_out = 0xFF;
  uint8_t data_in = 0;
  HAL_UART_Transmit(&huart1, &data_out, 1, HAL_MAX_DELAY);
  HAL_UART_Receive(&huart1, &data_in, 1, HAL_MAX_DELAY);

  return data_in & 0x01;
}

void wire_write(uint8_t byte)
{
  int i;
  for (i = 0; i < 8; i++) {
    write_bit(byte & 0x01);
    byte >>= 1;
  }
}

uint8_t wire_read(void)
{
  uint8_t value = 0;
  int i;
  for (i = 0; i < 8; i++) {
    value >>= 1;
    if (read_bit())
      value |= 0x80;
  }
  return value;
}

static uint8_t byte_crc(uint8_t crc, uint8_t byte)
{
  int i;
  for (i = 0; i < 8; i++) {
    uint8_t b = crc ^ byte;
    crc >>= 1;
    if (b & 0x01)
      crc ^= 0x8c;
    byte >>= 1;
  }
  return crc;
}

uint8_t wire_crc(const uint8_t* data, int len)
{
  int i;
    uint8_t crc = 0;

    for (i = 0; i < len; i++)
      crc = byte_crc(crc, data[i]);

    return crc;
}
