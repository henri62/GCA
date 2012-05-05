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


  TRISBbits.TRISB4 = 0;  // LED1 CBUS activity TX
  TRISBbits.TRISB5 = 0;  // LED2 CBUS activity RX
  TRISCbits.TRISC3 = 0;  // LED6 Learning mode
  TRISCbits.TRISC2 = 0;  // LED5 Running

  TRISAbits.TRISA0 = 1; /* SW */
  TRISAbits.TRISA1 = 0; /* LNSCAN */

  TRISCbits.TRISC4 = 0; /* LED3 LocoNet activity TX */
  TRISCbits.TRISC5 = 0; /* LED4 LocoNet activity RX */
  TRISCbits.TRISC6 = 0; /* LocoNet TX */
  TRISCbits.TRISC7 = 1; /* LocoNet RX */

  LED1_CBUSTX   = PORT_OFF;
  LED2_CBUSRX   = PORT_OFF;
  LED3_LNTX     = PORT_OFF;
  LED4_LNRX     = PORT_OFF;
  LED5_RUN      = PORT_OFF;
  LED6_FLIM     = PORT_OFF;
  LNTX          = PORT_OFF;
  LNSCAN        = PORT_OFF;

  if( checkFlimSwitch() || eeRead(EE_CLEAN) == 0xFF ) {
    eeWrite(EE_CLEAN, 0);
  }


}


// Called every 3ms.
void doLEDTimers(void) {
  if( ledCBUSRXtimer > 0 ) {
    ledCBUSRXtimer--;
    if( ledCBUSRXtimer == 0 ) {
      LED2_CBUSRX = 0;
    }
  }
  if( ledCBUSTXtimer > 0 ) {
    ledCBUSTXtimer--;
    if( ledCBUSTXtimer == 0 ) {
      LED1_CBUSTX = 0;
    }
  }
  if( ledLNTXtimer > 0 ) {
    ledLNTXtimer--;
    if( ledLNTXtimer == 0 ) {
      LED3_LNTX = 0;
    }
  }
  if( ledLNRXtimer > 0 ) {
    ledLNRXtimer--;
    if( ledLNRXtimer == 0 ) {
      LED4_LNRX = 0;
    }
  }

}

unsigned char checkFlimSwitch(void) {
  unsigned char val = SW;
  return !val;
}




static unsigned char __LED6_FLIM = 0;
void doLEDs(void) {
  if( Wait4NN || isLearning) {
    LED6_FLIM = __LED6_FLIM;
    __LED6_FLIM ^= 1;
  }
  else {
    LED6_FLIM = PORT_OFF;
  }

}


static unsigned char __LED4 = 0;
void doLED250(void) {
}





