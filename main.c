/* 
 * File:   main.c
 * Author: user
 *
 * Created on 2015. m�jus 25., 11:40
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
#include "ADSetDisplay.h"
#include "TOTDisplay.h"
#include "tables.h"

/* EEPROM initialize. If define EEPROM_INIT then initialize EEPROM contents with
 general setting values. (The burning time mostly long than without EEPROM initialize.)*/

#if defined (EEPROM_INIT)
  #include "EEPROM_D.h"
#endif


/* All channes datas to initialize features. */
u_chan_datas CHAN_FEATS[MAX_AD_COUNT];
s_summa_datas CHAN_SUMMAS[MAX_AD_COUNT];

DRAW_STATES DRAW_STATE = DRAW_INIT;
DISPLAY_STATES CURRENT_DISPLAY = MAIN_DISPLAY;


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
        sprintf(buf, "%10.0f  %s", CHAN_SUMMAS[0].sum_1, dim_table[CHAN_SUMMAS[0].sum_dim]);
        LCDSendStr(buf);
        NEW_SUMMA_COUNT = 0;
     }

     PrintMeasValue(DD_RAM_ADDR, 2, 0);

      if (CURRENT_MESSAGE == BUT_DN_LONG)
      {
        CURRENT_DISPLAY = ADSET_DISPLAY;
        CURRENT_MESSAGE = 0;
        DRAW_STATE = DRAW_INIT;
      }

     if (CURRENT_MESSAGE == BUT_UP_LONG)
     {
       CURRENT_DISPLAY = TOTALIZER_DISPLAY;
       CURRENT_MESSAGE = 0;
       DRAW_STATE = DRAW_INIT;
     }
    break;
    case DRAW_EXIT:
    break;
  }
}

void ClearSummas(int channel)
{ CHAN_SUMMAS[channel].sum_1 = 0; CHAN_SUMMAS[channel].sum_2 = 0; CHAN_SUMMAS[channel].sum_dim = 11; }

void ClearAllSummas()
{ int i; for (i = 0; i < MAX_AD_COUNT; i++) { ClearSummas(i); } }

void SetBounds()
{

}

int WelcomeScreen()
{ char buf[20]; int u = 2;
  uint16_t CHECKED_EEPROM_SIZE = sizeof(CHAN_FEATS);
  uint16_t CRC_ADDRESS = _EEPROMSIZE - 2; // Last word the CRC checksum.
  uint16_t CALCCHECKSUM;
  uint16_t STOREDCHECKSUM;

  LCD_LIGHT = 1;
  LCDSendCmd(DISP_ON);
  LCDSendCmd(CLR_DISP);
  LCDSendCmd(DD_RAM_ADDR);
  sprintf(buf, "Type: %s %i",  TYPE_LETTER, TYPE_NUMBER);
  LCDSendStr(buf);
  LCDSendCmd(DD_RAM_ADDR2);

  sprintf(buf, "VERSION: %i.%i",  VER_H, VER_L);

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

int main(int, char** ) {
  
  InitTimers();

  InitLCD();
  InitAD();
  InitADValues();
  
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
        if (ADSetDisplay(0))
        {
          WriteDataEEP((unsigned char*) &CHAN_FEATS, 0, sizeof(CHAN_FEATS));
        }
      break;
      case TOTALIZER_DISPLAY:
        if (TOTDisplay(0))
        {
          
        }
        break;
    }
  CURRENT_MESSAGE = 0;
  }
  return (EXIT_SUCCESS);
}

