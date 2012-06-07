/*
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


#include "defs.h"
#include "cangc9.h"
#include "serial.h"
#include "io.h"

static byte puls = 0;
// TMR0 generates a heartbeat every 32000000/4/2/208 == 19230.
#pragma interrupt sendSerial
void sendSerial(void) {

  // Timer0 interrupt handler
  if( INTCONbits.T0IF ) {
    //byte inLN = LNRX;

    INTCONbits.T0IF  = 0;     // Clear interrupt flag
    TMR0L = 256 - 208;         // Reset counter with a correction of 10 cycles


    LED4_SER = puls; // Debug.
    
    puls ^= 1;
  }
}