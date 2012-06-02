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

#include <delays.h>
#include <string.h>

#pragma udata access VARS_DISPLAY


// TMR0 generates a heartbeat every 16000000/4/32/125  == 1ms.
static byte clk = 0;
#pragma interrupt writeDisplays
void writeDisplays(void) {
  
  // Timer0 interrupt handler
  if( INTCONbits.T0IF ) {
    INTCONbits.T0IF  = 0;     // Clear interrupt flag
    TMR0L = 256 - 125; // 4MHz resonator
    clk ^= 1;
    LCD1_CLK = clk;
    LCD2_CLK = clk;


    if( !clk ) {
      byte i;
      for( i = 0; i < MAXDISPLAYS; i++ ) {
        if( Display[i].pending ) {
          byte m = Display[i].mode[Display[i].byteidx/8];
          if( i == 0 ) {
            LCD1_CSB = PORT_OFF;
            LCD1_RS = (m >> (Display[i].byteidx%8));
          }
          else if( i == 1 ) {
            LCD2_CSB = PORT_OFF;
            LCD2_RS = (m >> (Display[i].byteidx%8));
          }
          if( Display[i].bitidx < 8 ) {
            // set bit
            byte b = Display[i].buffer[Display[i].byteidx];
            if( i == 0 ) {
              LCD1_SI = (b >> (7-Display[i].bitidx));
            }
            else if( i == 1 ) {
              LCD2_SI = (b >> (7-Display[i].bitidx));
            }
            
            Display[i].bitidx++;
          }

          if( Display[i].bitidx == 8 ) {
            Display[i].byteidx++;
            Display[i].bitidx = 0;

            if( Display[i].buffer[Display[i].byteidx] == 0 || Display[i].byteidx >= BUFFERSIZE )
            {
              Display[i].byteidx = 0;
              Display[i].pending = FALSE;

              if( i == 0 )
                LCD1_CSB = PORT_ON;
              if( i == 1 )
                LCD1_CSB = PORT_ON;

            }
          }
        }
      }
    }

  }

}

void setupDisplayIO(void) {

  TRISAbits.TRISA0 = 0;  // LCD1_SI
  TRISAbits.TRISA1 = 0;  // LCD1_CLK
  TRISAbits.TRISA3 = 0;  // LCD1_CSB
  TRISAbits.TRISA4 = 0;  // LCD1_RS

  TRISCbits.TRISC0 = 0;  // LCD2_SI
  TRISCbits.TRISC1 = 0;  // LCD2_CLK
  TRISCbits.TRISC2 = 0;  // LCD2_CSB
  TRISCbits.TRISC3 = 0;  // LCD2_RS

  
  LCD1_CSB = PORT_ON;
  LCD1_RS  = PORT_OFF; // RS-Pin to low -> command mode
  LCD1_SI  = PORT_OFF;

  LCD2_CSB = PORT_ON;
  LCD2_RS  = PORT_OFF; // RS-Pin to low -> command mode
  LCD2_SI  = PORT_OFF;

}


void setupDisplays(void) {
  byte i;
  for( i = 0; i < MAXDISPLAYS; i++ ) {
    Display[i].mode[0] = 0x00; // All commands.
    Display[i].buffer[0] = 0x39;
    Display[i].buffer[1] = 0x1D;
    Display[i].buffer[2] = 0x50;
    Display[i].buffer[3] = 0x6C;
    Display[i].buffer[4] = 0x7C;
    Display[i].buffer[5] = 0x38;
    Display[i].buffer[6] = 0x0C;
    Display[i].buffer[7] = 0x01;

    Display[i].mode[1] = 0xFC;
    Display[i].buffer[8] = 0x06;
    Display[i].buffer[9] = 0x02;
    Display[i].buffer[10] = 'R';
    Display[i].buffer[11] = 'o';
    Display[i].buffer[12] = 'c';
    Display[i].buffer[13] = 'r';
    Display[i].buffer[14] = 'a';
    Display[i].buffer[15] = 'i';

    Display[i].mode[2] = 0x01;
    Display[i].buffer[16] = 'l';
    Display[i].buffer[17] = 0; // Terminating zero.
    
    Display[i].byteidx = 0;
    Display[i].bitidx = 0;
    Display[i].pending = TRUE; // Send one time.
    
  }
}


void nextLine(void) {
}

void home(void) {
}

void cls(void) {
}

void initDisplays(void) {
  byte i, n;

  for( i = 0; i < MAXDISPLAYS; i++ ) {
    Display[i].pending = FALSE;
    Display[i].byteidx = 0;
    Display[i].bitidx = 0;
    for( n = 0; n < MODESIZE; n++ )
      Display[i].mode[n] = 0;
    for( n = 0; n < BUFFERSIZE; n++ )
      Display[i].buffer[n] = 0;
  }

  setupDisplayIO();

}

void setDisplayData(int addr, byte flags, byte char0, byte char1, byte char2, byte char3) {
  byte i = flags & 0x03;
  for( i = 0; i < MAXDISPLAYS; i++ ) {
    if( Display[i].addr == addr ) {
      byte part = flags >> 4;

      // Set mode flags to data:
      byte modeidx = (part*4)/8;
      byte mode = Display[i].mode[modeidx];
      Display[i].mode[modeidx] = mode | (0x0F << (part%2));

      Display[i].buffer[part*4+0] = char0;
      Display[i].buffer[part*4+1] = char1;
      Display[i].buffer[part*4+2] = char2;
      Display[i].buffer[part*4+3] = char3;
      if( char0 == 0 || char1 == 0 || char2 == 0 || char3 == 0 ) {
        Display[i].pending = TRUE; // Send one time.
      }
      break;
    }
  }
}



