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

#ifndef __locoio_h
#define __locoio_h

// --------------------------------------------------------
// Runtime info
// --------------------------------------------------------

typedef struct {
        union {
                unsigned char value;
        } setup;
        unsigned char address;
        unsigned char sub_address;
} locoio_setup;
extern locoio_setup setup;

// --------------------------------------------------------
// SV addresses
// --------------------------------------------------------

#define SV_LOCOIO_SETUP         0
#define SV_LOCOIO_LOW_ADR       1
#define SV_LOCOIO_SUB_ADR       2

// Config triple for each port (0..15)
#define SV_PORT_CONFIG(port)    (3 + (port * 3))
#define SV_PORT_VALUE1(port)    (4 + (port * 3))
#define SV_PORT_VALUE2(port)    (5 + (port * 3))

// --------------------------------------------------------
// Configuration byte
// --------------------------------------------------------

#define CFG_IS_OUTPUT(value)    ((value) & 0x80)
#define CFG_IS_INPUT(value)     (!CFG_IS_OUTPUT(value))

// --------------------------------------------------------
// Functions
// --------------------------------------------------------


/*
Detect changes in the input ports
*/
void DetectInputChanges();

/*
Load SV status and setup IO ports.
*/
void InitializeIO();

#endif // __locoio_h
