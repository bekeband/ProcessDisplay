/* 
 * File:   init.h
 * Author: user
 *
 * Created on 2015. augusztus 14., 12:43
 */

#ifndef INIT_H
#define	INIT_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <xc.h>
#include <plib.h>
#include "configuration.h"
#include "LCDDriver.h"
#include "globalextern.h"

extern u_chan_datas CHAN_FEATS[MAX_AD_COUNT];
extern s_summa_datas CHAN_SUMMAS[MAX_AD_COUNT];

void ReadDataEEP(char* d_ptr, int baddr, int size);
void WriteDataEEP(unsigned char* d_ptr, int baddr, int size);
void InitTimers();
void InitButtons();
void InitAD();
void InitADValues();
void SetADChanges(uint8_t new_value);
int   mysprintf(char *, const char *, ...);

#ifdef	__cplusplus
}
#endif

#endif	/* INIT_H */

