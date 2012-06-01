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

#ifndef __DISPLAY__
#define __DISPLAY__

#define MAXDISPLAYS 2


#define LCD1_SI   PORTAbits.RA0
#define LCD1_CLK  PORTAbits.RA1
#define LCD1_CSB  PORTAbits.RA3
#define LCD1_RS   PORTAbits.RA4

#define LCD2_SI   PORTCbits.RC0
#define LCD2_CLK  PORTCbits.RC1
#define LCD2_CSB  PORTCbits.RC2
#define LCD2_RS   PORTCbits.RC3

#define MODESIZE 7
#define BUFFERSIZE 8 * MODESIZE

typedef struct {
  byte config;
  int  addr;
  byte pending;
  byte byteidx;
  byte bitidx;
  byte mode[MODESIZE]; // low bit = command
  byte buffer[BUFFERSIZE];
} DisplayDef;



extern far DisplayDef Display[MAXDISPLAYS];

void setDisplayData(int addr, byte flags, byte char0, byte char1, byte char2, byte char3);

extern void writeDisplays(void);
void initDisplays(void);
void setupDisplays(void);



#endif
