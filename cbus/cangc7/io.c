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
#include "cangc7.h"
#include "io.h"

static byte pos_bcd[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
static byte neg_bcd[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};

static byte pos_wd[] = {0x00, 0x01, 0x20, 0x02, 0x10, 0x40, 0x04, 0x08};
static byte neg_wd[] = {0xFF, 0xFE, 0xDF, 0xFD, 0xEF, 0xBF, 0xFB, 0xF7};


static byte pos_Dash = 0x40;
static byte neg_Dash = 0xBF;

static byte pos_H = 0x76;
static byte pos_E = 0x79;
static byte pos_L = 0x38;
static byte pos_P = 0x73;

static byte neg_H = 0x89;
static byte neg_E = 0x86;
static byte neg_L = 0xC7;
static byte neg_P = 0x8C;

static int showdate_timer = 0;

void setupIO(byte clr) {
  int idx = 0;

  // all digital I/O
  ADCON0 = 0x00;
  ADCON1 = 0x0F;
  
  TRISCbits.TRISC0 = 0; /* a POINT1 */
  TRISCbits.TRISC1 = 0; /* b */
  TRISCbits.TRISC2 = 0; /* c */
  TRISCbits.TRISC3 = 0; /* d POINT2 */
  TRISCbits.TRISC4 = 0; /* e DASH   */
  TRISCbits.TRISC5 = 0; /* f */
  TRISCbits.TRISC6 = 0; /* g */
  TRISCbits.TRISC7 = 0; /*   */
  TRISAbits.TRISA1 = 0; /* DIS5 */
  TRISAbits.TRISA2 = 0; /* DIS4 */
  TRISAbits.TRISA3 = 0; /* DIS3 */
  TRISAbits.TRISA4 = 0; /* DIS2 */
  TRISAbits.TRISA5 = 0; /* DIS1 */

  TRISBbits.TRISB0 = 0; /* DIS6 */
  TRISBbits.TRISB1 = 0; /* LED2 */
  TRISBbits.TRISB4 = 1; /* SW */
  TRISBbits.TRISB5 = 0; /* LED1 */


  LED1 = PORT_OFF;
  DIS5 = PORT_OFF;


  // following presets are written to eeprom if the flim switch is preshed at boot


}


void writeOutput(int idx, unsigned char val) {
}

unsigned char readInput(int idx) {
  unsigned char val = 0;
  return !val;
}


// Called every 4ms.
void doLEDTimers(void) {
  byte* bcd    = pos_display ? pos_bcd:neg_bcd;
  byte* wd     = pos_display ? pos_wd:neg_wd;
  byte Dash    = pos_display ? pos_Dash:neg_Dash;
  byte charH   = pos_display ? pos_H:neg_H;
  byte charE   = pos_display ? pos_E:neg_E;
  byte charL   = pos_display ? pos_L:neg_L;
  byte charP   = pos_display ? pos_P:neg_P;
  byte dispON  = pos_display ? DISPLAY_ON:DISPLAY_OFF;
  byte dispOFF = pos_display ? DISPLAY_OFF:DISPLAY_ON;



  if( led1timer > 0 ) {
    led1timer--;
    if( led1timer == 0 ) {
      LED1 = 0;
    }
  }

  if( date_enabled ) {
    if( !showdate ) {
      showdate_timer++;
      if( showdate_timer > 3000 ) {
        showdate = TRUE;
        showdate_timer = 1000;
      }
    }
    else {
      showdate_timer--;
      if( showdate_timer <= 0 ) {
        showdate = FALSE;
      }
    }
  }
  else {
    showdate = FALSE;
  }

  if( FastClock.div > 0 && FastClock.issync ) {
    FastClock.synctime++;

    if( FastClock.synctime > (333*60 / FastClock.div) ) {
      FastClock.issync = FALSE;
    }
  }

  if( Wait4NN ) {
    DIS1   = dispOFF;
    DIS2   = dispOFF;
    DIS3   = dispOFF;
    DIS4   = dispOFF;
    DIS5   = dispOFF;
    DIS6   = dispOFF;
    POINT1 = dispON;
    POINT2 = dispOFF;
    DIS5   = dispON;
    return;
  }

  switch( display ) {
    case 0:
      DIS2 = dispOFF;
      DIS3 = dispOFF;
      DIS4 = dispOFF;
      DIS5 = dispOFF;
      DIS6 = dispOFF;
      if( FastClock.issync || FastClock.div == 0 )
        if( showdate)
          PORTC = bcd[FastClock.mon % 10];
        else
          PORTC = bcd[FastClock.mins % 10] + (FastClock.wday&0x01?0x80:0x00);
      else
        PORTC = FastClock.gotfirstsync ? charP:Dash;
      DIS1 = dispON;
      break;
    case 1:
      DIS1 = dispOFF;
      DIS3 = dispOFF;
      DIS4 = dispOFF;
      DIS5 = dispOFF;
      DIS6 = dispOFF;
      if( FastClock.issync || FastClock.div == 0 )
        if( showdate)
          PORTC = bcd[FastClock.mon / 10];
        else
          PORTC = bcd[FastClock.mins / 10] + (FastClock.wday&0x02?0x80:0x00);
      else
        PORTC = FastClock.gotfirstsync ? charL:Dash;
      DIS2 = dispON;
      break;
    case 2:
      DIS1 = dispOFF;
      DIS2 = dispOFF;
      DIS4 = dispOFF;
      DIS5 = dispOFF;
      DIS6 = dispOFF;
      if( FastClock.issync || FastClock.div == 0 )
        if( showdate)
          PORTC = bcd[FastClock.mday % 10];
        else
          PORTC = bcd[FastClock.hours % 10] + (FastClock.wday&0x04?0x80:0x00);
      else
        PORTC = FastClock.gotfirstsync ? charE:Dash;
      DIS3 = dispON;
      break;
    case 3:
      DIS1 = dispOFF;
      DIS2 = dispOFF;
      DIS3 = dispOFF;
      DIS5 = dispOFF;
      DIS6 = dispOFF;
      if( FastClock.issync || FastClock.div == 0) {
        if( showdate && FastClock.mday / 10 != 0) {
          PORTC = bcd[FastClock.mday / 10];
          DIS4 = dispON;
        }
        else if( !showdate && FastClock.hours / 10 != 0) {
          PORTC = bcd[FastClock.hours / 10] + (FastClock.wday&0x08?0x80:0x00);
          DIS4 = dispON;
        }
        else if( !showdate ) {
          PORTC = FastClock.wday&0x08?0x80:0x00;
          DIS4 = dispON;
        }
      }
      else if( !FastClock.issync ) {
        PORTC = FastClock.gotfirstsync ? charH:Dash;
        DIS4 = dispON;
      }
      break;
    case 4:
      DIS1 = dispOFF;
      DIS2 = dispOFF;
      DIS3 = dispOFF;
      DIS4 = dispOFF;
      DIS6 = dispOFF;

      if( !showdate && FastClock.issync && pointtimer < (50/FastClock.div) || FastClock.div == 0 ) {
        POINT1 = dispON;
        POINT2 = dispON;
        DASH   = dispOFF;
      }
      else if( showdate && FastClock.issync) {
        POINT1 = dispOFF;
        POINT2 = dispON;
        DASH   = dispON;
      }
      else {
        POINT1 = dispOFF;
        POINT2 = dispOFF;
        DASH   = dispOFF;
      }
      DIS5 = dispON;
      pointtimer++;
      if( pointtimer > (100/FastClock.div) )
        pointtimer = 0;
      break;
    case 5:
      DIS1 = dispOFF;
      DIS2 = dispOFF;
      DIS3 = dispOFF;
      DIS4 = dispOFF;
      DIS5 = dispOFF;
      // week day
      PORTC = wd[FastClock.wday&0x07];
      DIS6 = dispON;
      break;
  }
  display++;
  if( display > 5 ) {
    display = 0;
  }

}

void doIOTimers(void) {
  int i = 0;
}

void doTimedOff(int i) {
}


unsigned char checkFlimSwitch(void) {
  unsigned char val = SW;
  return !val;
}

unsigned char checkInput(unsigned char idx) {
  unsigned char ok = 1;
  return ok;
}



void saveOutputStates(void) {
  int idx = 0;
  byte o1 = 0;
  byte o2 = 0;

  //eeWrite(EE_PORTSTAT + 1, o2);
  

}



void restoreOutputStates(void) {
  int idx = 0;
  //byte o1 = eeRead(EE_PORTSTAT + 0);

}



void doLEDs(void) {
}

byte getPortStates(int group) {
}


void setOutput(ushort nn, ushort addr, byte on) {
  int i = 0;
}
