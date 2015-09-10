/* 
 * File:   TOTDisplay.h
 * Author: user
 *
 * Created on 2015. szeptember 7., 9:10
 */

#ifndef TOTDISPLAY_H
#define	TOTDISPLAY_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "globalextern.h"
#include "configuration.h"

extern DRAW_STATES DRAW_STATE;
extern s_summa_datas CHAN_SUMMAS[MAX_AD_COUNT];

int TOTDisplay(int number);


#ifdef	__cplusplus
}
#endif

#endif	/* TOTDISPLAY_H */

