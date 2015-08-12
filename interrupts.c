

#include <plib.h>
#include "configuration.h"
#include "globalextern.h"

/* AD datas varialbles. */
unsigned long AD_DATA[MAX_AD_COUNT];
unsigned long AD_BUFFER[MAX_AD_COUNT];
int AD_COUNTER = 0;
int AD_AVERAGE_FACTOR = DEFAULT_AD_AVERAGE_FACTOR;
int AD_AVERAGE_COUNTER;
int NEW_AD_DATA = 0;
int NEW_SUMMA_COUNT = 0;

int AD_RESTART_DIS = 0;

int TIMER_COUNTER_VALUE;

/* The one interrupt routine, whitch handle all interrupts. */

/* Reset all AD buffers, and datas. */
void ResetADBuffers()
{ int i;
  ADC_INT_DISABLE();
  AD_AVERAGE_COUNTER = 0;
  NEW_AD_DATA = 0;
  AD_COUNTER = 0;
  for (i = 0; i < MAX_AD_COUNT; i++)
  {
    AD_BUFFER[i] = 0;
    AD_DATA[i] = 0;
  }
  ADC_INT_ENABLE();
  ConvertADC();
}

int CURRENT_MESSAGE = 0;
int BUTTON_COUNTER[BUT_NUMBER] = {0,0,0,0};
int INHIBIT[BUT_NUMBER] = {0,0,0,0};
int SEC_TIMER = 0;

void interrupt isr(void)
{ int i;

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
  if ((SEC_TIMER++) == 40)
  {
    NEW_SUMMA_COUNT = 1;
    LED1 = !LED1;
    SEC_TIMER = 0;
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

/* ------------------------------- BUTTON ENTER --------------------------------*/

  if  (!BUT_ENT)
  {
    if (BUTTON_COUNTER[2]++ >= BUTTON_DELAY_2)
    {
      CURRENT_MESSAGE = BUT_ENT_LONG;
      INHIBIT[2] = 1;
      BUTTON_COUNTER[2] = 0;
    };
  } else
  {
    if ((BUTTON_COUNTER[2] >= BUTTON_DELAY) && (!INHIBIT[2]))
    {
      CURRENT_MESSAGE = BUT_ENT_MESSAGE;
    }
    INHIBIT[2] = 0;
    BUTTON_COUNTER[2] = 0;
  }

  /* ------------------------------- BUTTON ESCAPE --------------------------------*/

  if  (!BUT_ESC)
  {
    if (BUTTON_COUNTER[3]++ >= BUTTON_DELAY_2)
    {
      CURRENT_MESSAGE = BUT_ESC_LONG;
      INHIBIT[3] = 1;
      BUTTON_COUNTER[3] = 0;
    };
  } else
  {
    if ((BUTTON_COUNTER[3] >= BUTTON_DELAY) && (!INHIBIT[3]))
    {
      CURRENT_MESSAGE = BUT_ESC_MESSAGE;
    }
    INHIBIT[3] = 0;
    BUTTON_COUNTER[3] = 0;
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
        NEW_AD_DATA = 1;
        AD_COUNTER = 0;
      };
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
//  if (PIR1bits.TMR1IF)
};

