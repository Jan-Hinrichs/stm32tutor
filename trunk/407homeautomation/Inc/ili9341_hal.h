#pragma once

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

#define LCD_GPIO		GPIOB
#define LCD_RST_PIN 		GPIO_PIN_14	// PB14 (RST)
#define LCD_A0_PIN		GPIO_PIN_11	// PB11 (DC)
#define	LCD_CSE_PIN		GPIO_PIN_12	// PB12 (CS)


#define ILI9341_ChipEnable() 	(HAL_GPIO_WritePin(LCD_GPIO, LCD_CSE_PIN, GPIO_PIN_RESET))
#define ILI9341_ChipDisable() 	(HAL_GPIO_WritePin(LCD_GPIO, LCD_CSE_PIN, GPIO_PIN_SET))
#define ILI9341_ChipReset() 	(HAL_GPIO_WritePin(LCD_GPIO, LCD_RST_PIN, GPIO_PIN_RESET))
#define ILI9341_ChipNoReset() 	(HAL_GPIO_WritePin(LCD_GPIO, LCD_RST_PIN, GPIO_PIN_SET))

#define ILI9341_OutCommand() 	(HAL_GPIO_WritePin(LCD_GPIO, LCD_A0_PIN, GPIO_PIN_RESET))
#define ILI9341_OutData() 	(HAL_GPIO_WritePin(LCD_GPIO, LCD_A0_PIN, GPIO_PIN_SET))

#define DelayMs(t)		(HAL_Delay(t))

void ILI9341_InitHal(void);
void ILI9341_SendByte(uint8_t dat);
