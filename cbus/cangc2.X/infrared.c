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

#include "infrared.h"
#include "io.h"
#include "cbusdefs.h"
#include "cbus.h"

int IR(int lp, byte in);

void checkIR(void) {
  int i = 0;
  for( i = 0; i < 2; i++ ) {
    if( Ports[i].cfg & PORTCFG_IR ) {
      IR(i, readInput(i));
    }
  }
}

int IR( int lp, byte in ) {
  if( !IRPorts[lp].gotIRsync ) {
    IRPorts[lp].IRsync = IRPorts[lp].IRsync << 1;
    IRPorts[lp].IRsync |= in;
    if( (IRPorts[lp].IRsync & IR_SYNCMASK) == IR_SYNC ) {
      IRPorts[lp].gotIRsync = TRUE;
      IRPorts[lp].IRaddr = 0;
      IRPorts[lp].IRdata = 0;
      IRPorts[lp].IRdatacnt = 0;
    }
  }
  else { // IR data bits
    IRPorts[lp].IRdatacnt++;
    IRPorts[lp].IRdata = IRPorts[lp].IRdata << 1;
    IRPorts[lp].IRdata |= in;

    if( IRPorts[lp].IRdatacnt == 16 ) {
      IRPorts[lp].IRaddr = IRPorts[lp].IRdata;
    }

    if( IRPorts[lp].IRdatacnt >= 18 ) {
      IRPorts[lp].gotIRsync = FALSE;
      IRPorts[lp].IRsync = 0;
    }

  }

  return IRPorts[lp].IRaddr;

}