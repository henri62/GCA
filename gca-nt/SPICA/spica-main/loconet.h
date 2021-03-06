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


#include "spica.h"

#ifndef __LOCONET__
#define __LOCONET__

typedef struct {
  byte   status;
  byte   data[16];
  byte   len;
} LNPACKET;


extern near byte work;
extern near byte LNstatus;
extern near byte sampledata;
extern near byte dataready;
extern near byte sample;
extern near byte bitcnt;
extern near byte mode;
extern near byte txtry;
extern near byte samplepart;
extern near byte idle;
extern near byte overrun;

extern far byte SampleData[8];
extern far byte SampleFlag;
extern far byte readP;
extern far byte writeP;

extern far byte LNPacket[32];
extern far byte LNIndex;
extern far byte LNSize;
extern far byte LNTimeout;
extern far byte LNByteIndex;
extern far byte LNBitIndex;
extern far byte LNWrittenBit;

#define LN_BUFFER_SIZE 14

extern far LNPACKET LNBuffer[LN_BUFFER_SIZE];
extern far byte LNBufferIndex;

extern void LNDCC(void);
void initLN(void);
byte doLocoNet(void);
void LocoNetWD(void);
void send2LocoNet(void);

#define LN_STATUS_FREE 0
#define LN_STATUS_USED 1
#define LN_STATUS_PENDING 2


#define STATUS_WAITSTART 0x00
#define STATUS_CONFSTART 0x01
#define STATUS_IGN1      0x02
#define STATUS_IGN2      0x04
#define STATUS_SAMPLE    0x08
#define STATUS_STOPEXP   0x10
#define STATUS_FULL      0x20
#define STATUS_RECVREADY 0x40
#define STATUS_ERROR     0x80

#define ERROR_FRAME   0x01
#define ERROR_OVERRUN 0x02

#define STX 0x02
#define ETX 0x03

#define LN_MODE_READ 0
#define LN_MODE_WRITE 1
#define LN_MODE_WRITE_REQ 2


#endif
