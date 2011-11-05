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

void setup(void);


void main(void) {

  setup();
  
  do {

  } while( True );
}


void setup(void) {
  // Timer0 Registers:
  // Prescaler=1:1; TMR0 Preset=131; Freq=8.00kHz; Period=125,000 ns
  /*
  OPTION_REG.T0CS = 0;// bit 5 TMR0 Clock Source Select bit:0=Internal Clock (CLKO) / 1=Transition on T0CKI pin
  OPTION_REG.T0SE = 0;// bit 4 TMR0 Source Edge Select bit: 0=low/high / 1=high/low
  OPTION_REG.PSA  = 1;// bit 3 Prescaler Assignment bit: 0=Prescaler is assigned to the Timer0
  OPTION_REG.PS2  = 0;// bits 2-0  PS2:PS0: Prescaler Rate Select bits
  OPTION_REG.PS1  = 0;
  OPTION_REG.PS0  = 0;
  TMR0 = 131;           // preset for timer register
  */
  
}

static void interrupt
isr(void) // Here be interrupt function - the name is unimportant.
{
  INTF = 0; // clear the interrupt
}
