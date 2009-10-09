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
#include "loconet.h"

#define LN_BIT_TIME             300 // Instruction cycles / LocoNet bit (20MHz, 16,66KBaud)
#define LN_TMR0                 (0xFF + 27 - (LN_BIT_TIME / 3))       // Sample 3 times per bit
#define CD_BACKOFF_INIT         (CD_BACKOFF * 3)        // We sample 3 times per bit

#define RESET_CD_BACKOFF        { cdBackoff = CD_BACKOFF_INIT; }

static unsigned char lnRx = 0;
static unsigned char lnReceiverState = 0;
static unsigned char lnRxData = 0;
static unsigned char lnRxBitsRead = 0;
unsigned char cdBackoff = 0;

/*
Timer interrupt
*/
void Isr() interrupt 0
{
        if (!T0IF) {
                RC2 = 1;
                return;
        }

        // Keep the order of these statements, since they are time critical
        TMR0 = LN_TMR0;         // Reset timer
        T0IF = 0;               // Reset Timer0 Flag 
        lnRx = LN_RX;           // Read RX state now so we have a fixed timing
        
        if (lnReceiverState == 0) {
                // Wait for start bit
                if (lnRx == LN_SPACE) {
                        // Start bit detected.
                        lnReceiverState = 1;
                        lnRxData = 0;
                        lnRxBitsRead = 0;
                        RESET_CD_BACKOFF
                        LN_LED = 1;
                } else {
                        // Decrement CD_BACKOFF
                        if (cdBackoff != 0) {
                                cdBackoff--;
                        }
                        LN_LED = 0;
                }
        } else if (lnReceiverState == 4) {
                if (lnRxBitsRead != 8) {
                        // Read data bit
                        lnRxData >>= 1;
                        if (lnRx == LN_MARK) { lnRxData |= 0x80; }
                        lnRxBitsRead++;
                        lnReceiverState = 2;
                } else {
                        // Read stop bit
                        if (lnRx == LN_MARK) {
                                // Stop bit found
                                AddByteToRxBuffer(lnRxData); // Record received byte.
                                RC7 = 0;
                        } else {
                                // Stop bit not found, error
                                RC7 = 1;
                        }
                        // Since stop bit is MARK, we can go directly to state 0 and wait for a start bit
                        lnReceiverState = 0;
                        RESET_CD_BACKOFF
                }
        } else if (lnReceiverState == 1) {
                // Test if start bit still present
                if (lnRx == LN_SPACE) {
                        lnReceiverState = 2;
                } else {
                        // It was a glitch, wait for the next start
                        lnReceiverState = 0;
                }
        } else {
                lnReceiverState++;
        }
}

/*
Initialize the receiver.
*/
void LnSetupReceiver() 
{
        T0CS = 0;       // Use internal clock 
        TMR0 = LN_TMR0; // Setup internal 
        T0IF = 0;       // Reset Timer0 Flag 
        T0IE = 1;       // Enable interrupt 
        GIE = 1;        // Global enable interrupts
}

