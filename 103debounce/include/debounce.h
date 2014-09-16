#pragma once

#include <stm32f10x.h>

typedef struct{
  GPIO_TypeDef* 	PORT;
  u32 				RCC_APB2Periph;

} DEBOUNCE_BusHandle;

typedef enum {
  DHT_ERROR									= 1,
  DEBOUNCE_NO_ERROR							= 0
} DEBOUNCE_ErrorLevel;


DEBOUNCE_ErrorLevel 	DEBOUNCE_Init(DEBOUNCE_BusHandle *bus);
void					DEBOUNCE_Handler(DEBOUNCE_BusHandle *bus);
