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
#define LED4    PORTAbits.RA0   // servo busy

#define SW      PORTAbits.RA1	// Flim switch

#define GCA137  PORTCbits.RC6	// GCA137 polarisation Tx

//#define CANGC6_21
#ifdef CANGC6_21
    #define SERVO1  PORTBbits.RB5
    #define SERVO2  PORTBbits.RB4
    #define SERVO3  PORTBbits.RB1
    #define SERVO4  PORTBbits.RB0
    #define T1S     PORTCbits.RC2
    #define T1R     PORTCbits.RC5
    #define T2S     PORTCbits.RC3
    #define T2R     PORTCbits.RC4
    #define T3S     PORTCbits.RC0
    #define T3R     PORTCbits.RC7
    #define T4S     PORTCbits.RC1
    #define T4R     PORTAbits.RA2
#else
    #define SERVO1  PORTBbits.RB0
    #define SERVO2  PORTBbits.RB1
    #define SERVO3  PORTBbits.RB4
    #define SERVO4  PORTBbits.RB5
    #define T1S     PORTCbits.RC0
    #define T1R     PORTCbits.RC7
    #define T2S     PORTCbits.RC1
    #define T2R     PORTAbits.RA2
    #define T3S     PORTCbits.RC2
    #define T3R     PORTCbits.RC5
    #define T4S     PORTCbits.RC3
    #define T4R     PORTCbits.RC4
#endif

typedef struct {
  byte   config;
  byte   left;
  byte   right;
  byte   speedL;
  byte   speedR;
  byte   position;
  byte   wantedpos;
  ushort pulse;
  int    swnn;
  int    swaddr;
  int    fbaddr;
  byte   endtime;
  byte   bounceL;
  byte   bounceR;
} ServoDef;

#define SERVOCONF_POLAR  0x01
#define SERVOCONF_EXTSEN 0x02
#define SERVOCONF_BOUNCE 0x04

#define TURNOUT_UNKNOWN 0
#define TURNOUT_STRAIGHT 1
#define TURNOUT_THROWN 2


extern ram ServoDef Servo[4];
extern near unsigned char led1timer;


void setupIO(byte clr);
void doLEDTimers(void);
unsigned char checkInput(unsigned char idx, unsigned char sod);
void doLED250(void);
void doLEDs(void);
void saveOutputStates(void);
void setOutput(ushort nn, ushort addr, byte on);
unsigned char checkFlimSwitch(void);

#endif	// __IO_H
