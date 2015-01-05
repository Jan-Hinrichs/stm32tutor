#pragma once
#include <inttypes.h>

/**
 * Public enum
 *
 * Select orientation for LCD
 */
typedef enum {
	LCD_Orientation_Portrait=0,
	LCD_Orientation_Portrait_Flipped=1,
	LCD_Orientation_Landscape=2,
	LCD_Orientation_Landscape_Flipped=3
} LCD_Orientation_t;

void LCD_SetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void LCD_PushColor(uint16_t color);
void LCD_SetOrientation(LCD_Orientation_t orientation);

uint16_t LCD_GetWidth(void);
uint16_t LCD_GetHeight(void);
