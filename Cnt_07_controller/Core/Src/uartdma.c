/**
  ******************************************************************************
  * @file           : TFT_ILI9341.c
  * @project		: free_michelle
  * @author			: grados73 - https://github.com/grados73
  * @purpose		: plik z funkcjami do obslugi UART przez DMA.
  ******************************************************************************
  **/

#include "main.h"
#include "uartdma.h"

//
// DMA stuff from HAL libs
//
typedef struct
{
	__IO uint32_t ISR;   // DMA interrupt status register
	__IO uint32_t Reserved0;
	__IO uint32_t IFCR;  // DMA interrupt flag clear register
} DMA_Base_Registers;

//
//	UART IDLE interrupt handler
//		Put into USARTx_IRQHandler in user code section
//
void UARTDMA_UartIrqHandler(UARTDMA_HandleTypeDef *huartdma)
{
	if(huartdma->huart->Instance->SR & UART_FLAG_IDLE)       // Check if Idle flag is set
	{
		volatile uint32_t tmp;
		tmp = huartdma->huart->Instance->SR;                      // Read status register
		tmp = huartdma->huart->Instance->DR;                      // Read data register

		huartdma->huart->hdmarx->Instance->CR &= ~DMA_SxCR_EN; // Disable DMA - it will force Transfer Complete interrupt if it's enabled

		tmp = tmp; // For unused warning
	}
}

//
//	DMA Receive interrupt handler
//		Put into DMA Stream Rx IRQHandler in first user code section. End with return;
//
void UARTDMA_DmaReceiveIrqHandler(UARTDMA_HandleTypeDef *huartdma)
{
	uint8_t *DmaBufferPointer;
	uint16_t i;
	uint16_t Length;	// Message length

	DMA_Base_Registers *DmaRegisters = (DMA_Base_Registers *) huartdma->huart->hdmarx->StreamBaseAddress; // Take registers base address

	if (__HAL_DMA_GET_IT_SOURCE(huartdma->huart->hdmarx, DMA_IT_TC) != RESET) // Check if interrupt source is Transfer Complete
	{
		DmaRegisters->IFCR = DMA_FLAG_TCIF0_4 << huartdma->huart->hdmarx->StreamIndex;	// Clear Transfer Complete flag

		Length = DMA_RX_BUFFER_SIZE - huartdma->huart->hdmarx->Instance->NDTR; // Get the Length of transfered data

		DmaBufferPointer = 	huartdma->DMA_RX_Buffer;

		for(i = 0; i < Length; i++) // Write all bytes into Ring Buffer
		{
			RB_Write(&huartdma->UART_RX_Buffer, DmaBufferPointer[i]);

			if((DmaBufferPointer[i] == '\n'))
			{
				huartdma->UartRxBufferLines++;
			}
		}

		DmaRegisters->IFCR = 0x3FU << huartdma->huart->hdmarx->StreamIndex; 		// Clear all interrupts
		huartdma->huart->hdmarx->Instance->M0AR = (uint32_t) huartdma->DMA_RX_Buffer; // Set memory address for DMA again
		huartdma->huart->hdmarx->Instance->NDTR = DMA_RX_BUFFER_SIZE; // Set number of bytes to receive
		huartdma->huart->hdmarx->Instance->CR |= DMA_SxCR_EN;            	// Start DMA transfer
	}
}

//
//	Put one char to UART Transmit buffer
//
int UARTDMA_PutCharToTxBuffer(UARTDMA_HandleTypeDef *huartdma, char c)
{

	if(RB_OK != RB_Write(&huartdma->UART_TX_Buffer, c)) // Check if put to Ring Buffer ended with success
	{
		return 1; // Error code
	}
	return 0; // Success code
}

//
// Get complete line (end with \n) from UART buffer
//
uint8_t UARTDMA_GetLineFromReceiveBuffer(UARTDMA_HandleTypeDef *huartdma, char *OutBuffer)
{
	char *OurBufferPtr; // Helper pointer

	OurBufferPtr = OutBuffer; // Set helper pointer
	if(huartdma->UartRxBufferLines) // If there id something to read
	{
		while(RB_OK == RB_Read(&huartdma->UART_RX_Buffer, (uint8_t*)OurBufferPtr)) // Get from Ring Buffer till end
		{
			if(*OurBufferPtr == '\n') // If end line byte hit
			{
				*OurBufferPtr = 0; // Change it to end cstring '\0' byte
				huartdma->UartRxBufferLines--; // Decrease received lines counter
				return 0; // Exit if end line
			}

			OurBufferPtr++; // Increase pointer
		}
		return 0; // Return o error (but no full line... place for improovement)
	}
	return 1; // Return an error
}

//
// Put message to UART buffer. It will be send with UARTDMA_TransmitEvent
//
void UARTDMA_Print(UARTDMA_HandleTypeDef *huartdma, char *Message)
{
	char *MsgPointer; // Helper pointer
	char CharToPut; // Current char to print
	MsgPointer = (char*)Message; // Set helper pointer

	while((CharToPut = *MsgPointer)) // Go through whole Message - till '\0' byte
	{
		UARTDMA_PutCharToTxBuffer(huartdma, CharToPut); // Put current char into transmit buffer
		MsgPointer++; // Next char

		if(CharToPut == '\n') // Check if end line byte occurs
		{
			huartdma->UartTxBufferLines++; // Increment line to transmit counter
		}
	}
}
//
// Check if received data are ready
//
uint8_t UARTDMA_IsDataReceivedReady(UARTDMA_HandleTypeDef *huartdma)
{
	if(huartdma->UartRxBufferLines)
		return 1; // At least one line is available
	else
		return 0; // No lines are available
}

//
// Check if data for transfer are ready
//
uint8_t UARTDMA_IsDataTransferReady(UARTDMA_HandleTypeDef *huartdma)
{
	if(huartdma->UartTxBufferLines)
		return 1; // At least one line is available
	else
		return 0; // No lines are available
}

//
//	UART Transmit interrupt handler
//		Put in main loop
//
void UARTDMA_TransmitEvent(UARTDMA_HandleTypeDef *huartdma)
{
	char CharToSend; // Current char to transmit
	uint16_t i = 0; // Iterator

	if(huartdma->huart->hdmatx->State != HAL_DMA_STATE_BUSY) // If DMA is ready to transmit
	{
		while(RB_OK == RB_Read(&huartdma->UART_TX_Buffer, (uint8_t*)&CharToSend)) // If there is something to transfer
		{
			if(CharToSend == '\n') // Check end line byte
			{
				huartdma->UartTxBufferLines--; // Decrease lines (may be delete because it is no more used)
			}

			huartdma->DMA_TX_Buffer[i++] = CharToSend; // Put this char into DMA buffer
		}

		HAL_UART_Transmit_DMA(huartdma->huart, huartdma->DMA_TX_Buffer, i); // Push DMA buffer to UART
	}
}



//
// Initialization
//
void UARTDMA_Init(UARTDMA_HandleTypeDef *huartdma, UART_HandleTypeDef *huart)
{
	huartdma->huart = huart;

	// IDLE Enable
	__HAL_UART_ENABLE_IT(huartdma->huart, UART_IT_IDLE);   	// UART Idle Line interrupt

	// DMA TC Enable
	__HAL_DMA_ENABLE_IT(huartdma->huart->hdmarx, DMA_IT_TC); // UART DMA Transfer Complete interrupt
	__HAL_DMA_ENABLE_IT(huartdma->huart->hdmatx, DMA_IT_TC); // UART DMA Transfer Complete interrupt

	// Run DMA UART on Buffer RX

	HAL_UART_Receive_DMA(huartdma->huart, huartdma->DMA_RX_Buffer, DMA_RX_BUFFER_SIZE); // Run DMA for whole DMA buffer

	// DMA HT Disable
	__HAL_DMA_DISABLE_IT(huartdma->huart->hdmarx, DMA_IT_HT); // UART DMA Half Transfer Complete interrupt
	__HAL_DMA_DISABLE_IT(huartdma->huart->hdmatx, DMA_IT_HT); // UART DMA Half Transfer Complete interrupt
}
