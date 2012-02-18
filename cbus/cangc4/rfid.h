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

#ifndef __RFID__
#define __RFID__


extern void scanRFID(void);
void initRFID(void);
void doRFID(void);
void doRFIDTimedOff(int i);

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


#endif
