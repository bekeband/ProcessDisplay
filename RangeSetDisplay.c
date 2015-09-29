
#include <stdio.h>
#include <math.h>
#include "RangeSetDisplay.h"
#include "ADSetDisplay.h"
#include "LCDDriver.h"

/* AD low, and high level setting display. A/D corrections. */

int RangeSetDisplay(int channel)
{ char buf[20]; 
  static int decimals;
  int retval = 0;
  static float new_minrange;
  static float new_maxrange;
  switch (DRAW_STATE)
  {
    /* Initialize display for A/D check manu. */
    case DRAW_INIT:
      LCDSendCmd(DISP_ON);
      LCDSendCmd(CLR_DISP);
      LCDSendCmd(DD_RAM_ADDR);
      sprintf(buf, "RANGE CHN%u", channel);
      LCDSendStr(buf);
      LCDSendCmd(DD_RAM_ADDR2);
      new_minrange = CHAN_FEATS[channel].eeprom_datas.max_val;
      sprintf(buf, "%8.2f", new_minrange);
      LCDSendStr(buf);
      PrintUPButton(" OK");
      DRAW_STATE = DRAW_ROUND;

    break;
    case  DRAW_ROUND:
//      LCDSendCmd(CUR_HOME);
//      LCDSendCmd(CUR_DOWN);
//      LCDSendCmd(CUR_ON_UNDER);
//      LCDSendCmd(CUR_ON_BLINK);
      DRAW_STATE = DRAW_RUN;
      decimals = 4;
      break;
    /* BUTTON_UP the escape button. */
    case DRAW_RUN:

      switch (CURRENT_MESSAGE)
      {
        case BUT_UP_UP:
            DRAW_STATE = DRAW_EXIT;
            CURRENT_MESSAGE = 0;
          break;
        case BUT_DN_UP:
 //           LCDSendCmd(CUR_RIGHT);
            new_minrange += 0.1;
            if (new_minrange > 4)
            {
              new_minrange = 3;
            };
            LCDSendCmd(DD_RAM_ADDR2);
            sprintf(buf, "%8.2f", new_minrange);
            LCDSendStr(buf);
            CURRENT_MESSAGE = 0;
          break;
      }

        
      break;
    case DRAW_TEXT:
      DRAW_STATE = DRAW_EXIT;
      break;
    case DRAW_EXIT:
        CURRENT_DISPLAY = MAIN_DISPLAY;
        DRAW_STATE = DRAW_INIT;
//        LCDSendCmd(CUR_OFF);
        CHAN_FEATS[channel].eeprom_datas.max_val = new_minrange;
        retval = 1;
      break;
  }
  return retval;
}

