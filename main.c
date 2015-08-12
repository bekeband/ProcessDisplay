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
/* EEPROM initialize. If define EEPROM_INIT then initialize EEPROM contents with
 general setting values. (The burning time mostly long than without EEPROM initialize.)*/

#if defined (EEPROM_INIT)
  #include "EEPROM_D.h"
#endif
#include "crc.h"

/* All channes datas to initialize features. */
u_chan_datas CHAN_FEATS[MAX_AD_COUNT];
s_summa_datas CHAN_SUMMAS[MAX_AD_COUNT];

static DRAW_STATES DRAW_STATE = DRAW_INIT;

uint16_t CHECKSUM;

unsigned int old_ad_value;
float new_data;

const char* dim_table[] = {"m3/sec", "m3/min", "m3/hr", "kg/sec", "kg/min", "kg/hr", "t/sec", "t/min", "t/hr", "mm3", "cm3", "m3",
  "gramm", "dkg", "kg", "q", "t", "sec", "mins", "hours", "days", "week", "mount", "years", "mm", "cm", "m", "km"};

void ReadDataEEP(char* d_ptr, int baddr, int size)
{ int i;
  for (i=0; i < size; i++)
  {
    *d_ptr++ = Read_b_eep(baddr++);
  }
}

void WriteDataEEP(unsigned char* d_ptr, int baddr, int size)
{int i;
  for (i = 0; i < size; i++)
  {
    Write_b_eep(baddr++, *d_ptr++);
  }
}

void InitTimers()
{

  T5GCON  = 0b00000000;
  TMR5L   = TMR5LVAL;
  TMR5H   = TMR5HVAL;

  T5CONbits.T5RD16 = 1;     /* Enables register read/write of Timer1/3/5 in one 16-bit operation */
  T5CONbits.T5SYNC = 1;
  T5CONbits.TMR5CS = 0b01;  /* Fosc/4 instruction clock source.*/
  T5CONbits.T5CKPS = 0b11;  /*  11 = 1:8 Prescale value 10 = 1:4 Prescale value
                                01 = 1:2 Prescale value 00 = 1:1 Prescale value */
  T5CONbits.TMR5ON = 1;

}

void InitButtons()
{

  TRISB   = 0b11110001;  // RB4, RB5, RB6, RB7 inputs
  PORTE = 0;
  LATE = 0;
  ANSELE = 0;
  TRISE = 0b101;
}

void InitAD()
{
  OpenADC( ADC_FOSC_32 & ADC_RIGHT_JUST & ADC_12_TAD, ADC_CH0 & ADC_INT_ON, 15 );
  ADC_INT_ENABLE();
}

void InitADValues()
{ int i;
  for (i = 0; i < MAX_AD_COUNT; i++)
  {
    CHAN_FEATS[i].eeprom_datas.input_dim = 0;     // input dim %
    CHAN_FEATS[i].eeprom_datas.input_type = 0;    // input type 4-20 mA
    CHAN_FEATS[i].eeprom_datas.min_eng = 0;       //6547;    // MIN. input integer value
    CHAN_FEATS[i].eeprom_datas.max_eng = 32736;   // MAX. input integer value
    CHAN_FEATS[i].eeprom_datas.min_val = 0;       //
    CHAN_FEATS[i].eeprom_datas.max_val = 11.11;     //
  }
}

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

void MainDisplay()
{ char buf[20];

  switch (DRAW_STATE)
  {
    case DRAW_INIT:
      DelayDisplayValue(40);
      LCDSendCmd(CLR_DISP);

      DRAW_STATE = DRAW_RUN;
      
      break;
    case DRAW_RUN:

     if (NEW_SUMMA_COUNT)
          {
            new_data = GetADValue(0);
            CHAN_SUMMAS[0].sum_1 += new_data;
            LCDSendCmd(DD_RAM_ADDR2);
            sprintf(buf, "%12.1f  %s", CHAN_SUMMAS[0].sum_1, dim_table[CHAN_SUMMAS[0].sum_dim]);
            LCDSendStr(buf);
            NEW_SUMMA_COUNT = 0;
          }

      if (NEW_AD_DATA)
        {
          NEW_AD_DATA = 0;
 
        if (AD_DATA[0] != old_ad_value)
        {
          old_ad_value = AD_DATA[0];
          new_data = GetADValue(0);
          LCDSendCmd(DD_RAM_ADDR);
          sprintf(buf, "%9.2f %s", new_data, dim_table[CHAN_FEATS[0].eeprom_datas.input_dim]);
          LCDSendStr(buf);
        }
      };
      break;
    case DRAW_EXIT:

      break;
  }
};

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

int ADTestDisplay(int channel)
{ char buf[20];
  switch (DRAW_STATE)
  {
    case DRAW_INIT:
      LCD_LIGHT = 1;
      LCDSendCmd(DISP_ON);
      LCDSendCmd(CLR_DISP);
      LCDSendCmd(CUR_ON_UNDER);
      LCDSendCmd(DD_RAM_ADDR);

      LCDSendStr("CHN ");
      LCDSendCmd(DD_RAM_ADDR2);
      LCDSendStr("VAL = ");
      old_ad_value = -30000;
      DRAW_STATE = DRAW_RUN;
      break;
    case DRAW_RUN:
      if (NEW_AD_DATA)
        {
          NEW_AD_DATA = 0;

        if (AD_DATA[channel] != old_ad_value)
        {
          old_ad_value = AD_DATA[channel];
          new_data = GetADValue(channel);
          LCDSendCmd(DD_RAM_ADDR + 4);
          sprintf(buf, "%1i = %5i", channel, old_ad_value);
          LCDSendStr(buf);
          LCDSendCmd(DD_RAM_ADDR2 + 4);
          sprintf(buf, "%7.3f", new_data);
          LCDSendStr(buf);
        }
      };

      break;

    case DRAW_EXIT:

      break;
  }
  return  1;
}

int WelcomeScreen()
{ char buf[20];
//  uint16_t EEPROM_SIZE =
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

    DelayDisplayValue(40);

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
  LED2 = 0;
  
  ei();

  ResetADBuffers();

  InitButtons();
  /* Flash check, and rewrite if did'nt OK. */
  WelcomeScreen();

  DRAW_STATE = DRAW_INIT;

  while (1){

/*    if (CURRENT_MESSAGE == BUT_UP_LONG)
    {
      LED1 = !LED1;
      CURRENT_MESSAGE = 0;
    } else
      if (CURRENT_MESSAGE == BUT_UP_MESSAGE)
      {
        LED2 = !LED2;
        CURRENT_MESSAGE = 0;
      }*/
    MainDisplay();
//    BUTTestDisplay();
//    ADTestDisplay(0);
  }
  return (EXIT_SUCCESS);
}

