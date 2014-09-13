
#include <stdio.h>
#include "diag/Trace.h"

#include "timing.h"
#include "dht22.h"
static float temperature =0.0;
static float humidity = 0.0;

int
main(int __attribute__((unused)) argc, __attribute__((unused)) char* argv[])
{

	printf("Hello user, my clock is %uHz\n", (unsigned int)SystemCoreClock);

	DHT_BusHandle bus = {
			.PORT=GPIOB,
			.RCC_APB2Periph=RCC_APB2Periph_GPIOB,
			.GPIO_Pin=GPIO_Pin_0,
	};

	TIMING_Init();
	DHT_Init_Sync(&bus);

	DHT_ErrorLevel dhtErrorLevel;

	// Infinite loop
	while (1)
	{
		dhtErrorLevel = DHT_Start_Sync(&bus);
		if(dhtErrorLevel == DHT_ERROR_NO_ERROR_SET){
			temperature=DHT_GetTemp(0);
			humidity = DHT_GetHumidity(0);
			printf("Temperature: %f Grad Celsius, Humidity: %f rel.\n", temperature, humidity);
		}else{
			printf("Error: %u while reading DHT Sensors\n", dhtErrorLevel);
		}
		TIMING_DelayMs(2000);
	}
}
