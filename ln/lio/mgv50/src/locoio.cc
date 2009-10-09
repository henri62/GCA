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
#include "loconet.h"
#include "locoio.h"

// Calculate IO port number (1..16) to port index (0..2)
#define PORT_INDEX(ioport)      (((ioport) == 16) ? 0 : (((ioport) >= 8) ? 2 : 1))
// Calculate IO port number (1..16) to port bit (0..7)
#define PORT_BIT(ioport)        (((ioport) == 16) ? 5 : (((ioport) >= 8) ? (15 - (ioport)) : (((ioport) >= 5) ? (7 - (ioport)) : (8 - (ioport)))))
// Calculate IO port number (1..16) to port bit mask (0..0x80)
#define PORT_BIT_MASK(ioport)   (1 << PORT_BIT(ioport))

typedef unsigned char io_ports[3];

static io_ports input_mask = { 0, 0, 0 };
static io_ports last_status = { 0, 0, 0 };
static io_ports current_status = { 0, 0, 0 };
locoio_setup setup;

/*
Detect changes in the input ports
*/
void DetectInputChanges() {
        unsigned char i;
        unsigned char index;
        unsigned char mask;
        
        // Read PORTA
        current_status[0] = PORTA & input_mask[0];
        current_status[1] = PORTB & input_mask[1];
        current_status[2] = PORTC & input_mask[2];
        
        // Detect changes per port
        for (i = 1; i <= 16; i++) {
                mask = PORT_BIT_MASK(i);
                index = PORT_INDEX(i);
                if ((last_status[index] & mask) != (current_status[index] & mask)) {
                        // A change is detected
                }
        }
        
}

/*
Load SV status and setup IO ports.
*/
void InitializeIO() {
        unsigned char i;
        unsigned char config;
        
	// Read setup
	setup.setup.value = ReadEEData(SV_LOCOIO_SETUP);
	setup.address = ReadEEData(SV_LOCOIO_LOW_ADR);
	setup.sub_address = ReadEEData(SV_LOCOIO_SUB_ADR);

        for (i = 0; i != 3; i++) {
                input_mask[i] = 0;
                last_status[i] = 0;
        }
        
        // Read SV's for all ports
        for (i = 1; i != 17; i++) {
                config = ReadEEData(SV_PORT_CONFIG(i));
                if (CFG_IS_INPUT(config)) {
                        input_mask[PORT_INDEX(i)] |= PORT_BIT_MASK(i);
                }                
        }
        
        // Clear ports
        PORTA = 0;
        PORTB = 0;
        PORTC = 0;
        
        // Setup TRIS, ADCON registers
        // TRISA; 
        //      RA0 output
        //      RA1 input
        //      RA2 output, 
        //      RA5 configurable
        ADCON1 = 0x06;
        TRISA = 0xDA | (input_mask[0] & 0x20);
        // TRISB; RB3 input, other configurable
        TRISB = 0x08 | input_mask[1];
        // TRISC; all configurable
//        TRISC = input_mask[2];        
        TRISC = 0; // All output for testing
        RC0 = 1;
}
