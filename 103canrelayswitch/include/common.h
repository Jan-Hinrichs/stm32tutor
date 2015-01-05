//Muss f�r alle STM32-Mitglieder unver�ndert funktionieren

#pragma once


#include <inttypes.h>

#define UU __attribute__((unused))
#define AW __attribute__((weak))

typedef enum {COMMON_FALSE = 0, COMMON_TRUE = !COMMON_FALSE} COMMON_bool;


#define countof(a) (sizeof(a) / sizeof(*(a)))

void COMMON_Init(void);
void  COMMON_DelayUs(uint32_t us); // microseconds
void  COMMON_DelayMs(uint32_t ms); // microseconds
void  COMMON_DelayQuarterUs(uint32_t quarterUs); //"Ticks"-Delay for SingleWire
uint32_t  COMMON_GetMsTick(void); // microseconds
uint32_t  COMMON_GetUsTick(void); // microseconds

COMMON_bool COMMON_BufferEqual(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength);

#define  COMMON_GetNativeTick(void)	(DWT->CYCCNT)
