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
  
  TRISBbits.TRISB6 = 0; /* LED1 */
  TRISBbits.TRISB7 = 0; /* LED2 */
  TRISBbits.TRISB5 = 0; /* LED3 */
  TRISAbits.TRISA2 = 1; /* Push button */

  LED1 = 0;
  LED2 = 0;
  LED3 = 0;


  // following presets are written to eeprom if the flim switch is preshed at boot


}

void writeOutput(int idx, unsigned char val) {
  switch(idx) {
    case  0: PORT1  = val; break;
    case  1: PORT2  = val; break;
    case  2: PORT3  = val; break;
    case  3: PORT4  = val; break;
    case  4: PORT5  = val; break;
    case  5: PORT6  = val; break;
    case  6: PORT7  = val; break;
    case  7: PORT8  = val; break;
    case  8: PORT9  = val; break;
    case  9: PORT10 = val; break;
    case 10: PORT11 = val; break;
    case 11: PORT12 = val; break;
    case 12: PORT13 = val; break;
    case 13: PORT14 = val; break;
    case 14: PORT15 = val; break;
    case 15: PORT16 = val; break;
  }
}

unsigned char readInput(int idx) {
  unsigned char val = 0;
  switch(idx) {
    case 0:  val = PORT1;  break;
    case 1:  val = PORT2;  break;
    case 2:  val = PORT3;  break;
    case 3:  val = PORT4;  break;
    case 4:  val = PORT5;  break;
    case 5:  val = PORT6;  break;
    case 6:  val = PORT7;  break;
    case 7:  val = PORT8;  break;
    case 8:  val = PORT9;  break;
    case 9:  val = PORT10; break;
    case 10: val = PORT11; break;
    case 11: val = PORT12; break;
    case 12: val = PORT13; break;
    case 13: val = PORT14; break;
    case 14: val = PORT15; break;
    case 15: val = PORT16; break;
  }
  return !val;
}


void doLEDTimers(void) {
  if( led1timer > 0 ) {
    led1timer--;
    if( led1timer == 0 )
      LED1 = 0;
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
