
#include <stdio.h>
#include "LCDDriver.h"
#include "configuration.h"
#include "TOTDisplay.h"
#include "tables.h"
#include "ADSetDisplay.h"
#include "globalextern.h"

int TOTDisplay(int number)
{ char buf[20];
  switch (DRAW_STATE)
  {
    /* Totalizer reset display. */
    case DRAW_INIT:
      LCDSendCmd(DISP_ON);
      LCDSendCmd(CLR_DISP);
      LCDSendCmd(DD_RAM_ADDR);
      sprintf(buf, "%8.0f %s", CHAN_SUMMAS[number].sum_1, dim_table[CHAN_SUMMAS[number].sum_dim]);
      LCDSendStr(buf);
      LCDSendCmd(DD_RAM_ADDR2);
      sprintf(buf, "CLEAR T%i ?", number);
      LCDSendStr(buf);
      PrintButtonLabels("ESC", "YES");
      DRAW_STATE = DRAW_RUN;
      break;

    case DRAW_RUN:
      if (CURRENT_MESSAGE == BUT_DN_MESSAGE)
        {
          CHAN_SUMMAS[number].sum_1 = 0;
          CURRENT_MESSAGE = 0;
          DRAW_STATE = DRAW_EXIT;
        };
      if (CURRENT_MESSAGE == BUT_UP_MESSAGE)
        { /* Exit the setting program. */
          CURRENT_MESSAGE = 0;
          DRAW_STATE = DRAW_EXIT;
        };
      
      break;
    case DRAW_EXIT:
        CURRENT_DISPLAY = MAIN_DISPLAY;
        DRAW_STATE = DRAW_INIT;
      break;
  };
}

