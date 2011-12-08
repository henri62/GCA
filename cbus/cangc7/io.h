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


#define LED1    PORTCbits.RC0
#define LED2    PORTCbits.RC3

#define DIS1    PORTAbits.RA5
#define DIS2    PORTAbits.RA4
#define DIS3    PORTAbits.RA3
#define DIS4    PORTAbits.RA2
#define DIS5    PORTAbits.RA1


// port config
#define PORTCFG_IO          0x01   // port I/O mask
#define PORTCFG_IN          0x01
#define PORTCFG_OUT         0x00
#define PORTCFG_OFFDELAY    0x02   // input 2 seconds off delay for occupancy detectors
#define PORTCFG_PULSE       0x02   // output 0.5 seconds pulse length
#define PORTCFG_INV         0x04   // invert
#define PORTCFG_IR          0x08   // Infrared port


typedef struct {
  byte   mins;
  byte   hours;
  byte   wday;
  byte   div;
  byte   spare1;
  byte   spare2;
  byte   issync;
} Clock;

extern ram Clock FastClock;
extern near unsigned char led1timer;
extern near unsigned char display;


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

#endif	// __IO_H
