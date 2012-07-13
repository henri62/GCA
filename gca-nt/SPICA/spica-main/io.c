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

  TRISA = 0;
  TRISB = 0;
  TRISC = 0;

  // LEDs
  TRISBbits.TRISB0 = 0;  // LED1_PT
  TRISBbits.TRISB1 = 0;  // LED2_RUN
  TRISAbits.TRISA5 = 0;  // LED3_BOOSTER
  TRISBbits.TRISB4 = 0;  // LED4_BUS
  TRISBbits.TRISB5 = 0;  // LED5_ACK

  // Buttons
  TRISCbits.TRISC4 = 1; // PB2_PT
  TRISCbits.TRISC5 = 1; // PB1_POWER
  TRISCbits.TRISC3 = 1; // PB3_FLIM

  // LocoNet
  TRISCbits.TRISC1 = 0; // LNTX
  TRISCbits.TRISC2 = 1; // LNRX

  // Buzzer
  TRISCbits.TRISC0 = 0; // BUZZER

  LED1_PT      = PORT_OFF;
  LED2_RUN     = PORT_OFF;
  LED3_BOOSTER = PORT_OFF;
  LED4_BUS     = PORT_OFF;
  LED5_ACK     = PORT_OFF;
  LNTX         = PORT_OFF;

  // DCC
  TRISCbits.TRISC6 = 0;  // DCC_OUT_POS
  TRISCbits.TRISC7 = 0;  // DCC_OUT_NEG

  // PT
  TRISAbits.TRISA0 = 1;  // DCC_PT_ACK
  TRISAbits.TRISA1 = 0;  // DCC_NEG
  TRISAbits.TRISA2 = 0;  // DCC_POS
  TRISAbits.TRISA3 = 0;  // DCC_EN

  // Setup A/D - 1 i/ps with internal reference
  ADCON2 = 0b10000110; // result right justified, Fosc/64
  ADCON1 = 0b00001110; // Internal Vref, AN0 analogue input
  ADCON0 = 0b00000001; // Channel 0, On


  DCC_OUT_POS = PORT_OFF;
  DCC_OUT_NEG = PORT_OFF;

  DCC_POS = PORT_OFF;
  DCC_NEG = PORT_OFF;
  DCC_EN  = PORT_OFF;

}

// Called every 3ms.
void doLEDTimers(void) {
  if( ledBUStimer > 0 ) {
    ledBUStimer--;
    if( ledBUStimer == 0 ) {
      LED4_BUS = 0;
    }
  }

}


unsigned char checkFlimSwitch(void) {
  unsigned char val = PB3_FLIM;
  return !val;
}




static unsigned char __LED6_FLIM = 0;
void doLEDs(void) {
  if( Wait4NN || isLearning) {
    LED2_RUN = __LED6_FLIM;
    __LED6_FLIM ^= 1;
  }
  else {
    LED2_RUN = PORT_ON;
  }

}


static unsigned char __LED4 = 0;
void doLED250(void) {
}





