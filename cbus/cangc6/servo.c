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
static byte pending  = FALSE;

void doServoPosition(void) {
  //Servo[servoIdx].position = Servo[servoIdx].wantedpos;
  
  if( Servo[servoIdx].wantedpos > Servo[servoIdx].position ) {
    Servo[servoIdx].position += Servo[servoIdx].speed;
    if( Servo[servoIdx].position > Servo[servoIdx].wantedpos )
      Servo[servoIdx].position = Servo[servoIdx].wantedpos;
  }
  else if( Servo[servoIdx].wantedpos < Servo[servoIdx].position ) {
    Servo[servoIdx].position -= Servo[servoIdx].speed;
    if( Servo[servoIdx].position < Servo[servoIdx].wantedpos )
      Servo[servoIdx].position = Servo[servoIdx].wantedpos;
  }

  if( Servo[servoIdx].position < 50 )
    Servo[servoIdx].position = 50;
  if( Servo[servoIdx].position > 250 )
    Servo[servoIdx].position = 250;
  
}

// called every 5ms
void doServo(void) {
  if( !pending ) {
    pending = TRUE;
    doServoPosition();
    if( servoIdx == 0 ) {
      TMR2 = 0;
      PR2  = Servo[servoIdx].position-1;
      T2CONbits.TMR2ON  = 1; // Timer2 on
      SERVO1 = PORT_ON;
    }
    else if( servoIdx == 1 ) {
      TMR2 = 0;
      PR2  = Servo[servoIdx].position-1;
      T2CONbits.TMR2ON  = 1; // Timer2 on
      SERVO2 = PORT_ON;
    }
    else if( servoIdx == 2 ) {
      TMR2 = 0;
      PR2  = Servo[servoIdx].position-1;
      T2CONbits.TMR2ON  = 1; // Timer2 on
      SERVO3 = PORT_ON;
    }
    else if( servoIdx == 3 ) {
      TMR2 = 0;
      PR2  = Servo[servoIdx].position-1;
      T2CONbits.TMR2ON  = 1; // Timer2 on
      SERVO4 = PORT_ON;
    }
  }
}

void endServoPulse(void) {
  if( pending ) {
    SERVO1 = PORT_OFF;
    SERVO2 = PORT_OFF;
    SERVO3 = PORT_OFF;
    SERVO4 = PORT_OFF;

    servoIdx++;
    if( servoIdx > 3 )
      servoIdx = 0;

    pending = FALSE;
  }
}