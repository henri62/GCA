
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



#include "spica.h"
#include "io.h"
#include "loconet.h"
#include "utils.h"
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



void doLNcmd(void) {
  unsigned int addrL, addrH, addr;
  unsigned int valL, valH, value;
  byte dir, type;

  LED4_BUS = PORT_ON;
  ledBUStimer = 20;
  switch( LNPacket[0]) {

    case OPC_GPON:
      LED3_BOOSTER = PORT_ON;
      break;

    case OPC_GPOFF:
      LED3_BOOSTER = PORT_OFF;
      break;

  }
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
        //ln2CBusErr();
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
        // Process packet.
        doLNcmd();
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
    //Wait4NN = TRUE;
    //LED6_FLIM = PORT_ON;
    return;
  }

  if( mode != LN_MODE_WRITE_REQ ) {
    idle = 0; // reset idle timer
    txtry = 0;
  }


}



