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
#include "isr.h"
#include "cangcln.h"
#include "io.h"
#include "loconet.h"

#pragma udata access VARS_ISR
near unsigned short led500ms_timer;
near unsigned short led250ms_timer;
near unsigned short io_timer;
near unsigned short led_timer;
near unsigned short dim_timer;

//#pragma code ISR

//
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
    if (--led_timer == 0) {
      led_timer = 5;
      doLEDTimers();
    }

    // I/O timeout - 50ms
    if (--io_timer == 0) {
      io_timer = 50;
      if (can_transmit_timeout != 0) {
        --can_transmit_timeout;
      }
    }

    // Timer 200ms
    if (--led250ms_timer == 0) {
        led250ms_timer = 200;
        doLED250();
    }

    // Timer 500ms
    if (--led500ms_timer == 0) {
      led500ms_timer = 500;
      doLEDs();
    }

  }
  
  if (PIR3bits.ERRIF == 1) {
    PIR3bits.ERRIF = 0;

    if (TXB1CONbits.TXLARB) { // lost arbitration
      if (Latcount == 0) { // already tried higher priority
        can_transmit_failed = 1;
        TXB1CONbits.TXREQ = 0;
      }
      else if (--Latcount == 0) { // Allow tries at lower level priority first
        TXB1CONbits.TXREQ = 0;
        Tx1[sidh] &= 0b00111111; // change priority
        TXB1CONbits.TXREQ = 1; // try again
      }
    }

    if (TXB1CONbits.TXERR) { // bus error
      can_transmit_failed = 1;
      TXB1CONbits.TXREQ = 0;
    }

  }

}
