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
  
  if( Servo[servoIdx].config & SERVOCONF_EXTSEN ) {
    readExtSensors(servoIdx);
    return;
  }

  canmsg.opc = rightSide ? OPC_ASOF:OPC_ASON;
  canmsg.d[0] = (NN_temp / 256) & 0xFF;
  canmsg.d[1] = (NN_temp % 256) & 0xFF;
  canmsg.d[2] = (Servo[servoIdx].fbaddr / 256) & 0xFF;
  canmsg.d[3] = (Servo[servoIdx].fbaddr % 256) & 0xFF;
  canmsg.len = 4; // data bytes
  canQueue(&canmsg);
}

void setServoRelaybits(byte servo) {
  if( Servo[servo].position == Servo[servo].right )
    RelayEnd(servo, Servo[servo].config & SERVOCONF_POLAR ? 2:1 );
  else
    RelayEnd(servo, Servo[servo].config & SERVOCONF_POLAR ? 1:2 );
}

/* Called every 20ms for a servo.
 * The pulse range is 250...1250 = 1000 steps
 * The position range is 50...250 = 200 steps
 */
byte doServoPosition(void) {
  if( Servo[servoIdx].wantedpos > Servo[servoIdx].position ) {
    Servo[servoIdx].endtime = 0;
    RelayStart(servoIdx);

    Servo[servoIdx].pulse += Servo[servoIdx].speed;
    Servo[servoIdx].position = Servo[servoIdx].pulse / 5;

    if( Servo[servoIdx].position >= Servo[servoIdx].wantedpos ) {
      Servo[servoIdx].position = Servo[servoIdx].wantedpos;
      reportServoPosition(TRUE);
      setServoRelaybits(servoIdx);
    }
  }
  else if( Servo[servoIdx].wantedpos < Servo[servoIdx].position ) {
    Servo[servoIdx].endtime = 0;
    RelayStart(servoIdx);

    Servo[servoIdx].pulse -= Servo[servoIdx].speed;
    Servo[servoIdx].position = Servo[servoIdx].pulse / 5;

    if( Servo[servoIdx].position <= Servo[servoIdx].wantedpos ) {
      Servo[servoIdx].position = Servo[servoIdx].wantedpos;
      reportServoPosition(FALSE);
      setServoRelaybits(servoIdx);
    }
  }

  if( Servo[servoIdx].position < SERVO_MINPOS )
    Servo[servoIdx].position = SERVO_MINPOS;
  if( Servo[servoIdx].position > SERVO_MAXPOS )
    Servo[servoIdx].position = SERVO_MAXPOS;
  
   return (Servo[servoIdx].position == Servo[servoIdx].wantedpos) ? TRUE:FALSE;
}

// called every 5ms
void doServo(void) {
  ushort rest = 0;

  if( doServoPosition() ) {
    if( Servo[servoIdx].endtime < SERVO_ENDTIME )
      Servo[servoIdx].endtime++;
  }

  rest = 0xFFFF - (Servo[servoIdx].pulse*15);
  TMR0H = (byte)(rest / 256);
  TMR0L = (byte)(rest % 256);

  if( servoIdx == 0 ) {
    SERVO1 = (Servo[servoIdx].endtime < SERVO_ENDTIME) ? PORT_ON:PORT_OFF;
  }
  else if( servoIdx == 1 ) {
    SERVO2 = (Servo[servoIdx].endtime < SERVO_ENDTIME) ? PORT_ON:PORT_OFF;
  }
  else if( servoIdx == 2 ) {
    SERVO3 = (Servo[servoIdx].endtime < SERVO_ENDTIME) ? PORT_ON:PORT_OFF;
  }
  else if( servoIdx == 3 ) {
    SERVO4 = (Servo[servoIdx].endtime < SERVO_ENDTIME) ? PORT_ON:PORT_OFF;
  }
  T0CONbits.TMR0ON = 1; // Timer0 on

}

void endServoPulse(void) {
  SERVO1 = PORT_OFF;
  SERVO2 = PORT_OFF;
  SERVO3 = PORT_OFF;
  SERVO4 = PORT_OFF;

  servoIdx++;
  if( servoIdx > 3 )
    servoIdx = 0;
}


/* returns 0 for unknown, 1 for straight, 2 for thrown */
byte __extPosS[4] = {0,0,0,0};
byte __extPosT[4] = {0,0,0,0};
byte __extIdx = 0;
byte readExtSensors( byte servo ) {
  byte straight = 0;
  byte thrown   = 0;

  /* periodically called by the isr 500ms */
  if( servo == 0xFF ) {
    servo = __extIdx;
    __extIdx++;
    if( __extIdx > 3 )
      __extIdx = 0;
  }

  if( servo == 0 ) {
    straight = T1S;
    thrown   = T1R;
  }
  else if( servo == 1 ) {
    straight = T2S;
    thrown   = T2R;
  }
  else if( servo == 2 ) {
    straight = T3S;
    thrown   = T3R;
  }
  else if( servo == 3 ) {
    straight = T4S;
    thrown   = T4R;
  }

  if( straight == thrown )
    return TURNOUT_UNKNOWN;

  if( straight == __extPosS[servo] && thrown == __extPosT[servo] )
    return TURNOUT_UNKNOWN;

  __extPosS[servo] = straight;
  __extPosT[servo] = thrown;

  canmsg.opc = thrown ? OPC_ASON:OPC_ASOF;
  canmsg.d[0] = (NN_temp / 256) & 0xFF;
  canmsg.d[1] = (NN_temp % 256) & 0xFF;
  canmsg.d[2] = (Servo[servo].fbaddr / 256) & 0xFF;
  canmsg.d[3] = (Servo[servo].fbaddr % 256) & 0xFF;
  canmsg.len = 4; // data bytes
  canQueue(&canmsg);

  return thrown ? TURNOUT_THROWN:TURNOUT_STRAIGHT;
}