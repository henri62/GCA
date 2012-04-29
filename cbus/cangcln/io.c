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
#include "cangcln.h"
#include "io.h"

//#pragma code IO

void setupIO(byte clr) {
  int idx = 0;

  // all digital I/O
  ADCON0 = 0x00;
  ADCON1 = 0x0F;

  TRISBbits.TRISB4 = 0;  // LED1 cbus activity
  TRISBbits.TRISB5 = 0;  // LED4 cbus error
  TRISCbits.TRISC3 = 0;  // LED2 learning mode
  TRISCbits.TRISC2 = 0;  // LED3 running

  TRISAbits.TRISA0 = 1; /* SW */

  TRISCbits.TRISC6 = 1; /* LocoNet activity */
  TRISCbits.TRISC7 = 1; /* LocoNet error */

  LED1   = PORT_OFF;
  LED2   = PORT_OFF;
  LED3   = PORT_OFF;
  LED4   = PORT_OFF;
  LED5   = PORT_OFF;
  LED6   = PORT_OFF;

  if( checkFlimSwitch() || eeRead(EE_CLEAN) == 0xFF ) {
    eeWrite(EE_CLEAN, 0);
  }


}


// Called every 3ms.
void doLEDTimers(void) {
  if( ledCBUStimer > 0 ) {
    ledCBUStimer--;
    if( ledCBUStimer == 0 ) {
      LED1 = 0;
    }
  }
  if( ledCBUSERRtimer > 0 ) {
    ledCBUSERRtimer--;
    if( ledCBUSERRtimer == 0 ) {
      LED4 = 0;
    }
  }

}

unsigned char checkFlimSwitch(void) {
  unsigned char val = SW;
  return !val;
}




static unsigned char __LED2 = 0;
void doLEDs(void) {
  if( Wait4NN || isLearning) {
    LED2 = __LED2;
    __LED2 ^= 1;
  }
  else {
    LED2 = PORT_OFF;
  }

}


static unsigned char __LED4 = 0;
void doLED250(void) {
}





