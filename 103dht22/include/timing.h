/*
 * Util.h
 *
 *  Created on: 15.08.2014
 *      Author: klaus
 */

#pragma once

#include <inttypes.h>


void TIMING_Init(void);
void TIMING_DelayUs(uint32_t us); // microseconds
void TIMING_DelayMs(uint32_t ms); // microseconds
void TIMING_DelayQuarterUs(uint32_t quarterUs); //"Ticks"-Delay for SingleWire
uint32_t TIMING_GetMsTick(void); // microseconds
uint32_t TIMING_GetUsTick(void); // microseconds

#define TIMING_GetNativeTick(void)	(DWT->CYCCNT)
