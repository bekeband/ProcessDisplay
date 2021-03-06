/* 
 * File:   configuration.h
 * Author: user
 *
 * Created on 2015. m�jus 25., 11:41
 */

#ifndef CONFIGURATION_H
#define	CONFIGURATION_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <xc.h>
#include <stdint.h>

#define TYPE_LETTER "PD"
#define TYPE_NUMBER 786

  /* Version constants. */
#define VER_H 2
#define VER_L 5

#define _XTAL_FREQ  20000000.0
#define FOSCP4  (_XTAL_FREQ / 4)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG1H
#pragma config FOSC = HSHP      // Oscillator Selection bits (HS oscillator (high power > 16 MHz))
#pragma config PLLCFG = OFF     // 4X PLL Enable (Oscillator used directly)
#pragma config PRICLKEN = ON    // Primary clock enable bit (Primary clock is always enabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRTEN = ON      // Power-up Timer Enable bit (Power up timer enabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
#pragma config BORV = 190       // Brown Out Reset Voltage bits (VBOR set to 1.90 V nominal)

// CONFIG2H
#pragma config WDTEN = OFF      // Watchdog Timer Enable bits (Watch dog timer is always disabled. SWDTEN has no effect.)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC1  // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<5:0> pins are configured as digital I/O on Reset)
#pragma config CCP3MX = PORTB5  // P3A/CCP3 Mux bit (P3A/CCP3 input/output is multiplexed with RB5)
#pragma config HFOFST = ON      // HFINTOSC Fast Start-up (HFINTOSC output and ready status are not delayed by the oscillator stable status)
#pragma config T3CMX = PORTC0   // Timer3 Clock input mux bit (T3CKI is on RC0)
#pragma config P2BMX = PORTD2   // ECCP2 B output mux bit (P2B is on RD2)
#pragma config MCLRE = EXTMCLR  // MCLR Pin Enable bit (MCLR pin enabled, RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = ON         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled if MCLRE is also 1)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection Block 0 (Block 0 (000800-003FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection Block 1 (Block 1 (004000-007FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection Block 2 (Block 2 (008000-00BFFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection Block 3 (Block 3 (00C000-00FFFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection Block 0 (Block 0 (000800-003FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection Block 1 (Block 1 (004000-007FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection Block 2 (Block 2 (008000-00BFFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection Block 3 (Block 3 (00C000-00FFFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection Block 0 (Block 0 (000800-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection Block 1 (Block 1 (004000-007FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection Block 2 (Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection Block 3 (Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)


#define MAX_AD_COUNT 1
#define DEFAULT_AD_AVERAGE_FACTOR 1024

#define TMR1_LOAD_DATA          65424

  /* ------------------------ DISPLAY DEFINES --------------------------*/
#define		E       LATDbits.LATD2
#define		RW			LATDbits.LATD1
#define		RS			LATDbits.LATD0

#define		CLR_DISP      0x01
#define		DISP_ON       0x0C
#define		DISP_OFF      0x08
#define		CUR_HOME      0x02
#define		CUR_OFF       0x0C
#define 	CUR_ON_UNDER  0x0E
#define		CUR_ON_BLINK  0x0F
#define 	CUR_LEFT      0x10
#define		CUR_RIGHT     0x14
#define		CUR_UP        0x80
#define		CUR_DOWN      0xC0
#define		ENTER         0xC0
#define		DD_RAM_ADDR		0x80
#define		DD_RAM_ADDR2	0xC0
/* -------------------------------------------------------------------------*/

  /* I/O LED defines */
#define		LED1      LATBbits.LATB2
#define		LED2      LATBbits.LATB1
#define   LCD_LIGHT LATDbits.LATD3

#define   BUT_UP    PORTBbits.RB5

#define   BUT_UP_DOWN       0x234
#define   BUT_UP_UP         0x122
#define   BUT_UP_LONG       0x345
  
#define   BUT_DN    PORTBbits.RB4
#define   BUT_DN_DOWN       0x235
#define   BUT_DN_UP         0x765
#define   BUT_DN_LONG       0x564
/* Low power input. */
#define   LOWPOWER_INPUT   PORTBbits.RB0

#define BUTTON_DELAY    2
#define BUTTON_DELAY_2  60
#define INHIBIT_TIME    2
#define BUT_NUMBER      4

#define TMR5LVAL   0xDC
#define TMR5HVAL   0x0B

#define TMR0LVAL  0x8F
#define TMR0HVAL  0xFD

/*#define TMR0LVAL  0x78
#define TMR0HVAL  0xEC*/

#define VERSION_DELAY   100

#define MIN00MA 0000
#define VAL00MA 100 
#define MAX00MA 582
#define MIN3_6MA  4770
#define MIN04MA 5200  // 5707
#define VAL04MA 5309
#define MAX04MA 5450  // 5940
#define MIN20MA 26000 // 28536
#define VAL20MA 26660
#define MAX20MA 27500 // 29701
#define MIN22MA 28700
#define VAL22MA 29335 //
#define MAX22MA 30000
#define ABSMAX  32767

#define MIN_SUMMAS_VALUE  0.0
#define MAX_SUMMAS_VALUE  99999999.0

#define RANGE_MIN 0
#define RANGE_MAX 3.3

typedef union  {
struct s_eeprom_data {
  uint8_t input_type; // 0 = 4 - 20 mA, 1 = 0- 20 mA
  float min_val;
  float max_val;
  uint16_t min_eng;
  uint16_t max_eng;
  uint8_t input_dim;
} eeprom_datas;
unsigned char chan_datas[32];
} u_chan_datas;

typedef struct {
  float sum_1;
  float sum_2;
  uint8_t sum_dim;
} s_summa_datas;

typedef union {
    struct {
        unsigned UP                     :1;
        unsigned DN                     :1;
    };
    unsigned BT                         :8;
} BUTTONS_T;

typedef enum
{
  EXTREME_LOW = 0,
  FAILSAFE_LOW = 1,
  NORMAL = 2,
  FAILSAFE_HIGH = 3,
  EXTREME_HIGH = 4
} AD_VAL_ERROR;

typedef char s_dim_text[7];

/* Exter declaration of interrupt.c. */
void ResetADBuffers();

#ifdef	__cplusplus
}
#endif

#endif	/* CONFIGURATION_H */

