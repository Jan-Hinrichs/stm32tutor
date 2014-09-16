/*
 * Util.c
 *
 *  Created on: 15.08.2014
 *      Author: klaus
 */

#include "stm32f10x.h"
#include "timing.h"
extern uint32_t SystemCoreClock;

static __IO uint32_t uwTick;


void SysTick_Handler_Main (u32 ms);

// ----------------------------------------------------------------------------

uint32_t
TIMING_GetMsTick(void){
  return uwTick;
}

uint32_t
TIMING_GetUsTick(void)
{
  return (DWT->CYCCNT)/(SystemCoreClock/1000000);
}



// ----- SysTick_Handler() ----------------------------------------------------

void
SysTick_Handler (void)
{
  uwTick++;
  SysTick_Handler_Main(uwTick);
}


void TIMING_Init(void)
{
  SysTick_Config (SystemCoreClock / 1000);

  //if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk))
  {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  }
}



void TIMING_DelayUs(uint32_t us) // microseconds
{
  uint32_t tickstart = TIMING_GetNativeTick();
  uint32_t wait = us * (SystemCoreClock/1000000);
  while ((TIMING_GetNativeTick() - tickstart) < wait);
}

void TIMING_DelayQuarterUs(uint32_t quarterUs) // microseconds
{
  uint32_t tickstart = TIMING_GetNativeTick();
  uint32_t wait = quarterUs * (SystemCoreClock/4000000);
  while ((TIMING_GetNativeTick() - tickstart) < wait);
}

void TIMING_DelayMs (uint32_t ms)
{
  uint32_t tickstart = 0;
  tickstart = TIMING_GetMsTick();
  while((TIMING_GetMsTick() - tickstart) < ms);
}
