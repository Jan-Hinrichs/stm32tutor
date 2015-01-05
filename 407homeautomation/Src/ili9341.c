#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "ili9341.h"
#include "ili9341_hal.h"

static LCD_Orientation_t orientation = LCD_Orientation_Portrait;


static void
sendCmd (uint8_t c)
{
  ILI9341_OutCommand();
  ILI9341_SendByte (c);
}

static void
sendData (uint8_t c)
{
  ILI9341_OutData();
  ILI9341_SendByte (c);
}

#define writedata(d) (sendData(d))
#define writecommand(c) (sendCmd(c))


// Rather than a bazillion writecommand() and writedata() calls, screen
// initialization commands and arguments are organized in these tables
// stored in PROGMEM.  The table may look bulky, but that's mostly the
// formatting -- storage-wise this is hundreds of bytes more compact
// than the equivalent code.  Companion function follows.
#define DELAY_MASK	0xE0
#define DELAY_NONE	0x00
#define DELAY_10ms	0x40
#define DELAY_50ms	0x80
#define DELAY_150ms 	0xC0
#define DELAY_500ms	0xE0

static const uint8_t cmdlist[] =
  {
      24,						// 24 commands in list:
      ILI9341_RESET,		0+DELAY_150ms,
      ILI9341_POWERA, 		5+DELAY_NONE,
      0x39, 0x2C, 0x00, 0x34, 0x02,
      ILI9341_POWERB, 		3+DELAY_NONE,
      0x00, 0xC1, 0x30,
      ILI9341_DTCA,		3+DELAY_NONE,
      0x85, 0x00, 0x78,
      ILI9341_DTCB,		2+DELAY_NONE,
      0x00, 0x00,
      ILI9341_POWER_SEQ,	4+DELAY_NONE,
      0x64, 0x03, 0x12, 0x81,
      ILI9341_PRC,		1+DELAY_NONE,
      0x20,
      ILI9341_POWER1,		1+DELAY_NONE,
      0x23,
      ILI9341_POWER2,		1+DELAY_NONE,
      0x10,
      ILI9341_VCOM1,		2+DELAY_NONE,
      0x3E, 0x28,
      ILI9341_VCOM2,		1+DELAY_NONE,
      0x86,
      ILI9341_MAC, 		1+DELAY_NONE,
      0x48,
      ILI9341_PIXEL_FORMAT,	1+DELAY_NONE,
      0x55,
      ILI9341_FRC,		2+DELAY_NONE,
      0x00, 0x18,
      ILI9341_DFC, 		3+DELAY_NONE,
      0x08, 0x82, 0x27,
      ILI9341_3GAMMA_EN,	1+DELAY_NONE,
      0x00,
      ILI9341_COLUMN_ADDR,	3+DELAY_NONE,
      0x00, 0x00, 0x00, 0xEF,
      ILI9341_PAGE_ADDR,	4+DELAY_NONE,
      0x00, 0x00, 0x01, 0x3F,
      ILI9341_GAMMA,		1+DELAY_NONE,
      0x01,
      ILI9341_PGAMMA,		15+DELAY_NONE,
      0x0F,0x31,0x2B,0x0C,0x0E,0x08,0x4E,0xF1,0x37,0x07,0x10,0x03,0x0E,0x09,0x00,
      ILI9341_NGAMMA,		15+DELAY_NONE,
      0x00,0x0E,0x14,0x03,0x11,0x07,0x31,0xC1,0x48,0x08,0x0F,0x0C,0x31,0x36,0x0F,
      ILI9341_SLEEP_OUT,	0+DELAY_150ms,
      ILI9341_DISPLAY_ON,	0+DELAY_NONE,
      ILI9341_GRAM,		0+DELAY_NONE
  };


// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
static void
commandList (const uint8_t *addr)
{
  uint8_t numCommands, numArgs;
  uint16_t ms;

  numCommands = *addr++;   // Number of commands to follow
  while (numCommands--)
    {                 // For each command...
      sendCmd (*addr++); //   Read, issue command
      numArgs = *addr++;    //   Number of args to follow
      ms = numArgs & DELAY_MASK;          //   If hibit set, delay follows args
      numArgs &= (uint8_t) ~DELAY_MASK;                   //   Mask out delay bit
      while (numArgs--)
	{                   //   For each argument...
	  sendData (*addr++);  //     Read, issue argument
	}

      if (ms)
	{
	  ms = ms >> 5;
	  DelayMs(ms * ms * ms);
	}
    }
}

// Initialization code common to both 'B' and 'R' type displays
void
ILI9341_Init (LCD_Orientation_t m)
{
  ILI9341_InitHal ();
  ILI9341_ChipEnable();
  ILI9341_ChipReset();
  DelayMs(200);
  ILI9341_ChipNoReset();
  DelayMs(150);
  //commandList (cmdlist);
  writecommand(0xEF);
  writedata(0x03);
  writedata(0x80);
  writedata(0x02);

  writecommand(0xCF);
  writedata(0x00);
  writedata(0XC1);
  writedata(0X30);

  writecommand(0xED);
  writedata(0x64);
  writedata(0x03);
  writedata(0X12);
  writedata(0X81);

  writecommand(0xE8);
  writedata(0x85);
  writedata(0x00);
  writedata(0x78);

  writecommand(0xCB);
  writedata(0x39);
  writedata(0x2C);
  writedata(0x00);
  writedata(0x34);
  writedata(0x02);

  writecommand(0xF7);
  writedata(0x20);

  writecommand(0xEA);
  writedata(0x00);
  writedata(0x00);

  writecommand(ILI9341_PWCTR1);    //Power control
  writedata(0x23);   //VRH[5:0]

  writecommand(ILI9341_PWCTR2);    //Power control
  writedata(0x10);   //SAP[2:0];BT[3:0]

  writecommand(ILI9341_VMCTR1);    //VCM control
  writedata(0x3e);
  writedata(0x28);

  writecommand(ILI9341_VMCTR2);    //VCM control2
  writedata(0x86);  //--

  writecommand(ILI9341_MADCTL);    // Memory Access Control
  writedata(0x48);

  writecommand(ILI9341_PIXFMT);
  writedata(0x55);

  writecommand(ILI9341_FRMCTR1);
  writedata(0x00);
  writedata(0x18);

  writecommand(ILI9341_DFUNCTR);    // Display Function Control
  writedata(0x08);
  writedata(0x82);
  writedata(0x27);

  writecommand(0xF2);    // 3Gamma Function Disable
  writedata(0x00);

  writecommand(ILI9341_GAMMASET);    //Gamma curve selected
  writedata(0x01);

  writecommand(ILI9341_GMCTRP1);    //Set Gamma
  writedata(0x0F);
  writedata(0x31);
  writedata(0x2B);
  writedata(0x0C);
  writedata(0x0E);
  writedata(0x08);
  writedata(0x4E);
  writedata(0xF1);
  writedata(0x37);
  writedata(0x07);
  writedata(0x10);
  writedata(0x03);
  writedata(0x0E);
  writedata(0x09);
  writedata(0x00);

  writecommand(ILI9341_GMCTRN1);    //Set Gamma
  writedata(0x00);
  writedata(0x0E);
  writedata(0x14);
  writedata(0x03);
  writedata(0x11);
  writedata(0x07);
  writedata(0x31);
  writedata(0xC1);
  writedata(0x48);
  writedata(0x08);
  writedata(0x0F);
  writedata(0x0C);
  writedata(0x31);
  writedata(0x36);
  writedata(0x0F);
  writecommand(ILI9341_SLPOUT);    //Exit Sleep
  DelayMs(120);
  writecommand(ILI9341_DISPON);    //Display on
  LCD_SetOrientation(m);
}

uint16_t
LCD_GetWidth ()
{
  switch (orientation)
    {
    case LCD_Orientation_Portrait:
    case LCD_Orientation_Portrait_Flipped:
      return ILI9341_WIDTH;
    case LCD_Orientation_Landscape:
    case LCD_Orientation_Landscape_Flipped:
      return ILI9341_HEIGHT;

    default:
      return 0;
    }
}
uint16_t
LCD_GetHeight ()
{
  switch (orientation)
    {
    case LCD_Orientation_Portrait:
    case LCD_Orientation_Portrait_Flipped:
      return ILI9341_HEIGHT;
    case LCD_Orientation_Landscape:
    case LCD_Orientation_Landscape_Flipped:
      return ILI9341_WIDTH;

    default:
      return 0;
    }
}

void
LCD_SetAddrWindow (uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	sendCmd(ILI9341_COLUMN_ADDR);
	sendData(x0 >> 8);
	sendData(x0 & 0xFF);
	sendData(x1 >> 8);
	sendData(x1 & 0xFF);

	sendCmd(ILI9341_PAGE_ADDR);
	sendData(y0 >> 8);
	sendData(y0 & 0xFF);
	sendData(y1 >> 8);
	sendData(y1 & 0xFF);
	sendCmd(ILI9341_GRAM);
}

void
LCD_PushColor (uint16_t color)
{
  ILI9341_OutData();
  ILI9341_SendByte (color >> 8);
  ILI9341_SendByte (color & 0xFF);
}
void
LCD_SetOrientation(LCD_Orientation_t m)
{
  sendCmd(ILI9341_MAC);
  switch(m)
  {
    case LCD_Orientation_Portrait: sendData(0x58); break;
    case LCD_Orientation_Portrait_Flipped: sendData(0x88); break;
    case LCD_Orientation_Landscape: sendData(0x28); break;
    case LCD_Orientation_Landscape_Flipped: sendData(0xE8); break;
  }
  orientation=m;
}
