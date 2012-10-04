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
#include "canbus.h"
#include "utils.h"
#include "cbusdefs.h"
#include "lnconst.h"

#pragma udata access VARS_LOCONET1
near byte work;
near byte LNstatus;
near byte sampledata;
near byte dataready;
near byte framingerr;
near byte sample;
near byte bitcnt;
near byte mode;
near byte txtry;
near byte samplepart;
near byte idle;
near byte overrun;
near byte packetpending;
near byte CDBackoff;


#pragma udata VARS_LOCONET2
far byte SampleData[8];
far byte SampleFlag;
far byte readP;
far byte writeP;
far unsigned short throttleid[32];
far byte slottimer[LN_SLOTS];


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

#pragma udata VARS_LOCONET5
far LNSLOT slotmap[LN_SLOTS];
far byte dispatchSlot;

// #define INVERTLNTX 0x00

// TMR0 generates a heartbeat every 32000000/4/2/80 == 50kHz.
#pragma interrupt scanLN

void scanLN(void) {

  // Timer0 interrupt handler
  if (INTCONbits.T0IF) {
    byte inLN = LNRX;

    INTCONbits.T0IF = 0; // Clear interrupt flag
    //        TMR0L = 195; // Reset counter with a correction of 10 cycles
    TMR0L = 197; // Reset counter with a correction of 12 cycles

    //LNSCAN = PORT_ON;

    samplepart++;
    if (samplepart > 2) {
      samplepart = 0;
      LNSCAN = PORT_ON;

      if (CDBackoff > 0) {
        CDBackoff--;
        LNTX = PORT_OFF;
      }
    }

    if (CDBackoff > 0) {
      return;
    }

    if (mode != LN_MODE_WRITE && LNstatus == STATUS_WAITSTART && inLN == 0) {
      idle = 0;
      LNstatus = STATUS_CONFSTART;
    } else if (LNstatus != STATUS_WAITSTART) {
      if (LNstatus == STATUS_CONFSTART) {
        if (inLN == 0) {
          LNstatus = STATUS_IGN1;
        } else
          LNstatus = STATUS_WAITSTART;
      } else if (LNstatus == STATUS_IGN1) {
        LNstatus = STATUS_IGN2;
      } else if (LNstatus == STATUS_IGN2) {
        LNstatus = STATUS_SAMPLE;
      } else if (LNstatus == STATUS_SAMPLE) {
        if (bitcnt == 8) {
          if (inLN == 1) {
            sampledata = sample;
            if (dataready)
              overrun = TRUE;
            dataready = TRUE;

            SampleData[writeP] = sample;
            SampleFlag |= (1 << writeP);
            writeP++;
            if (writeP > 7)
              writeP = 0;
          } else {
            // No stop bit; Framing error.
            LNIndex = 0;
            packetpending = FALSE;

            framingerr = TRUE;
            overrun = FALSE;
            dataready = FALSE;
          }
          sample = 0;
          bitcnt = 0;
          LNstatus = STATUS_WAITSTART;
        } else {
          LNstatus = STATUS_IGN1;
          sample >>= 1;
          sample |= (inLN << 7);
          bitcnt++;
        }
      }
    }

    if (samplepart == 0) {
      if (mode == LN_MODE_READ) {
        // End of stop bit.
        LNTX = PORT_ON;
        dataready = FALSE;
        txtry = 0;
      }



      if (inLN == 1 && mode == LN_MODE_WRITE_REQ) {
        idle++;
        if (mode == LN_MODE_WRITE && txtry > 20) {
          // Give up...
          mode = LN_MODE_READ;
          Wait4NN = TRUE;
          LED6_FLIM = PORT_ON;
        } else if (idle > (6 + (20 - txtry))) {
          byte i;
          for (i = 0; i < LN_BUFFER_SIZE; i++) {
            if (LNBuffer[i].status == LN_STATUS_USED) {
              LNBuffer[i].status = LN_STATUS_PENDING;
              LNIndex = i;
              break;
            }
          }
          if (i < LN_BUFFER_SIZE) {
            // Try to send...
            txtry++;
            mode = LN_MODE_WRITE;
            LNByteIndex = 0;
            LNBitIndex = 0;
            LNWrittenBit = 1;

            LED3_LNTX = PORT_ON;
            ledLNTXtimer = 20;
          } else {
            // nothing todo...
            mode = LN_MODE_READ;
          }
        }
      }

      // The write...
      if (mode == LN_MODE_WRITE) {
        // Check if the inLN equals the last send bit for collision detection.
        if (LNWrittenBit != inLN) {
          // Collision!
          CDBackoff = 20;
          LNWrittenBit = PORT_ON;

          LNByteIndex = 0;
          LNBitIndex = 0;
          mode = LN_MODE_WRITE_REQ;
          Wait4NN = TRUE;
          LED6_FLIM = PORT_ON;
        } else {
          if (LNBitIndex == 0) {
            // Start bit
            LNWrittenBit = PORT_OFF;
            LNBitIndex++;
          } else if (LNBitIndex == 9) {
            // Stop bit
            LNWrittenBit = PORT_ON;
            LNBitIndex = 0;
            LNByteIndex++;
            if (LNBuffer[LNIndex].len == LNByteIndex) {
              LNBuffer[LNIndex].status = LN_STATUS_FREE;
              mode = LN_MODE_READ;
            }
          } else {
            LNWrittenBit = (LNBuffer[LNIndex].data[LNByteIndex] >> (LNBitIndex - 1)) & 0x01;
            LNBitIndex++;
          }
        }
        LNTX = LNWrittenBit; // Invert?

      }


    }

    LNSCAN = PORT_OFF;
  }

}

void ln2CBusErr(void) {
  CANMsg canmsg;
  canmsg.b[d0] = 0xEA;
  canmsg.b[d1] = LNPacket[0];
  canmsg.b[d2] = LNPacket[1];
  canmsg.b[d3] = LNPacket[2];
  canmsg.b[d4] = LNPacket[3];
  canmsg.b[d5] = LNPacket[4];
  canmsg.b[d6] = LNPacket[5];
  canmsg.b[d7] = LNPacket[6];
  canmsg.b[dlc] = 8;
  canbusSend(&canmsg);

}

void ln2CBusDebug(byte idx) {
  CANMsg canmsg;
  canmsg.b[d0] = 0xEA;
  canmsg.b[d1] = LNBuffer[idx].data[0];
  canmsg.b[d2] = LNBuffer[idx].data[1];
  canmsg.b[d3] = LNBuffer[idx].data[2];
  canmsg.b[d4] = LNBuffer[idx].data[3];
  canmsg.b[d5] = LNBuffer[idx].data[4];
  canmsg.b[d6] = LNBuffer[idx].data[5];
  canmsg.b[d7] = LNBuffer[idx].data[6];
  canmsg.b[dlc] = 8;
  canbusSend(&canmsg);

}

void checksumLN(byte idx) {
  byte chksum = 0xff;
  int i;
  for (i = 0; i < LNBuffer[idx].len - 1; i++) {
    chksum ^= LNBuffer[idx].data[i];
  }
  LNBuffer[idx].data[i] = chksum;
}

void longAck(byte i, byte opc, byte rc) {
  LNBuffer[i].len = 4;
  LNBuffer[i].data[0] = OPC_LONG_ACK;
  LNBuffer[i].data[1] = (opc & 0x7F);
  LNBuffer[i].data[2] = (rc & 0x7F);
  checksumLN(i);
  LNBuffer[i].status = LN_STATUS_USED;
}

/*
20120824.075435.390 r0000B lnudprea lbudp    *trace dump( 0x0EEBADA6: length=4 )
    offset:   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |ASCII...........|
    --------------------------------------------------------- |----------------|
    00000000: BF 00 5B 1B                                     |..[.            |
20120824.075435.391 r0000B lnudprea lbudp    *trace dump( 0x0EEBADA6: length=2 )
    offset:   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |ASCII...........|
    --------------------------------------------------------- |----------------|
    00000000: 81 7E                                           |.~              |
20120824.075435.392 r0000B lnudprea lbudp    *trace dump( 0x0EEBADA6: length=14 )
    offset:   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |ASCII...........|
    --------------------------------------------------------- |----------------|
    00000000: E7 0E 17 03 5B 00 00 04 00 00 00 00 00 5D       |....[........]  |
20120824.075435.393 r0000B lnudprea lbudp    *trace dump( 0x0EEBADA6: length=4 )
    offset:   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |ASCII...........|
    --------------------------------------------------------- |----------------|
    00000000: BA 17 17 45                                     |...E            |
20120824.075435.394 r0000B lnudprea lbudp    *trace dump( 0x0EEBADA6: length=14 )
    offset:   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |ASCII...........|
    --------------------------------------------------------- |----------------|
    00000000: E7 0E 17 33 5B 00 00 04 00 00 00 00 00 6D       |...3[........m  |
20120824.075435.395 r0000B lnudprea lbudp    *trace dump( 0x0EEBADA6: length=14 )
    offset:   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |ASCII...........|
    --------------------------------------------------------- |----------------|
    00000000: EF 0E 17 33 5B 00 00 04 00 00 00 66 75 76       |...3[......fuv  |
20120824.075435.397 r9999c lnreader lnmon    0288 Request slot for loco address 91
20120824.075435.397 r0000B lnudprea lbudp    *trace dump( 0x0EEBADA6: length=4 )
    offset:   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |ASCII...........|
    --------------------------------------------------------- |----------------|
20120824.075435.397 r9999B lnreader OLocoNet 1001 *** read dump:
    00000000: B4 6F 7F 5B                                     |.o.[            |
20120824.075435.397 r0000B lnreader OLocoNet *trace dump( 0x0EEFDE28: length=4 )
    offset:   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |ASCII...........|
    --------------------------------------------------------- |----------------|
    00000000: BF 00 5B 1B                                     |..[.            |

 */

// offset 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D
//              ## S1 A1 SP DF TK S2 A2 SD ID ID CK
// data   E7 0E 17 33 5B 00 00 04 00 00 00 00 00 6D DCS100
//        E7 0E 17 23 5B 00 08 06 00 00 00 66 75 64

//        E7 0E 01 33 5B 01 20 05 00 00 00 00 00 5B GCLN
//        E7 0E 01 33 5B 01 20 05 00 00 00 00 00 5B
//        E7 0E 01 33 13 01 20 05 00 07 00 00 00 14

void slotRead(byte i, byte slot, byte inuse) {
  LNBuffer[i].len = 14;
  LNBuffer[i].data[ 0] = OPC_SL_RD_DATA;
  LNBuffer[i].data[ 1] = 0x0E;
  LNBuffer[i].data[ 2] = slot;
  LNBuffer[i].data[ 3] = DEC_MODE_128; // status 1
  if (inuse || throttleid[slot] > 0)
    LNBuffer[i].data[ 3] = DEC_MODE_128 | LOCO_IN_USE; // status 1
  LNBuffer[i].data[ 4] = (slotmap[slot].addr & 0x7F);
  LNBuffer[i].data[ 5] = (slotmap[slot].speed & 0x7F);
  LNBuffer[i].data[ 6] = (slotmap[slot].speed & 0x80) ? 0 : DIRF_DIR;
  LNBuffer[i].data[ 6] |= (slotmap[slot].f[0] & 0x01) ? DIRF_F1 : 0;
  LNBuffer[i].data[ 6] |= (slotmap[slot].f[0] & 0x02) ? DIRF_F2 : 0;
  LNBuffer[i].data[ 6] |= (slotmap[slot].f[0] & 0x04) ? DIRF_F3 : 0;
  LNBuffer[i].data[ 6] |= (slotmap[slot].f[0] & 0x08) ? DIRF_F3 : 0;
  LNBuffer[i].data[ 6] |= (slotmap[slot].f[0] & 0x10) ? DIRF_F0 : 0;
  LNBuffer[i].data[ 7] = GTRK_MLOK1 | GTRK_POWER; // track status
  LNBuffer[i].data[ 8] = 0;
  LNBuffer[i].data[ 9] = ((slotmap[slot].addr / 128) & 0x7F);
  LNBuffer[i].data[10] = 0; // sound
  LNBuffer[i].data[10] |= (slotmap[slot].f[1] & 0x01) ? SND_F5 : 0;
  LNBuffer[i].data[10] |= (slotmap[slot].f[1] & 0x02) ? SND_F6 : 0;
  LNBuffer[i].data[10] |= (slotmap[slot].f[1] & 0x04) ? SND_F7 : 0;
  LNBuffer[i].data[10] |= (slotmap[slot].f[1] & 0x08) ? SND_F8 : 0;
  LNBuffer[i].data[11] = (throttleid[slot] & 0x7F); // IDL
  LNBuffer[i].data[12] = (throttleid[slot] >> 7); // IDH
  checksumLN(i);
  LNBuffer[i].status = LN_STATUS_USED;
}

void ln2CBus(void) {
  unsigned int addrL, addrH, addr;
  unsigned int valL, valH, value;
  byte dir, type, i, slot;
  CANMsg canmsg;

  ln2CBusErr();

  LED4_LNRX = PORT_ON;
  ledLNRXtimer = 20;
  switch (LNPacket[0]) {

    case OPC_GPON:
      canmsg.b[d0] = OPC_RTON;
      canmsg.b[dlc] = 1;
      canbusSend(&canmsg);
      break;

    case OPC_GPOFF:
      canmsg.b[d0] = OPC_RTOF;
      canmsg.b[dlc] = 1;
      canbusSend(&canmsg);
      break;

    case OPC_SL_RD_DATA:
      break;

    case OPC_WR_SL_DATA:
      slot = LNPacket[2];
      if (slot != 0 && slot < LN_SLOTS && slotmap[slot].session != LN_SLOT_UNUSED) {
        // ack with a slot read
        throttleid[slot] = LNPacket[12];
        throttleid[slot] <<= 7;
        throttleid[slot] += LNPacket[11];
        for (i = 0; i < LN_BUFFER_SIZE; i++) {
          if (LNBuffer[i].status == LN_STATUS_FREE) {
            longAck(i, OPC_WR_SL_DATA, 0x7F); // weird return code
            break;
          }
        }
      } else if (slot == 0x7C) {
        /* Programming slot */
        unsigned short cvNumber;
        byte cvData;
        byte pcmd = LNPacket[3]; // programmer command
        byte hopsa = LNPacket[5]; // Ops mode - 7 high address bits of loco to program
        byte lopsa = LNPacket[6]; // Ops mode - 7 low  address bits of loco to program
        byte cvh = LNPacket[8]; // hi 3 bits of CV# and msb of data7
        byte cvl = LNPacket[9]; // lo 7 bits of CV#
        byte data7 = LNPacket[10]; // 7 bits of data to program, msb is in cvh above
        unsigned short addr = (((lopsa & 0x7f) * 128) + (hopsa & 0x7f));
        byte n;

        cvData = (((cvh & CVH_D7) << 6) | (data7 & 0x7f)); // was PROG_DATA
        cvNumber = (((((cvh & CVH_CV8_CV9) >> 3) | (cvh & CVH_CV7)) * 128)
          + (cvl & 0x7f)) + 1; // was PROG_CV_NUM(progTask)

        if (pcmd & PCMD_RW) {
          // write CV
          if (pcmd & PCMD_OPS_MODE) {
            slot = LN_SLOTS;
            for (n = 0; n < LN_SLOTS; n++) {
              if (slotmap[n].session != LN_SLOT_UNUSED && slotmap[n].addr == addr) {
                slot = n;
                canmsg.b[d0] = OPC_WCVO;
                canmsg.b[dlc] = 5;
                canmsg.b[d1] = slotmap[n].session;
                canmsg.b[d2] = cvNumber / 256;
                canmsg.b[d3] = cvNumber % 256;
                canmsg.b[d4] = cvData;
                canbusSend(&canmsg);
                break;
              }
            }
            if (slot == LN_SLOTS) {
              // no session
              for (i = 0; i < LN_BUFFER_SIZE; i++) {
                if (LNBuffer[i].status == LN_STATUS_FREE) {
                  longAck(i, OPC_WR_SL_DATA, 0);
                  break;
                }
              }
            }
          } else {
            canmsg.b[d0] = OPC_WCVS;
            canmsg.b[dlc] = 6;
            canmsg.b[d1] = 0;
            canmsg.b[d2] = cvNumber / 256;
            canmsg.b[d3] = cvNumber % 256;
            canmsg.b[d4] = CVMODE_PAGE;
            canmsg.b[d5] = cvData;
            canbusSend(&canmsg);
          }
        } else {
          // read CV
          canmsg.b[d0] = OPC_QCVS;
          canmsg.b[dlc] = 5;
          canmsg.b[d1] = 0;
          canmsg.b[d2] = cvNumber / 256;
          canmsg.b[d3] = cvNumber % 256;
          canmsg.b[d4] = CVMODE_PAGE;
          canbusSend(&canmsg);
        }

        for (i = 0; i < LN_BUFFER_SIZE; i++) {
          if (LNBuffer[i].status == LN_STATUS_FREE) {
            longAck(i, OPC_WR_SL_DATA, 1);
            break;
          }
        }
      } else {
        for (i = 0; i < LN_BUFFER_SIZE; i++) {
          if (LNBuffer[i].status == LN_STATUS_FREE) {
            longAck(i, OPC_WR_SL_DATA, 0);
            break;
          }
        }
      }
      break;


    case OPC_LOCO_ADR:
    {
      byte n;
      addrH = LNPacket[1]&0x7F;
      addrL = LNPacket[2]&0x7F;
      addr = addrL + (addrH << 7);

      slot = LN_SLOTS;
      for (n = 0; n < LN_SLOTS; n++) {
        if (slotmap[n].session != LN_SLOT_UNUSED && slotmap[n].addr == addr) {
          slot = n;
          break;
        }
      }

      if (slot == LN_SLOTS) {
        /*
        for( i = 0; i < LN_BUFFER_SIZE; i++ ) {
          if( LNBuffer[i].status == LN_STATUS_FREE) {
            LNBuffer[i].len = 2;
            LNBuffer[i].data[0] = OPC_GPBUSY;
            LNBuffer[i].status = LN_STATUS_USED;
            checksumLN(i);
          }
        }
         */
        canmsg.b[d0] = OPC_RLOC;
        canmsg.b[dlc] = 3;
        canmsg.b[d1] = addr / 256;
        canmsg.b[d2] = addr % 256;
        if (addr > 127) {
          canmsg.b[d1] |= 0xC0;
        }
        canbusSend(&canmsg);
      } else {
        for (i = 0; i < LN_BUFFER_SIZE; i++) {
          if (LNBuffer[i].status == LN_STATUS_FREE) {
            slotRead(i, slot, FALSE);
            break;
          }
        }
      }
    }
      break;

    case OPC_LOCO_SPD:
      slot = LNPacket[1];
      if (slot < LN_SLOTS && slotmap[slot].session != LN_SLOT_UNUSED) {
        slotmap[slot].speed &= 0x80; // save direction flag
        slotmap[slot].speed |= (LNPacket[2] & 0x7F);
        slottimer[slot] = 0;
        canmsg.b[d0] = OPC_DSPD;
        canmsg.b[dlc] = 3;
        canmsg.b[d1] = slotmap[slot].session;
        canmsg.b[d2] = slotmap[slot].speed;
        canbusSend(&canmsg);
      }
      break;

    case OPC_LOCO_DIRF:
      slot = LNPacket[1];
      if (slot < LN_SLOTS && slotmap[slot].session != LN_SLOT_UNUSED) {
        slotmap[slot].speed &= 0x7F; // save speed
        slotmap[slot].speed |= (LNPacket[2] & DIRF_DIR) ? 0x00 : 0x80;
        slotmap[slot].f[0] = (LNPacket[2] & DIRF_F1) ? 0x01 : 0x00;
        slotmap[slot].f[0] |= (LNPacket[2] & DIRF_F2) ? 0x02 : 0x00;
        slotmap[slot].f[0] |= (LNPacket[2] & DIRF_F3) ? 0x04 : 0x00;
        slotmap[slot].f[0] |= (LNPacket[2] & DIRF_F4) ? 0x08 : 0x00;
        slotmap[slot].f[0] |= (LNPacket[2] & DIRF_F0) ? 0x10 : 0x00;
        slottimer[slot] = 0;

        canmsg.b[d0] = OPC_DSPD;
        canmsg.b[dlc] = 3;
        canmsg.b[d1] = slotmap[slot].session;
        canmsg.b[d2] = slotmap[slot].speed;
        canbusSend(&canmsg);

        canmsg.b[d0] = OPC_DFUN;
        canmsg.b[dlc] = 4;
        canmsg.b[d1] = slotmap[slot].session;
        canmsg.b[d2] = 1;
        canmsg.b[d3] = slotmap[slot].f[0];
        canbusSend(&canmsg);

      }
      break;

    case OPC_LOCO_SND:
      slot = LNPacket[1];
      if (slot < LN_SLOTS && slotmap[slot].session != LN_SLOT_UNUSED) {
        slotmap[slot].f[1] = (LNPacket[2] & SND_F5) ? 0x01 : 0x00;
        slotmap[slot].f[1] |= (LNPacket[2] & SND_F6) ? 0x02 : 0x00;
        slotmap[slot].f[1] |= (LNPacket[2] & SND_F7) ? 0x04 : 0x00;
        slotmap[slot].f[1] |= (LNPacket[2] & SND_F8) ? 0x08 : 0x00;
        slottimer[slot] = 0;

        canmsg.b[d0] = OPC_DFUN;
        canmsg.b[dlc] = 4;
        canmsg.b[d1] = slotmap[slot].session;
        canmsg.b[d2] = 2;
        canmsg.b[d3] = slotmap[slot].f[1];
        canbusSend(&canmsg);

      }
      break;

    case OPC_MOVE_SLOTS:
      if (LNPacket[1] > 0 && LNPacket[2] > 0 && LNPacket[1] == LNPacket[2]) {
        // Set slot in use.
        slot = LNPacket[1] & 0x7f;
        if (slot < LN_SLOTS && slotmap[slot].session != LN_SLOT_UNUSED) {
          throttleid[slot] = 1; // temp id
          // ack with a slot read
          for (i = 0; i < LN_BUFFER_SIZE; i++) {
            if (LNBuffer[i].status == LN_STATUS_FREE) {
              slotRead(i, slot, TRUE);
              break;
            }
          }
        }
      } else if (LNPacket[1] > 0 && LNPacket[2] == 0) {
        // dispatch put
        dispatchSlot = LNPacket[1] & 0x7f;
        if (dispatchSlot < LN_SLOTS && slotmap[dispatchSlot].session != LN_SLOT_UNUSED) {
          // ack with a slot read
          throttleid[slot] = 0; // temp id
          for (i = 0; i < LN_BUFFER_SIZE; i++) {
            if (LNBuffer[i].status == LN_STATUS_FREE) {
              slotRead(i, dispatchSlot, FALSE);
              break;
            }
          }
        } else {
          for (i = 0; i < LN_BUFFER_SIZE; i++) {
            if (LNBuffer[i].status == LN_STATUS_FREE) {
              longAck(i, OPC_MOVE_SLOTS, 0);
              break;
            }
          }
        }
      } else if (LNPacket[1] == 0 && LNPacket[2] == 0) {
        // dispatch get
        if (dispatchSlot != LN_SLOT_UNUSED) {
          // ack with a slot read
          throttleid[dispatchSlot] = 1; // temp id
          for (i = 0; i < LN_BUFFER_SIZE; i++) {
            if (LNBuffer[i].status == LN_STATUS_FREE) {
              slotRead(i, dispatchSlot, FALSE);
              break;
            }
          }
          dispatchSlot = LN_SLOT_UNUSED;
        } else {
          for (i = 0; i < LN_BUFFER_SIZE; i++) {
            if (LNBuffer[i].status == LN_STATUS_FREE) {
              longAck(i, OPC_MOVE_SLOTS, 0);
              break;
            }
          }
        }
      }
      break;

    case OPC_INPUT_REP:
      addrL = LNPacket[1] & 0x7f;
      addrH = LNPacket[2] & 0x0f;
      addr = addrL + (addrH << 7);
      addr = 1 + addr * 2 + ((((unsigned int) LNPacket[2] & 0x0020) >> 5));
      value = (LNPacket[2] & 0x10) >> 4;
      canmsg.b[d0] = value ? OPC_ASON : OPC_ASOF;
      canmsg.b[d1] = 0;
      canmsg.b[d2] = 0;
      canmsg.b[d3] = addr / 256;
      canmsg.b[d4] = addr % 256;
      canmsg.b[dlc] = 5;
      canbusSend(&canmsg);
      break;

    case OPC_SW_REQ:
    case OPC_SW_STATE:
      addrL = LNPacket[1] & 0x7f;
      addrH = LNPacket[2] & 0x0f;
      addr = addrL + (addrH << 7);
      if (addr == 1017 && (NV1 & CFG_ENABLE_SOD)) {
        // SoD
        addr == SOD;
        canmsg.b[d0] = OPC_ASRQ;
      } else {
        addr *= 2;
        addr += (LNPacket[2] & 0x20) >> 5;
        canmsg.b[d0] = (LNPacket[2] & 0x10) ? OPC_ASON : OPC_ASOF;
      }
      canmsg.b[d1] = 0;
      canmsg.b[d2] = 0;
      canmsg.b[d3] = addr / 256;
      canmsg.b[d4] = addr % 256;
      canmsg.b[dlc] = 5;
      canbusSend(&canmsg);
      break;

    case OPC_LISSY_REP: // E4
      addr = LNPacket[4] & 0x7F;
      valL = LNPacket[6] & 0x7F;
      valH = LNPacket[5] & 0x7F;
      value = valL + (valH << 7); // Ident.
      dir = (LNPacket[3] & 0x20) ? 1 : 0;
      canmsg.b[d0] = OPC_ACON3;
      canmsg.b[d1] = 0;
      canmsg.b[d2] = 0;
      canmsg.b[d3] = addr / 256;
      canmsg.b[d4] = addr % 256;
      canmsg.b[d5] = value / 256;
      canmsg.b[d6] = value % 256;
      canmsg.b[d7] = dir + ((LNPacket[2]&0x40) ? 0x02 : 0x00); // Direction and Wheel counter flag
      canmsg.b[dlc] = 8;
      canbusSend(&canmsg);
      break;

    case OPC_MULTI_SENSE:
      type = LNPacket[1] & OPC_MULTI_SENSE_MSG;
      if (type == OPC_MULTI_SENSE_PRESENT || type == OPC_MULTI_SENSE_ABSENT) {
        char zone;
        byte present;
        byte enter;
        unsigned int locoaddr;
        addrL = LNPacket[2];
        addrH = LNPacket[1] & 0x1F;
        addr = addrL + (addrH << 7);
        enter = (LNPacket[1] & 0x20) != 0 ? TRUE : FALSE;

        addr++;

        if (LNPacket[3] == 0x7D)
          locoaddr = LNPacket[4];
        else
          locoaddr = LNPacket[3]*128 + LNPacket[4];

        if ((LNPacket[2]&0x0F) == 0x00) zone = 'A';
        else if ((LNPacket[2]&0x0F) == 0x02) zone = 'B';
        else if ((LNPacket[2]&0x0F) == 0x04) zone = 'C';
        else if ((LNPacket[2]&0x0F) == 0x06) zone = 'D';
        else if ((LNPacket[2]&0x0F) == 0x08) zone = 'E';
        else if ((LNPacket[2]&0x0F) == 0x0A) zone = 'F';
        else if ((LNPacket[2]&0x0F) == 0x0C) zone = 'G';
        else if ((LNPacket[2]&0x0F) == 0x0E) zone = 'H';

        canmsg.b[d0] = (type == OPC_MULTI_SENSE_PRESENT) ? OPC_ACON3 : OPC_ACOF3;
        canmsg.b[d1] = 0;
        canmsg.b[d2] = 0;
        canmsg.b[d3] = addr / 256;
        canmsg.b[d4] = addr % 256;
        canmsg.b[d5] = locoaddr / 256;
        canmsg.b[d6] = locoaddr % 256;
        canmsg.b[d7] = zone;
        canmsg.b[dlc] = 8;
        canbusSend(&canmsg);

      }
      break;

  }

}

byte getLNSize(byte opc) {
  if (opc == 0xE0) {
    /* Uhli exceptions */
    return -2;
  } else {
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

  if (SampleFlag & (1 << readP)) {
    byte b = SampleData[readP];
    SampleFlag &= ~(1 << readP);
    readP++;
    if (readP > 7)
      readP = 0;

    LNTimeout = 0;

    if (LNIndex == 0) {
      if ((b & 0x80) == 0) {
        // invalid start
        LNPacket[0] = b;
        LNPacket[1] = 0;
        LNPacket[2] = 0x99;
        LNPacket[3] = overrun;
        LNPacket[4] = framingerr;
        LNPacket[5] = 0x00;
        LNPacket[6] = 0x00;
        ln2CBusErr();
        return 1;
      } else {
        /*
        LNPacket[0] = b;
        LNPacket[1] = 0;
        LNPacket[2] = 0x88;
        LNPacket[3] = overrun;
        LNPacket[4] = framingerr;
        LNPacket[5] = 0x00;
        LNPacket[6] = 0x00;
        ln2CBusErr();
         */
      }

      //memset(LNPacket, 0, sizeof(LNPacket));

      // OPC
      //LNSize = getLNSize(b);

      if (b == 0xE0) {
        /* Uhli exceptions */
        LNSize = -2;
      } else if ((b & 0xf0) == 0x80)
        LNSize = 2;
      else if ((b & 0xf0) == 0xA0)
        LNSize = 4;
      else if ((b & 0xf0) == 0xB0)
        LNSize = 4;
      else if ((b & 0xf0) == 0xC0)
        LNSize = 6;
      else if ((b & 0xf0) == 0xE0)
        LNSize = -1;
      else
        LNSize = 0;


      if (LNSize == 0 || LNSize == -2) {
        // invalid OPC
        LNPacket[0] = b;
        LNPacket[1] = LNSize;
        LNPacket[2] = 0xAA;
        LNPacket[3] = overrun;
        LNPacket[4] = framingerr;
        LNPacket[5] = 0x00;
        LNPacket[6] = 0x00;
        overrun = FALSE;
        framingerr = FALSE;
        ln2CBusErr();
        return 1;
      }

      packetpending = TRUE;

    }

    if (LNSize == -1 && LNIndex == 1) {
      LNSize = b & 0x7F;
    }

    if (LNIndex < 32) {
      LNPacket[LNIndex] = b;
      LNIndex++;
      if (LNIndex == LNSize) {
        // Packet complete.
        LNIndex = 0;
        packetpending = FALSE;
        // Translate to CBUS.
        ln2CBus();
      }
    } else {
      LNPacket[1] = 0xCC;
      LNPacket[2] = LNSize;
      LNPacket[3] = LNIndex;
      LNPacket[4] = 0;
      LNPacket[5] = 0;
      LNPacket[6] = 0;
      ln2CBusErr(); // debug
      // reset
      LNIndex = 0;
      packetpending = FALSE;

    }

    return 1;
  }


  if (packetpending == FALSE && mode == LN_MODE_READ && LNstatus == STATUS_WAITSTART) {
    byte i;
    for (i = 0; i < LN_BUFFER_SIZE; i++) {
      if (LNBuffer[i].status == LN_STATUS_USED) {
        mode = LN_MODE_WRITE_REQ;
        break;
      }
    }
  }



  return 0;
}


// 1ms

void LocoNetWD(void) {
  if (LNIndex > 0 || LNIndex == -1) {
    LNTimeout++;
    if (LNTimeout > 100) {
      // Reset
      LNIndex = 0;
    }
  }
}

void initLN(void) {
  byte i;
  for (i = 0; i < LN_BUFFER_SIZE; i++) {
    LNBuffer[i].status = LN_STATUS_FREE;
  }
  for (i = 0; i < LN_SLOTS; i++) {
    slotmap[i].session = LN_SLOT_UNUSED;
    throttleid[i] = 0;
    slottimer[i] = 0;
  }
  dispatchSlot = LN_SLOT_UNUSED;

  LNstatus = STATUS_WAITSTART;
  sampledata = 0;
  dataready = FALSE;
  SampleFlag = 0;
  readP = 0;
  writeP = 0;

  sample = 0;
  bitcnt = 0;
  LNIndex = 0;
  LNTimeout = 0;
  samplepart = 0;
  overrun = FALSE;
  framingerr = FALSE;
  packetpending = FALSE;
  mode = LN_MODE_READ;
  CDBackoff = 0;
}

void send2LocoNet(CANMsg *cmsg) {
  unsigned int addr;
  byte slot = 0;
  byte i = 0;
  byte n = 0;
  for (i = 0; i < LN_BUFFER_SIZE; i++) {
    if (LNBuffer[i].status == LN_STATUS_FREE) {
      break;
    }
  }

  if (i >= LN_BUFFER_SIZE) {
    // Buffer overflow...
    LED6_FLIM = PORT_ON;
    return;
  }

  if (mode != LN_MODE_WRITE_REQ) {
    idle = 0; // reset idle timer
    txtry = 0;
  }

  switch (cmsg->b[d0]) {
    case OPC_RTON:
      LNBuffer[i].len = 2;
      LNBuffer[i].data[0] = OPC_GPON;
      checksumLN(i);
      LNBuffer[i].status = LN_STATUS_USED;
      //ln2CBusDebug(i);
      break;

    case OPC_RTOF:
      LNBuffer[i].len = 2;
      LNBuffer[i].data[0] = OPC_GPOFF;
      checksumLN(i);
      LNBuffer[i].status = LN_STATUS_USED;
      //ln2CBusDebug(i);
      break;

    case OPC_ERR:
      longAck(i, 0x3F, 0);
      break;

    case OPC_DSPLOC:
      for (n = 1; n < LN_SLOTS; n++) {
        if (slotmap[n].session == cmsg->b[d1]) {
          dispatchSlot = n;
          LNBuffer[i].len = 4;
          LNBuffer[i].data[0] = OPC_MOVE_SLOTS;
          LNBuffer[i].data[1] = i;
          LNBuffer[i].data[2] = 0;
          checksumLN(i);
          LNBuffer[i].status = LN_STATUS_USED;
          //ln2CBusDebug(i);
          break;
        }
      }
      break;

    case OPC_PLOC:
      // read slot
      addr = cmsg->b[d2] * 256;
      addr += cmsg->b[d3];
      addr &= 0x3FFF;
      slot = LN_SLOTS;
      for (i = 1; i < LN_SLOTS; i++) {
        if (slotmap[i].session == cmsg->b[d1]) {
          slot = i;
          break;
        }
      }

      if (slot == LN_SLOTS) {
        // new session
        for (i = 1; i < LN_SLOTS; i++) {
          if (slotmap[i].session == LN_SLOT_UNUSED) {
            slot = i;
            slotmap[slot].session = cmsg->b[d1];
            slottimer[slot] = 0;
            break;
          }
        }
      }

      if (slot < LN_SLOTS) {
        // update slot
        slotmap[slot].addr = addr;
        slotmap[slot].speed = cmsg->b[d1]; // dir = (speed & 0x80)
        slotmap[slot].f[0] = cmsg->b[d5];
        slotmap[slot].f[1] = cmsg->b[d6];
        slotmap[slot].f[2] = cmsg->b[d7];
        // report slot read
        slotRead(i, slot, FALSE);
        //ln2CBusDebug(i);
      }
      break;

    case OPC_RESTP:
      LNBuffer[i].len = 2;
      LNBuffer[i].data[0] = OPC_IDLE;
      checksumLN(i);
      LNBuffer[i].status = LN_STATUS_USED;
      break;

    case OPC_ASRQ:
      addr = cmsg->b[d3] * 256;
      addr += cmsg->b[d4];
      if (addr == SOD && (NV1 & CFG_ENABLE_SOD)) {
        // Start Of Day
        LNBuffer[i].len = 4;
        LNBuffer[i].data[0] = OPC_SW_REQ;
        LNBuffer[i].data[1] = 1017 & 0x7F;
        LNBuffer[i].data[2] = (1017 / 128)&0x0F;
        checksumLN(i);
        LNBuffer[i].status = LN_STATUS_USED;
      }
      break;

    case OPC_PCVS:
    {
      byte cvh = cmsg->b[d2];
      byte cvl = cmsg->b[d3];
      unsigned short cv = cvh * 256 + cvl;
      byte value = cmsg->b[d4];

      LNBuffer[i].len = 14;
      LNBuffer[i].data[0] = OPC_SL_RD_DATA;
      LNBuffer[i].data[1] = 0x0E;
      LNBuffer[i].data[2] = 0x7C;
      LNBuffer[i].data[3] = (0x03 | 0x20); // command

      LNBuffer[i].data[4] = 0;
      LNBuffer[i].data[5] = 0;
      LNBuffer[i].data[6] = 0;
      LNBuffer[i].data[7] = 0; // track status

      LNBuffer[i].data[8] = (cv & 0x300) / 16 + (cv & 0x80) / 128 + (value & 0x80) / 128 * 2;
      LNBuffer[i].data[9] = cv & 0x7F;
      /* store low bits of CV value */
      LNBuffer[i].data[10] = value & 0x7F;

      LNBuffer[i].data[11] = 0; // Throttle ID
      LNBuffer[i].data[12] = 0;
      checksumLN(i);
      LNBuffer[i].status = LN_STATUS_USED;
    }
      break;

    case OPC_FCLK:
      LNBuffer[i].len = 14;
      LNBuffer[i].data[0] = OPC_WR_SL_DATA;
      LNBuffer[i].data[1] = 0x0E;
      LNBuffer[i].data[2] = 0x7B;
      LNBuffer[i].data[3] = cmsg->b[d4];

      LNBuffer[i].data[4] = 0x7F; // fractional minutes L
      LNBuffer[i].data[5] = 0x7F; // fractional minutes H
      LNBuffer[i].data[6] = (255 - (60 - cmsg->b[d1]))&0x7F; // 256 - minutes 43
      LNBuffer[i].data[7] = 0; // track status

      LNBuffer[i].data [8] = (256 - (24 - cmsg->b[d2]))&0x7F; // 256 - hours 14
      LNBuffer[i].data [9] = 0; // clock rollovers
      LNBuffer[i].data[10] = 0x70;
      LNBuffer[i].data[11] = 0x7F;
      LNBuffer[i].data[12] = 0x70;
      checksumLN(i);
      LNBuffer[i].status = LN_STATUS_USED;
      break;

    case OPC_ACON:
    case OPC_ACOF:
    case OPC_ASON:
    case OPC_ASOF:
      addr = cmsg->b[d3] * 256;
      addr += cmsg->b[d4];
      if (addr >= SWStart && addr <= SWEnd) {
        // Switch
        byte dir = addr % 2;
        addr = addr / 2;
        LNBuffer[i].len = 4;
        LNBuffer[i].data[0] = OPC_SW_REQ;
        LNBuffer[i].data[1] = addr & 0x7F;
        LNBuffer[i].data[2] = (addr >> 7)&0x0F;
        LNBuffer[i].data[2] += ((cmsg->b[d0] & 0x01) ? 0x00 : 0x10);
        LNBuffer[i].data[2] += (dir ? 0x20 : 0x00);
        checksumLN(i);
        LNBuffer[i].status = LN_STATUS_USED;
      } else if ((NV1 & CFG_ENABLE_FB2LN) && addr >= FBStart && addr <= FBEnd) {
        // Sensor
        byte dir = addr % 2;
        addr = addr / 2;
        LNBuffer[i].len = 4;
        LNBuffer[i].data[0] = OPC_INPUT_REP;
        LNBuffer[i].data[1] = addr & 0x7F;
        LNBuffer[i].data[2] = (addr >> 7)&0x0F;
        LNBuffer[i].data[2] += ((cmsg->b[d0] & 0x01) ? 0x00 : 0x10);
        LNBuffer[i].data[2] += (dir ? 0x20 : 0x00);
        checksumLN(i);
        LNBuffer[i].status = LN_STATUS_USED;
      }
      break;

  }

}

void doSlotTimers(void) {
  byte i;
  for (i = 0; i < LN_SLOTS; i++) {
    if (slotmap[i].session == LN_SLOT_UNUSED)
      slottimer[i] = 0;
    else
      slottimer[i]++;
  }
}

void doSlotPing(void) {
  byte i;
  CANMsg canmsg;
  for (i = 0; i < LN_SLOTS; i++) {
    if (slotmap[i].session != LN_SLOT_UNUSED && slottimer[i] > 20) {
      slottimer[i] = 0;
      canmsg.b[d0] = OPC_DKEEP;
      canmsg.b[d1] = slotmap[i].session;
      canmsg.b[dlc] = 2;
      canbusSend(&canmsg);
    }
  }
}
