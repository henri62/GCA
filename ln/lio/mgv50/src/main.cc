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
#include "ln_receiver.h"
#include "ln_rxbuffer.h"
#include "locoio.h"
#include "locoio_message_processor.h"

#define CONF_WORD 0x3f38

// --------------------------------------------------------
// Configuration bits 
// CP1:0     11 Code protection off
// DEBUG     1  In circuit debugger disabled
// WRT       0  Unprotected program memory masy not be writter to by EECON control
// CPD       1  Code protection off
// LVP       0  Low voltage programming disable (since no resistor found on MGV50)
// BODEN     1  Brown out reset enabled
// PWRTE     0 Powerup timer enabled
// WDTE      0 Watch dog timer disable
// FOSC1:0   10 High speed oscillator
// --------------------------------------------------------

unsigned int at 0x2007 CONFIG = 0x3D72;

ln_msg main_msg;

/* Main loop */
void main() 
{        
	// Initialize
	InitializeIO();         // Read SV's and setup IO ports
	SetupRxBuffer();        // Setup read buffer
	LnSetupReceiver();      // Setup receiver

        // Perform main loop for ever
	while (1) {
	        // Process messages (if any)
		ProcessLnMessages();
	        
	        // Detect change in input ports
	}
}
