/*
 Rocrail - Model Railroad Software

 Copyright (C) Rob Versluis <r.j.versluis@rocrail.net>

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

void waitMS(unsigned int ms) {
  unsigned int i;
  for( i=0; i < ms; i++) {
    unsigned int n;
    for(n=0; n < 300; n++);
  }
}


void waitUS(unsigned int us) {
  unsigned int i;
  unsigned int n = us/10;
  for( i=0; i < n; i++);
}


unsigned char readByte(const unsigned char address)
{
	static unsigned char byte;              // Variable hold the data that is read

	EEADR = address;                        // Read from this address

	EEPGD = 0;                              // Point to EE memory
	RD	  = 1;                              // Initiate a read cycle

	byte = EEDATA;                          // Fetch byte from dataregister
	return byte;                            // Return the read byte
}

void saveByte(const unsigned char address, const unsigned char databyte)
{
  EEIF   = 0;                             // Clear "write complete" flag

	EEADR  = address;                       // Address to write to
	EEDATA = databyte;                      // Data to write

  EEPGD  = 0;                             //write to EEPROM, not data memory

  WREN   = 1;                             // Enable writes to the EEProm
	GIE    = 0;                             // Disable interrupts during write

	EECON2 = 0x55;                          // Write "password" to EECON2
	EECON2 = 0xAA;
	WR     = 1;                             // Initiate a write cycle

	while(!EEIF);                           // Wait for write to complete
  EEIF   = 0;                             // Clear "write complete" flag
	WREN   = 0;                             // Disable writes to EEProm
}



