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
#include "ln_rxbuffer.h"
#include "loconet.h"
#include "eeprom.h"

extern ln_msg main_msg;

#define BUF_SIZE       32
static struct {
        unsigned char   buffer[BUF_SIZE];
        unsigned char   readIndex;
        volatile unsigned char   writeIndex;
} rxBuffer;

#define GO_NEXT_INDEX(index) { if ((index) == (BUF_SIZE-1)) { index = 0; } else { index++; } }

#pragma save
#pragma nooverlay // This function is called from the timer interrupt, do not overlay.
/*
Add a single byte to the receive buffer 
*/
void AddByteToRxBuffer(unsigned char value) {
        rxBuffer.buffer[rxBuffer.writeIndex] = value;
        GO_NEXT_INDEX(rxBuffer.writeIndex)       
}
#pragma restore

/*
Initialize the read buffer 
*/
void SetupRxBuffer() {
        for (rxBuffer.writeIndex = 0; rxBuffer.writeIndex != BUF_SIZE; rxBuffer.writeIndex++) {
                rxBuffer.buffer[rxBuffer.writeIndex] = 0x33;
        }
        rxBuffer.readIndex = 0;
        rxBuffer.writeIndex = 0;
}

/*
Read a waiting message into the given target space.
Returns 1 on message available, NULL on no message available.
*/
unsigned char ReadLnMsg() {
        unsigned char availBytes;
        unsigned char msgLen;
        unsigned char index;
        unsigned char j;
        unsigned char chsum;
        unsigned char dataByte;
                
        // Skip all bytes until an opcode is found
        while ((rxBuffer.readIndex != rxBuffer.writeIndex) && (!IS_LN_OPCODE(rxBuffer.buffer[rxBuffer.readIndex]))) {
                GO_NEXT_INDEX(rxBuffer.readIndex)
        }
        
        if (rxBuffer.readIndex != rxBuffer.writeIndex) {
                // There is data and readIndex starts at an opcode.
                
                // Calculate available bytes
                availBytes = rxBuffer.writeIndex;
                if (availBytes > rxBuffer.readIndex) { availBytes -= rxBuffer.readIndex; }
                else { availBytes += (BUF_SIZE - rxBuffer.readIndex); }
                
                // Message length is at least 2 bytes
                if (availBytes >= 2) {
                        // Calculate length of current message
                        msgLen = ((rxBuffer.buffer[rxBuffer.readIndex] & LNF_SIZE_MASK) >> 4) + 2;
                        if (msgLen == 8) {
                                // Variable length message
                                index = rxBuffer.readIndex;
                                GO_NEXT_INDEX(index)
                                msgLen = rxBuffer.buffer[index];
                        } 
                        
                        // Do we have all bytes of the message in the buffer?
                        if (availBytes >= msgLen) {
                                // Copy message & update checksum
                                index = rxBuffer.readIndex;
                                chsum = 0xFF;
                                for (j = 0; j < msgLen - 1; j++) {
                                        dataByte = rxBuffer.buffer[index];
                                        main_msg.raw[j] = dataByte;
                                        chsum ^= dataByte;                                       
                                        GO_NEXT_INDEX(index)
                                }

                                // Fetch checksum and compare                                
                                dataByte = rxBuffer.buffer[index];
                                GO_NEXT_INDEX(index)
                                
                                if (chsum == dataByte) {                                
                                        // Valid message, update readIndex
                                        rxBuffer.readIndex = index;
                                        
                                        return 1;
                                }
                                else {
                                        // Checksum is invalid, skip this opcode and continue
                                        GO_NEXT_INDEX(rxBuffer.readIndex)
                                }
                        }                        
                }                                
        }
        
        return 0;
}
