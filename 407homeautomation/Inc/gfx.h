#pragma once
#include <inttypes.h>

#define CENTER	-1
#define RIGHT	-2

void GFX_Init (uint16_t foregroundColor, uint16_t backgroundColor, uint8_t* font);
void GFX_PrintChar (uint8_t c, int x, int y);
void GFX_RotateChar (uint8_t c, int x, int y, int pos, int deg);
void GFX_Print (char *st, int x, int y, int deg);
void GFX_DrawPixel(int16_t x, int16_t y, uint16_t color);
void GFX_FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void GFX_FillScreen(uint16_t color) ;
void GFX_DrawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void GFX_DrawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
void GFX_DrawFastLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color) ;
void GFX_DrawRect(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2, uint16_t color);
void GFX_DrawCircle(int16_t x, int16_t y, int radius, uint16_t color);
void GFX_FillCircle(int16_t x, int16_t y, int radius, uint16_t color);
