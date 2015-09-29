/* 
 * File:   LCDDriver.h
 * Author: user
 *
 * Created on 2015. m�jus 25., 12:05
 */

#ifndef LCDDRIVER_H
#define	LCDDRIVER_H

#ifdef	__cplusplus
extern "C" {
#endif

  
void InitLCD(void);
void LCDSendStr(const char* str);
void LCDSendCmd(unsigned char c);
void LCDCursorAddress(unsigned char address);

#ifdef	__cplusplus
}
#endif

#endif	/* LCDDRIVER_H */

