/*
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

#ifndef CBUSETH_H
#define CBUSETH_H

#define CBUSETH_PORT            (5550)
#define MAX_CBUSETH_CONNECTIONS (4)
#define MAX_CBUSETH_CMD_LEN     (50)

#include "cbus.h"

void CBusEthTick(void);
void CBusEthInit(void);
void CBusEthServer(void);
unsigned char CBusEthBroadcast(CANMsg* msg);
unsigned char CBusEthBroadcastExt(EXTMsg* msg);
unsigned char ethQueue(CANMsg* msg);
unsigned char ethExtQueue(EXTMsg* msg);


#endif
