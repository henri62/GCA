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

#ifndef __SERVO__
#define __SERVO__

void doServo(void);
void endServoPulse(void);
byte doServoPosition(void);
byte readExtSensors( byte servo );
void setServoRelaybits(byte servo);

#define SERVO_MINPOS 50
#define SERVO_CENTERPOS 150
#define SERVO_MAXPOS 250
#define SERVO_ENDTIME 100

#endif

