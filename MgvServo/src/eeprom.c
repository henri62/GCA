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

#include "device.h"
#include "eeprom.h"

unsigned char ReadEEData(unsigned char address) 
{
	static unsigned char value;
	EEADR = address;        // Set address
	//EEPGD = 0;              // Point to Data memory
    RD = 1;                 // Start read operation
	value = EEDATA;         // Read data.        
	return value;           // Return data
}

void WriteEEData(unsigned char address, unsigned char value) 
{
	EEIF   = 0;                             // Clear "write complete" flag

	EEADR  = address;                       // Address to write to
	EEDATA = value;                      // Data to write

	//EEPGD  = 0;                             //write to EEPROM, not data memory

	WREN   = 1;                             // Enable writes to the EEProm
	GIE    = 0;                             // Disable interrupts during write

	EECON2 = 0x55;                          // Write "password" to EECON2
	EECON2 = 0xAA;
	WR     = 1;                             // Initiate a write cycle

	while(!EEIF);                           // Wait for write to complete
	EEIF   = 0;                             // Clear "write complete" flag
	WREN   = 0;                  // Disable writes to EEProm
	GIE = 1;                // Re-enable interrupts
}

