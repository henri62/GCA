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
#include "ln_transmitter.h"
#include "ln_receiver.h"
#include "loconet.h"

static void WaitOneBit() {
}

static LnSendStatus TrySendLnMsg(unsigned char* msg, unsigned char length) {
        unsigned char index;
        unsigned char bit;
        unsigned char dataByte;
        unsigned char mask;
        
        // CD BACKOFF complete?
        if (!CD_BACKOFF_ENDED()) {
                return CdBackoff;
        }
        
        // Current activity?
        
        
        // Send bytes
        for (index = 0; index != length; index++) {
                dataByte = msg[index];
                mask = 1;
        
                // Send start bit
                LN_TX = LN_SPACE;
                WaitOneBit();
                
                // Send bits
                for (bit = 0; bit != 8; bit++) {
                        // Set port level
                        LN_TX = (dataByte & mask) ? 1 : 0;
                        WaitOneBit();
                        
                        // Detect collisions
                        if (LN_RX != ((dataByte & mask) ? 1 : 0)) {
                                // Collision detected
                                return Collision;
                        }
                        
                        // Shift 
                        mask <<= 1;
                }
                
                // Send stop bit
                LN_TX = LN_MARK;
                WaitOneBit();
        }
        
        return Success;        
}

/*
Send a given LocoNet message
*/
LnSendStatus SendLnMsg(unsigned char* msg, unsigned char length) {
        unsigned char chsum;
        unsigned char i;
        LnSendStatus status;
        
        // Calculate checksum first
        chsum = 0xFF;
        for (i = 0; i != length - 1; i++) {
                chsum ^= msg[i];
        }
        msg[length - 1] = chsum;

        // Now try to send 
        for (i = 0; i != TRANSMIT_ATTEMPTS; i++) {
                // Try to send the packet
                status = TrySendLnMsg(msg, length);
                switch (status) {
                        case Success:
                                return Success;
                        case CdBackoff:
                                while (!CD_BACKOFF_ENDED()) { /* wait */ }
                                break;
                }                
        }        
        
        // Now we give up
        return TimeOut;                      
}

