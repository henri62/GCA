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


#define LED1    PORTAbits.RA3   // cbus activity
#define LED2    PORTAbits.RA4   // learning mode
#define LED3    PORTAbits.RA5   // running

#define SW      PORTAbits.RA1	// Flim switch

#define GCA137  PORTAbits.RA2	// GCA137 polarisation Tx

#define SERVO1  PORTBbits.RB0
#define SERVO2  PORTBbits.RB1
#define SERVO3  PORTBbits.RB4
#define SERVO4  PORTBbits.RB5

#define T1S     PORTCbits.RC0
#define T1R     PORTCbits.RC1
#define T2S     PORTCbits.RC2
#define T2R     PORTCbits.RC3
#define T3S     PORTCbits.RC4
#define T3R     PORTCbits.RC5
#define T4S     PORTCbits.RC6
#define T4R     PORTCbits.RC7

typedef struct {
  byte   config;
  byte   left;
  byte   right;
  byte   speed;
  int    swevent;
  int    fbevent;
} ServoDef;

#define SERVOCONF_POLAR  0x01
#define SERVOCONF_EXTSEN 0x02


extern ram ServoDef Servo[4];
extern near unsigned char led1timer;


void setupIO(byte clr);
void writeOutput(int port, unsigned char val);
unsigned char readInput(int port);
void doIOTimers(void);
void doLEDTimers(void);
unsigned char checkInput(unsigned char idx);
void doLEDs(void);
void saveOutputStates(void);
void restoreOutputStates(void);
void setOutput(ushort nn, ushort addr, byte on);
unsigned char checkFlimSwitch(void);

#endif	// __IO_H
