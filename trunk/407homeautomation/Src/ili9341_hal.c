#include "ili9341_hal.h"
#include "stm32f4xx_hal_spi.h"

extern SPI_HandleTypeDef hspi2;


void ILI9341_InitHal(void) {
  //is done in CubeMX
}

//--------------------------------------------------------------
// interne Funktion
//--------------------------------------------------------------
void ILI9341_SendByte(uint8_t byte) {
  HAL_SPI_Transmit(&hspi2, &byte, 1, 1000);
}
