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
#include "cangcln.h"
#include "io.h"
#include "loconet.h"
#include "cbus.h"
#include "utils.h"
#include "cbusdefs.h"
#include "lnconst.h"

#pragma udata access VARS_LOCONET1
near byte work;
near byte LNstatus;
near byte sampledata;
near byte dataready;
near byte sample;
near byte bitcnt;

#pragma udata VARS_LOCONET2
far byte LNPacket[32];
far byte LNIndex;
far byte LNSize;

// TMR0 generates a heartbeat every 32000000/4/2/80 == 50kHz.
#pragma interrupt scanLN
void scanLN(void) {

  // Timer0 interrupt handler
  if( INTCONbits.T0IF ) {
    byte inLN = LNRX;

    INTCONbits.T0IF  = 0;     // Clear interrupt flag
    TMR0L = 256 - 80;         // Reset counter with a correction of 10 cycles

    //LED4_LNRX = PORT_ON;

    if( LNstatus == STATUS_WAITSTART && inLN == 0 ) {
      LNstatus = STATUS_CONFSTART;
    }
    else if(LNstatus != STATUS_WAITSTART) {
      if( LNstatus == STATUS_CONFSTART ) {
        if(inLN == 0)
          LNstatus = STATUS_IGN1;
        else
          LNstatus = STATUS_WAITSTART;
      }
      else if( LNstatus == STATUS_IGN1 ) {
        LNstatus = STATUS_IGN2;
      }
      else if( LNstatus == STATUS_IGN2 ) {
        LNstatus = STATUS_SAMPLE;
      }
      else if( LNstatus == STATUS_SAMPLE ) {
        if( bitcnt == 8 ) {
          sampledata = sample;
          dataready  = TRUE;
          sample = 0;
          bitcnt = 0;
          LNstatus = STATUS_WAITSTART;
        }
        else {
          LNstatus = STATUS_IGN1;
          sample >>= 1;
          sample |= inLN;
          bitcnt++;
        }
      }
    }

    //LED4_LNRX = PORT_OFF;
  }

}


void ln2CBus(void) {
  switch( LNPacket[0]) {
    case OPC_GPON:
        canmsg.opc = OPC_RTON;
        canmsg.len = 0;
        canQueue(&canmsg);
      break;
    case OPC_GPOFF:
        canmsg.opc = OPC_RTOF;
        canmsg.len = 0;
        canQueue(&canmsg);
      break;
  }
  
}

byte getLNSize(byte opc) {
  if( opc == 0xE0 ) {
    /* Uhli exceptions */
    return 0;
  }
  else {
    switch (opc & 0xf0) {
    case 0x80:
        return 2;
    case 0xa0:
    case 0xb0:
        return 4;
    case 0xc0:
        return 6;
    case 0xe0:
      return -1; // Next byte is size.
    }
  }
  return 0;
}

void doLocoNet(void) {
  if( dataready ) {
    byte b = sampledata;
    if( LNIndex == 0 ) {
      if( b & 0x80 == 0 ) {
        // invalid start
        return;
      }

      LED4_LNRX = PORT_ON;
      ledLNRXtimer = 20;

      // OPC
      LNSize = getLNSize(b);
      if( LNSize == 0 ) {
        // invalid OPC
        return;
      }

    }

    if( LNSize == -1 && LNIndex == 1 ) {
      LNSize = b & 0x7F;
    }

    if( LNIndex < 32 ) {
      LNPacket[LNIndex] = b;
      LNIndex++;
      if( LNIndex == LNSize ) {
        // Packet complete.
        LNIndex = 0;
        // TODO: Translate to CBUS.
        ln2CBus();
      }
    }
    else {
      // reset
      LNIndex = 0;
    }
  }
}


byte checksumLN(byte* ln, byte crc) {
  byte x;
  return x == crc ? TRUE:FALSE;
}




void initLN(void) {
  LNstatus = STATUS_WAITSTART;
  sampledata = 0;
  dataready  = FALSE;
  sample = 0;
  bitcnt = 0;
  LNIndex = 0;
}

