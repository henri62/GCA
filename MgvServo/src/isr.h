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

/*
Setup timer interrupt
*/
void SetupTimer();

/*
Change the target of the servo
Servo: 0,1,2,3
pulseWidthTarget: 50..250
relayBits: exactly 1 bit must be set. 
*/
#ifdef RELAY
void SetServoTarget(unsigned char index, unsigned char pulseWidthTarget, unsigned char relayBits);
#else
void SetServoTarget(unsigned char index, unsigned char pulseWidthTarget);
#endif

/*
Change the change mode of the servo
Servo: 0,1,2,3
adjustMask: 0,1,2,3
*/
void SetServoAdjust(unsigned char servo, unsigned char adjustMask);

#ifdef FEEDBACK
/*
* Update the feedback bits based on the current input bits
*/
void UpdateFeedbacks(unsigned char input);
#endif

/*
The GIE may be enabled after this method has been called.
*/
void ReadyForInterrupts();
