/*
 Modelspoorgroep Venlo MGV81 Firmware

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

#ifdef MGV81_V1

#define INPUT1  RB0
#define INPUT2  RB1
#define INPUT3  RB2
#define INPUT4  RB3

#define CLK_BIT(index) (1 << ((index) + 4))
#define OUTPUT PORTB
#define SPEED

/*
Chip initialization
*/
#define InitializeIO() \
{ \
	TRISA = 0xF0; \
	TRISB = 0x01; \
	CMCON = 0x07; \
}

#endif

#ifdef MGV81_V1_2

#define INPUT1  RB0
#define INPUT2  RB1
#define INPUT3  RB2
#define INPUT4  RB3

#define FEEDBACK1 RA6
#define FEEDBACK2 RA7
#define FEEDBACK3 RA2
#define FEEDBACK4 RA3
#define FEEDBACK PORTA

#define CLK_BIT(index) (1 << ((index) + 4))
#define OUTPUT PORTB

#define LED RA4
#define LED_ON 0
#define LED_OFF 1

/*
Chip initialization
*/
#define InitializeIO() \
{ \
	TRISA = 0x01; \
	TRISB = 0x0F; \
	CMCON = 0x07; \
}

#endif

#ifdef MGV136

#define INPUT1  RA4
#define INPUT2  RA6
#define INPUT3  RA7
#define INPUT4  RB0

#define FEEDBACK1 RA0
#define FEEDBACK2 RA1
#define FEEDBACK3 RA2
#define FEEDBACK4 RA3
#define FEEDBACK PORTA

#define CLK_BIT(index) (1 << ((index) + 4))
#define OUTPUT PORTB
#define SPEED

#define LED RB3
#define LED_ON 1
#define LED_OFF 0

#define RELAY
#define RELAY_MASK(servo) (relayMasks[servo])

/*
Chip initialization
*/
#define InitializeIO() \
{ \
	TRISA = 0xF0; \
	TRISB = 0x07; \
	CMCON = 0x07; \
	BRGH = 0; \
	SPBRG = 129; /* 1200 Baud */ \
	/* Enable serial port */ \
	SYNC = 0; \
	SPEN = 1; \
	TXEN = 1; \
}

/*
Turn of relays when servo starts
*/
#define RelayStart(servo, relayBits) \
{ \
	allRelayBits &= ~RELAY_MASK(servo); \
}

/*
Update relay settings for middle position 
RelayMiddle is not needed for 136/137
*/

/*
Update relay settings for target position 
*/
#define RelayEnd(servo, relayBits) \
{ \
	relayTmp = relayBits; \
	allRelayBits &= ~RELAY_MASK(servo); \
	allRelayBits |= relayTmp; \
}

/*
Called during each timer interrupt, send relay bits to serial port (if free)
*/
#define RelayUpdate() \
{ \
	if (TRMT) { \
		TXREG = allRelayBits; \
	} \
}

#endif

// used with UU16
# define LSB 0
# define MSB 1

typedef unsigned char U8;
typedef unsigned int U16;

typedef signed char S8;
typedef signed int S16;

typedef union UU16
{
   U16 U16;
   S16 S16;
   U8 U8[2];
   S8 S8[2];
} UU16;