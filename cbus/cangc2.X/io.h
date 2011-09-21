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


#define SW      PORTAbits.RA2	// Flim switch
#define LED1    PORTBbits.RB6
#define LED2    PORTBbits.RB7
#define LED3    PORTBbits.RB5

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


// port config
#define PORTCFG_IO          0x01   // port I/O mask
#define PORTCFG_IN          0x01
#define PORTCFG_OUT         0x00
#define PORTCFG_OFFDELAY    0x02   // input 2 seconds off delay for occupancy detectors
#define PORTCFG_PULSE       0x02   // output 0.5 seconds pulse length
#define PORTCFG_INV         0x04   // invert
#define PORTCFG_IR          0x08   // Infrared port


typedef struct {
  byte   cfg;
  byte   status;
  ushort addr;
  byte   timedoff;
  ushort timer;
  ushort evtnn;
} Port;

typedef struct {
  byte   gotsync;
  byte   data;
  byte   datacnt;
  ushort addr;
  ushort prevaddr;
} IRPort;

extern ram Port Ports[16];
extern ram IRPort IRPorts[8];
extern near unsigned char led1timer;


void setupIO(byte clr);
void writeOutput(int port, unsigned char val);
unsigned char readInput(int port);
void doIOTimers(void);
void doLEDTimers(void);
void doTimedOff(int i);
void checkInput(unsigned char idx, unsigned char sod);
void resetOutputs(void);
unsigned char checkFlimSwitch(void);
void doLEDs(void);
void saveOutputStates(void);
void restoreOutputStates(void);
void configPort(int idx );
byte getPortStates(int group);
void setOutput(ushort nn, ushort addr, byte on);

#endif	// __IO_H
