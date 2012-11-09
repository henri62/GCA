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




extern near unsigned char led1timer;


void setupIO(byte clr);
void doLEDTimers(void);
void doLED250(void);
void doLEDs(void);
unsigned char checkFlimSwitch(void);
void doIOTimers(void);
void doFastClock(void);

typedef struct {
    byte mins;
    byte hours;
    byte wday;
    byte rate;
    byte mday;
    byte mon;
    byte issync;
    int synctime;
    byte gotfirstsync;
    char temp;
    ushort timer;
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
