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

#pragma udata access VARS_DISPLAY


// TMR0 generates a heartbeat every 32000000/4/2/139 == 28776,98 Hz.
//                                  16000000/4/2/69  == 28985.51 Hz.

#pragma interrupt writeDisplays
void writeDisplays(void) {

  // Timer0 interrupt handler
  if( INTCONbits.T0IF ) {
    INTCONbits.T0IF  = 0;     // Clear interrupt flag
    //TMR0L = 256 - 139 + 10;   // Reset counter with a correction of 10 cycles
    TMR0L = 256 - 69; // 4MHz resonator
  }

}

void displayDelay(byte cnt) {
  byte i;
  for( i = 0; i < cnt; i++ );
}

void writeChar(char data) {
  LCD_PORT &= 0x0F; // clear data lines
  LCD_PORT |= (data&0xF0); //output high nibble
  LCD_EN=1;
  displayDelay(2);
  LCD_EN=0;
  displayDelay(2);
  
  LCD_PORT &= 0x0F; // clear data lines
  LCD_PORT |= ((data&0x0F) << 4); //output low nibble
  LCD_EN=1;
  displayDelay(2);
  LCD_EN=0;
  displayDelay(10);

}

void setupDisplay(void) {

  TRISCbits.TRISC0 = 0;  // LCD_RW
  TRISCbits.TRISC1 = 0;  // LCD_EN
  TRISCbits.TRISC2 = 0;  // LCD_GND
  TRISCbits.TRISC3 = 0;  // LCD_RS
  TRISCbits.TRISC4 = 0;  // LCD_data4 LSB
  TRISCbits.TRISC5 = 0;  // LCD_data5
  TRISCbits.TRISC6 = 0;  // LCD_data6
  TRISCbits.TRISC7 = 0;  // LCD_data7 MSB

  LCD_GND = 0; //GND-Pin to low
  LCD_RS  = 0; //RS-Pin to low
  LCD_RW  = 0; //RW-Pin to low
  LCD_EN  = 0; //EN-Pin to low


  writeChar(0x33); //reset and 4 bit mode
  writeChar(0x32); //reset and 4 bit mode sequence
  writeChar(0x28); //2 lines, 5x7 dots
  writeChar(0x06); //Cursor left to right, don't shift display
  writeChar(0x0C); //Display on, cursor off, blink at cursor off
  writeChar(0x01); //clear display, start at DD address 0

}


void nextLine(void) {
  LCD_RS  = 0;
  writeChar(0xC0); //CR LF
  LCD_RS  = 1;
}

void home(void) {
  LCD_RS  = 0;
  writeChar(0x02); //home
  LCD_RS  = 1;
}

void cls(void) {
  LCD_RS  = 0;
  writeChar(0x01); //clear
  LCD_RS  = 1;
}

void initDisplays(void) {
  byte i, n;

  for( i = 0; i < MAXDISPLAYS; i++ ) {
    for( n = 0; n < 20; n++ )
      Display[i].text1[n] = 0;
      Display[i].text2[n] = 0;
  }

  setupDisplay();

  cls();
  home();
  LCD_RS  = 1;
  writeChar('R');
  writeChar('o');
  writeChar('c');
  writeChar('r');
  writeChar('a');
  writeChar('i');
  writeChar('l');
}

void writeDisplay(byte idx) {
  byte i;
  cls();
  home();
  for( i = 0; i < 8; i++ ) {
    writeChar(Display[idx].text1[i]);
  }
  nextLine();
  for( i = 0; i < 8; i++ ) {
    writeChar(Display[idx].text2[i]);
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
          writeDisplay(i);
        }
      }
      else {
        Display[i].text1[part*4+0] = char0;
        Display[i].text1[part*4+1] = char1;
        Display[i].text1[part*4+2] = char2;
        Display[i].text1[part*4+3] = char3;
        if( char0 == 0 || char1 == 0 || char2 == 0 || char3 == 0 ) {
          Display[i].line1 = 1; // Send one time.
          writeDisplay(i);
        }
      }
      break;
    }
  }
}



/*

void initDisplay(void)
{
  RS_DD=1; //RS-Pin as Output
  EN_DD=1; //EN-Pin as Output
  RW_DD=1; //RW-Pin as Output
  RS = 0; //RS-Pin to low
  RW = 0; //RW-Pin to low
  EN = 0; //EN-Pin to low
  send_nibble(0x03); //Be sure to
  send_nibble(0x03); //be in
  send_nibble(0x03); //8-Bit-Mode
  send_nibble(0x02); //Switch to 4 Bit
  Wait(50); //Wait 5us
  WriteIns(0x28);//4-Bit-Mode
  WriteIns(0x08);//display off
  WriteIns(0x06);//entry mode set increment cursor by 1 not shifting display
  WriteIns(0x17);//Character mode and internel power on
  WriteIns(0x01);//clear display
  WriteIns(0x02);//return home
  WriteIns(0x0C); //display on
}
void send_nibble (char data)
{
  DATA_PORT = data; //output data
  EN=1;
  Wait(10); //wait 1us (stabilize outupt)
  EN=0;
  Wait(10); //wait 1us (stabilize outupt)
  }
void WriteIns(char instruction)
{
  DATA_PORT_DD=0x0F; //Dataport as Output
  RS = 0;
  RW = 0;
  send_nibble((instruction&0xF0)>>4); //Highbyte
  send_nibble(instruction&0x0F); //Lowbyte
  CheckBusy();
}


 */