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


#include "project.h"
#include "cangc8.h"
#include "io.h"
#include "display.h"
#include "cbus.h"
#include "utils.h"
#include "cbusdefs.h"

#pragma udata access VARS_DISPLAY


// TMR0 generates a heartbeat every 32000000/4/2/139 == 28776,98 Hz.
#pragma interrupt writeDisplays
void writeDisplays(void) {

  // Timer0 interrupt handler
  if( INTCONbits.T0IF ) {
    byte inC;

    INTCONbits.T0IF  = 0;     // Clear interrupt flag
    TMR0L = 256 - 139 + 10;   // Reset counter with a correction of 10 cycles

    //LED2 = PORT_ON;


    //LED2 = PORT_OFF;
  }

}


void initDisplays(void) {
  byte i, n;
  
  for( i = 0; i < MAXDISPLAYS; i++ ) {
    for( n = 0; n < 20; n++ )
      Display[i].text1[n] = 0;
      Display[i].text2[n] = 0;
  }

}


void setDisplayData(int addr, byte flags, byte char0, byte char1, byte char2, byte char3) {
  byte i = flags & 0x03;
  for( i = 0; i < MAXDISPLAYS; i++ ) {
    if( Display[i].addr == addr ) {
      byte part = flags >> 4;
      if( flags & 0x01 ) {
        Display[i].text2[part*4+0] = char0;
        Display[i].text2[part*4+1] = char1;
        Display[i].text2[part*4+2] = char2;
        Display[i].text2[part*4+3] = char3;
        if( char0 == 0 || char1 == 0 || char2 == 0 || char3 == 0 ) {
          Display[i].line2 = 1; // Send one time.
          LED2 = PORT_ON;
        }
      }
      else {
        Display[i].text1[part*4+0] = char0;
        Display[i].text1[part*4+1] = char1;
        Display[i].text1[part*4+2] = char2;
        Display[i].text1[part*4+3] = char3;
        if( char0 == 0 || char1 == 0 || char2 == 0 || char3 == 0 ) {
          Display[i].line1 = 1; // Send one time.
          LED2 = PORT_ON;
        }
      }
      break;
    }
  }
}

