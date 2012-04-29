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


#define LED1    PORTBbits.RB4   // cbus activity
#define LED4    PORTBbits.RB5   // cbus error
#define LED2    PORTCbits.RC3   // learning mode
#define LED3    PORTCbits.RC2   // running

#define SW      PORTAbits.RA0	// Flim switch


#define LED5    PORTCbits.RC6 // LocoNet activity
#define LED6    PORTCbits.RC7 // LocoNet error


typedef struct {
  byte   opc;
  byte   data[5];
} LNPACKET;

typedef struct {
  byte   opc;
  byte   data[5];
} CBUSPACKET;


extern near unsigned char led1timer;


void setupIO(byte clr);
void doLEDTimers(void);
void doLED250(void);
void doLEDs(void);
unsigned char checkFlimSwitch(void);


#endif	// __IO_H
