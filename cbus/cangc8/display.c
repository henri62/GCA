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
        if( DisplayA[i].pending ) {
          byte m = DisplayA[i].mode[DisplayA[i].byteidx/8];
          if( i == 0 ) {
            LCD1_CSB = PORT_OFF;
            LCD1_RS = (m >> (DisplayA[i].byteidx%8));
          }
          else if( i == 1 ) {
            LCD2_CSB = PORT_OFF;
            LCD2_RS = (m >> (DisplayA[i].byteidx%8));
          }
          if( DisplayA[i].bitidx < 8 ) {
            // set bit
            byte b = DisplayA[i].buffer[DisplayA[i].byteidx];
            if( i == 0 ) {
              LCD1_SI = (b >> (7-DisplayA[i].bitidx));
            }
            else if( i == 1 ) {
              LCD2_SI = (b >> (7-DisplayA[i].bitidx));
            }
            
            DisplayA[i].bitidx++;
          }

          if( DisplayA[i].bitidx == 8 ) {
            DisplayA[i].byteidx++;
            DisplayA[i].bitidx = 0;

            if( DisplayA[i].buffer[DisplayA[i].byteidx] == 0 || DisplayA[i].byteidx >= BUFFERSIZE )
            {
              DisplayA[i].byteidx = 0;
              DisplayA[i].pending = FALSE;
            }
          }
        }
        else {
          // Deselect chip:
          if( i == 0 )
            LCD1_CSB = PORT_ON;
          if( i == 1 )
            LCD2_CSB = PORT_ON;
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
    DisplayA[i].mode[0] = 0x00; // All commands.
    DisplayA[i].buffer[0] = 0x39; // Function set -> IS2
    DisplayA[i].buffer[1] = 0x1D; // IS2: 3 lines
    DisplayA[i].buffer[2] = 0x50; // Power control: Booster off
    DisplayA[i].buffer[3] = 0x6C; // Follower control
    DisplayA[i].buffer[4] = 0x70 + (DisplayA[i].config & 0x0F); // Contrast
    DisplayA[i].buffer[5] = 0x38; // Goto IS0
    DisplayA[i].buffer[6] = 0x0C; // Cursor
    DisplayA[i].buffer[7] = 0x01; // Clear display

    DisplayA[i].mode[1] = 0xFC;
    DisplayA[i].buffer[8] = 0x06; // Display ON
    DisplayA[i].buffer[9] = 0x02; // Return home
    DisplayA[i].buffer[10] = 'C';
    DisplayA[i].buffer[11] = 'o';
    DisplayA[i].buffer[12] = 'p';
    DisplayA[i].buffer[13] = 'y';
    DisplayA[i].buffer[14] = 'r';
    DisplayA[i].buffer[15] = 'i';

    DisplayA[i].mode[2] = 0xF7;
    DisplayA[i].buffer[16] = 'g';
    DisplayA[i].buffer[17] = 'h';
    DisplayA[i].buffer[18] = 't';
    DisplayA[i].buffer[19] = 0x80 + 16;
    DisplayA[i].buffer[20] = 'R';
    DisplayA[i].buffer[21] = 'o';
    DisplayA[i].buffer[22] = 'b';
    DisplayA[i].buffer[23] = ' ';
    DisplayA[i].mode[3] = 0xFF;
    DisplayA[i].buffer[24] = 'V';
    DisplayA[i].buffer[25] = 'e';
    DisplayA[i].buffer[26] = 'r';
    DisplayA[i].buffer[27] = 's';
    DisplayA[i].buffer[28] = 'l';
    DisplayA[i].buffer[29] = 'u';
    DisplayA[i].buffer[30] = 'i';
    DisplayA[i].buffer[31] = 's';
    DisplayA[i].mode[4] = 0xFE;
    DisplayA[i].buffer[32] = 0x80 + 32;
    DisplayA[i].buffer[33] = 'R';
    DisplayA[i].buffer[34] = 'o';
    DisplayA[i].buffer[35] = 'c';
    DisplayA[i].buffer[36] = 'r';
    DisplayA[i].buffer[37] = 'a';
    DisplayA[i].buffer[38] = 'i';
    DisplayA[i].buffer[39] = 'l';
    DisplayA[i].mode[5] = 0xFF;
    DisplayA[i].buffer[40] = '.';
    DisplayA[i].buffer[41] = 'n';
    DisplayA[i].buffer[42] = 'e';
    DisplayA[i].buffer[43] = 't';
    DisplayA[i].buffer[44] = ',';
    DisplayA[i].buffer[45] = 'M';
    DisplayA[i].buffer[46] = 'E';
    DisplayA[i].buffer[47] = 'R';
    DisplayA[i].mode[6] = 0x01;
    DisplayA[i].buffer[48] = 'G';
    DisplayA[i].buffer[49] = 0;
    
    DisplayA[i].byteidx = 0;
    DisplayA[i].bitidx = 0;
    DisplayA[i].pending = TRUE; // Send one time.
    
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
    DisplayA[i].pending = FALSE;
    DisplayA[i].byteidx = 0;
    DisplayA[i].bitidx = 0;
    for( n = 0; n < MODESIZE; n++ )
      DisplayA[i].mode[n] = 0;
    for( n = 0; n < BUFFERSIZE; n++ )
      DisplayA[i].buffer[n] = 0;
  }

  setupDisplayIO();

}

void setDisplayData(int addr, byte flags, byte char0, byte char1, byte char2, byte char3) {
  byte i = flags & 0x03;
  for( i = 0; i < MAXDISPLAYS; i++ ) {

    if( DisplayA[i].addr == addr ) {
    
      byte part = flags >> 4;

      // Set mode flags to data:
      byte modeidx = (part*4)/8;
      byte mode = DisplayA[i].mode[modeidx];

      if( part == 0 ) {
        DisplayA[i].buffer[0] = 0x01; // cls
        DisplayA[i].mode[0] = 0xFE;
      }
      
      if( char0 == '|' ) {
        byte offset = ((part*4+0)/16) + 16;
        byte modebyte = (part*4+1) / 8;
        byte modebit  = (part*4+1) % 8;
        DisplayA[i].mode[modebyte] &= ~(0x01 << modebit);
        DisplayA[i].buffer[part*4+1] = 0x80 + offset;
      }
      else {
        byte modebyte = (part*4+1) / 8;
        byte modebit  = (part*4+1) % 8;
        DisplayA[i].mode[modebyte] |= (0x01 << modebit);
        DisplayA[i].buffer[part*4+1] = char0;
      }

      if( char1 == '|' ) {
        byte offset = ((part*4+1)/16) + 16;
        byte modebyte = (part*4+2) / 8;
        byte modebit  = (part*4+2) % 8;
        DisplayA[i].mode[modebyte] &= ~(0x01 << modebit);
        DisplayA[i].buffer[part*4+2] = 0x80 + offset;
      }
      else {
        byte modebyte = (part*4+2) / 8;
        byte modebit  = (part*4+2) % 8;
        DisplayA[i].mode[modebyte] |= (0x01 << modebit);
        DisplayA[i].buffer[part*4+2] = char1;
      }

      if( char2 == '|' ) {
        byte offset = ((part*4+2)/16) + 16;
        byte modebyte = (part*4+3) / 8;
        byte modebit  = (part*4+3) % 8;
        DisplayA[i].mode[modebyte] &= ~(0x01 << modebit);
        DisplayA[i].buffer[part*4+3] = 0x80 + offset;
      }
      else {
        byte modebyte = (part*4+3) / 8;
        byte modebit  = (part*4+3) % 8;
        DisplayA[i].mode[modebyte] |= (0x01 << modebit);
        DisplayA[i].buffer[part*4+3] = char2;
      }

      if( char3 == '|' ) {
        byte offset = ((part*4+3)/16) + 16;
        byte modebyte = (part*4+4) / 8;
        byte modebit  = (part*4+4) % 8;
        DisplayA[i].mode[modebyte] &= ~(0x01 << modebit);
        DisplayA[i].buffer[part*4+4] = 0x80 + offset;
      }
      else {
        byte modebyte = (part*4+4) / 8;
        byte modebit  = (part*4+4) % 8;
        DisplayA[i].mode[modebyte] |= (0x01 << modebit);
        DisplayA[i].buffer[part*4+4] = char3;
      }

      if( char0 == 0 || char1 == 0 || char2 == 0 || char3 == 0 ) {
        DisplayA[i].byteidx = 0;
        DisplayA[i].bitidx = 0;
        DisplayA[i].pending = TRUE; // Send one time.
      }
    }
  }
}



