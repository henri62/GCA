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

static byte bcd[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

void setupIO(byte clr) {
  int idx = 0;

  // all digital I/O
  ADCON0 = 0x00;
  ADCON1 = 0x0F;
  
  TRISCbits.TRISC0 = 0; /* a LED1 */
  TRISCbits.TRISC1 = 0; /* b */
  TRISCbits.TRISC2 = 0; /* c */
  TRISCbits.TRISC3 = 0; /* d LED2 */
  TRISCbits.TRISC4 = 0; /* e */
  TRISCbits.TRISC5 = 0; /* f */
  TRISCbits.TRISC6 = 0; /* g */
  TRISCbits.TRISC7 = 0; /*  */
  TRISAbits.TRISA1 = 0; /* DIS5 */
  TRISAbits.TRISA2 = 0; /* DIS4 */
  TRISAbits.TRISA3 = 0; /* DIS3 */
  TRISAbits.TRISA4 = 0; /* DIS2 */
  TRISAbits.TRISA5 = 0; /* DIS1 */

  TRISBbits.TRISB0 = 0; /* LED1 */
  TRISBbits.TRISB1 = 0; /* LED2 */
  TRISBbits.TRISB4 = 1; /* PB1 */
  TRISBbits.TRISB5 = 1; /* PB2 */


  LED1 = PORT_OFF;
  LED2 = PORT_OFF;
  DIS5 = PORT_OFF;


  // following presets are written to eeprom if the flim switch is preshed at boot


}


void writeOutput(int idx, unsigned char val) {
}

unsigned char readInput(int idx) {
  unsigned char val = 0;
  return !val;
}


// Called every 4ms.
void doLEDTimers(void) {
  if( led1timer > 0 ) {
    led1timer--;
    if( led1timer == 0 ) {
      LED1 = 0;
    }
  }

  if( FastClock.div > 0 && FastClock.issync ) {
    FastClock.synctime++;

    if( FastClock.synctime > (250*60 / FastClock.div) ) {
      FastClock.issync = FALSE;
    }
  }

  switch( display ) {
    case 0:
      DIS2 = PORT_OFF;
      DIS3 = PORT_OFF;
      DIS4 = PORT_OFF;
      DIS5 = PORT_OFF;
      if( FastClock.issync || FastClock.div == 0 )
        PORTC = bcd[FastClock.mins % 10];
      else
        PORTC = 0x73;
      DIS1 = PORT_ON;
      break;
    case 1:
      DIS1 = PORT_OFF;
      DIS3 = PORT_OFF;
      DIS4 = PORT_OFF;
      DIS5 = PORT_OFF;
      if( FastClock.issync || FastClock.div == 0 )
        PORTC = bcd[FastClock.mins / 10];
      else
        PORTC = 0x38;
      DIS2 = PORT_ON;
      break;
    case 2:
      DIS1 = PORT_OFF;
      DIS2 = PORT_OFF;
      DIS4 = PORT_OFF;
      DIS5 = PORT_OFF;
      if( FastClock.issync || FastClock.div == 0 )
        PORTC = bcd[FastClock.hours % 10];
      else
        PORTC = 0x79;
      DIS3 = PORT_ON;
      break;
    case 3:
      DIS1 = PORT_OFF;
      DIS2 = PORT_OFF;
      DIS3 = PORT_OFF;
      DIS5 = PORT_OFF;
      if( (FastClock.issync || FastClock.div == 0) && FastClock.hours / 10 != 0) {
        PORTC = bcd[FastClock.hours / 10];
        DIS4 = PORT_ON;
      }
      else if( !FastClock.issync ) {
        PORTC = 0x76;
        DIS4 = PORT_ON;
      }
      break;
    case 4:
      DIS1 = PORT_OFF;
      DIS2 = PORT_OFF;
      DIS3 = PORT_OFF;
      DIS4 = PORT_OFF;

      if( FastClock.issync && pointtimer < (50/FastClock.div) || FastClock.div == 0 ) {
        POINT1 = PORT_ON;
        POINT2 = PORT_ON;
      }
      else if( FastClock.issync) {
        POINT1 = PORT_OFF;
        POINT2 = PORT_OFF;
      }
      else {
        POINT1 = PORT_OFF;
        POINT2 = PORT_OFF;
      }
      DIS5 = PORT_ON;
      pointtimer++;
      if( pointtimer > (100/FastClock.div) )
        pointtimer = 0;
      break;
  }
  display++;
  if( display > 4 ) {
    display = 0;
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
