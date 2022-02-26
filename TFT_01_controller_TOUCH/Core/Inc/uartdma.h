/*
 * uartdma.h
 *
 */

#ifndef INC_UARTDMA_H_
#define INC_UARTDMA_H_

#include "ring_buffer.h"


#define DMA_RX_BUFFER_SIZE          64
#define DMA_TX_BUFFER_SIZE          128

typedef struct
{
	UART_HandleTypeDef* huart;					// UART handler

	//
	//	Receive buffer stuff
	//
	uint8_t DMA_RX_Buffer[DMA_RX_BUFFER_SIZE]; // Buffer only for DMA usage
	RingBuffer UART_RX_Buffer;  // Buffer to use for receiving
	uint8_t  UartRxBufferLines; // Lines end with '\n'

	//
	//	Transmit buffer stuff
	//
	uint8_t DMA_TX_Buffer[DMA_TX_BUFFER_SIZE]; // Buffer only for DMA usage
	RingBuffer UART_TX_Buffer; // Buffer to use for transmitting
	uint8_t  UartTxBufferLines; // Lines end with '\n'

}UARTDMA_HandleTypeDef;


void UARTDMA_Init(UARTDMA_HandleTypeDef *huartdma, UART_HandleTypeDef *huart); // Pass in *huart used UART

void UARTDMA_UartIrqHandler(UARTDMA_HandleTypeDef *huartdma); // Put in DMAx_Streamx_IRQHandler for receiver
void UARTDMA_DmaReceiveIrqHandler(UARTDMA_HandleTypeDef *huartdma); // Put in USARTx_IRQHandler

void UARTDMA_Print(UARTDMA_HandleTypeDef *huartdma, char *Message); // Printing to UART

void UARTDMA_TransmitEvent(UARTDMA_HandleTypeDef *huartdma); // Put this in main loop

uint8_t UARTDMA_IsDataReceivedReady(UARTDMA_HandleTypeDef *huartdma);
uint8_t UARTDMA_GetLineFromReceiveBuffer(UARTDMA_HandleTypeDef *huartdma, char *OutPuffer);

#endif /* INC_UARTDMA_H_ */
