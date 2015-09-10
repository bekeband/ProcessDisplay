
#include <stdio.h>
#include "LCDDriver.h"
#include "ADSetDisplay.h"
#include "init.h"
#include "tables.h"

static ADSET_STATES ADSET_STATE = AD_SET_INIT;

static uint16_t last_ad_data;

int PrintRawValue(int ramaddr, int pos, int channel)
{char buf[20];
  if (NEW_AD_CHANGES[0])
  {
    LCDSendCmd(ramaddr + pos);
    last_ad_data = AD_DATA[channel];
    sprintf(buf, format_table[1], last_ad_data);
    LCDSendStr(buf);
    NEW_AD_CHANGES[0] = 0;
    return 1;
  }
return 0;
}

int PrintThresholdValue(int ramaddr, int pos, int minvalue, int maxvalue)
{ int retval = 0;
/*  if (newvalue)
  {*/
    LCDSendCmd(ramaddr + pos);
    if (last_ad_data < minvalue)
    {
      LCDSendStr("LOW");
    } else if (last_ad_data > maxvalue)
    {
      LCDSendStr("HIG");
    } else
    {
      LCDSendStr("FIT");
      retval = 1;
    }
/*  }*/
return retval;
}

void PrintButtonLabels(const char* BUP, const char* BDN)
{ LCDSendCmd(DD_RAM_ADDR + 13); LCDSendStr(BUP);
  LCDSendCmd(DD_RAM_ADDR2 + 13); LCDSendStr(BDN);
}

void PrintUPButton(const char* BUP)
{
  LCDSendCmd(DD_RAM_ADDR + 13); LCDSendStr(BUP);
}

void PrintDNButton(const char* BDN)
{
  LCDSendCmd(DD_RAM_ADDR2 + 13); LCDSendStr(BDN);
}

char GetmAChar(uint8_t i)
{
  if (i) return '0';
  else return '4';
}

void PrintLowmA(uint8_t i)
{
if (i == 0) { LCDSendStr("LOW 4 mA "); } else { LCDSendStr("LOW 0 mA "); };
}


/* AD low, and high level setting display. A/D corrections. */

int ADSetDisplay(int channel)
{ char buf[20]; int is_value_ok; int retval = 0;
  static uint16_t new_minval, new_maxval;
  switch (ADSET_STATE)
  {
    /* Initialize display for A/D check manu. */
    case AD_SET_INIT:
      LCDSendCmd(DISP_ON);
      LCDSendCmd(CLR_DISP);
      LCDSendCmd(DD_RAM_ADDR);
      sprintf(buf, "LOW %c mA", GetmAChar(CHAN_FEATS[channel].eeprom_datas.input_type));
      LCDSendStr(buf);
      LCDSendCmd(DD_RAM_ADDR2);

      ADSET_STATE = AD_CHECK;
      PrintUPButton("ESC");
      SetADChanges(1);
    break;
    /* AD check to LOW value. */
    case AD_CHECK:
      /* A/D check to low value.*/
      if (PrintRawValue(DD_RAM_ADDR2, 2, 0))
      {
        if (PrintThresholdValue(DD_RAM_ADDR2, 8, MIN04MA, MAX04MA))
        {
          PrintDNButton("OK");
          is_value_ok = 1;
        } else
        {
          PrintDNButton("--");
          is_value_ok = 0;
        }
      }
      if ((CURRENT_MESSAGE == BUT_DN_MESSAGE) && (is_value_ok))
        {
          CURRENT_MESSAGE = 0;
          new_minval = last_ad_data;
          ADSET_STATE = HIGH_INIT;
        };
      if (CURRENT_MESSAGE == BUT_UP_MESSAGE)
        { /* Exit the setting program. */
          CURRENT_MESSAGE = 0;
          ADSET_STATE = AD_SET_EXIT;
        };
    break;
    case HIGH_INIT:

      LCDSendCmd(DD_RAM_ADDR);
      LCDSendStr("HIGH 20 mA ");
      LCDSendCmd(DD_RAM_ADDR2);

      ADSET_STATE = HIGH_CHECK;
      PrintUPButton("ESC");
      SetADChanges(1);

    break;
    case HIGH_CHECK:
      /* A/D check to low value.*/
      if (PrintRawValue(DD_RAM_ADDR2, 2, 0))
      {
        if (PrintThresholdValue(DD_RAM_ADDR2, 8, MIN20MA, MAX20MA))
        {
          PrintDNButton("OK");
          is_value_ok = 1;
        } else
        {
          PrintDNButton("--");
          is_value_ok = 0;
        }
      }
      
    if ((CURRENT_MESSAGE == BUT_DN_MESSAGE) && (is_value_ok))
      {
        CURRENT_MESSAGE = 0;
        ADSET_STATE = HIGH_SET;
      };
    if (CURRENT_MESSAGE == BUT_UP_MESSAGE)
      { /* Exit the setting program. */
        CURRENT_MESSAGE = 0;
        ADSET_STATE = AD_SET_EXIT;
      };
    break;
    case HIGH_SET:
      new_maxval = last_ad_data;
      LCDSendCmd(DD_RAM_ADDR);
      sprintf(buf, "%2cmA: %5u", GetmAChar(CHAN_FEATS[channel].eeprom_datas.input_type), new_minval);
      LCDSendStr(buf);
      LCDSendCmd(DD_RAM_ADDR2);
      sprintf(buf, "20mA: %5u", new_maxval);
      LCDSendStr(buf);
      PrintButtonLabels("ESC", "OK ");

      if (CURRENT_MESSAGE == BUT_DN_MESSAGE)
        {
          CHAN_FEATS[channel].eeprom_datas.min_eng = new_minval;
          CHAN_FEATS[channel].eeprom_datas.max_eng = new_maxval;
          CURRENT_MESSAGE = 0;
          ADSET_STATE = AD_SET_EXIT;
          retval = 1;
        };

      if (CURRENT_MESSAGE == BUT_UP_MESSAGE)
        {
          CURRENT_MESSAGE = 0;
          ADSET_STATE = AD_SET_EXIT;
        };

    break;
    case AD_SET_EXIT:
      CURRENT_DISPLAY = MAIN_DISPLAY;
      CURRENT_MESSAGE = 0;
      DRAW_STATE = DRAW_INIT;
      ADSET_STATE = AD_SET_INIT;
      break;
  }
  return retval;
}

