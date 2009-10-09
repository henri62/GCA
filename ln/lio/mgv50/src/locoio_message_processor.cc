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
#include "eeprom.h"

extern ln_msg main_msg;

/*
Update the main message wrt D1..D8.
Insert the MSB bits from PXCT1,2 into them
*/
static void FixPeerXFerData() {
	unsigned char i;

	for (i = 0; i != 4; i++) {
		if (main_msg.peer_xfer.pxct1 & (1 << i)) { main_msg.raw[6 + i] |= 0x80; }
		if (main_msg.peer_xfer.pxct2 & (1 << i)) { main_msg.raw[11 + i] |= 0x80; }
	}
}

/*
Process an SV_WRITE command
*/
static void ProcessSvWrite() {
	// Store the SV
	WriteEEData(main_msg.peer_xfer.sv_index, main_msg.peer_xfer.sv_value);
}

/*
Process messages waiting in the LocoNet receive buffer.
*/
void ProcessLnMessages() 
{        
	// Process messages (if any)
	while (ReadLnMsg()) {
		// We have a LocoNet message, process it
	        switch (main_msg.raw[0]) {
	        	case OPC_INPUT_REP:
				break;
	                case OPC_SW_REP:
	                        break;
			case OPC_SW_REQ:
				if (main_msg.raw[1] == 0x25) { PORTC = 0xFF; }
	                        else if (main_msg.raw[1] == 0x23) { PORTC = 0; }
	                        break;
			case OPC_PEER_XFER:
				if (main_msg.peer_xfer.sv_type == LOCOIO_DSTH) {
					// Can be an SV READ/WRITE, check address
					if ((main_msg.peer_xfer.dst == 0) || 
					    ((main_msg.peer_xfer.dst == setup.address) && (main_msg.peer_xfer.sub_addr == setup.sub_address))) {
						// Address matches (broadcast of exact)
						if (main_msg.peer_xfer.cmd == SV_WRITE) { 
							FixPeerXFerData();
							ProcessSvWrite(); 
						}
					}
				}
				break;
		}
	}
}
