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



#ifndef __CANGC1E_H
#define __CANGC1E_H


#define MAJOR_VER 1
#define MINOR_VER 'b'	// Minor version character

#define EVT_NUM 0
#define EVperEVT 0
#define NV_NUM 16

// EEPROM addresses
#define EE_MAGIC 0
#define EE_NV 1          // 1 byte configuration
#define EE_NN 2          // 2 bytes node number of this unit
#define EE_CANID EE_NN + 2       // 1 byte CANID
#define EE_IPADDR EE_CANID + 1
#define EE_NETMASK EE_IPADDR + 4
#define EE_MACADDR EE_NETMASK + 4
#define EE_CLEAN EE_MACADDR + 6
#define EE_IDLETIME EE_CLEAN + 1





// values
#define MAGIC 93

// node var 1
#define CFG_ALL  0xFF
#define CFG_IDLE_TIMEOUT    0x01
#define CFG_POWEROFF_ATIDLE 0x02




extern near unsigned short NN_temp;
extern near unsigned char NV1;
extern near unsigned char CANID;
extern near unsigned char IdleTime;
extern near unsigned char led1timer;
extern near unsigned char led2timer;
extern near unsigned char led3timer;
extern near unsigned char ticktimer;
extern near unsigned char ioIdx;
extern volatile near unsigned char tmr0_reload;
extern near unsigned char Wait4NN;
extern near unsigned char isLearning;
extern near unsigned char maxcanq;
extern near unsigned char maxethq;
extern near unsigned char currentEthQ;

#endif
