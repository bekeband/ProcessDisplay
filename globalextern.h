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

  struct s_ADENG_VALUE {
    int mineng;
    int maxeng;
    float minvalue;
    float maxvalue;
  };

extern int NEW_AD_DATA;
extern int NEW_SUMMA_COUNT;
extern unsigned long AD_DATA[MAX_AD_COUNT];
extern uint8_t NEW_AD_DATAS[MAX_AD_COUNT];
extern uint8_t NEW_AD_CHANGES[MAX_AD_COUNT];
extern int CURRENT_MESSAGE;
extern int TIMER_COUNTER_VALUE;


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
  ADSET_DISPLAY
} DISPLAY_STATES;

typedef enum
{
  AD_CHECK,
  LOW_SET,
  HIGH_SET
}ADSET_STATES;

#ifdef	__cplusplus
}
#endif

#endif	/* GLOBALEXTERN_H */

