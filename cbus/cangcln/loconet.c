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

#include <string.h>
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
near byte mode;
near byte txtry;
near byte samplepart;
near byte idle;

#pragma udata VARS_LOCONET2
far byte LNPacket[32];
far byte LNIndex;
far byte LNSize;
far byte LNTimeout;
far byte LNByteIndex;
far byte LNBitIndex;
far byte LNWrittenBit;

#pragma udata VARS_LOCONET3
far LNPACKET LNBuffer[LN_BUFFER_SIZE];
far byte LNBufferIndex;


// TMR0 generates a heartbeat every 32000000/4/2/80 == 50kHz.
#pragma interrupt scanLN
void scanLN(void) {

  // Timer0 interrupt handler
  if( INTCONbits.T0IF ) {
    byte inLN = LNRX;

    INTCONbits.T0IF  = 0;     // Clear interrupt flag
    TMR0L = 256 - 80 + 18;         // Reset counter with a correction of 10 cycles

    //LNSCAN = PORT_ON;

    samplepart++;
    if( samplepart > 2 ) {
      samplepart = 0;
      LNSCAN = PORT_ON;
    }
    
    if( LNstatus == STATUS_WAITSTART && inLN == 0 ) {
      idle = 0;
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
          if( inLN == 1 ) {
            sampledata = sample;
            dataready  = TRUE;
          }
          else {
            // No stop bit; Framing error.
            LNIndex = 0;
          }
          sample = 0;
          bitcnt = 0;
          LNstatus = STATUS_WAITSTART;
        }
        else {
          LNstatus = STATUS_IGN1;
          sample >>= 1;
          sample |= (inLN << 7);
          bitcnt++;
        }
      }
    }

    if( samplepart == 0 ) {
      if( inLN == 1 && mode == LN_MODE_WRITE_REQ ) {
        idle++;
        if( mode == LN_MODE_WRITE && txtry > 20 ) {
          // Give up...
          mode = LN_MODE_READ;
        }
        else if( idle > (6 + (20-txtry))) {
          byte i;
          for( i = 0; i < LN_BUFFER_SIZE; i++ ) {
            if( LNBuffer[i].status == LN_STATUS_USED) {
              LNBuffer[i].status = LN_STATUS_PENDING;
              LNIndex = i;
              break;
            }
          }
          if( i < LN_BUFFER_SIZE ) {
            // Try to send...
            txtry++;
            mode = LN_MODE_WRITE;
            LNByteIndex = 0;
            LNBitIndex = 0;
            LNWrittenBit = 1;

            LED3_LNTX = PORT_ON;
            ledLNTXtimer = 20;
          }
          else {
            // nothing todo...
            mode = LN_MODE_READ;
          }
        }
      }

      // The write...
      if( mode == LN_MODE_WRITE ) {
        // Check if the inLN equals the last send bit for collision detection.
        if( LNWrittenBit != inLN ) {
          // Collision!
          LNByteIndex = 0;
          LNBitIndex = 0;
          mode = LN_MODE_WRITE_REQ;
          LED6_FLIM = PORT_ON;
        }
        else {
          if( LNBitIndex == 0 ) {
            // Start bit
            LNWrittenBit = PORT_OFF;
          }
          else if( LNBitIndex == 9 ) {
            // Stop bit
            LNWrittenBit = PORT_ON;
            LNBitIndex = 0;
            LNByteIndex++;
            if( LNBuffer[LNIndex].len == LNByteIndex ) {
              mode = LN_MODE_READ;
            }
          }
          else {
            LNWrittenBit = (LNBuffer[LNIndex].data[LNByteIndex] >> (LNBitIndex-1)) & 0x01;
          }
        }
        LNTX = LNWrittenBit ^ 0x01; // Invert?
        LNBitIndex++;

        if( mode == LN_MODE_READ ) {
          LNTX = PORT_OFF;
        }
      }
    }

    LNSCAN = PORT_OFF;
  }

}


void ln2CBusErr(void) {
  canmsg.opc = OPC_ACON3;
  canmsg.d[0] = LNPacket[0];
  canmsg.d[1] = LNPacket[1];
  canmsg.d[2] = LNPacket[2];
  canmsg.d[3] = LNPacket[3];
  canmsg.d[4] = LNPacket[4];
  canmsg.d[5] = LNPacket[5];
  canmsg.d[6] = LNPacket[6];
  canmsg.len = 7;
  canQueue(&canmsg);
  
}

void ln2CBus(void) {
  LED4_LNRX = PORT_ON;
  ledLNRXtimer = 20;
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
    return -2;
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

      memset(LNPacket, 0, sizeof(LNPacket));

      // OPC
      LNSize = getLNSize(b);
      if( LNSize == 0 || LNSize == -2 ) {
        // invalid OPC
        LNPacket[0] = b;
        LNPacket[1] = LNSize;
        ln2CBusErr();
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


void initLN(void) {
  byte i;
  for( i = 0; i < LN_BUFFER_SIZE; i++ ) {
    LNBuffer[i].status = LN_STATUS_FREE;
  }

  LNstatus = STATUS_WAITSTART;
  sampledata = 0;
  dataready  = FALSE;
  sample = 0;
  bitcnt = 0;
  LNIndex = 0;
  LNTimeout = 0;
  samplepart = 0;
  mode = LN_MODE_READ;
}

void checksumLN(byte idx) {
  byte chksum = 0xff;
  int i;
  for (i = 0; i < LNBuffer[idx].len-1; i++) {
    chksum ^= LNBuffer[idx].data[i];
  }
  LNBuffer[idx].data[i] = chksum;
}

void send2LocoNet(void) {
  byte i = 0;
  for( i = 0; i < LN_BUFFER_SIZE; i++ ) {
    if( LNBuffer[i].status == LN_STATUS_FREE) {
      break;
    }
  }

  if( i >= LN_BUFFER_SIZE ) {
    // Buffer overflow...
    LED6_FLIM = PORT_ON;
    return;
  }

  if( mode != LN_MODE_WRITE_REQ ) {
    idle = 0; // reset idle timer
    txtry = 0;
  }

  switch(rx_ptr->d0) {
    case OPC_RTON:
      LNBuffer[i].len = 2;
      LNBuffer[i].data[0] = OPC_GPON;
      checksumLN(i);
      LNBuffer[i].status = LN_STATUS_USED;
      if( mode == LN_MODE_READ )
        mode = LN_MODE_WRITE_REQ;
      break;
    case OPC_RTOF:
      LNBuffer[i].len = 2;
      LNBuffer[i].data[0] = OPC_GPOFF;
      checksumLN(i);
      LNBuffer[i].status = LN_STATUS_USED;
      if( mode == LN_MODE_READ )
        mode = LN_MODE_WRITE_REQ;
      break;
  }

}
