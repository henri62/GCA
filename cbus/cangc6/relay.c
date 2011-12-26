/*
 Rocrail - Model Railroad Software

 Copyright (C) Rob Versluis <r.j.versluis@rocrail.net>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "project.h"
#include "relay.h"

void initRelayTx(void) {
   /* 480 baud = 32000000 / 4 [ (16666 + 1) ]  */
  SPBRGH = 0x41;
  SPBRG  = 0x1A;
  BAUDCONbits.BRG16 = 1;

	/* Enable serial TX */
  TXSTA = 0;
  TXSTAbits.BRGH = 1;
  TXSTAbits.TXEN = 1;
	/* Disable serial RX */
  RCSTA = 0;
  RCSTAbits.SPEN = 1;

  TRISCbits.TRISC6 = 1; /* GCA137 TX */
}

static byte relayMasks[] = { 0x03, 0x0C, 0x30, 0xC0 };
static byte allRelayBits = 0xAA;


/*
Turn of both relays when servo starts
*/
void RelayStart(byte servo) {
	allRelayBits &= ~relayMasks[servo];
}

/*
 * Update relay settings for target position;
 *   Only one relay may be activated depending of polarisation.
 */
void RelayEnd(byte servo, byte relayBits) {
	allRelayBits &= ~relayMasks[servo];
	allRelayBits |= relayBits << (servo*2);
}

/*
Called during each timer interrupt, send relay bits to serial port (if free)
*/
void RelayUpdate(void) {
	if( TXSTAbits.TRMT ) {
		TXREG = allRelayBits;
	}
}
