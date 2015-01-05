
#include "lcdBaseGFX.h"
#include "gfx.h"
#include "DefaultFonts.h"
#include "math.h"
#include "string.h"

static uint8_t *_font;
static uint16_t _fg;
static uint16_t _bg;
static uint8_t _transparent = 0;

void GFX_Init(uint16_t foregroundColor, uint16_t backgroundColor, uint8_t* font)
{
  _font = font;
  _fg = foregroundColor;
  _bg = backgroundColor;

}

void
GFX_PrintChar (uint8_t c, int x, int y)
{
  uint8_t i, ch, fz;
  uint16_t j;
  uint16_t temp;
  int zz;

  if (FONT_XSIZE < 8)
    fz = FONT_XSIZE;
  else
    fz = FONT_XSIZE / 8;
  if (!_transparent)
    {
      LCD_SetAddrWindow (x, y, x + FONT_XSIZE - 1, y + FONT_YSIZE - 1);

      temp = ((c - FONT_OFFSET) * ((fz) * FONT_YSIZE)) + 4;
      for (j = 0; j < ((fz) * FONT_YSIZE); j++)
	{
	  ch = _font[temp];
	  for (i = 0; i < 8; i++)
	    {
	      if ((ch & (1 << (7 - i))) != 0)
		{
		  LCD_PushColor (_fg);
		}
	      else
		{
		  LCD_PushColor (_bg);
		}
	    }
	  temp++;
	}
    }
  else
    {
      temp = ((c - FONT_OFFSET) * ((fz) * FONT_YSIZE)) + 4;
      for (j = 0; j < FONT_YSIZE; j++)
	{
	  for (zz = 0; zz < (fz); zz++)
	    {
	      ch = _font[temp + zz];
	      for (i = 0; i < 8; i++)
		{
		  LCD_SetAddrWindow (x + i + (zz * 8), y + j,
				     x + i + (zz * 8) + 1, y + j + 1);

		  if ((ch & (1 << (7 - i))) != 0)
		    {
		      LCD_PushColor (_fg);
		    }
		}
	    }
	  temp += (fz);
	}
    }
}

void
GFX_RotateChar (uint8_t c, int x, int y, int pos, int deg)
{
  uint8_t i, j, ch, fz;
  uint16_t temp;
  int newx, newy;
  double radian = deg * 0.0175;
  int zz;

  if (FONT_XSIZE < 8)
    fz = FONT_XSIZE;
  else
    fz = FONT_XSIZE / 8;
  temp = ((c - FONT_OFFSET) * ((fz) * FONT_YSIZE)) + 4;
  for (j = 0; j < FONT_YSIZE; j++)
    {
      for (zz = 0; zz < (fz); zz++)
	{
	  ch = _font[temp + zz];
	  for (i = 0; i < 8; i++)
	    {
	      newx = x
		  + (((i + (zz * 8) + (pos * FONT_XSIZE)) * cos (radian))
		      - ((j) * sin (radian)));
	      newy = y
		  + (((j) * cos (radian))
		      + ((i + (zz * 8) + (pos * FONT_XSIZE)) * sin (radian)));

	      LCD_SetAddrWindow (newx, newy, newx + 1, newy + 1);

	      if ((ch & (1 << (7 - i))) != 0)
		{
		  LCD_PushColor (_fg);
		}
	      else
		{
		  if (!_transparent)
		    LCD_PushColor (_bg);
		}
	    }
	}
      temp += (fz);
    }
}

void
GFX_Print (char *st, int x, int y, int deg)
{
  int stl, i;

  stl = strlen (st);

  if (x == RIGHT)
    x = (LCD_GetWidth () + 1) - (stl * FONT_XSIZE);
  if (x == CENTER)
    x = ((LCD_GetHeight () + 1) - (stl * FONT_XSIZE)) / 2;

  for (i = 0; i < stl; i++)
    if (deg == 0)
      GFX_PrintChar (*st++, x + (i * (FONT_XSIZE)), y);
    else
      GFX_RotateChar (*st++, x, y, i, deg);
}

// draw color pixel on screen
void
GFX_DrawPixel (int16_t x, int16_t y, uint16_t color)
{

  if ((x < 0) || (x >= LCD_GetWidth ()) || (y < 0) || (y >= LCD_GetHeight ()))
    return;

  LCD_SetAddrWindow ((uint16_t)x, (uint16_t)y, (uint16_t)(x + 1), (uint16_t)(y + 1));
  LCD_PushColor (color);
}

// fill a rectangle
void
GFX_FillRect (int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  // rudimentary clipping (drawChar w/big text requires this)
  if ((x >= LCD_GetWidth ()) || (y >= LCD_GetHeight ()))
    return;
  if ((x + w - 1) >= LCD_GetWidth ())
    w = (int16_t)(LCD_GetWidth () - x);
  if ((y + h - 1) >= LCD_GetHeight ())
    h = (int16_t)(LCD_GetHeight () - y);

  LCD_SetAddrWindow ((uint16_t)x, (uint16_t)y, (uint16_t)(x + w - 1), (uint16_t)(y + h - 1));
  for (y = h; y > 0; y--)
    {
      for (x = w; x > 0; x--)
	{
	  LCD_PushColor (color);
	}
    }
}

void
GFX_FillScreen (uint16_t color)
{
  GFX_FillRect (0, 0, (int16_t)LCD_GetWidth (), (int16_t)LCD_GetHeight (), color);
}

void
GFX_DrawFastVLine (int16_t x, int16_t y, int16_t h, uint16_t color)
{
  // Rudimentary clipping
  if ((x >= LCD_GetWidth ()) || (y >= LCD_GetHeight ()))
    return;
  if ((y + h - 1) >= LCD_GetHeight ())
    h = LCD_GetHeight () - y;
  LCD_SetAddrWindow (x, y, x, y + h - 1);
  while (h--)
    {
      LCD_PushColor (color);
    }
}

void
GFX_DrawFastHLine (int16_t x, int16_t y, int16_t w, uint16_t color)
{
  // Rudimentary clipping
  if ((x >= LCD_GetWidth ()) || (y >= LCD_GetHeight ()))
    return;
  if ((x + w - 1) >= LCD_GetWidth ())
    w = LCD_GetWidth () - x;
  LCD_SetAddrWindow (x, y, x + w - 1, y);
  while (w--)
    {
      LCD_PushColor (color);
    }
}

void
GFX_DrawFastLine (uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,
		  uint16_t color)
{
  signed char dx, dy, sx, sy;
  unsigned char x, y, mdx, mdy, l;

  if (x1 == x2)
    {
      GFX_FillRect (x1, y1, x1, y2, color);
      return;
    }

  if (y1 == y2)
    {
      GFX_FillRect (x1, y1, x2, y1, color);
      return;
    }

  dx = x2 - x1;
  dy = y2 - y1;

  if (dx >= 0)
    {
      mdx = dx;
      sx = 1;
    }
  else
    {
      mdx = x1 - x2;
      sx = -1;
    }
  if (dy >= 0)
    {
      mdy = dy;
      sy = 1;
    }
  else
    {
      mdy = y1 - y2;
      sy = -1;
    }

  x = x1;
  y = y1;

  if (mdx >= mdy)
    {
      l = mdx;
      while (l > 0)
	{
	  if (dy > 0)
	    {
	      y = y1 + mdy * (x - x1) / mdx;
	    }
	  else
	    {
	      y = y1 - mdy * (x - x1) / mdx;
	    }
	  GFX_DrawPixel (x, y, color);
	  x = x + sx;
	  l--;
	}
    }
  else
    {
      l = mdy;
      while (l > 0)
	{
	  if (dy > 0)
	    {
	      x = x1 + ((mdx * (y - y1)) / mdy);
	    }
	  else
	    {
	      x = x1 + ((mdx * (y1 - y)) / mdy);
	    }
	  GFX_DrawPixel (x, y, color);
	  y = y + sy;
	  l--;
	}
    }
  GFX_DrawPixel (x2, y2, color);
}

void
GFX_DrawRect (uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color)
{
  GFX_DrawFastHLine (x1, y1, x2 - x1, color);
  GFX_DrawFastVLine (x2, y1, y2 - y1, color);
  GFX_DrawFastHLine (x1, y2, x2 - x1, color);
  GFX_DrawFastVLine (x1, y1, y2 - y1, color);
}

void
GFX_DrawCircle (int16_t x, int16_t y, int radius, uint16_t color)
{
  int f = 1 - radius;
  int ddF_x = 1;
  int ddF_y = -2 * radius;
  int x1 = 0;
  int y1 = radius;

  LCD_SetAddrWindow (x, y + radius, x, y + radius);
  LCD_PushColor (color);
  LCD_SetAddrWindow (x, y - radius, x, y - radius);
  LCD_PushColor (color);
  LCD_SetAddrWindow (x + radius, y, x + radius, y);
  LCD_PushColor (color);
  LCD_SetAddrWindow (x - radius, y, x - radius, y);
  LCD_PushColor (color);
  while (x1 < y1)
    {
      if (f >= 0)
	{
	  y1--;
	  ddF_y += 2;
	  f += ddF_y;
	}
      x1++;
      ddF_x += 2;
      f += ddF_x;
      LCD_SetAddrWindow (x + x1, y + y1, x + x1, y + y1);
      LCD_PushColor (color);
      LCD_SetAddrWindow (x - x1, y + y1, x - x1, y + y1);
      LCD_PushColor (color);
      LCD_SetAddrWindow (x + x1, y - y1, x + x1, y - y1);
      LCD_PushColor (color);
      LCD_SetAddrWindow (x - x1, y - y1, x - x1, y - y1);
      LCD_PushColor (color);
      LCD_SetAddrWindow (x + y1, y + x1, x + y1, y + x1);
      LCD_PushColor (color);
      LCD_SetAddrWindow (x - y1, y + x1, x - y1, y + x1);
      LCD_PushColor (color);
      LCD_SetAddrWindow (x + y1, y - x1, x + y1, y - x1);
      LCD_PushColor (color);
      LCD_SetAddrWindow (x - y1, y - x1, x - y1, y - x1);
      LCD_PushColor (color);
    }
}

void
GFX_FillCircle (int16_t x, int16_t y, int radius, uint16_t color)
{
  int x1, y1;
  for (y1 = -radius; y1 <= 0; y1++)
    for (x1 = -radius; x1 <= 0; x1++)
      if (x1 * x1 + y1 * y1 <= radius * radius)
	{
	  GFX_DrawFastHLine (x + x1, y + y1, 2 * (-x1), color);
	  GFX_DrawFastHLine (x + x1, y - y1, 2 * (-x1), color);
	  break;
	}
}
