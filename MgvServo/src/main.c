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

#include "device.h"
#include "isr.h"
#include "eeprom.h"

// EEPROM addresses
// Start address for given servo (0..3)
#define EEPROM_START(index)	((index) << 2)
#define EEPROM_LEFT		0
#define EEPROM_RIGHT	1
#define EEPROM_ADJUST	2
#define EEPROM_RELAY	3
#define EEPROM_RELAY_LEFT_LSB	0x00
#define EEPROM_RELAY_RIGHT_LSB	0x01
#define EEPROM_RELAY_MASK		0x01

// Configuration word
unsigned int at 0x2007 CONFIG = 0x3F50;

typedef struct {
	unsigned char left;
	unsigned char right;
#ifdef RELAY
	unsigned char relayBitsLeft;
	unsigned char relayBitsRight;
#endif
} ServoConfig;
ServoConfig Configs[4];
unsigned char CurrentState;
static unsigned char input;
static unsigned char enterProgramState;
static unsigned char programmingSequence[] = { 1, 3, 7, 15, 14, 12, 8, 0 };
unsigned char programMode;
static ServoConfig* config;

/*
Read and normalize a servo left or right position 
*/
static unsigned char ReadPosition(unsigned char offset)
{
	static unsigned char value;
	value = ReadEEData(offset);
	if (value > 100) { value = 50; }
	return value;
}

/*
Read 4 bits from the input.
Test for programming sequence
Output is passed in static input variable.
*/
static void ReadInput() 
{
	static unsigned char counter;
	static unsigned char i;
	static unsigned char j;
	
	while (1) 
	{
		// Read current input
		input = 0;
		if (INPUT1) { input |= 0x01; }
		if (INPUT2) { input |= 0x02; }
		if (INPUT3) { input |= 0x04; }
		if (INPUT4) { input |= 0x08; }
	
		if (input == programmingSequence[enterProgramState]) 
		{
			// Found the correct input for the current state of the programming sequence.
			if (enterProgramState == 7) 
			{
				// Found correct programming sequence
				programMode = 1;
				enterProgramState = 0;
				return;
			} 
			else 
			{
				enterProgramState++;
				counter = 100;
			}
		} 
		else if ((enterProgramState != 0) && (input == programmingSequence[enterProgramState-1])) 
		{
			// Found input for the previous state, do nothing
			// if the counter reaches 0, we'll go out of the routine.
		}
		else 
		{
			// Input is not a part of the programming sequence.
			goto no_programming_sequence;
		}
		
		// Bail out if counter reached 0.
		if (counter == 0) 
		{
			// Programming sequence not completed in time.
			goto no_programming_sequence;
		}
		else 
		{
			counter--;
			
			i = 64;
			while (i--);
		}
	}
	
	no_programming_sequence:
	enterProgramState = 0;
	
	return;
}

/*
Read a single 4-bit nibble
Value returned in input variable
*/
void ReadNibble() 
{
	// Wait until INPUT4 is set (note: inverted by optocoupler)
	while (INPUT4 == 0); 
	
	// Now we can read the input
	input = 0;
	if (INPUT1) { input |= 0x01; }
	if (INPUT2) { input |= 0x02; }
	if (INPUT3) { input |= 0x04; }
	
	// Wait until INPUT4 is not set (note: inverted by optocoupler)
	while (INPUT4 != 0); 	
}

/*
Read 2 nibbles, lowest 3 bits first, highest 3 bits last
Value returned in input variable
*/
void ReadPositionNibble() 
{
	static unsigned char tmp;
	
	// Read LSB nibble
	ReadNibble();
	tmp = input;
	// Read MSB nibble
	ReadNibble();
	input <<= 3;
	input |= tmp;
	input <<= 1;
	if (input > 100) { input = 100; }
}

/*
Process programming commands
*/
void ProgramLoop()
{
	static unsigned int target;
	static unsigned int tmp;
	
	target = 0;
	config = &Configs[0];
#ifdef LED
	LED = LED_ON;
#endif
	while (programMode)
	{
		// Read command
		ReadNibble();
		
		// Progress command
		if (input == 0) 
		{
			// NOP
		}
		else if (input == 0x01)
		{
			// Set target
			ReadNibble();
			target = input & 0x03;
			config = &Configs[target];
		}
		else if (input == 0x02) 
		{
			// Set left angle
			ReadPositionNibble(); 
			WriteEEData(EEPROM_START(target) + EEPROM_LEFT, input);
			input = 150 - input;
			config->left = input;
#ifdef RELAY			
			SetServoTarget(target, input, config->relayBitsLeft);
#else
			SetServoTarget(target, input);
#endif		
		}
		else if (input == 0x03) 
		{
			// Set right angle
			ReadPositionNibble();
			WriteEEData(EEPROM_START(target) + EEPROM_RIGHT, input);
			input = 150 + input;
			config->right = input;
#ifdef RELAY			
			SetServoTarget(target, input, config->relayBitsRight);
#else
			SetServoTarget(target, input);
#endif		
		}
		else if (input == 0x04)
		{
			// Set relais position
			ReadNibble();
#ifdef RELAY			
			input = input & EEPROM_RELAY_MASK;
			WriteEEData(EEPROM_START(target) + EEPROM_RELAY, input);
			tmp = 0x01 << (target * 2);
			if ((input & EEPROM_RELAY_MASK) == EEPROM_RELAY_LEFT_LSB) {
				config->relayBitsLeft = tmp;
				config->relayBitsRight = tmp << 1;
			}	
			else	 
			{	
				config->relayBitsLeft = tmp << 1;
				config->relayBitsRight = tmp;
			}
#endif
		} 
		else if (input == 0x05)
		{
			// Set adjust
			ReadNibble(); // LSB
			tmp = input;
			ReadNibble(); // MSB
			input <<= 3;
			input |= tmp;
			if (input == 0) { input = 1; }
			if (input > 0x0F) { input = 0x0F; }
			WriteEEData(EEPROM_START(target) + EEPROM_ADJUST, input);
			SetServoAdjust(target, input);
		} 
		else if (input == 0x06)
		{
			// NOP
		}
		else 
		{
			// Exit programming mode
			programMode = 0;			
		}
	}
#ifdef LED
	LED = LED_OFF;
#endif
}

/* Main loop */
void main() 
{
	static unsigned char tmp;
#ifdef RELAY
	static unsigned char tmp2;
#endif
	static unsigned char mask;
	static unsigned char i;
	
	// Initialize
	InitializeIO();  
AfterInitializeIO:	
#ifdef LED
	LED = LED_ON;
#endif

	// Setup interrupt
	SetupTimer();

	// Load configuration from EEPROM
	config = &Configs[0];
	for (i = 0; i != 4; i++) 
	{
		// Calculate EEPROM offset
		tmp = EEPROM_START(i);
		
		// Get adjust mask
		mask = ReadPosition(tmp + EEPROM_ADJUST);
		if (mask == 0) { mask = 1; }
		if (mask > 0x0F) { mask = 0x0F; }
		SetServoAdjust(i, mask);
		
		// Get left position
		mask = ReadPosition(tmp + EEPROM_LEFT);
		config->left = 150 - mask;
		
		// Get right position
		mask = ReadPosition(tmp + EEPROM_RIGHT);
		config->right = 150 + mask;

#ifdef RELAY
		mask = ReadEEData(tmp + EEPROM_RELAY);
		tmp2 = 0x01 << (i * 2);
		if ((mask & EEPROM_RELAY_MASK) == EEPROM_RELAY_LEFT_LSB) {
			config->relayBitsLeft = tmp2;
			config->relayBitsRight = tmp2 << 1;
		}
		else 
		{
			config->relayBitsLeft = tmp2 << 1;
			config->relayBitsRight = tmp2;
		}
		tmp2 = config->relayBitsLeft;
#endif
		
		// Set current target
		tmp = config->left;
#ifdef RELAY
		SetServoTarget(i, tmp, tmp2);
#else
		SetServoTarget(i, tmp);
#endif
		
		// Go to next config
		config++;
	}
	
	// Final initialization
	enterProgramState = 0;
	programMode = 0;
	CurrentState = 0;
#ifdef LED
	LED = LED_OFF;
#endif

	// We're now ready for normal operations, turn on the interrupts
	GIE = 1; 	// Global enable interrupts
	ReadyForInterrupts();

	// Perform main loop for ever
	while (1) {
        // Detect change in input
		// Get input
		ReadInput();
		
		// Detect programming sequence
		if (programMode)
		{
			// Process programming commands
			ProgramLoop();
			// Jump to initialization to do an "almost" full reset.
			GIE = 0;
			goto AfterInitializeIO;
		} 
		else
		{		
			mask = 0x01;
			config = &Configs[0];
			
			// Update servo's
			for (i = 0; i != 4; i++) 
			{
				if (((input ^ CurrentState) & mask) != 0) 
				{
					// Input changed
					// Update state
					CurrentState ^= mask; 
					// Update ISR parameters
					if ((CurrentState & mask) == 0) 
					{
						tmp = config->right;
#ifdef RELAY
						tmp2 = config->relayBitsRight;
#endif
					}
					else 
					{
						tmp = config->left;
#ifdef RELAY
						tmp2 = config->relayBitsLeft;
#endif
					}
#ifdef RELAY
					SetServoTarget(i, tmp, tmp2);
#else
					SetServoTarget(i, tmp);
#endif
#ifdef LED
					LED = LED_ON;
#endif
				}
				
				// Go to next input
				config++;
				mask <<= 1;
			}
#ifdef FEEDBACK
			// Update FEEDBACK bits (unless masked)
			UpdateFeedbacks(input);
#endif
			
#ifdef LED
			LED = LED_OFF;
#endif
		}
	}
}
