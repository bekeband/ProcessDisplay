
#include <stdarg.h>
#include "init.h"
#include "globalextern.h"

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

//  T0CONbits.TMR0ON

}

void InitButtons()
{
  PORTB = 0;
  TRISB   = 0b11110001;  // RB4, RB5, RB6, RB7 inputs
  PORTE = 0;
  LATE = 0;
  ANSELE = 0;
  TRISE = 0b101;
}

void InitAD()
{
  VREFCON0bits.FVREN = 1;
  VREFCON0bits.FVRS = 0b10;
  while (!VREFCON0bits.FVRST);

  ADCON2bits.ADFM   = 1;      // A/D Conversion Result Format Select bit 1 = Right justified 0 = Left justified
  ADCON2bits.ACQT   = 0b111;  // 111 = 20 TAD
  ADCON2bits.ADCS   = 0b110;  // 110 = FOSC/64
  ADCON1bits.PVCFG  = 0b10;   //

  ADCON0bits.ADON = 1;        //*/
//  OpenADC( ADC_FOSC_32 & ADC_RIGHT_JUST & ADC_12_TAD, ADC_CH0 & ADC_INT_ON, 15 );
  ADC_INT_ENABLE();
}

void InitADValues()
{ int i;
  for (i = 0; i < MAX_AD_COUNT; i++)
  {
    CHAN_FEATS[i].eeprom_datas.input_dim = 0;     // input dim %
    CHAN_FEATS[i].eeprom_datas.input_type = 0;    // input type 4-20 mA
    CHAN_FEATS[i].eeprom_datas.min_eng = VAL04MA;       //6547;    // MIN. input integer value
    CHAN_FEATS[i].eeprom_datas.max_eng = VAL20MA;   // MAX. input integer value
    CHAN_FEATS[i].eeprom_datas.min_val = 0;       //
    CHAN_FEATS[i].eeprom_datas.max_val = 11.11;     //
  }
}

void SetADChanges(uint8_t new_value)
{ int i;
  for (i = 0; i < MAX_AD_COUNT; i++)
  {
    NEW_AD_CHANGES[i] = new_value;
  }
}
