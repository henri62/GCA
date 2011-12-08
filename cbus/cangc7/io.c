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



#include <p18cxxx.h>
#include <stdio.h>

#include "cbus.h"
#include "utils.h"
#include "cbusdefs.h"
#include "cangc7.h"
#include "io.h"


void setupIO(byte clr) {
  int idx = 0;

  // all digital I/O
  ADCON0 = 0x00;
  ADCON1 = 0x0F;
  
  TRISCbits.TRISC0 = 0; /* LED1 */
  TRISCbits.TRISC3 = 0; /* LED2 */
  TRISAbits.TRISA1 = 0; /* DSI5 */


  LED1 = PORT_OFF;
  LED2 = PORT_OFF;
  DIS5 = PORT_ON;


  // following presets are written to eeprom if the flim switch is preshed at boot


}

void writeOutput(int idx, unsigned char val) {
}

unsigned char readInput(int idx) {
  unsigned char val = 0;
  return !val;
}


void doLEDTimers(void) {
  if( led1timer > 0 ) {
    led1timer--;
    if( led1timer == 0 ) {
      LED1 = 0;
    }
  }
}

void doIOTimers(void) {
  int i = 0;
}

void doTimedOff(int i) {
}


unsigned char checkInput(unsigned char idx) {
  unsigned char ok = 1;
  return ok;
}



void saveOutputStates(void) {
  int idx = 0;
  byte o1 = 0;
  byte o2 = 0;

  //eeWrite(EE_PORTSTAT + 1, o2);
  

}



void restoreOutputStates(void) {
  int idx = 0;
  //byte o1 = eeRead(EE_PORTSTAT + 0);

}



static unsigned char __LED2 = 0;
void doLEDs(void) {
  LED2 = __LED2;
  __LED2 ^= 1;
}

byte getPortStates(int group) {
}


void setOutput(ushort nn, ushort addr, byte on) {
  int i = 0;
}