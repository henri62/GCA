/*
 Copyright (C) MERG CBUS

 Rocrail - Model Railroad Software

 Copyright (C) Rob Versluis <r.j.versluis@rocrail.net>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */


#ifndef __IO_H
#define __IO_H


#define LED1    PORTBbits.RB6   // cbus activity
#define LED2    PORTBbits.RB7   // learning mode
#define LED3    PORTBbits.RB5   // running

#define SW      PORTAbits.RA2	// Flim switch

#define PORT1   PORTAbits.RA0
#define PORT2   PORTAbits.RA1
#define PORT3   PORTAbits.RA3
#define PORT4   PORTAbits.RA4
#define PORT5   PORTAbits.RA5
#define PORT6   PORTBbits.RB0
#define PORT7   PORTBbits.RB4
#define PORT8   PORTBbits.RB1

#define PORT9   PORTCbits.RC0
#define PORT10  PORTCbits.RC1
#define PORT11  PORTCbits.RC2
#define PORT12  PORTCbits.RC3
#define PORT13  PORTCbits.RC7
#define PORT14  PORTCbits.RC6
#define PORT15  PORTCbits.RC5
#define PORT16  PORTCbits.RC4

#define CLSW    PORT5	// toogle clock 800/2000 switch



extern near unsigned char led1timer;


void setupIO(byte clr);
void doLEDTimers(void);
void doLED250(void);
void doLEDs(void);
unsigned char checkFlimSwitch(void);
unsigned char checkClockSwitch(void);
void doIOTimers(void);
void doFastClock(void);

extern void fc2ee(void);
extern void ee2fc(void);

typedef struct {
    byte mins;
    byte hours;
    byte wday;
    byte rate;
    byte mday;
    byte mon;
    char temp;
    byte issync;
    int synctime;
    ushort timer;
    byte run;
} Clock;

extern ram Clock FastClock;
extern near unsigned char date_enabled;
extern near unsigned char temp_enabled;
extern near unsigned char showdate;
extern volatile unsigned char __LED2;

extern volatile byte altDisplay;
extern volatile int showdate_timer;


#define DISPLAY_DATE 1
#define DISPLAY_TEMP 2

#endif	// __IO_H
