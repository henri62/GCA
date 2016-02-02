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



#ifndef __CANGCLN_H
#define __CANGCLN_H


#define MAJOR_VER 2     // zero is developer version
#define MINOR_VER 'e'	// Minor version character

#define EVT_NUM 5
#define EVperEVT 0
#define NV_NUM 2

// EEPROM addresses
#define EE_CLEAN 0
#define EE_NV 1          // 1 byte configuration
#define EE_NN 2          // 2 bytes node number of this unit
#define EE_SOD EE_NN + 2     // 2 bytes start off day address to report all input states
#define EE_CANID EE_SOD + 2  // 1 byte CANID
#define EE_SWSTART EE_CANID + 1  // 2 short event port 1
#define EE_SWEND EE_SWSTART + 2  // 2 short event port 1
#define EE_FBSTART EE_SWEND + 2  // 2 short event port 1
#define EE_FBEND EE_FBSTART + 2  // 2 short event port 1

#define EE_SWSTATE  0x0100
#define EE_BOOT 0x03FF

// for Uhlenbrock FRED in dispatch mode
#define GPBUSY

// values
#define MAGIC 93

// node var 1
#define CFG_ALL  0xFF
#define CFG_READONLY 0x01
#define CFG_ENABLE_FB2LN 0x02
#define CFG_ENABLE_SOD 0x04
#define CFG_ENABLE_SSW 0x08
#define CFG_ENABLE_FCLK 0x10




extern near unsigned short NN_temp;
extern near unsigned char NV1;
extern near unsigned char CANID;
extern near unsigned char ledCBUSRXtimer;
extern near unsigned char ledCBUSTXtimer;
extern near unsigned char ledLNRXtimer;
extern near unsigned char ledLNTXtimer;
extern near unsigned char ioIdx;
extern near unsigned char Wait4NN;
extern near unsigned char isLearning;
extern near unsigned short SOD;
extern near unsigned char doSOD;
extern near unsigned char doEV;
extern near unsigned char evIdx;
extern far unsigned short SWStart;
extern far unsigned short SWEnd;
extern far unsigned short FBStart;
extern far unsigned short FBEnd;
extern far int            swSaveStart;
extern far byte           doSwSave;

extern byte swState[256];

#endif
