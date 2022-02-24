/*
 * ring_buffer.h
 *
 *  Created on: Aug 21, 2020
 *      Author: Mateusz Salamon
 */

#ifndef INC_RING_BUFFER_H_
#define INC_RING_BUFFER_H_

#define RING_BUFFER_SIZE 128

//
// Return status from Ring Buffer functions
//
typedef enum
{
	RB_OK = 0,
	RB_ERROR = 1
}RB_Status;

//
// Simple Ring Buffer
//
typedef struct
{
	uint8_t Head; // Head pointer
	uint8_t Tail; // Tail pointer
	uint8_t Buffer[RING_BUFFER_SIZE]; // Working buffer
}RingBuffer;

RB_Status RB_Read(RingBuffer *rb, uint8_t *Value); // Reading function
RB_Status RB_Write(RingBuffer *rb, uint8_t Value); // Writing function

#endif /* INC_RING_BUFFER_H_ */
