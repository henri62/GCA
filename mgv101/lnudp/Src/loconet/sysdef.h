
/****************************************************************************
    Copyright (C) 2002 Alex Shepherd

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************

 Title :    Systemwide Definitions for LocoLinx
 Author:    Bernd Wisotzki [BA] <wsb562000@sourceforge.net>
 Date:      19-Mar-2004
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Filename:  sysdef.h

DESCRIPTION

  Systemwide Definitions to get all those things to one place, wich are
  suspect to change very often; e.g. Baudrate, Frequency of Oscalator,
  MCU-Type or Board.

****************************************************************************/

#ifndef _LN_SYSDEF_INCLUDED

#define _LN_SYSDEF_INCLUDED

    /******* General Itmes **************************************************/

    // #define HOST_BAUD_RATE 115200
    // #define HOST_BAUD_RATE 19200
#define HOST_BAUD_RATE	    57600

    /******* MCU *************************************************************
          MCU is defined in the makefile and can be accessed by:
          #if defined (__AVR_ATmega128__)
    *************************************************************************/

    /******* Board **********************************************************/
#define BOARD_ETHERNET_LOCONET_BUFFER_MGV_101
    // #define BOARD_LOCO_DEV
    // #define BOARD_PROTO_128
    // #define BOARD_PROTO_128_STEFAN

#define LN_TIMER_TX_RELOAD_ADJUST  20

#define BV(bit) _BV(bit)
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif
#endif
