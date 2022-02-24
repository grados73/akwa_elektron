/**
  ******************************************************************************
  * @file           : ring_buffer.c
  * @project		: free_michelle
  * @author			: grados73 - https://github.com/grados73
  * @purpose		: bufor kolowy do obslugi UARTA zrobiony na podstawie pomyslu msalomon
  ******************************************************************************
  **/

#include "main.h"
#include "ring_buffer.h"

RB_Status RB_Read(RingBuffer *rb, uint8_t *Value)
{
	if(rb->Head == rb->Tail) // Check if there is something to read (Tail hits Head pointer)
	{
		return RB_ERROR; // Nothing to read
	}

	*Value = rb->Buffer[rb->Tail];  // Write byte to variable from Value pointer

	rb->Tail = (rb->Tail + 1) % RING_BUFFER_SIZE; // New Tail pointer value

	return RB_OK;	// Return Success
}


RB_Status RB_Write(RingBuffer *rb, uint8_t Value)
{
	uint8_t TmpHead = (rb->Head + 1) % RING_BUFFER_SIZE; // Temp Head pointer for free space check

	if(TmpHead == rb->Tail) // Check if there is one free space in front of Had pointer
	{
		return RB_ERROR; // No free space error
	}

	rb->Buffer[rb->Head] = Value; // Write new byte in Head position
	rb->Head = TmpHead; 	// New Head pointer value

	return RB_OK;	// Return Success
}

