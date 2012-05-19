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
near byte overrun;


#pragma udata VARS_LOCONET2
far byte SampleData[8];
far byte SampleFlag;
far byte readP;
far byte writeP;

#pragma udata VARS_LOCONET3
far byte LNPacket[32];
far byte LNIndex;
far byte LNSize;
far byte LNTimeout;
far byte LNByteIndex;
far byte LNBitIndex;
far byte LNWrittenBit;

#pragma udata VARS_LOCONET4
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

    LNSCAN = PORT_ON;

    samplepart++;
    if( samplepart > 2 ) {
      samplepart = 0;
      //LNSCAN = PORT_ON;
    }
    
    if( mode != LN_MODE_WRITE && LNstatus == STATUS_WAITSTART && inLN == 0 ) {
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
            if(dataready)
              overrun = TRUE;
            dataready  = TRUE;

            SampleData[writeP] = sample;
            SampleFlag |= (1 << writeP);
            writeP++;
            if( writeP > 7 )
              writeP = 0;
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
      if( mode == LN_MODE_READ ) {
        // End of stop bit.
        LNTX = PORT_OFF;
      }



      if( inLN == 1 && mode == LN_MODE_WRITE_REQ ) {
        idle++;
        if( mode == LN_MODE_WRITE && txtry > 20 ) {
          // Give up...
          mode = LN_MODE_READ;
          Wait4NN = TRUE;
          LED6_FLIM = PORT_ON;
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
          Wait4NN = TRUE;
          LED6_FLIM = PORT_ON;
        }
        else {
          if( LNBitIndex == 0 ) {
            // Start bit
            LNWrittenBit = PORT_OFF;
            LNBitIndex++;
          }
          else if( LNBitIndex == 9 ) {
            // Stop bit
            LNWrittenBit = PORT_ON;
            LNBitIndex = 0;
            LNByteIndex++;
            if( LNBuffer[LNIndex].len == LNByteIndex ) {
              LNBuffer[LNIndex].status = LN_STATUS_FREE;
              mode = LN_MODE_READ;
            }
          }
          else {
            LNWrittenBit = (LNBuffer[LNIndex].data[LNByteIndex] >> (LNBitIndex-1)) & 0x01;
            LNBitIndex++;
          }
        }
        LNTX = LNWrittenBit ^ 0x01; // Invert?

      }


    }

    LNSCAN = PORT_OFF;
  }

}


void ln2CBusErr(void) {
  canmsg.opc = 0xEA;
  canmsg.d[0] = LNPacket[0];
  canmsg.d[1] = LNPacket[1];
  canmsg.d[2] = LNPacket[2];
  canmsg.d[3] = LNPacket[3];
  canmsg.d[4] = LNPacket[4];
  canmsg.d[5] = LNPacket[5];
  canmsg.d[6] = LNPacket[6];
  canmsg.len = 7;
  canQueue(&canmsg);
  Wait4NN = TRUE;
  LED6_FLIM = PORT_ON;

}

void ln2CBusDebug(byte idx) {
  canmsg.opc = 0xEA;
  canmsg.d[0] = LNBuffer[idx].data[0];
  canmsg.d[1] = LNBuffer[idx].data[1];
  canmsg.d[2] = LNBuffer[idx].data[2];
  canmsg.d[3] = LNBuffer[idx].data[3];
  canmsg.d[4] = LNBuffer[idx].data[4];
  canmsg.d[5] = LNBuffer[idx].data[5];
  canmsg.d[6] = LNBuffer[idx].data[6];
  canmsg.len = 7;
  canQueue(&canmsg);

}

void ln2CBus(void) {
  unsigned int addrL, addrH, addr;
  unsigned int valL, valH, value;
  byte dir, type;

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

    case OPC_INPUT_REP:
      addrL = LNPacket[1] & 0x7f;
      addrH = LNPacket[2] & 0x0f;
      addr = addrL + (addrH << 7);
      addr = 1 + addr * 2 + ((((unsigned int) LNPacket[2] & 0x0020) >> 5));
      value = (LNPacket[2] & 0x10) >> 4;
      canmsg.opc = value ? OPC_ASON:OPC_ASOF;
      canmsg.d[0] = 0;
      canmsg.d[1] = 0;
      canmsg.d[2] = addr / 256;
      canmsg.d[3] = addr % 256;
      canmsg.len = 4;
      canQueue(&canmsg);
      break;

    case OPC_SW_REQ:
    case OPC_SW_STATE:
      addrL = LNPacket[1] & 0x7f;
      addrH = LNPacket[2] & 0x0f;
      addr = addrL + (addrH << 7);
      addr *= 2;
      addr += (LNPacket[2] & 0x20) >> 5;
      canmsg.opc = (LNPacket[2] & 0x10) ? OPC_ASON:OPC_ASOF;
      canmsg.d[0] = 0;
      canmsg.d[1] = 0;
      canmsg.d[2] = addr / 256;
      canmsg.d[3] = addr % 256;
      canmsg.len = 4;
      canQueue(&canmsg);
      break;

    case OPC_LISSY_REP: // E4
      addr = LNPacket[4] & 0x7F;
      valL = LNPacket[6] & 0x7F;
      valH = LNPacket[5] & 0x7F;
      value = valL + (valH << 7); // Ident.
      dir   = ( LNPacket[3] & 0x20 ) ? TRUE:FALSE;
      canmsg.opc  = OPC_ACON3;
      canmsg.d[0] = 0;
      canmsg.d[1] = 0;
      canmsg.d[2] = addr / 256;
      canmsg.d[3] = addr % 256;
      canmsg.d[4] = value / 256;
      canmsg.d[5] = value % 256;
      canmsg.d[6] = 0;
      canmsg.len = 7;
      canQueue(&canmsg);
      break;

    case OPC_MULTI_SENSE:
      type = LNPacket[1] & OPC_MULTI_SENSE_MSG;
      if( type == OPC_MULTI_SENSE_PRESENT || type == OPC_MULTI_SENSE_ABSENT ) {
        char zone;
        byte present;
        byte enter;
        unsigned int locoaddr;
        addrL = LNPacket[2];
        addrH = LNPacket[1] & 0x1F;
        addr = addrL + (addrH << 7);
        enter     = (LNPacket[1] & 0x20) != 0 ? TRUE:FALSE;

        addr++;

        if (LNPacket[3]==0x7D)
          locoaddr=LNPacket[4];
        else
          locoaddr=LNPacket[3]*128+LNPacket[4];

        if      ((LNPacket[2]&0x0F) == 0x00) zone = 'A';
        else if ((LNPacket[2]&0x0F) == 0x02) zone = 'B';
        else if ((LNPacket[2]&0x0F) == 0x04) zone = 'C';
        else if ((LNPacket[2]&0x0F) == 0x06) zone = 'D';
        else if ((LNPacket[2]&0x0F) == 0x08) zone = 'E';
        else if ((LNPacket[2]&0x0F) == 0x0A) zone = 'F';
        else if ((LNPacket[2]&0x0F) == 0x0C) zone = 'G';
        else if ((LNPacket[2]&0x0F) == 0x0E) zone = 'H';
        
        canmsg.opc  = (type == OPC_MULTI_SENSE_PRESENT) ? OPC_ACON3:OPC_ACOF3;
        canmsg.d[0] = 0;
        canmsg.d[1] = 0;
        canmsg.d[2] = addr / 256;
        canmsg.d[3] = addr % 256;
        canmsg.d[4] = locoaddr / 256;
        canmsg.d[5] = locoaddr % 256;
        canmsg.d[6] = zone;
        canmsg.len = 7;
        canQueue(&canmsg);
        
      }
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

byte doLocoNet(void) {
  /*

  SampleData[writeP] = sample;
  SampleFlag |= (1 << writeP);
  writeP++;
  if( writeP > 7 )
    writeP = 0;

   */

  if( SampleFlag & (1 << readP) ) {
    byte b = SampleData[readP];
    SampleFlag &= ~(1 << readP);
    readP++;
    if( readP > 7 )
      readP = 0;

    LNTimeout = 0;
    
    if( LNIndex == 0 ) {
      if( b & 0x80 == 0 ) {
        // invalid start
        return 1;
      }

      //memset(LNPacket, 0, sizeof(LNPacket));

      // OPC
      //LNSize = getLNSize(b);

      if( b == 0xE0 ) {
        /* Uhli exceptions */
        LNSize = -2;
      }
      else if( (b & 0xf0) == 0x80 )
        LNSize = 2;
      else if( (b & 0xf0) == 0xA0 )
        LNSize = 4;
      else if( (b & 0xf0) == 0xB0 )
        LNSize = 4;
      else if( (b & 0xf0) == 0xC0 )
        LNSize = 6;
      else if( (b & 0xf0) == 0xE0 )
        LNSize = -1;
      else
        LNSize = 0;


      if( LNSize == 0 || LNSize == -2 ) {
        // invalid OPC
        LNPacket[0] = b;
        LNPacket[1] = LNSize;
        LNPacket[2] = 0x0A;
        LNPacket[3] = overrun;
        LNPacket[4] = 0xFF;
        overrun = FALSE;
        ln2CBusErr();
        return 1;
      }

    }

    if( LNSize == -1 && LNIndex == 1 ) {
      LNSize = b & 0x7F;
      //LNPacket[1] = LNSize;
      //LNPacket[2] = 2;
      //LNPacket[3] = overrun;
      //overrun = FALSE;
      //ln2CBusErr(); // debug
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

    return 1;
  }


  return 0;
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
  SampleFlag = 0;
  readP = 0;
  writeP = 0;

  sample = 0;
  bitcnt = 0;
  LNIndex = 0;
  LNTimeout = 0;
  samplepart = 0;
  overrun = FALSE;
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
  unsigned int addr;
  byte i = 0;
  for( i = 0; i < LN_BUFFER_SIZE; i++ ) {
    if( LNBuffer[i].status == LN_STATUS_FREE) {
      break;
    }
  }

  if( i >= LN_BUFFER_SIZE ) {
    // Buffer overflow...
    Wait4NN = TRUE;
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
      //ln2CBusDebug(i);
      break;

    case OPC_RTOF:
      LNBuffer[i].len = 2;
      LNBuffer[i].data[0] = OPC_GPOFF;
      checksumLN(i);
      LNBuffer[i].status = LN_STATUS_USED;
      if( mode == LN_MODE_READ )
        mode = LN_MODE_WRITE_REQ;
      //ln2CBusDebug(i);
      break;

    case OPC_RESTP:
      LNBuffer[i].len = 2;
      LNBuffer[i].data[0] = OPC_IDLE;
      checksumLN(i);
      LNBuffer[i].status = LN_STATUS_USED;
      if( mode == LN_MODE_READ )
        mode = LN_MODE_WRITE_REQ;
      break;

    case OPC_ASRQ:
      addr  = rx_ptr->d3 * 256;
      addr += rx_ptr->d4;
      if( addr == SOD && (NV1 & CFG_ENABLE_SOD) ) {
        // Start Of Day
        LNBuffer[i].len = 4;
        LNBuffer[i].data[0] = OPC_SW_REQ;
        LNBuffer[i].data[1] = 1017&0x7F;
        LNBuffer[i].data[2] = (1017/128)&0x0F;
        checksumLN(i);
        LNBuffer[i].status = LN_STATUS_USED;
        if( mode == LN_MODE_READ )
          mode = LN_MODE_WRITE_REQ;
      }
      break;

    case OPC_ACON:
    case OPC_ACOF:
      addr  = rx_ptr->d3 * 256;
      addr += rx_ptr->d4;
      if( addr >= SWStart && addr <= SWEnd ) {
        // Switch
        byte dir = addr % 2;
        addr = addr / 2;
        LNBuffer[i].len = 4;
        LNBuffer[i].data[0] = OPC_SW_REQ;
        LNBuffer[i].data[1] = addr & 0x7F;
        LNBuffer[i].data[2] = (addr >> 7)&0x0F;
        LNBuffer[i].data[2] += ((rx_ptr->d0 == OPC_ACON) ? 0x10:0x00);
        LNBuffer[i].data[2] += (dir ? 0x20:0x00);
        checksumLN(i);
        LNBuffer[i].status = LN_STATUS_USED;
        if( mode == LN_MODE_READ )
          mode = LN_MODE_WRITE_REQ;
      }
      else if( (NV1 & CFG_ENABLE_FB2LN) && addr >= FBStart && addr <= FBEnd ) {
        // Sensor
        byte dir = addr % 2;
        addr = addr / 2;
        LNBuffer[i].len = 4;
        LNBuffer[i].data[0] = OPC_INPUT_REP;
        LNBuffer[i].data[1] = addr & 0x7F;
        LNBuffer[i].data[2] = (addr >> 7)&0x0F;
        LNBuffer[i].data[2] += ((rx_ptr->d0 == OPC_ACON) ? 0x10:0x00);
        LNBuffer[i].data[2] += (dir ? 0x20:0x00);
        checksumLN(i);
        LNBuffer[i].status = LN_STATUS_USED;
        if( mode == LN_MODE_READ )
          mode = LN_MODE_WRITE_REQ;
      }
      break;

  }

}
