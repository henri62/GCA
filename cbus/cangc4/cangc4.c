/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2011 - Rob Versluis <r.j.versluis@rocrail.net>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include	<htc.h>

#include "cangc4.h"

__CONFIG(FOSC_INTRCIO & WDTE_OFF & PWRTE_ON & MCLRE_OFF & BOREN_OFF);


void setup(void);
void makeBitStream(void);

const int addr = 4733;

static char f40khz    = 0;
static char modBitCnt = 0;
static char bitStatus = 0;
static char bitIndex  = 0;

//static unsigned long ir = 0L;
static byte ir[4];



void main(void) {
  makeBitStream();
  setup();
  
  do {
    ;
  } while( True );
}


void setup(void) {
  // Timer0 Registers:
  // Prescaler=1:1; TMR0 Preset=244; Freq=83.33333kHz; Period=12,000 ns
  OPTION_REGbits.T0CS = 0;// bit 5 TMR0 Clock Source Select bit:0=Internal Clock (CLKO) / 1=Transition on T0CKI pin
  OPTION_REGbits.T0SE = 0;// bit 4 TMR0 Source Edge Select bit: 0=low/high / 1=high/low
  OPTION_REGbits.PSA  = 1;// bit 3 Prescaler Assignment bit: 0=Prescaler is assigned to the Timer0
  OPTION_REGbits.PS2  = 0;// bits 2-0  PS2:PS0: Prescaler Rate Select bits
  OPTION_REGbits.PS1  = 0;
  OPTION_REGbits.PS0  = 0;
  TMR0 = 244;           // preset for timer register

  // Setup Timer0 interrupt.
  INTCON = 0;
  INTCONbits.T0IE = 1;
  INTCONbits.T0IF = 0;
  INTCONbits.GIE  = 1;

  /* I/O
  Port 	Use 	    Info bit
  GP2 	IR-LED 1 	0
  GP4 	IR-LED 2 	1
  */
  TRISIO = 0; // All output.
  PIE1   = 0;
  PIR1   = 0;
}

void makeBitStream(void) {
  unsigned short bits;
  byte p1;
  byte p0;
  byte i;

  //---------- BIT stream
  // d13 ... d0 p1 p0
  // address
  bits = addr & 0x3FFF;

  // parity bit 1
  p1 = bits & 0x0001;
  for( i = 1; i < 14; i+=2 ) {
    p1 = (byte)(p1 ^ (bits >> i & 0x0001));
  }
  bits |= (p1&1) << 14;

  // parity bit 0
  p0 = bits & 0x0001;
  for( i = 2; i < 14; i+=2 ) {
    p0 = (byte)(p0 ^ (bits>>i & 0x0001));
  }
  bits |= (p1&1) << 15;

  //---------- IR stream
  // 11=sync, 00=0, 01=1
  // sync
  ir[0] = 0;
  ir[1] = 0;
  ir[2] = 0;
  ir[3] = 0;

  for( i = 0; i < 16; i++ ) {
    if( (bits >> i & 0x0001) == 1 ) {
      ir[(i*2)/8] |= 1 << ((i*2)%8+1);
    }
  }

}

char getBitStatus() {
  modBitCnt++;
  if( modBitCnt >= 20 ) {

    bitStatus = 0;
    if( bitIndex < 2 )
      bitStatus = 1;
    else {
      if( ir[(bitIndex-2) / 8] & (1 << (bitIndex-2) % 8) )
        bitStatus = 1;
    }

    bitIndex++;
    if( bitIndex >= 34)
      bitIndex = 0;

    modBitCnt = 0;
  }
  if( bitStatus == 1 && f40khz == 1 )
    return 1;
  return 0;
}

static void interrupt
isr(void) // 80kHz interrupt for generating a 40kHz block signal.
{
  f40khz ^= 1;
  GPIO2 = getBitStatus(); // bit stream scope test
  TMR0 = 244; // preset for timer register
  INTF = 0;   // clear the interrupt
}
