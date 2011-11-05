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

static char f40khz;

void main(void) {
  f40khz = 0;
  setup();
  
  do {
    f40khz ^= 1;
    GPIO2 = f40khz;
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
  //INTCONbits.T0IE = 1;
  INTCONbits.T0IF = 0;
  //INTCONbits.GIE  = 1;

  /* I/O
  Port 	Use 	    Info bit
  GP2 	IR-LED 1 	0
  GP4 	IR-LED 2 	1
  */
  TRISIO = 0; // All output.
  PIE1   = 0;
  PIR1   = 0;
}

static void interrupt
isr(void) // Here be interrupt function - the name is unimportant.
{
  TMR0 = 244; // preset for timer register
  f40khz ^= 1;
  GPIO2 = f40khz;
  INTF = 0;   // clear the interrupt
}
