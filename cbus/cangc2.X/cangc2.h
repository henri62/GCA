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



#ifndef __CANGC2_H
#define __CANGC2_H


#define MAJOR_VER 2
#define MINOR_VER 'd'	// Minor version character

#define EVT_NUM 17
#define EVperEVT 0
#define NV_NUM 17

// EEPROM addresses
#define EE_MAGIC 0
#define EE_NV 1                         // 1 byte configuration
#define EE_NN 2                         // 2 bytes node number of this unit
#define EE_SOD EE_NN + 2                // 2 bytes start off day address to report all input states
#define EE_PORTCFG EE_SOD + 2           // 16 * 1 byte port configuration
#define EE_PORTNN EE_PORTCFG + 16       // 16 * 2 bytes event node number
#define EE_PORTADDR EE_PORTNN + 32      // 16 * 2 bytes event address (device id)
#define EE_PORTACT EE_PORTADDR + 32     // 16 * 2 bytes event variable
#define EE_PORTSTAT EE_PORTACT + 32     // 2 bytes port output status saved at power off command
#define EE_CANID EE_PORTSTAT + 2        // 1 byte CANID
#define EE_FIRSTTIME EE_CANID + 1       // 1 byte first time flag

// values
#define MAGIC 93

// node var 1
#define CFG_SAVEOUTPUT 0x01
#define CFG_SHORTEVENTS 0x02
#define CFG_PULSETIME_MASK 0x0C
#define CFG_PULSETIME_250 0x00
#define CFG_PULSETIME_500 0x04
#define CFG_PULSETIME_1000 0x08
#define CFG_PULSETIME_2000 0x0C
#define CFG_SOD_REPORTALL 0x10
#define CFG_PULSETIME_LONG 0x20         // for 1500 3000 6000 12000ms Pulse Time




extern near unsigned short NN_temp;
extern near unsigned char NV1;
extern near unsigned char CANID;
extern near unsigned short SOD;
extern near unsigned char Wait4NN;
extern near unsigned char isLearning;
extern near unsigned char led1timer;
extern near unsigned char doSOD;
extern near unsigned char ioIdx;
extern near unsigned char doEV;
extern near unsigned char evIdx;
extern volatile near unsigned char tmr0_reload;

#endif
