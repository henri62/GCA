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
   /* 1200 baud = 32000000 / 4 [ (6665 + 1) ]  */
  SPBRGH = 0x1A;
  SPBRG  = 0x09;
  BAUDCONbits.BRG16 = 1;

	/* Enable serial TX */
  TXSTAbits.BRGH = 1;
  TXSTAbits.SYNC = 0;
  TXSTAbits.TXEN = 1;
	/* Disable serial RX */
  RCSTAbits.SPEN = 0;
}

static byte relayMasks[] = { 0x03, 0x0C, 0x30, 0xC0 };

#define RELAY_MASK(servo) (relayMasks[servo])
static byte allRelayBits = 0;

/*
Turn of relays when servo starts
*/
void RelayStart(byte servo) {
	allRelayBits &= ~RELAY_MASK(servo);
}

/*
Update relay settings for middle position
RelayMiddle is not needed for 136/137
*/

/*
Update relay settings for target position
*/
void RelayEnd(byte servo, byte relayBits) {
	allRelayBits &= ~RELAY_MASK(servo);
	allRelayBits |= relayBits;
}

/*
Called during each timer interrupt, send relay bits to serial port (if free)
*/
void RelayUpdate(void) {
	if( TXSTAbits.TRMT ) {
		TXREG = allRelayBits;
	}
}
