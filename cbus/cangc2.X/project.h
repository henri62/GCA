/*
 MERG CBUS
 web:  http://www.sprog-dcc.co.uk
 e-mail: sprog@sprog-dcc.co.uk

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


#ifndef __PROJECT_H
#define __PROJECT_H

#define CANGC2

#include <p18f2480.h>

// CAN mode for setup
#define ECAN_MODE_2

// CBUS headers
// One Rx buffer and one Tx buffer must be defined before
// including cbus_common.h
#define TX1

// This device has a fixed CAN ID

// 06/04/11 Roger Healey - Put CANCMD constants inside #ifdef
//  					 - Add MODULE_ID as MTYP_CANCMD		

#include "cbus.h"

    
#define FIXED_CAN_ID 0x72
#define DEFAULT_NN   0xFFEE
#define DEFAULT_SOD  4711

#define TRUE 1
#define FALSE 0
#define PORT_ON 1
#define PORT_OFF 0

typedef unsigned short ushort;
typedef unsigned char byte;


extern near unsigned char Latcount;
extern near unsigned short NN_temp;
extern const rom unsigned char params[32];

// DEFINE INPUT AND OUTPUT PINS
// ??? NOTE - BC1a still wip, ddr and init not yet worked out 

//
// Port A analogue inputs and digital I/O
//

// RA0 is AN0 current sense
//
#define TMR0_NORMAL	0x91

#endif	// __PROJECT_H
