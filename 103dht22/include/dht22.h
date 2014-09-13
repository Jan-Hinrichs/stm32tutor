#pragma once

#include <stm32f10x.h>

typedef struct{
  GPIO_TypeDef* 	PORT;
  u32 				RCC_APB2Periph;
  u16 				GPIO_Pin;
  //IRQn_Type 		EXTIx_IRQn;
  //u8 				GPIO_PortSource;
  //u8 				GPIO_PinSource;
  //u32 				EXTI_Line;
} DHT_BusHandle;

typedef enum {
  DHT_ERROR_NO_ERROR_SET                   	= 0,
  DHT_ERROR_NO_DEVICES_ON_NET              	= 1,
  DHT_ERROR_CRC								= 2,
  DHT_NO_ERROR_BUT_CONTINUE_CALLING_ME		= 3
} DHT_ErrorLevel;


DHT_ErrorLevel 	DHT_Init_Sync(DHT_BusHandle *bus);
DHT_ErrorLevel 	DHT_Start_Sync(DHT_BusHandle *bus);

float 	DHT_GetTemp(u8 index);
float 	DHT_GetHumidity(u8 index);
