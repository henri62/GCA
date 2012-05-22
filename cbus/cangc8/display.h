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


#define LCD_PORT PORTC
#define LCD_RW   PORTCbits.RC0
#define LCD_EN   PORTCbits.RC1
#define LCD_GND  PORTCbits.RC2
#define LCD_RS   PORTCbits.RC3

typedef struct {
  byte config;
  int  addr;
  byte line1;
  byte line2;
  byte text1[32];
  byte text2[32];
} DisplayDef;



extern far DisplayDef Display[MAXDISPLAYS];

void setDisplayData(int addr, byte flags, byte char0, byte char1, byte char2, byte char3);

extern void writeDisplays(void);
void initDisplays(void);



#endif
