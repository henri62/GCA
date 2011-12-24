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
#include "io.h"
#include "servo.h"


static byte servoIdx = 0;

// called every 5ms
void doServo(void) {
  if( servoIdx == 0 ) {
    SERVO1 = PORT_ON;
    TMR2 = Servo[0].position;
    PR2  = Servo[0].position;
  }
  else if( servoIdx == 1 ) {
    SERVO2 = PORT_ON;
    TMR2 = Servo[1].position;
    PR2  = Servo[1].position;
  }
  else if( servoIdx == 2 ) {
    SERVO3 = PORT_ON;
    TMR2 = Servo[2].position;
    PR2  = Servo[2].position;
  }
  else if( servoIdx == 3 ) {
    SERVO4 = PORT_ON;
    TMR2 = Servo[3].position;
    PR2  = Servo[3].position;
  }

  T2CONbits.TMR2ON  = 1; // Timer2 on

}

void endServoPulse(void) {
  if( servoIdx == 0 )
    SERVO1 = PORT_OFF;
  else if( servoIdx == 1 )
    SERVO2 = PORT_OFF;
  else if( servoIdx == 2 )
    SERVO3 = PORT_OFF;
  else if( servoIdx == 3 )
    SERVO4 = PORT_OFF;

  servoIdx++;
  if( servoIdx > 3 )
    servoIdx = 0;
}