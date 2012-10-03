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


#define MAJOR_VER 2
#define MINOR_VER 'd'	// Minor version character

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
#define CFG_COMMAND_ACK     0x04
#define CFG_DHCP_CLIENT     0x08




extern unsigned short NN_temp;
extern unsigned char NV1;
extern unsigned char CANID;
extern unsigned char IdleTime;
extern unsigned char led1timer;
extern unsigned char led2timer;
extern unsigned char led3timer;
extern unsigned char Wait4NN;
extern unsigned char isLearning;
extern unsigned char maxcanq;
extern unsigned char maxethq;
extern unsigned char maxtxerr;
extern unsigned char maxrxerr;
extern unsigned char cmdticker;

extern volatile unsigned short tmr1_reload;

#endif
