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



#ifndef __CANGC7_H
#define __CANGC7_H


#define MAJOR_VER 1
#define MINOR_VER 'a'	// Minor version character

#define EVT_NUM 0
#define EVperEVT 0
#define NV_NUM 1

// EEPROM addresses
#define EE_MAGIC 0
#define EE_NV 1          // 1 byte configuration
#define EE_NN 2          // 2 bytes node number of this unit
#define EE_CANID 3       // 1 byte CANID

// values
#define MAGIC 93

// node var 1
#define CFG_DISPDIM  0x0F
#define CFG_SHOWDATE 0x10




extern near unsigned short NN_temp;
extern near unsigned char NV1;
extern near unsigned char CANID;
extern near unsigned char led1timer;
extern near unsigned char ioIdx;
extern volatile near unsigned char tmr0_reload;

#endif
