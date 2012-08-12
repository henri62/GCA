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

#ifndef __LOCONET__
#define __LOCONET__


extern void scanLN(void);
void initLN(void);
byte doLocoNet(void);
void LocoNetWD(void);
void send2LocoNet(void);

typedef struct {
  byte   status;
  byte   data[16];
  byte   len;
} LNPACKET;

typedef struct {
  byte   session;
  unsigned short addr;
  byte   speed; // dir = 0x80 & speed
  byte   f[3];
} LNSLOT;

#define LN_BUFFER_SIZE 14
#define LN_SLOTS 32

#define LN_STATUS_FREE 0
#define LN_STATUS_USED 1
#define LN_STATUS_PENDING 2

#define LN_SLOT_UNUSED 0xFF


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
