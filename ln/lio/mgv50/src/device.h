/*
 Modelspoorgroep Venlo LocoNet Firmware

 Copyright (C) Ewout Prangsma <ewout@prangsma.net>

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

#include <pic16f873.h>
//#include <pic16f628.h>

#define LN_TX           RA0     // LocoNet transmit pin
#define LN_RX           RA1     // LocoNet receive pin

#define LN_LED          RA2     // Led indicating LocoNet traffic
#define LN_LED_TOGGLE() { LN_LED = !LN_LED; }

//         __BADRAM H'F0'-H'FF'

