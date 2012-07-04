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
  // Setup A/D - 1 i/ps with internal reference
  
  ADCON2 = 0b10000110; // result right justified, Fosc/64
  ADCON1 = 0b00001110; // Internal Vref, AN0 analogue input
  ADCON0 = 0b00000001; // Channel 0, On

  // LEDs
  TRISBbits.TRISB0 = 0;  // LED5_RUN
  TRISAbits.TRISA4 = 0;  // LED6_PT

  // DCC
  TRISBbits.TRISB2 = 0;  // DCC_OUTA
  TRISBbits.TRISB3 = 0;  // DCC_OUTB

  // PT
  TRISAbits.TRISA0 = 1;  // DCC_PT_ACK
  TRISBbits.TRISB5 = 0;  // DCC_PT_DCCA
  TRISAbits.TRISA2 = 0;  // DCC_PT_DCCB
  TRISAbits.TRISA1 = 0;  // DCC_PT_ENABLE

  TRISBbits.TRISB7 = 1;  // PB2_PT



  LED5_RUN = PORT_OFF;
  LED6_PT  = PORT_OFF;

  DCC_OUTA = PORT_OFF;
  DCC_OUTB = PORT_OFF;

  DCC_PT_DCCA   = PORT_OFF;
  DCC_PT_DCCB   = PORT_OFF;
  DCC_PT_ENABLE = PORT_OFF;
  
}


