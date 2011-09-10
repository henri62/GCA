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

#include "lissy.h"
#include "io.h"
#include "cbusdefs.h"
#include "cbus.h"

void checkLissy(void) {
  int i = 0;
  for( i = 0; i < 8; i++ ) {
    if( Ports[i].cfg & PORTCFG_LISSY ) {
      if( lissy(i, readInput(i)) > 0 ) {
        // TODO: Generate an event.
        Tx1[d0] = OPC_ACON2;
        Tx1[d1] = (NN_temp / 256) & 0xFF;
        Tx1[d2] = (NN_temp % 256) & 0xFF;
        Tx1[d3] = (Ports[i].addr / 256) & 0xFF;
        Tx1[d4] = (Ports[i].addr % 256) & 0xFF;
        Tx1[d5] = (LissyPorts[i].lissyaddr / 256) & 0xFF;
        Tx1[d6] = (LissyPorts[i].lissyaddr % 256) & 0xFF;
        can_tx(7);
      }
    }
  }
}

int lissy( int lp, byte in ) {
  if( !LissyPorts[lp].gotlissysync ) {
    LissyPorts[lp].lissysync = LissyPorts[lp].lissysync << 1;
    LissyPorts[lp].lissysync |= in;
    if( (LissyPorts[lp].lissysync & LISSY_SYNCMASK) == LISSY_SYNC ) {
      LissyPorts[lp].gotlissysync = TRUE;
      LissyPorts[lp].lissyaddr = 0;
      LissyPorts[lp].lissydata = 0;
      LissyPorts[lp].lissydatacnt = 0;
    }
  }
  else { // lissy data bits
    LissyPorts[lp].lissydatacnt++;
    LissyPorts[lp].lissydata = LissyPorts[lp].lissydata << 1;
    LissyPorts[lp].lissydata |= in;

    if( LissyPorts[lp].lissydatacnt == 16 ) {
      LissyPorts[lp].lissyaddr = LissyPorts[lp].lissydata & 0x3FFF;
    }

    if( LissyPorts[lp].lissydatacnt >= 18 ) {
      LissyPorts[lp].gotlissysync = FALSE;
      LissyPorts[lp].lissysync = 0;
    }

  }

  return LissyPorts[lp].lissyaddr;

}