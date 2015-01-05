#pragma once
#include "stm32f407xx.h"
#include "stm32f4xx_hal_gpio.h"
#include "stdbool.h"

#define MIN_TIME_BETWEEN_SWITCH_CHANGE 20
#define MIN_MAIN_CYCLE_TIME_MS	20
#define SHORT_CLICK_MS	300
#define DIMMER_CYCLE_MS 4096
#define PID_CYCLE_MS	300000
#define MS_PER_DIMSTEP (DIMMER_CYCLE_MS/255)
#define INITIAL_LAST_ON 0xFF
#define CNT(a)	sizeof(a)/sizeof(a[0])

#define CIRCULARBUFFERSIZE	64


void HOMECONTROL_Eternity(void);


//Bennenungsschema:
//Stock (KG, EG, OG, DG)
//Zimmer
//Schaltergruppe: Nummerierung ausgehend von der T�r im UZS, bei mehreren T�ren ist jene T�r die Referenzt�r, die der Haust�r am n�chsten ist
//Schalter: Zeilenweise, beginnend Oben rechts (Die Taster einer Wippe haben das idR eine Differenz von 2)

#define SWITCH_BASE_ID		0
#define EG_WOZI_G0S0		0 //Gruppe 0 Schalter 0
#define EG_WOZI_G0S1		1 //Gruppe 0 Schalter 1
#define EG_WOZI_G0S2		2 //Gruppe 0 Schalter 2

#define DIMMER_BASE_ID		1000
#define EG_WOZI_D0_ID		0
#define EG_WOZI_D1_ID		1

#define JALOUSIE_BASE_ID	2000
#define EG_WOZI_J0_ID		0

#define POWERITEM_BASE_ID	3000
#define	EG_WOZI_P_L0_ID		0 //es gibt Light-PowerItems ("L")...
#define	EG_WOZI_P_P0_ID		1 //...und Plug-PowerItems (schaltbare Steckdosen-Phasen, "P")
#define EG_FLUR_P_L0_ID		2

#define PISOTIMER_BASE_ID	4000
#define PISOTIMER_CEILING_ID	0

typedef enum
{
	SwitchState_PRESSED=1, SwitchState_RELEASED=0
} SwitchState_t;

typedef enum
{
	SyncState_Committed, SyncState_Dirty
} SyncState_t;

typedef enum
{
	DimmerState_CONSTANT, DimmerState_DIMUP, DimmerState_DIMDOWN
} DimmerState_t;

typedef enum
{
	DimmerGroupType_I2C, DimmerGroupType_PWMDMA
}DimmerGroupType_t;

typedef enum
{
	JalousieState_STOP, JalousieState_DOWN, JalousieState_UP
} JalousieState_t;

typedef enum
{
	PowerState_ON, PowerState_OFF
} PowerState_t;

typedef enum
{
  ActiveState_ACTIVE, ActiveState_INACTIVE
} ActiveState_t;

typedef enum
{
	PowerItemGroupType_GPIO, PowerItemGroupType_I2C
} PowerItemGroupType_t;

typedef struct Switch_Struct Switch_t;
struct Switch_Struct
{
	uint32_t	ID;
	SwitchState_t 	CurrentState;
	uint64_t	LastChange;
	GPIO_TypeDef 	*port;
	uint8_t		pin;
	void 		(*handler)(Switch_t *, uint64_t); //ref to switch and the timestamp
	uint32_t 	HandlersPayload;
};

typedef struct
{
	uint32_t	ID;
	DimmerState_t 	CurrentState;
	uint64_t	LastChange; //LastChange wird beim Ver�ndern des CurrentState gesetzt!
	SyncState_t	SyncState; //SyncState wird beim Ver�ndern der Brighness gesetzt!
	uint8_t		Brightness;
	uint8_t		BrightnessBeforeDim; //den Dimmwert vor Schalterbet�tigung, damit die Lampe mit genau diesem Wert ausgeschaltet werden kann
	uint8_t		LastOnBrightness;
	uint8_t		Channel;
	uint8_t		GroupIndex;
} Dimmer_t;

typedef struct
{
	const uint8_t 	GroupSize;
	Dimmer_t 	**Items;
	const uint8_t 	HWAdress;
	SyncState_t	SyncState; //SyncState wird beim Ver�ndern des CurrentState gesetzt!
	const DimmerGroupType_t GroupType;
}DimmerGroup_t;

typedef struct
{
	uint32_t	ID;
	JalousieState_t CurrentState;
	uint64_t	LastChange; //LastChange wird beim Ver�ndern des CurrentState gesetzt!
	SyncState_t	SyncState; //SyncState wird beim Ver�ndern des CurrentState gesetzt!
	//Im Gegensatz zum Dimmer wird hier der Aktorstatus nicht �ber die zus�tzliche
	//Variable "Brighthness", sondern �ber den JalousieState definiert.
	//Deshalb wird SyncState neu gesetzt, wenn CurrentState ver�ndert wird
	//eine Jalousie braucht zwei IOs, einen f�r AUF, einen f�r ab
	//TODO: Bitte pr�fen: Die Ausgabemethode muss sicher stellen, dass entweder AUF oder AB gesetzt ist
	//TODO: beides zusammen muss vermieden werden
	GPIO_TypeDef 		*Port_Up;
	uint16_t		Pin_Up;
	GPIO_TypeDef 		*Port_Down;
	uint16_t 		Pin_Down;
}Jalousie_t;

typedef struct
{
	uint32_t		ID;
	PowerState_t 		CurrentState;
	uint64_t		LastChange; //LastChange muss beim Verändern des CurrentState gesetzt werden
	uint16_t		Pin; //hat je nach HW eine andere Bedeutung. Möglichst sollte eine Bitmask gesetzt werden
	uint8_t			GroupIndex;
}PowerItem_t;

typedef struct
{
  uint32_t			ID;
  uint32_t			PowerItemID; //"Fremdschlüssel"
  uint64_t			SwitchOffTime;
}PowerItemSwitchOffTimer_t;

typedef struct
{
	const uint8_t 		GroupSize;
	const PowerItem_t 	**Items;
	const uint32_t 		HWAdress;
	SyncState_t		SyncState; //SyncState wird beim Verändern des CurrentState eines Items gesetzt!
	const PowerItemGroupType_t GroupType;
	uint8_t 		scratchpad[4]; //Dient beim Übertragen per I2C oder per CAN zum "Zusammenbauen" der Daten ohne dass es zu Konflikten mit anderen PIGs kommen könnte
}PowerItemGroup_t;

typedef struct
{
	uint32_t		ID;
	DimmerState_t 		CurrentState;
	uint64_t		LastChange; //LastChange wird beim Ver�ndern des CurrentState gesetzt!
	SyncState_t		SyncState; //SyncState wird beim Ver�ndern der Brighness gesetzt!
	uint8_t			Level;
	uint32_t		hwAdress; //Custom format eg: 8bit I2C-Device-Adress, 8 bit interne DAC, 16bit unused
} Ventile_t;
