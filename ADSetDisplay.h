/* 
 * File:   ADSetDisplay.h
 * Author: user
 *
 * Created on 2015. szeptember 1., 12:56
 */

#ifndef ADSETDISPLAY_H
#define	ADSETDISPLAY_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "globalextern.h"
#include "configuration.h"

  /* All channes datas to initialize features. */
extern u_chan_datas CHAN_FEATS[MAX_AD_COUNT];
extern s_summa_datas CHAN_SUMMAS[MAX_AD_COUNT];

extern DISPLAY_STATES CURRENT_DISPLAY;
extern DRAW_STATES DRAW_STATE;

int ADSetDisplay(int channel);
void PrintButtonLabels(const char* BUP, const char* BDN);
void PrintUPButton(const char* BUP);

#ifdef	__cplusplus
}
#endif

#endif	/* ADSETDISPLAY_H */

