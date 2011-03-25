/*
 Modelspoorgroep Venlo MGV137 Firmware

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

#include <pic16f628a.h>

// Configuration word
unsigned int at 0x2007 CONFIG = 0x3F50;

/*
Chip initialization
*/
#define InitializeIO() \
{ \
	PORTA = 0xFF; \
	PORTB = 0xFF; \
	TRISA = 0x00; \
	TRISB = 0x06; \
	CMCON = 0x07; \
	BRGH = 0; \
	SPBRG = 129; /* 1200 Baud */ \
	/* Enable serial port */ \
	SYNC = 0; \
	SPEN = 1; \
	CREN = 1; \
}

#define SET(output, mask) { if ((value & mask) == 0) { output = 1; } else { output = 0; } }

/* Main loop */
void main() 
{
	static unsigned char value;
	static unsigned char oldValue;
	static unsigned char i;
	static unsigned char j;
	
	// Initialize
	InitializeIO();  

	// Initialialy turn everything off
	PORTA = 0xFF;
	PORTB = 0xFF;
	
	// Wait a while for the 136 to become stable
	for (i = 0; i != 0xFF; i++) {
		for (j = 0; j != 0xFF; j++) {
			// Do nothing
		}
	}
	
	// Main loop
	oldValue = 0;
	// Enable reception
	CREN = 1;
	while (1) 
	{
		if (RCIF)
		{
			if (FERR) 
			{
				// Framing error, just read, but do not process
				i = RCREG;
			}
			else 
			{
				// Read data
				value = RCREG;
			}
			if (value == oldValue) 
			{
				// Only update relays if we've received the same result twice
				SET(RA0, 0x01);
				SET(RB4, 0x02);
				SET(RA3, 0x04);
				SET(RB0, 0x08);
				SET(RA1, 0x10);
				SET(RB3, 0x20);
				SET(RA2, 0x40);
				SET(RB5, 0x80);
			}
			oldValue = value;

			if (OERR)
			{
				// Overrun error
				// Reset 
				CREN = 0;
				// Now re-activate
				CREN = 1;
			}
		}
	}
}
