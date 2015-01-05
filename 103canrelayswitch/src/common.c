/*
 * Util.c
 *
 *  Created on: 15.08.2014
 *      Author: klaus
 */

#include "stm32f10x.h"
#include "common.h"

extern uint32_t SystemCoreClock;

static __IO uint32_t uwTick;


AW void SysTick_Handler_Main (UU uint32_t ms){
	return;
}


// ----------------------------------------------------------------------------

uint32_t
COMMON_GetMsTick(void){
  return uwTick;
}

uint32_t
COMMON_GetUsTick(void)
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


void
COMMON_Init(void)
{
  SysTick_Config (SystemCoreClock / 1000);

  //if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk))
  {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  }
}



void
COMMON_DelayUs(uint32_t us) // microseconds
{
  uint32_t tickstart = COMMON_GetNativeTick();
  uint32_t wait = us * (SystemCoreClock/1000000);
  while ((COMMON_GetNativeTick() - tickstart) < wait);
}

void
COMMON_DelayQuarterUs(uint32_t quarterUs) // microseconds
{
  uint32_t tickstart = COMMON_GetNativeTick();
  uint32_t wait = quarterUs * (SystemCoreClock/4000000);
  while ((COMMON_GetNativeTick() - tickstart) < wait);
}

void
COMMON_DelayMs (uint32_t ms)
{
  uint32_t tickstart = 0;
  tickstart = COMMON_GetMsTick();
  while((COMMON_GetMsTick() - tickstart) < ms);
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval TRUE: pBuffer1 identical to pBuffer2
  *         FALSE: pBuffer1 differs from pBuffer2
  */
COMMON_bool COMMON_BufferEqual(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return COMMON_FALSE;
    }

    pBuffer1++;
    pBuffer2++;
  }

  return COMMON_TRUE;
}
