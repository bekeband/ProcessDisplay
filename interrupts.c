

#include <plib.h>
#include "configuration.h"
#include "globalextern.h"

/* AD datas varialbles. */
volatile unsigned long AD_DATA[MAX_AD_COUNT];
volatile unsigned long AD_BUFFER[MAX_AD_COUNT];
volatile uint16_t OLD_AD_VALUES[MAX_AD_COUNT];
volatile uint8_t NEW_AD_DATAS[MAX_AD_COUNT];
volatile uint8_t NEW_AD_CHANGES[MAX_AD_COUNT];

volatile int AD_COUNTER = 0;
volatile int AD_AVERAGE_FACTOR = DEFAULT_AD_AVERAGE_FACTOR;
volatile int AD_AVERAGE_COUNTER;
volatile int NEW_SUMMA_COUNT = 0;

volatile int CURRENT_MESSAGE = 0;
static volatile int BUTTON_COUNTER[BUT_NUMBER] = {0,0,0,0};
static BUTTONS_T BUTTONS_OLD_STATE;
static BUTTONS_T BUTTONS_STATE;
static BUTTONS_T reading;

int AD_RESTART_DIS = 0;

volatile int TIMER_COUNTER_VALUE;
volatile int HOUR_FLAG = 0;

/* The one interrupt routine, whitch handle all interrupts. */

/* Reset all AD buffers, and datas. */
void ResetADBuffers()
{ int i;
  ADC_INT_DISABLE();
  AD_AVERAGE_COUNTER = 0;
//  NEW_AD_DATA = 0;
  AD_COUNTER = 0;
  for (i = 0; i < MAX_AD_COUNT; i++)
  {
    AD_BUFFER[i] = 0;
    AD_DATA[i] = 0;
  }
  ADC_INT_ENABLE();
  ConvertADC();
}

int SEC_TIMER = 0;
int SEC10TH_TIMER = 0;
int TZ = 0;
int HOUR_TIMER = 0;

void interrupt isr(void)
{
  if (INTCONbits.TMR0IF)
  {
    INTCONbits.TMR0IF = 0;
    TMR0L = TMR0LVAL;
    TMR0H = TMR0HVAL;
/*    if (TZ++ == 1000)
    {
      LED1 = !LED1;
      TZ = 0;
    }*/
  }
  if (INTCONbits.RBIF)  // RB port changed interrupt ?
  {
    INTCONbits.RBIF = 0;
  }

  if (INTCONbits.T0IF)  // T0 timer interrupt
  {
    INTCONbits.T0IF = 0;
  }

  if (PIR5bits.TMR5IF)
  {
    TMR5L   = TMR5LVAL;
    TMR5H   = TMR5HVAL;
    PIR5bits.TMR5IF = 0;
    /* One sec timer counter.  */
    if ((SEC_TIMER++) == 40)
    {
      NEW_SUMMA_COUNT = 1;
      NEW_AD_CHANGES[0] = 1;
      SEC_TIMER = 0;
      if (HOUR_TIMER++ == 3600)
      {
        HOUR_TIMER = 0;
        HOUR_FLAG = 1;
      }
      /* ------------------------------------------------ */

    }
    TIMER_COUNTER_VALUE++;
  /* Button inputs and debounce programs. */
/* ------------------------------- BUTTON UP --------------------------------*/

    reading.UP = !BUT_UP;
    if (reading.UP != BUTTONS_OLD_STATE.UP)
    {
      if (reading.UP)
      {
        CURRENT_MESSAGE = BUT_UP_DOWN;
      } else
      {
        CURRENT_MESSAGE = BUT_UP_UP;
      }
      BUTTONS_OLD_STATE.UP = reading.UP;
    }

/*    if (BUTTON_COUNTER[0]++ >= BUTTON_DELAY)
    {
      if (reading.UP != BUTTONS_OLD_STATE.UP)
      {
        BUTTONS_STATE.UP = reading.UP;
        if (!BUTTONS_STATE.UP)
        {
          CURRENT_MESSAGE = BUT_UP_DOWN;
        }
      }
    }*/


    reading.DN = !BUT_DN;
    if (reading.DN != BUTTONS_OLD_STATE.DN)
    {
      if (reading.DN)
      {
        CURRENT_MESSAGE = BUT_DN_DOWN;        
      } else
      {
        CURRENT_MESSAGE = BUT_DN_UP;                
      }
      BUTTONS_OLD_STATE.DN = reading.DN;
    }

/*    if (BUTTON_COUNTER[1]++ >= BUTTON_DELAY)
    {
      if (reading.DN != BUTTONS_OLD_STATE.DN)
      {
        BUTTONS_STATE.DN = reading.DN;
        if (!BUTTONS_STATE.UP)
        {
          CURRENT_MESSAGE = BUT_DN_DOWN;
        }
      }
    }*/



/*
  if  (!BUT_UP) /* button up pressed ?
  {
    if (!BUTTONS_OLD_STATE.UP)
    { /* rising edge 
      BUTTONS_OLD_STATE.UP = 1;
      BUTTON_COUNTER[0] = 0;
    }
    BUTTON_COUNTER[0]++;
    if (BUTTON_COUNTER[0] >= BUTTON_DELAY_2)
    {
      CURRENT_MESSAGE = BUT_UP_LONG;
      BUTTONS_OLD_STATE.UP = 0;
//      INHIBIT[0] = 1;
    }

  } else  /* button up released. 
  {
    if (BUTTONS_OLD_STATE.UP)
    {
      BUTTONS_OLD_STATE.UP = 0;
      if (BUTTON_COUNTER[0] >= BUTTON_DELAY)
      {
        CURRENT_MESSAGE = BUT_UP_MESSAGE;
      }
  };
  }

/* ------------------------------- BUTTON DOWN --------------------------------

/* ------------------------------- BUTTON UP --------------------------------

  if  (!BUT_DN) /* button up pressed ?
  {
    if (!BUTTONS_OLD_STATE.DN)
    { /* rising edge 
      BUTTONS_OLD_STATE.DN = 1;
      BUTTON_COUNTER[1] = 0;
    }
    BUTTON_COUNTER[1]++;
    if (BUTTON_COUNTER[1] >= BUTTON_DELAY_2)
    {
      CURRENT_MESSAGE = BUT_DN_LONG;
      BUTTONS_OLD_STATE.DN = 0;
//      INHIBIT[0] = 1;
    }

  } else  /* button up released. 
  {
    if (BUTTONS_OLD_STATE.DN)
    {
      BUTTONS_OLD_STATE.DN = 0;
      if (BUTTON_COUNTER[1] >= BUTTON_DELAY)
      {
        CURRENT_MESSAGE = BUT_DN_MESSAGE;
      }
  };
  }
*/
}

if (PIR1bits.ADIF)
{
  PIR1bits.ADIF = 0;
  if (AD_AVERAGE_COUNTER == (AD_AVERAGE_FACTOR))
    {
      AD_DATA[AD_COUNTER] = (AD_BUFFER[AD_COUNTER] >>= 5);
      AD_BUFFER[AD_COUNTER] = 0;
      AD_AVERAGE_COUNTER = 0;
      /* Next AD data... */
      if (++AD_COUNTER == MAX_AD_COUNT)
      {
        NEW_AD_DATAS[AD_COUNTER] = 1;
        AD_COUNTER = 0;
        /* if the new A/D datas and changes as well. */
        if (OLD_AD_VALUES[AD_COUNTER] != AD_DATA[AD_COUNTER])
        {
//          NEW_AD_CHANGES[AD_COUNTER] = 1;
        }
      };
      /* TODO majd kivenni !*/
      SetChanADC(AD_COUNTER);
    }
    AD_AVERAGE_COUNTER++;
    /* Select AD channel. */
    AD_BUFFER[AD_COUNTER] += (ADRESH * 256) + ADRESL;
    if (!AD_RESTART_DIS)
    {
      ADCON0bits.GO = 1;  /* Restart A/D conversion. */
    }
  }
};

