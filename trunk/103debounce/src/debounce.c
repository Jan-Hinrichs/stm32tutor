#include "debounce.h"

#define T_DEBOUNCE 30
#define UU __attribute__((unused))
#define AW __attribute__((weak))


DEBOUNCE_ErrorLevel DEBOUNCE_Init(DEBOUNCE_BusHandle *bus) {

	RCC_APB2PeriphClockCmd(bus->RCC_APB2Periph, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

	GPIO_Init(bus->PORT, &GPIO_InitStructure);
	return DEBOUNCE_NO_ERROR;
}


void AW DEBOUNCE_shortclick(u8 UU i){
	return;
}
void AW DEBOUNCE_longclick(u8 UU i, u32 UU t){
	return;
}
void AW DEBOUNCE_doubleclick(u8 UU i){
	return;
}

void AW DEBOUNCE_click(u8 UU i){
	return;
}
void AW DEBOUNCE_up(u8 UU i){
	return;
}


void DEBOUNCE_Handler(DEBOUNCE_BusHandle *bus){
	static s8 t[16] = {0};
	u8 i;
	u16 inp = GPIO_ReadInputData(bus->PORT);
	for(i=0;i<1;i++){
		if(inp & (1<<i)){
			//Taste losgelassen
			if(t[i]==+T_DEBOUNCE){
				DEBOUNCE_up(i);
				t[i]=-1;
			}
			if(t[i] > -T_DEBOUNCE){
				t[i]--;
			}
		}else{
			//Taste gedrückt
			if(t[i]==-T_DEBOUNCE){
				DEBOUNCE_click(i);
				t[i]=1;
			}
			if(t[i] < T_DEBOUNCE){
				t[i]++;
			}
		}
	}
}

///////////////////////////////////////////////////////
//everything below here is only an idea and not tested!
///////////////////////////////////////////////////////

#define T_MAXSHORTCLICK 300
typedef enum{
	RELEASED=0,
	IS_FIRST_DOWN=1,
	IS_WAITING_FOR_DOUBLECLICK =2,
	IS_SECOND_DOWN=3
} keystates;

void debounce_OnlyAnIdea_NotTested(DEBOUNCE_BusHandle *bus, u32 now){
	static u32 t[16] = {0};
	static keystates state[16] = {0};
	u16 inp = GPIO_ReadInputData(bus->PORT);
	//beim Doppelklick wird kein zweites down/up gesendet

	//Zeit immer zuerst prüfen
	//Zeitaktualisierung, dann Statusaktualisierung, dann Event auslösen
	u8 i;
	u32 timeSinceLastEvent=0;
	for(i=0;i<16;i++){
		timeSinceLastEvent = now - t[i];
		switch(state[i]){
		case RELEASED:
			if(inp & (1<<i)){
				//Taste gedrückt
				t[i]=now;
				state[i] = IS_FIRST_DOWN;
				DEBOUNCE_click(i);
			}
			break;
		case IS_FIRST_DOWN:
			if((timeSinceLastEvent>T_DEBOUNCE) && !(inp & (1<<i))){
				DEBOUNCE_up(i);
				if(timeSinceLastEvent<T_MAXSHORTCLICK){
					t[i]=now;
					state[i] = IS_WAITING_FOR_DOUBLECLICK;
				}else{
					state[i] = RELEASED;
					DEBOUNCE_longclick(i, now-t[i]);
				}
			}
			break;
		case IS_WAITING_FOR_DOUBLECLICK:
			if(timeSinceLastEvent > T_DEBOUNCE && (inp & (1<<i)) && (timeSinceLastEvent<T_MAXSHORTCLICK)){
				t[i]=now;
				state[i] = IS_SECOND_DOWN;
				DEBOUNCE_doubleclick(i);
			}
			if(timeSinceLastEvent >= T_MAXSHORTCLICK){
				state[i] = RELEASED;
				DEBOUNCE_shortclick(i);
			}
			break;
		case IS_SECOND_DOWN:
			if((timeSinceLastEvent>T_DEBOUNCE) && !(inp & (1<<i))){
				state[i] = RELEASED;
			}
		}
	}
}
