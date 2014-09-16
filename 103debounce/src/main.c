#include <stdio.h>
#include "diag/Trace.h"

#include "timing.h"
#include "debounce.h"
#include "BlinkLed.h"


//Zur Vermeidung von Compilerwarnungen in bewusst gekennzeichneten (!) Fällen
#define UU __attribute__((unused))



DEBOUNCE_BusHandle debounce_bus = {
	.PORT=GPIOB,
	.RCC_APB2Periph=RCC_APB2Periph_GPIOB,
 };

void DEBOUNCE_click(u8 UU i){
	blink_led_toggle();
}

//void AW DEBOUNCE_up(u8 UU i) wird nirgends überschrieben --> Standardfunktionalität in  debounce.c kommt zum Einsatz

void SysTick_Handler_Main(UU u32 uwTick){
	DEBOUNCE_Handler(&debounce_bus);
}

int main(UU int argc, UU char* argv[])
{
  trace_printf("System clock: %uHz\n", SystemCoreClock);

  TIMING_Init();

  DEBOUNCE_Init(&debounce_bus);
  blink_led_init();

  while (1)
  {
  }


}
