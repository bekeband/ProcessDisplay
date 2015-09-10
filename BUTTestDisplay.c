

#include <string.h>
#include "LCDDriver.h"
#include "BUTTestDisplay.h"

int BUTTestDisplay()
{ char buf[20];
  switch (DRAW_STATE)
  {
    case DRAW_INIT:
      LCD_LIGHT = 1;
      LCDSendCmd(DISP_ON);
      LCDSendCmd(CLR_DISP);
      LCDSendCmd(DD_RAM_ADDR);
      LCDSendStr("BUTTON TEST");
      DRAW_STATE = DRAW_RUN;
      LCDSendCmd(DD_RAM_ADDR2);
      break;
    case DRAW_RUN:
      if (CURRENT_MESSAGE != 0)
      {
        LCDSendCmd(CLR_DISP);
        LCDSendStr("BUTTON TEST");
        LCDSendCmd(DD_RAM_ADDR2);
        switch (CURRENT_MESSAGE)
        {
          case BUT_UP_MESSAGE:
            strcpy(buf, "BUT_UP");
            break;
          case BUT_DN_MESSAGE:
            strcpy(buf, "BUT_DN");
            break;
          case BUT_ENT_MESSAGE:
            strcpy(buf, "BUT_ENT");
            break;
          case BUT_ESC_MESSAGE:
            strcpy(buf, "BUT_ESC");
            break;
        };
      CURRENT_MESSAGE = 0;
      LCDSendStr(buf);
      }
  }
}
