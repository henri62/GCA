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
#include "cangc6.h"
#include "io.h"
#include "servo.h"
#include "cbusdefs.h"
#include "relay.h"


static byte servoIdx = 0;
static byte pending  = FALSE;

void reportServoPosition(byte rightSide) {
  if( !(Servo[servoIdx].config & SERVOCONF_EXTSEN) ) {
    canmsg.opc = rightSide ? OPC_ASON:OPC_ASOF;
    canmsg.d[0] = (NN_temp / 256) & 0xFF;
    canmsg.d[1] = (NN_temp % 256) & 0xFF;
    canmsg.d[2] = (Servo[servoIdx].fbaddr / 256) & 0xFF;
    canmsg.d[3] = (Servo[servoIdx].fbaddr % 256) & 0xFF;
    canmsg.len = 4; // data bytes
    canQueue(&canmsg);
  }
}


void doServoPosition(void) {
  if( Servo[servoIdx].wantedpos > Servo[servoIdx].position ) {
    RelayStart(servoIdx);
    Servo[servoIdx].position += Servo[servoIdx].speed;
    if( Servo[servoIdx].position >= Servo[servoIdx].wantedpos ) {
      Servo[servoIdx].position = Servo[servoIdx].wantedpos;
      reportServoPosition(TRUE);
      RelayEnd(servoIdx, Servo[servoIdx].config & SERVOCONF_POLAR ? 2:1 );
    }
  }
  else if( Servo[servoIdx].wantedpos < Servo[servoIdx].position ) {
    RelayStart(servoIdx);
    Servo[servoIdx].position -= Servo[servoIdx].speed;
    if( Servo[servoIdx].position <= Servo[servoIdx].wantedpos ) {
      Servo[servoIdx].position = Servo[servoIdx].wantedpos;
      reportServoPosition(FALSE);
      RelayEnd(servoIdx, Servo[servoIdx].config & SERVOCONF_POLAR ? 1:2 );
    }
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
    TMR2 = 0;
    if( servoIdx == 0 ) {
      PR2  = Servo[servoIdx].position-1;
      SERVO1 = PORT_ON;
    }
    else if( servoIdx == 1 ) {
      PR2  = Servo[servoIdx].position-1;
      SERVO2 = PORT_ON;
    }
    else if( servoIdx == 2 ) {
      PR2  = Servo[servoIdx].position-1;
      SERVO3 = PORT_ON;
    }
    else if( servoIdx == 3 ) {
      PR2  = Servo[servoIdx].position-1;
      SERVO4 = PORT_ON;
    }
    T2CONbits.TMR2ON  = 1; // Timer2 on
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