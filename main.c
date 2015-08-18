/* 
 * File:   main.c
 * Author: user
 *
 * Created on 2015. május 25., 11:40
 */

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <plib.h>
#include <string.h>
#include <stdint.h>
#include "configuration.h"
#include "LCDDriver.h"
#include "globalextern.h"
#include "init.h"
#include "crc.h"

/* EEPROM initialize. If define EEPROM_INIT then initialize EEPROM contents with
 general setting values. (The burning time mostly long than without EEPROM initialize.)*/

#if defined (EEPROM_INIT)
  #include "EEPROM_D.h"
#endif


/* All channes datas to initialize features. */
u_chan_datas CHAN_FEATS[MAX_AD_COUNT];
s_summa_datas CHAN_SUMMAS[MAX_AD_COUNT];

static DRAW_STATES DRAW_STATE = DRAW_INIT;
static DISPLAY_STATES CURRENT_DISPLAY = MAIN_DISPLAY;
static ADSET_STATES ADSET_STATE = AD_CHECK;


const char* dim_table[] = {"m3/sec", "m3/min", "m3/hr", "kg/sec", "kg/min", "kg/hr", "t/sec", "t/min", "t/hr", "mm3", "cm3", "m3",
  "gramm", "dkg", "kg", "q", "t", "sec", "mins", "hours", "days", "week", "mount", "years", "mm", "cm", "m", "km"};

const char* format_table[] = {"%9.3f %s", "%5i"};


int WriteStandardDatasToEEPROM()
{
  InitADValues();
  WriteDataEEP((unsigned char*) &CHAN_FEATS, 0, sizeof(CHAN_FEATS));
  return 0;
}

float GetADValue(int num)
{
  float tgns = CHAN_FEATS[num].eeprom_datas.max_val / (CHAN_FEATS[0].eeprom_datas.max_eng - CHAN_FEATS[0].eeprom_datas.min_eng);
  int rval = ((AD_DATA[num] - CHAN_FEATS[num].eeprom_datas.min_eng));
  return (rval * tgns) + CHAN_FEATS[num].eeprom_datas.min_val ;
}

void DelayDisplayValue(int delay_value)
{
  TIMER_COUNTER_VALUE = 0;
  while (TIMER_COUNTER_VALUE < delay_value);
}

int ReadFlashMemory()
{
  WriteDataEEP((unsigned char*) &CHAN_FEATS, 0, sizeof(CHAN_FEATS));
  return 0;
}

void PrintRawValue(int ramaddr, int pos, int channel)
{char buf[20];
  if (NEW_AD_CHANGES[0])
  {
    LCDSendCmd(ramaddr + pos);
    sprintf(buf, format_table[1], AD_DATA[channel]);
    LCDSendStr(buf);
    NEW_AD_CHANGES[0] = 0;
  }
}

void PrintMeasValue(int ramaddr, int pos, int channel)
{ char buf[20];
  if (NEW_AD_CHANGES[channel])
  {
    LCDSendCmd(ramaddr + pos);
    sprintf(buf, format_table[0], GetADValue(channel), dim_table[CHAN_FEATS[channel].eeprom_datas.input_dim]);
    LCDSendStr(buf);
    NEW_AD_CHANGES[channel] = 0;
  }
}

void MainDisplay()
{ char buf[20];
  switch (DRAW_STATE)
  {
    case DRAW_INIT:
      LCDSendCmd(CLR_DISP);

      DRAW_STATE = DRAW_RUN;
      SetADChanges(1);
      break;

    case DRAW_RUN:

     if (NEW_SUMMA_COUNT)
      {
        CHAN_SUMMAS[0].sum_1 += GetADValue(0);
        LCDSendCmd(DD_RAM_ADDR2);
        sprintf(buf, "%12.1f  %s", CHAN_SUMMAS[0].sum_1, dim_table[CHAN_SUMMAS[0].sum_dim]);
        LCDSendStr(buf);
        NEW_SUMMA_COUNT = 0;
     }

     PrintMeasValue(DD_RAM_ADDR, 2, 0);

      if (CURRENT_MESSAGE == BUT_DN_LONG)
      {
        CURRENT_MESSAGE = 0;
        DRAW_STATE = DRAW_EXIT;
      }

      break;

    case DRAW_EXIT:
      CURRENT_DISPLAY = ADSET_DISPLAY;
      CURRENT_MESSAGE = 0;
      DRAW_STATE = DRAW_INIT;
      break;
      
  }
}

void IOTestDisplay()
{

}

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

void ClearSummas(int channel)
{
  CHAN_SUMMAS[channel].sum_1 = 0;
  CHAN_SUMMAS[channel].sum_2 = 0;
  CHAN_SUMMAS[channel].sum_dim = 11;
}

void ClearAllSummas()
{ int i;
  for (i = 0; i < MAX_AD_COUNT; i++)
  {
    ClearSummas(i);
  }
}

void PrintButtonLabels(const char* BUP, const char* BDN)
{
  LCDSendCmd(DD_RAM_ADDR + 13);
  LCDSendStr(BUP);
  LCDSendCmd(DD_RAM_ADDR2 + 13);
  LCDSendStr(BDN);
}

/* AD low, and high level setting display. */

int ADSetDisplay(int channel)
{ //char buf[20];
  switch (DRAW_STATE)
  {
    case DRAW_INIT:
      LCD_LIGHT = 1;
      LCDSendCmd(DISP_ON);
      LCDSendCmd(CLR_DISP);
      LCDSendCmd(DD_RAM_ADDR);
      if (CHAN_FEATS[channel].eeprom_datas.input_type == 0)
      { LCDSendStr("LOW 4 mA "); } else
      { LCDSendStr("LOW 0 mA "); }
      LCDSendCmd(DD_RAM_ADDR2);

      LCDSendStr("VAL =  ");

      DRAW_STATE = DRAW_RUN;
      ADSET_STATE = AD_CHECK;
      PrintButtonLabels("ESC", "OK");
      SetADChanges(1);
      break;
    case DRAW_RUN:

      switch (ADSET_STATE)
      {
        case AD_CHECK:
        /* RAW value data print the 2. line.*/
        PrintRawValue(DD_RAM_ADDR2, 6, 0);
        
          if (CURRENT_MESSAGE == BUT_DN_MESSAGE)
          {
            CURRENT_MESSAGE = 0;
            ADSET_STATE = LOW_SET;
          };
          if (CURRENT_MESSAGE == BUT_UP_MESSAGE)
          { /* Exit the setting program. */
            CURRENT_MESSAGE = 0;
            DRAW_STATE = DRAW_EXIT;
          };
          break;
          case LOW_SET:

            CURRENT_MESSAGE = 0;
            CHAN_FEATS[channel].eeprom_datas.min_eng = AD_DATA[channel];
            ADSET_STATE = HIGH_INIT;
            PrintButtonLabels("ESC", "HIG");  // To high setting.

          if (CURRENT_MESSAGE == BUT_DN_MESSAGE)
          {
            CURRENT_MESSAGE = 0;
            ADSET_STATE = HIGH_INIT;
          };
          if (CURRENT_MESSAGE == BUT_UP_MESSAGE)
          { /* Exit the setting program. */
            CURRENT_MESSAGE = 0;
            DRAW_STATE = DRAW_EXIT;
          };
            break;
        case HIGH_INIT:

            LCDSendCmd(DD_RAM_ADDR);
            LCDSendStr("HIGH 20 mA ");
            LCDSendCmd(DD_RAM_ADDR2);

            LCDSendStr("VAL =  ");

            DRAW_STATE = DRAW_RUN;
            ADSET_STATE = HIGH_CHECK;
            PrintButtonLabels("ESC", "OK ");
            SetADChanges(1);

          break;
        case HIGH_CHECK:
        /* RAW value data print the 2. line.*/
        PrintRawValue(DD_RAM_ADDR2, 6, 0);

          if (CURRENT_MESSAGE == BUT_DN_MESSAGE)
          {
            CURRENT_MESSAGE = 0;
            ADSET_STATE = HIGH_SET;
          };
          if (CURRENT_MESSAGE == BUT_UP_MESSAGE)
          { /* Exit the setting program. */
            CURRENT_MESSAGE = 0;
            DRAW_STATE = DRAW_EXIT;
          };
          break;
        case HIGH_SET:
            CHAN_FEATS[channel].eeprom_datas.max_eng = AD_DATA[channel];
            ADSET_STATE = HIGH_CHECK;
            PrintButtonLabels("", "OK");  // To end.

          if (CURRENT_MESSAGE == BUT_DN_MESSAGE)
          {
            CURRENT_MESSAGE = 0;
            DRAW_STATE = DRAW_EXIT;
          };

          break;
      }

      break;

    case DRAW_EXIT:
      CURRENT_DISPLAY = MAIN_DISPLAY;
      CURRENT_MESSAGE = 0;
      DRAW_STATE = DRAW_INIT;
      break;
  }
  return  1;
}

int WelcomeScreen()
{ char buf[20];
  uint16_t CHECKED_EEPROM_SIZE = sizeof(CHAN_FEATS);
  uint16_t CRC_ADDRESS = _EEPROMSIZE - 2; // Last word the CRC checksum.
  uint16_t CALCCHECKSUM;
  uint16_t STOREDCHECKSUM;

  LCD_LIGHT = 1;
  LCDSendCmd(DISP_ON);
  LCDSendCmd(CLR_DISP);
  LCDSendCmd(DD_RAM_ADDR);
  LCDSendStr("Process Display");
  LCDSendCmd(DD_RAM_ADDR2);
  sprintf(buf, "VERSION: %i.%i", VER_H, VER_L);
  LCDSendStr(buf);

  do
  {

    DelayDisplayValue(80);

    LCDSendCmd(CLR_DISP);
    LCDSendCmd(DD_RAM_ADDR);
    LCDSendStr("Check EEPROM CRC");
    LCDSendCmd(DD_RAM_ADDR2);
    CALCCHECKSUM = gen_crc16(Read_b_eep, CHECKED_EEPROM_SIZE, 0);
    ReadDataEEP((char*) &STOREDCHECKSUM, CRC_ADDRESS, 2);
    if (CALCCHECKSUM == STOREDCHECKSUM)
      {
        ReadDataEEP((char*)&CHAN_SUMMAS, CHECKED_EEPROM_SIZE, sizeof(CHAN_SUMMAS));
        sprintf(buf, "CRC = %04X OK!", CALCCHECKSUM);
      } else
      {
      /* BAD checksum. The crc checksum is bad. The program will dig in the endless loop. */
        LCDSendCmd(CLR_DISP);
        LCDSendCmd(DD_RAM_ADDR);
        sprintf(buf, "CRC = %04X BAD!", CALCCHECKSUM);
        LCDSendStr(buf);
        LCDSendCmd(DD_RAM_ADDR2);
        sprintf(buf, "PUSH ENT TO DEF.");
        LCDSendStr(buf);
        /* Push enter to load the default values to EEPROM. */
        while (CURRENT_MESSAGE != BUT_DN_LONG);
        CURRENT_MESSAGE = 0;
        LCDSendCmd(CLR_DISP);

        LCDSendCmd(DD_RAM_ADDR);
        LCDSendStr("Write default");
        LCDSendCmd(DD_RAM_ADDR2);
        LCDSendStr("datas to FLASH!");
        WriteStandardDatasToEEPROM();
        CALCCHECKSUM = gen_crc16(Read_b_eep, CHECKED_EEPROM_SIZE, 0);
        WriteDataEEP((char*)&CALCCHECKSUM, CRC_ADDRESS, 2);
        ClearAllSummas();
        WriteDataEEP((char*)&CHAN_SUMMAS, CHECKED_EEPROM_SIZE, sizeof(CHAN_SUMMAS)); 
      }
  }while (CALCCHECKSUM != STOREDCHECKSUM);

  return 0;
}

int main(int argc, char** argv) {

  InitTimers();

  InitLCD();
  InitAD();
  InitADValues();
//  LED2 = 0;
  
  ei();

  ResetADBuffers();

  InitButtons();
  /* Flash check, and rewrite if did'nt OK. */
  WelcomeScreen();

  DRAW_STATE = DRAW_INIT;

  while (1){
    switch (CURRENT_DISPLAY)
    {
      case MAIN_DISPLAY:
        MainDisplay();
      break;
      case ADSET_DISPLAY:
        ADSetDisplay(0);
      break;
    }
  CURRENT_MESSAGE = 0;
  }
  return (EXIT_SUCCESS);
}

