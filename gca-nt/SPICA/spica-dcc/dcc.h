/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


#ifndef __DCC_H
#define __DCC_H

#include "spica.h"


typedef struct _slot {
  byte addrl;
  byte addrh;
  byte speed;
  byte fn[4];
} slot;

#define MAX_SLOTS 2
extern near slot slots[MAX_SLOTS];


// 14 Clear Timer interrupt flag
// Reload TMR0 for interrupt every 58us
// Tcyc = 250ns @ 16MHz
// Interrupt every 58/(.250) = 232 Tcyc
// Value loaded into TMR0 needs to be adjusted for:
// - TMR0 interrupt latency (3 Tcyc)
// - Number of instructions from interrupt to reload
// - TMR0 inhibit after reload (2 Tcyc with 2:1 prescaler)
// Prescaler = 2:1
// So value is 0 - (232 - 3 - 17 - 2)/2 = -105 = 0x97
// Modified for 32 MHz clock  by Mike B. value was 0x97
#define TMR0_DCC 0x91


void doDCC(void);

#endif
