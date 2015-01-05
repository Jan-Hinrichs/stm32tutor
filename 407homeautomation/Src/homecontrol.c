#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stdbool.h"
#include "homecontrol.h"
#include "pwmdma.h"

#define PIG_0_ADRESS 0x55

static uint64_t wideTick;
extern I2C_HandleTypeDef hi2c1;

void
HAL_SYSTICK_Callback (void)
{
  wideTick++;
}

static void
HNDLR_SW_DimUp (Switch_t *s, uint64_t ticksMs);
static void
HNDLR_SW_DimDown (Switch_t *s, uint64_t ticksMs);
static void
HNDLR_SW_JalousieUp (Switch_t *s, uint64_t ticksMs);
static void
HNDLR_SW_PowerItem (Switch_t *s, uint64_t ticksMs);
static void
HNDLR_SW_PowerItemSwitchOffTimer (Switch_t *s, uint64_t ticksMs);

//hier per Struct-Konstruktor all jene Switches eintragen, die 1:1 per Pin (PullUp + Suppressor-Diode) verbunden sind
//Die Position im Array hat eine Bedeutung!
//uint32_t ID; SwitchState_t CurrentState; uint32_t LastChange;	GPIO_TypeDef *port; uint8_t pin;
//void (*handler)(Switch_t *, uint32_t); uint32_t HandlersPayload;

//HandlersPayload beim Dimmer ist die DimmerID
//HandlersPayload bei TimeoutSwitches ist: 10LSB: PowerItemID: 6 nächste Bit TimerID 16MSB: Timeout in Sekunden
Switch_t switches[] =
  {

    { EG_WOZI_G0S0, SwitchState_RELEASED, 0, GPIOA, GPIO_PIN_1, HNDLR_SW_DimUp,
	EG_WOZI_D0_ID },
    { EG_WOZI_G0S1, SwitchState_RELEASED, 0, GPIOA, GPIO_PIN_2,
	HNDLR_SW_DimDown, EG_WOZI_D0_ID },
    { EG_WOZI_G0S2, SwitchState_RELEASED, 0, GPIOA, GPIO_PIN_3,
	HNDLR_SW_PowerItemSwitchOffTimer, EG_FLUR_P_L0_ID
	    | (PISOTIMER_CEILING_ID << 10) | 10 << 16 } };
#define SWITCH_CNT CNT(switches)

Dimmer_t dimmers[] =
  {
    { EG_WOZI_D0_ID, DimmerState_CONSTANT, 0, SyncState_Dirty, 0, 0,
    INITIAL_LAST_ON, 12, 0 },
    { EG_WOZI_D1_ID, DimmerState_CONSTANT, 0, SyncState_Dirty, 0, 0,
    INITIAL_LAST_ON, 13, 0 } };
#define DIMMER_CNT CNT(dimmers)

Dimmer_t *dimmersOfGroup0[] =
  { &dimmers[0], &dimmers[1] };

DimmerGroup_t dimmerGroups[] =
  {
    { 2, dimmersOfGroup0, 0xFF, SyncState_Dirty, DimmerGroupType_PWMDMA } };

Jalousie_t jalousies[] =
  {
    { EG_WOZI_J0_ID, JalousieState_STOP, 0, SyncState_Dirty, GPIOF, GPIO_PIN_0,
    GPIOF, GPIO_PIN_1 } };
#define JALOUSIE_CNT CNT(jalousies)

PowerItem_t powerItems[] =
  {
    { EG_WOZI_P_L0_ID, PowerState_OFF, (uint64_t) 0, GPIO_PIN_13, 0 },
    { EG_WOZI_P_P0_ID, PowerState_OFF, (uint64_t) 0, GPIO_PIN_14, 0 },
    { EG_FLUR_P_L0_ID, PowerState_OFF, (uint64_t) 0, GPIO_PIN_12, 1 } };
#define POWERITEM_CNT CNT(powerItems)

const PowerItem_t *powerItemsOfGroup0[] =
  { &powerItems[0], &powerItems[1] };
const PowerItem_t *powerItemsOfGroup1[] =
  { &powerItems[2] };

PowerItemGroup_t powerItemGroups[] =
  {
    { CNT(powerItemsOfGroup0), powerItemsOfGroup0, PIG_0_ADRESS,
	SyncState_Dirty, PowerItemGroupType_I2C },
    { CNT(powerItemsOfGroup1), powerItemsOfGroup1, (uint32_t) GPIOD,
	SyncState_Dirty, PowerItemGroupType_GPIO } };

PowerItemSwitchOffTimer_t powerItemSwitchOffTimers[] =
  {
    { PISOTIMER_CEILING_ID, EG_FLUR_P_L0_ID, UINT64_MAX } };
#define POWERITEMSWITCHOFFTIMER_CNT CNT(powerItemSwitchOffTimers)

static void
UpdateSwitches (uint64_t currentTicks)
{
  uint8_t si = 0;
  for (si = 0; si < SWITCH_CNT; si++)
    {
      Switch_t *s = &switches[si];
      GPIO_PinState val = HAL_GPIO_ReadPin (s->port, s->pin);
      SwitchState_t newstate =
	  val == GPIO_PIN_RESET ? SwitchState_PRESSED : SwitchState_RELEASED;
      if (newstate
	  != s->CurrentState&& currentTicks - s->LastChange > MIN_TIME_BETWEEN_SWITCH_CHANGE)
	{
	  s->CurrentState = newstate;
	  (s->handler) (s, currentTicks);
	  s->LastChange = currentTicks;
	}
    }
}

static void
UpdateTimers (uint64_t currentTicks)
{
  uint8_t ti = 0;
  for (ti = 0; ti < POWERITEMSWITCHOFFTIMER_CNT; ti++)
    {
      PowerItemSwitchOffTimer_t *t = &powerItemSwitchOffTimers[ti];
      if (t->SwitchOffTime < currentTicks)
	{
	  PowerItem_t *pi = &powerItems[t->PowerItemID];
	  pi->CurrentState = PowerState_OFF;
	  pi->LastChange = currentTicks;
	  t->SwitchOffTime = UINT64_MAX;
	  PowerItemGroup_t *powerItemGroup = &powerItemGroups[pi->GroupIndex];
	  powerItemGroup->SyncState = SyncState_Dirty;
	}
    }
}

static void
DoPIDControl (uint64_t currentTicks)
{

  //diese Funktion sollte alle 5 Minuten aufgerufen werden
  //wenn der Rechenaufwand  übersichtlich ist, kann das in einem Wupps für alle Ventile gemacht werden
  //ansonsten muss sich jeder Regler separat merken, wann er zuletzt aktualisiert wurde
  //in der Main-Schleife werden die Regler unabh�ngig voneinander in verschiedenen Schleifen geprüft,
  //ob sie aktualisiert werden müssen
}

static void
HNDLR_SW_Dim (Switch_t *s, uint64_t ticksMs, DimmerState_t dimmerState)
{
  Dimmer_t *d = &dimmers[s->HandlersPayload];
  if (s->CurrentState == SwitchState_PRESSED)
    {
      //if(d->CurrentState == DimmerState_CONSTANT){
      //Alles passiert unabhängig vom aktuellen Zustand!
      d->BrightnessBeforeDim = d->Brightness;
      d->LastChange = ticksMs;
      d->CurrentState = dimmerState;
      //}
    }
  else
    {
      //if(d->CurrentState == DimmerState_DIMUP){ Nein! unabhängig vom Status, wegen gleichzeitiger Mehrfachbedienung!
      if (ticksMs - s->LastChange < SHORT_CLICK_MS) //Kurze Betätigung?
	{	//Lampe ein oder ausschalten
	  if (d->BrightnessBeforeDim == 0) //wenn die Lampe vorher aus war
	    { //...dann einschalten mit gespeicherter Helligkeit; DirtyFlag setzen
	      d->Brightness = d->LastOnBrightness;
	      d->SyncState = SyncState_Dirty;
	      dimmerGroups[d->GroupIndex].SyncState = SyncState_Dirty;
	    }
	  else //wenn die Lampe vorher an war
	    { 	//dann ausschalten; Helligkeit vorher speichern!
	      d->LastOnBrightness = d->BrightnessBeforeDim; //und nicht d->Brightness!
	      d->Brightness = 0;
	      d->SyncState = SyncState_Dirty;
	      dimmerGroups[d->GroupIndex].SyncState = SyncState_Dirty;
	    }
	}
      //Lange Betätigung --> nichts zu tun, außer das Dimmen stoppen
      d->LastChange = ticksMs;
      d->CurrentState = DimmerState_CONSTANT;
    }
}

static void
HNDLR_SW_DimDown (Switch_t *s, uint64_t ticksMs)
{
  HNDLR_SW_Dim (s, ticksMs, DimmerState_DIMDOWN);
}

static void
HNDLR_SW_DimUp (Switch_t *s, uint64_t ticksMs)
{
  HNDLR_SW_Dim (s, ticksMs, DimmerState_DIMUP);
}

static void
HNDLR_SW_JalousieUp (Switch_t *s, uint64_t ticksMs)
{

  if (s->CurrentState == SwitchState_PRESSED)
    {
      Jalousie_t *j = &jalousies[s->HandlersPayload];
      if (j->CurrentState != JalousieState_STOP)
	{
	  j->CurrentState = JalousieState_STOP;
	}
      else
	{
	  j->CurrentState = JalousieState_UP;
	}
      j->LastChange = ticksMs;
      j->SyncState = SyncState_Dirty;
    }
}

static void
HNDLR_SW_PowerItem (Switch_t *s, uint64_t ticksMs)
{
  if (s->CurrentState == SwitchState_PRESSED)
    {
      PowerItem_t *p = &powerItems[s->HandlersPayload];

      if (p->CurrentState == PowerState_ON)
	{
	  p->CurrentState = PowerState_OFF;
	}
      else
	{
	  p->CurrentState = PowerState_ON;
	}
      p->LastChange = ticksMs;
      PowerItemGroup_t *powerItemGroup = &powerItemGroups[p->GroupIndex];
      powerItemGroup->SyncState = SyncState_Dirty;
    }
}

static void
HNDLR_SW_PowerItemSwitchOffTimer (Switch_t *s, uint64_t ticksMs)
{
  if (s->CurrentState == SwitchState_PRESSED)
    {
      PowerItem_t *p = &powerItems[(s->HandlersPayload & 0x000003FF)];

      if (p->CurrentState == PowerState_ON)
	{
	  p->CurrentState = PowerState_OFF;
	}
      else
	{
	  p->CurrentState = PowerState_ON;
	}
      p->LastChange = ticksMs;
      PowerItemGroup_t *powerItemGroup = &powerItemGroups[p->GroupIndex];
      powerItemGroup->SyncState = SyncState_Dirty;
    }
  else
    {
      //Prüfe, wie lange das Press schon her war
      if (ticksMs - s->LastChange < SHORT_CLICK_MS)
	{
	  PowerItemSwitchOffTimer_t *pisot =
	      &powerItemSwitchOffTimers[((s->HandlersPayload & 0x0000FC00) >> 10)];
	  uint32_t timeout = (s->HandlersPayload >> 16) * 1000;
	  pisot->SwitchOffTime = ticksMs + timeout;
	}
    }
}

static void
UpdateBrightnessOfActiveDimmers (long tickMs)
{
  //Berechne, wie viel Zeit seit Beginn des Dimm-Prozesses vergangen ist; Rechne Modulo und setze die Hellingkeit entsprechend
  //Mache dies nur im Status DIMUP oder DIMDOWN
  //Setze das Dirty-Flag
  //ein Dimm-Durchlauf dauert 4096ms?
  uint8_t di = 0;
  for (di = 0; di < DIMMER_CNT; di++)
    {
      Dimmer_t *d = &dimmers[di];
      if (d->CurrentState != DimmerState_CONSTANT)
	{
	  uint32_t time = (tickMs - d->LastChange) % DIMMER_CYCLE_MS; //time zwischen 0 und 4095
	  //zum urspr�nglichen Wert wird ein Wert zwischen 0 und 255 addiert/subtrahiert
	  //es gibt dann zwar einen Overflow, doch dieser ist explizit gew�nscht!
	  uint8_t newBrightness = d->BrightnessBeforeDim;
	  if (d->CurrentState == DimmerState_DIMUP)
	    {
	      newBrightness += (time / MS_PER_DIMSTEP);
	      if (newBrightness < d->Brightness)
		{			//Overflow?
		  d->Brightness = 0xFF;		//Setze auf Maximale Helligkeit
		  //d->CurrentState = DimmerState_CONSTANT; nein! - der State wird nur durch Loslassen des Schalter ge�ndert!
		  //Der Handler rennt dann zwar permanent in diese Schleife und stellt immer wieder den Overflow fest,
		  //aber das tut keinem weh...und die Verantwortlichkeiten sind sauber separiert
		}
	      else
		{
		  d->Brightness = newBrightness; //Setze auf errechnete Helligkeit
		}
	    }
	  else
	    {
	      newBrightness -= (time / MS_PER_DIMSTEP);
	      if (newBrightness > d->Brightness)
		{ //Underflow?
		  d->Brightness = 0; //Setze auf Minimale Helligkeit
		  //d->CurrentState = DimmerState_CONSTANT; nein! - der State wird nur durch Loslassen des Schalter ge�ndert!
		}
	      else
		{
		  d->Brightness = newBrightness; //Setze auf errechnete Helligkeit
		}
	    }
	  dimmerGroups[d->GroupIndex].SyncState = SyncState_Dirty;
	  d->SyncState = SyncState_Dirty;
	}
    }
}

static void
QueryTHSensor (uint8_t sensorIndex)
{
  //DHT22 abfragen
  //Idee: Alle DHTs an den gleichen Port hängen und per Timer-Interrupt die Werte über die maximal mögliche Zeitspanne in einem angemessenen
  //Abtastintervall einlesen und direkt auswerten

}

static void
CommitActors ()
{
  uint8_t gi = 0;
  for (gi = 0; gi < CNT(dimmerGroups); gi++)
    {
      DimmerGroup_t* dg = &dimmerGroups[gi];
      if (dg->SyncState == SyncState_Dirty)
	{
	  //Write the state to real hardware
	  int dj = 0;
	  for (dj = 0; dj < dg->GroupSize; dj++)
	    {
	      Dimmer_t *d = dg->Items[dj];
	      PWMDMA_SetPWM (d->Channel, d->Brightness);
	      d->SyncState = SyncState_Committed;
	    }
	  dg->SyncState = SyncState_Committed;
	}
    }
  for (gi = 0; gi < CNT(powerItemGroups); gi++)
    {
      PowerItemGroup_t *pig = &powerItemGroups[gi];
      uint8_t pii;
      GPIO_TypeDef *GPIOx;
      if (pig->SyncState == SyncState_Dirty)
	{
	  //Collect State of Write the state to real hardware
	  switch (pig->GroupType)
	    {
	    case PowerItemGroupType_I2C: //collect & write to I2C
	      pig->scratchpad[0] = pig->scratchpad[1] = pig->scratchpad[2] =
		  pig->scratchpad[3] = 0;

	      for (pii = 0; pii < pig->GroupSize; pii++)
		{
		  const PowerItem_t *pi = pig->Items[pii];
		  if (pi->CurrentState == PowerState_ON)
		    {
		      pig->scratchpad[pi->Pin >> 3] |= (1 << (pi->Pin & 0x07));
		    }

		}
	      HAL_I2C_Master_Transmit (&hi2c1, pig->HWAdress, pig->scratchpad,
				       4, 10000);
	      break;
	    case PowerItemGroupType_GPIO: //collect & write to GPIO
	      GPIOx = (GPIO_TypeDef *) (pig->HWAdress);
	      for (pii = 0; pii < pig->GroupSize; pii++)
		{
		  const PowerItem_t *pi = pig->Items[pii];
		  if (pi->CurrentState == PowerState_ON)
		    {
		      GPIOx->ODR |= pi->Pin;
		    }
		  else
		    {
		      GPIOx->ODR &= ~pi->Pin;
		    }
		}
	      break;
	    }
	  pig->SyncState = SyncState_Committed;
	}
    }
}

void
CheckAndProcessAlarms ()
{
  return;
}

uint32_t nextPIDUpdate = 0;

void
HOMECONTROL_Eternity (void)
{
  long loopCnt = 0;
  while (true)
    {
      uint64_t ticksMs = wideTick;
      CheckAndProcessAlarms ();
      UpdateSwitches (ticksMs);
      UpdateTimers (ticksMs);
      UpdateBrightnessOfActiveDimmers (ticksMs);
      CommitActors (ticksMs);
      switch (loopCnt % 16)
	{
	case 1:
	  QueryTHSensor (0);
	  break;
	case 2:
	  break;
	case 3:
	  break;
	case 4:
	  if ((int32_t) (ticksMs - nextPIDUpdate) > 0) //kann die Differenz wg uint überhaupt 0 bzw kleiner werden?
	    {
	      DoPIDControl (ticksMs);
	      nextPIDUpdate += PID_CYCLE_MS;
	    }
	}
      loopCnt++;
      uint32_t diff = HAL_GetTick () - ticksMs;
      if (diff < MIN_MAIN_CYCLE_TIME_MS)
	{
	  HAL_Delay (MIN_MAIN_CYCLE_TIME_MS - diff);
	}
    }
}
