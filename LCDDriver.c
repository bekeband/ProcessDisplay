
#include "configuration.h"
#include "LCDDriver.h"

/* 4 bit LCD driver. */

//Just simple delay
void DelayMs(unsigned long cntr) {
  int i;
  for (i = 0; i < cntr; i++)
  {
    __delay_ms(1);
  }
}

void E_Pulse(void) {
	E=1;
	asm("nop");
	asm("nop");
	asm("nop");
	E=0;
}

// Init LCD  after reset
void InitLCD(void) {

	// all LCD port is output
	PORTD=0;
	TRISD=0;

	RS=0;
	RW=0;
	DelayMs(110);
	LATD=0b00110000;
	E_Pulse();
	DelayMs(10);
	LATD=0b00110000;
	E_Pulse();
	DelayMs(10);
	LATD=0b00110000;
	E_Pulse();
	DelayMs(10);
	LATD=0b00100000;
	E_Pulse();
}

// Send char to LCD
LCDSendChar(unsigned char c) {
  unsigned char data;
	DelayMs(2);
	// get upper nibble
	data = c & 0b11110000;
	// send data to LCD
	LATD = ((LATD & 0b00001111) | data);
	// set LCD to write
	RW=0;
	// set LCD to data mode
	RS=1;
	E_Pulse();
	// get lower nibble
	data = c & 0b00001111;
	data = data << 4;
	LATD = ((LATD & 0b00001111) | data);
	// set LCD to write
	RW=0;
	// set LCD to data mode
	RS=1;
	E_Pulse();
}

// Send command to LCD
void LCDSendCmd(unsigned char c) {
  unsigned char data;
	DelayMs(2);
    // get upper nibble
	data = c & 0b11110000;
	// send data to LCD
	LATD = ((LATD & 0b00001111) | data);
	// set LCD to write
	RW=0;
	// set LCD to data mode
	RS=0;
	E_Pulse();
	// get lower nibble
	data = c & 0b00001111;
	data = data << 4;
	LATD = ((LATD & 0b00001111) | data);
	// set LCD to write
	RW=0;
	// set LCD to data mode
	RS=0;
	E_Pulse();
}

void LCDSendStr(const char* str) {
  int i = 0;
	while(str[i]) {
		LCDSendChar(str[i]);
		i++;
	}
}
