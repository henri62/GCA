/*
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


#include "defs.h"
#include "cangc9.h"
#include "isr.h"


// Interrupt Service Routine
//
// TMR2 generates a heartbeat every 1mS.


//
// Low priority interrupt. Used for CAN receive.
//
#pragma interruptlow isr_low
void isr_low(void) {
  // Timer2 interrupt handler
  if( PIR1bits.TMR2IF ) {
    PIR1bits.TMR2IF = 0; // Clear interrupt flag
    TMR2 = 0; // reset counter

    // I/O timeout - 5ms
    if (--timer5 == 0) {
      timer5 = 5;
      //doLEDTimers();
    }

    // I/O timeout - 50ms
    if (--timer50 == 0) {
      timer50 = 50;
    }

    // Timer 200ms
    if (--timer200 == 0) {
        timer200 = 200;
        //doLED200();
    }

    // Timer 500ms
    if (--timer500 == 0) {
      timer500 = 500;
      //doLEDs();
    }

  }

}


void initTimers(void) {
  timer500 = 500;  // 500ms
  timer200 = 200;  // 200ms
  timer50  = 50;   // 50ms
  timer5   = 5;    // 5ms

  // ***** Timer0 *****
  // 32000000/4/2/208 == 19230.
  T0CON = 0;
  // pre scaler 2:
  T0CONbits.PSA   = 0;
  T0CONbits.T0PS0 = 0;
  T0CONbits.T0PS1 = 0;
  T0CONbits.T0PS2 = 0;
  // 8 bit counter
  T0CONbits.T08BIT = 1;
  TMR0H = 0;
  TMR0L = 256 - 208;
  // timer on
  T0CONbits.TMR0ON = 1;
  // interrupt
  INTCONbits.TMR0IE  = 1;
  INTCON2bits.TMR0IP = 1;

  // ***** Timer2 *****
  T2CON = 4 << 3; // 5 post scaler
  T2CONbits.TMR2ON  = 1; // Timer2 on
  T2CONbits.T2CKPS0 = 0; // 16 pre scaler = 8MHz / 16
  T2CONbits.T2CKPS1 = 1;
  TMR2 = 0; // 1 mS
  PR2  = 100;
  PIE1bits.TMR2IE = 1;
  INTCONbits.PEIE = 1;
  IPR1bits.TMR2IP = 0; // high prio


}

