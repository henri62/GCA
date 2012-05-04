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
far byte LNTimeout;

// TMR0 generates a heartbeat every 32000000/4/2/80 == 50kHz.
#pragma interrupt scanLN
void scanLN(void) {

  // Timer0 interrupt handler
  if( INTCONbits.T0IF ) {
    byte inLN = LNRX;

    INTCONbits.T0IF  = 0;     // Clear interrupt flag
    TMR0L = 256 - 80 + 12;         // Reset counter with a correction of 10 cycles

    LED3_LNTX = PORT_ON;

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

    LED3_LNTX = PORT_OFF;
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
    dataready = FALSE;
    LNTimeout = 0;
    
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
        // Translate to CBUS.
        ln2CBus();
      }
    }
    else {
      // reset
      LNIndex = 0;
    }
  }
}


// 1ms
void LocoNetWD(void) {
  if( LNIndex > 0 || LNIndex == -1 ) {
    LNTimeout++;
    if( LNTimeout > 100 ) {
      // Reset
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
  LNTimeout = 0;
}

