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

/*
; This code is a sample UAR(no T) that receives 8 channels at 9600 baud on a 20Mhz 16F628A PIC
; The program works by handling timer interrupts at 3x the nominal bit rate and running a
; state machine for each channel to acquire the data.
;
; The interrupts are at the rate of:
;
;       20000000/4/174 == 28735.63 Hz.
 *      32000000/4/2/139 == 28776,98 Hz.
;
; The 'perfect' rate would be 9600*3 == 28800 Hz.
;
; The bit rate error is thus about 0.22 percent.
;
; The first trick behind this code is the way the state machines are run: instead of
; handling each channel one at a time the program uses 'vertical arithmetic' to
; process all eight channels together.
;
; The second trick is the way that the program works thru processing the eight
; input bits on the channels, accumulating them into the 'receiver shift register'
; variables, and determining when a byte has been completely received. This is done
; using only 3 instructions per channel.
;
; Using these two tricks results in code that uses only 76 instructions per interrupt,
; including context save and restore, but not interrupt latency. Since interrupts
; are generated every 174 instructions this leaves about 54 percent of the CPU available
; for 'task level' code.
;
; One important thing to note: since this code does _not_ double buffer the receiver,
; task level code must process a received byte within 4/3 of a bit time (approximately
; 390 task level instructions), otherwise the overrun flag will be set for that channel.

 */

#include "project.h"
#include "isr.h"
#include "cangc4.h"
#include "io.h"
#include "rfid.h"

#pragma udata access VARS
near unsigned short led500ms_timer;
near unsigned short led250ms_timer;
near unsigned short io_timer;
near unsigned short led_timer;
near unsigned short dim_timer;

#pragma code APP

//
// Interrupt Service Routine
//
// TMR2 generates a heartbeat every 1mS.
// TMR0 generates a pulse between 0.5 and 2.5mS.
//
#pragma interrupt isr_high
void isr_high(void) {

  // Timer0 interrupt handler
  if( INTCONbits.T0IF ) {
    INTCONbits.T0IF  = 0; // Clear interrupt flag
    TMR0L = 256 - 139;
    scanRFID();
  }


}


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
      } else if (--Latcount == 0) { // Allow tries at lower level priority first
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
