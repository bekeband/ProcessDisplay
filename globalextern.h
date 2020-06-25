/* 
 * File:   globalextern.h
 * Author: user
 *
 * Created on 2015. június 10., 14:53
 */

#ifndef GLOBALEXTERN_H
#define	GLOBALEXTERN_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "configuration.h"

  struct s_ADENG_VALUE {
    int mineng;
    int maxeng;
    float minvalue;
    float maxvalue;
  };

extern volatile int NEW_AD_DATA;
extern volatile int NEW_SUMMA_COUNT;
extern volatile unsigned long AD_DATA[MAX_AD_COUNT];
extern volatile uint8_t NEW_AD_DATAS[MAX_AD_COUNT];
extern volatile uint8_t NEW_AD_CHANGES[MAX_AD_COUNT];
extern volatile int CURRENT_MESSAGE;
extern volatile int TIMER_COUNTER_VALUE;
extern volatile int HOUR_FLAG;


/* MESSAGE_FUNC(int, int) */

typedef int (*MESSAGE_FUNC)(int, int);

/* DRAW_FUNC(void) */

typedef int (*DRAW_FUNC) (int, int);

/*********************************************************************
* DISPLAY draw states
********************************************************************/
typedef enum
{
  DRAW_INIT,
  DRAW_RUN,
  DRAW_ROUND,
  DRAW_TEXT,
  DRAW_TEXT_RUN,
  DRAW_FOCUS,
  DRAW_EXIT
} DRAW_STATES;
typedef enum
{
  MAIN_DISPLAY,
  IOTEST_DISPLAY,
  ADSET_DISPLAY,
  RANGESET_DISPLAY,
  TOTALIZER_DISPLAY
} DISPLAY_STATES;

typedef enum
{
  AD_SET_INIT,
  AD_CHECK,
  LOW_SET,
  HIGH_INIT,
  HIGH_CHECK,
  HIGH_SET,
  AD_SET_EXIT
}ADSET_STATES;


#ifdef	__cplusplus
}
#endif

#endif	/* GLOBALEXTERN_H */

