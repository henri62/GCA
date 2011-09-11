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

void IR(int l, byte in);

void checkIR(void) {
  int i = 0;
  for( i = 0; i < 2; i++ ) {
    if( Ports[i].cfg & PORTCFG_IR ) {
      IR(i, readInput(i));
    }
  }
}


/*
 * bit length is 250 micro seconds
 * 11 = sync
 * 10 = 0
 * 01 = 1
 *
 * <sync> t1 t0 d13 ... d0
 * t = type 0 = decoder address
 * d = data
 */

void IR( int p, byte in ) {
  if( !IRPorts[p].gotsync ) {
    IRPorts[p].data <<= 1;
    IRPorts[p].data |= in;
    if( (IRPorts[p].data & IR_BIT) == IR_SYNC ) {
      IRPorts[p].gotsync = TRUE;
      IRPorts[p].addr = 0;
      IRPorts[p].data = 0;
      IRPorts[p].datacnt = 0;
    }
  }
  else { // IR data bits
    IRPorts[p].datacnt++;
    IRPorts[p].data = IRPorts[p].data << 1;
    IRPorts[p].data |= in;

    if( IRPorts[p].datacnt % 2 == 0 ) {
      IRPorts[p].addr <<= 1;
      IRPorts[p].addr |= ((IRPorts[p].data & IR_BIT) == IR_ONE) ? 1:0;
      IRPorts[p].data = 0;
    }

    if( IRPorts[p].datacnt == 32 ) {
      IRPorts[p].gotsync = FALSE;
      IRPorts[p].data = 0;
    }

  }

}