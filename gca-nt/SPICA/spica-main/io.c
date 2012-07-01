/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

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

#include "io.h"
#include "spica.h"

void setupIO(void) {
  // all digital I/O
  ADCON0 = 0x00;
  ADCON1 = 0x0F;

  // LEDs
  TRISBbits.TRISB0 = 0;  // LED1_PT
  TRISBbits.TRISB1 = 0;  // LED2_RUN
  TRISBbits.TRISB4 = 0;  // LED3_BOOSTER
  TRISBbits.TRISB5 = 0;  // LED4_BUS

  // Buttons
  TRISCbits.TRISC4 = 1; // SW1_PT
  TRISCbits.TRISC5 = 1; // PB1_POWER

  // LocoNet
  TRISCbits.TRISC1 = 0; // LNTX
  TRISCbits.TRISC2 = 1; // LNRX

  // Buzzer
  TRISAbits.TRISA0 = 0; // BUZZER

  LED1_PT      = PORT_OFF;
  LED2_RUN     = PORT_OFF;
  LED3_BOOSTER = PORT_OFF;
  LED4_BUS     = PORT_OFF;

}

