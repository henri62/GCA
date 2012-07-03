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


#ifndef __CBUS_H
#define __CBUS_H

/*
 * Copyright (C) 2008 Andrew Crosland
 */

#include "spica.h"
extern near unsigned char can_transmit_timeout;
extern near unsigned char can_transmit_failed;
extern near unsigned char can_bus_off;

extern rom unsigned char defaultID;
extern rom unsigned char status;
extern rom unsigned short nodeID;
extern rom unsigned char bootflag;

void cbusSetup(void);

extern near unsigned char Tx1[14];

enum bufbytes {
	con=0,
	sidh,
	sidl,
	eidh,
	eidl,
	dlc,
	d0,
	d1,
	d2,
	d3,
	d4,
	d5,
	d6,
	d7
};

unsigned char fifoEmpty(void);

typedef struct {
	unsigned char con;
	unsigned char sidh;
	unsigned char sidl;
	unsigned char eidh;
	unsigned char eidl;
	unsigned char dlc;
	unsigned char d0;
	unsigned char d1;
	unsigned char d2;
	unsigned char d3;
	unsigned char d4;
	unsigned char d5;
	unsigned char d6;
	unsigned char d7;
} ecan_rx_buffer;
extern ecan_rx_buffer * rx_ptr;



typedef struct {
  unsigned char status;
  unsigned char opc;
  unsigned char len;
  unsigned char d[7];
} CANMsg;

extern ram CANMsg canmsg;


#define CANMSG_QSIZE   24
#define CANMSG_FREE    0x00
#define CANMSG_OPEN    0x01
#define CANMSG_PENDING 0x02

unsigned char canQueue(CANMsg* msg);
void canSendQ(void);

#endif	// __CBUS_COMMON_H
