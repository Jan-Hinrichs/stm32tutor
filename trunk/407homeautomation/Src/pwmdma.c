#include "stm32f4xx.h"
#include "stm32f4xx_hal_tim.h"
#include "pwmdma_conf.h"


extern TIM_HandleTypeDef htimx;
extern DMA_HandleTypeDef hdma;

static uint16_t PWM_DMA_PUFFER[PWM_DMA_WIDTH];  // 16bit
static uint16_t PWM_DMA_CH[16];

//--------------------------------------------------------------
// Init und start aller PWM Kanäle
//--------------------------------------------------------------
void
PWMDMA_Init (void)
{
  uint16_t n;

  // init vom Puffer
  for (n = 0; n < PWM_DMA_WIDTH; n++)
    {
      PWM_DMA_PUFFER[n] = 0;
    }

  // 16 Kanäle
  for (n = 0; n < 16; n++)
    {
      PWM_DMA_CH[n] = 0;
    }

  HAL_GPIO_WritePin (GPIOx, GPIO_PIN_All, GPIO_PIN_RESET);
  __HAL_TIM_ENABLE_DMA(&htimx, TIM_DMA_UPDATE);
  HAL_TIM_Base_Start (&htimx);
  HAL_DMA_Start (&hdma, (uint32_t) PWM_DMA_PUFFER, (uint32_t) (&(GPIOx->ODR)), PWM_DMA_WIDTH);
}

//--------------------------------------------------------------
// stellt einen PWM-Wert für einen Kanal ein
// kanal  : [0 bis 7] bei 8 Kanälen
//          [0 bis 15] bei 16 Kanälen
// wert   : [0 bis PWM_DMA_WIDTH-1]
//--------------------------------------------------------------
void
PWMDMA_SetPWM (uint8_t kanal, uint16_t wert)
{
  uint16_t n, bit_nr;

  if (wert >= PWM_DMA_WIDTH)
    wert = (PWM_DMA_WIDTH - 1);

  // Puffer-Werte aktualisieren
  // 16 Kanäle
  if (kanal > 15)
    kanal = 15;

  // Kanalwert setzen
  PWM_DMA_CH[kanal] = wert;

  for (n = 0; n < PWM_DMA_WIDTH; n++)
    {
      PWM_DMA_PUFFER[n] = 0;
      for (bit_nr = 0; bit_nr < 16; bit_nr++)
	{
	  if (PWM_DMA_CH[bit_nr] > n)
	    {
	      PWM_DMA_PUFFER[n] |= (1 << bit_nr);
	    }
	}
    }
}
