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


    // Note: a stop bit is a 1, a start bit is a 0

    // Here are the transitions of the state machine:
    //
    //       stop_exp                -> wait_start
    //                                   *set rcv_ready bits
    //                                   *set frame_err bits

    //       wait_start              -> INPUT==1 -> wait_start
    //                                  INPUT==0 -> confirm_start

    //       confirm_start           -> INPUT==1 -> wait_start
    //                                  INPUT==0 -> ignore1

    //       ignore1                 -> ignore2

    //       ignore2                 -> FULL==0 -> sample
    //                                  FULL==1 -> stop_exp
    //                                       *clear full bits

    //       sample                  -> ignore1
    //                                       *set overrun error bits
    //                                       *store data bits
    //                                       *set full bits

    // Turning this around into math to compute the new values of the state
    // bit vectors and 'full', 'rcv_ready', 'overrun_err' and 'frame_err' vectors
    // based on the old state bit vectors and the 'input' and 'full' vectors
    // we get:
    //
    //   NOTE: These comments are written assuming all the assignments happen
    //       simultaneously:
    //
    //       confirm_start   <- (wait_start & ~input)
    //       ignore1         <- (confirm_start & ~input) | sample
    //       ignore2         <- ignore1
    //       sample          <- (ignore2 & ~full)
    //       stop_exp        <- (ignore2 & full)
    //       full            <- full & ~ignore2
    //       wait_start      <- stop_exp
    //                        | (wait_start & input)
    //                        | (confirm_start & input)
    //       rcv_ready       <- rcv_ready | stop_exp
    //       frame_err       <- frame_err | (stop_exp & ~input)
    //       overrun_err     <- overrun_err | (sample & rcv_ready)

    // new_wait_start = ((wait_start | confirm_start) & input) | stop_exp



#include "project.h"
#include "cangc4.h"
#include "io.h"
#include "rfid.h"
#include "cbus.h"
#include "utils.h"
#include "cbusdefs.h"

#pragma udata access VARS_RFID
near byte work;


// TMR0 generates a heartbeat every 32000000/4/2/139 == 28776,98 Hz.
#pragma interrupt scanRFID
void scanRFID(void) {

  // Timer0 interrupt handler
  if( INTCONbits.T0IF ) {
    byte inC;

    INTCONbits.T0IF  = 0;     // Clear interrupt flag
    TMR0L = 256 - 129;    // Reset counter

    LED2 = PORT_ON;

    inC = PORTC;

    if( RFID[0].status == STATUS_WAITSTART && (inC & 0x01) == 0 ) {
      RFID[0].status = STATUS_IGN1;
    }
    else if( RFID[0].status == STATUS_IGN1 ) {
      RFID[0].status = STATUS_IGN2;
    }
    else if( RFID[0].status == STATUS_IGN2 ) {
      RFID[0].status = STATUS_SAMPLE;
    }
    else if( RFID[0].status == STATUS_SAMPLE ) {
      if( RFID[0].bitcnt == 8 ) {
        // stop bit
        RFID[0].status = STATUS_FULL;
      }
      else {
        RFID[0].status = STATUS_IGN1;
        RFID[0].sample <<= 1;
        RFID[0].sample |= (inC & 0x01);
        RFID[0].bitcnt++;
      }
    }

    LED2 = PORT_OFF;
  }

}

void doRFID(void) {
  byte i, ok;
  for( i = 0; i < 8; i++ ) {
    if( RFID[i].status == STATUS_FULL ) {
      //LED2 = PORT_ON;
      //if( RFID[i].rawcnt == 0 && RFID[i].sample == STX ) {
      if( RFID[i].rawcnt == 0 ) {
        // start
        RFID[i].rawcnt++;
      }
      //else if( RFID[i].rawcnt == 15 && RFID[i].sample == ETX ) {
      else if( RFID[i].rawcnt == 15 ) {
        // end -> convert raw to binary -> send event OPC_DDES
        RFID[i].rawcnt = 0;
        strToByte( RFID[i].raw, 10, RFID[i].data );
        canmsg.opc = OPC_DDES;
        canmsg.d[0] = (RFID[i].addr / 256) & 0xFF;
        canmsg.d[1] = (RFID[i].addr) & 0xFF;
        canmsg.d[2] = RFID[i].data[0];
        canmsg.d[3] = RFID[i].data[1];
        canmsg.d[4] = RFID[i].data[2];
        canmsg.d[5] = RFID[i].data[3];
        canmsg.d[6] = RFID[i].data[4];
        canmsg.len = 7; // data bytes
        ok = canQueue(&canmsg);
      }
      else if( RFID[i].rawcnt >= 1 && RFID[i].rawcnt < 11 ) {
        // data
        RFID[i].raw[RFID[i].rawcnt-1] = RFID[i].sample;
        RFID[i].rawcnt++;
      }
      else if( RFID[i].rawcnt >= 11 && RFID[i].rawcnt < 15 ) {
        // checksum + cr + lf
        RFID[i].rawcnt++;
      }

      RFID[0].bitcnt = 0;
      RFID[i].status = STATUS_WAITSTART;
    }
  }
}


void initRFID(void) {
  byte i, n;
  
  for( i = 0; i < 8; i++ ) {
    RFID[i].error  = 0;
    RFID[i].status = 0;
    RFID[i].sample = 0;
    RFID[i].bitcnt = 0;
    RFID[i].rawcnt = 0;
    for( n = 0; n < 5; n++ )
      RFID[i].data[n] = 0;
    for( n = 0; n < 10; n++ )
      RFID[i].raw[n] = 0;
  }

}