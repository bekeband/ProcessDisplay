

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

volatile int CURRENT_MESSAGE = 0;
volatile int BUTTON_COUNTER[BUT_NUMBER] = {0,0,0,0};
volatile int INHIBIT[BUT_NUMBER] = {0,0,0,0};
int SEC_TIMER = 0;
int HOUR_TIMER = 0;

void interrupt isr(void)
{
/*  if (INTCONbits.INT0IF)
  {
    LCD_LIGHT = 1;
    INTCONbits.INT0IF = 0;
  }*/
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
    }
    TIMER_COUNTER_VALUE++;
  /* Button inputs and debounce programs. */
/* ------------------------------- BUTTON UP --------------------------------*/
  if  (!BUT_UP)
  {
    if (BUTTON_COUNTER[0]++ >= BUTTON_DELAY_2)
    {
      CURRENT_MESSAGE = BUT_UP_LONG;
      INHIBIT[0] = 1;
      BUTTON_COUNTER[0] = 0;
    };
  } else
  {
    if ((BUTTON_COUNTER[0] >= BUTTON_DELAY) && (!INHIBIT[0]))
    {
      CURRENT_MESSAGE = BUT_UP_MESSAGE;
    }
    INHIBIT[0] = 0;
    BUTTON_COUNTER[0] = 0;
  }

/* ------------------------------- BUTTON DOWN --------------------------------*/

  if  (!BUT_DN)
  {
    if (BUTTON_COUNTER[1]++ >= BUTTON_DELAY_2)
    {
      CURRENT_MESSAGE = BUT_DN_LONG;
      INHIBIT[1] = 1;
      BUTTON_COUNTER[1] = 0;
    };
  } else
  {
    if ((BUTTON_COUNTER[1] >= BUTTON_DELAY) && (!INHIBIT[1]))
    {
      CURRENT_MESSAGE = BUT_DN_MESSAGE;
    }
    INHIBIT[1] = 0;
    BUTTON_COUNTER[1] = 0;
  }

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

