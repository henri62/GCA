/*
 Copyright (C) MERG CBUS

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



#include <p18cxxx.h>
#include <stdio.h>

#include "cbus.h"
#include "utils.h"
#include "cbusdefs.h"
#include "cangc6.h"
#include "io.h"
#include "servo.h"


void setupIO(byte clr) {
  int idx = 0;

  // all digital I/O
  ADCON0 = 0x00;
  ADCON1 = 0x0F;

  TRISCbits.TRISC0 = 1; /* T1S */
  TRISCbits.TRISC1 = 1; /* T2S */
  TRISCbits.TRISC2 = 1; /* T3S */
  TRISCbits.TRISC3 = 1; /* T4S */
  TRISCbits.TRISC4 = 1; /* T4R */
  TRISCbits.TRISC5 = 1; /* T3R */
  TRISAbits.TRISA2 = 1; /* T2R */
  TRISCbits.TRISC7 = 1; /* T1R */

  TRISAbits.TRISA1 = 1; /* SW */
  TRISCbits.TRISC6 = 1; /* GCA137 TX */
  TRISAbits.TRISA3 = 0; /* LED1 */
  TRISAbits.TRISA4 = 0; /* LED2 */
  TRISAbits.TRISA5 = 0; /* LED3 */
  TRISAbits.TRISA0 = 0; /* LED4 */

  TRISBbits.TRISB0 = 0; /* SERVO1 */
  TRISBbits.TRISB1 = 0; /* SERVO2 */
  TRISBbits.TRISB4 = 0; /* SERVO3 */
  TRISBbits.TRISB5 = 0; /* SERVO4 */

  GCA137 = PORT_OFF;
  LED1   = PORT_OFF;
  LED2   = PORT_OFF;
  LED3   = PORT_OFF;
  LED4   = PORT_OFF;

  // Keep servo output high to avoid unwanted movements.
  SERVO1 = PORT_ON;
  SERVO2 = PORT_ON;
  SERVO3 = PORT_ON;
  SERVO4 = PORT_ON;

  if( checkFlimSwitch() || eeRead(EE_CLEAN) == 0xFF ) {
    eeWrite(EE_CLEAN, 0);
    for( idx = 0; idx < 4; idx++ ) {
      eeWrite(EE_SERVO_CONFIG + idx, 0);
      eeWrite(EE_SERVO_LEFT + idx, 100);
      eeWrite(EE_SERVO_RIGHT + idx, 200);
      eeWrite(EE_SERVO_SPEEDL + idx, 1);
      eeWrite(EE_SERVO_SPEEDR + idx, 1);
      eeWrite(EE_SERVO_POSITION + idx, SERVO_CENTERPOS);
      eeWriteShort(EE_SERVO_FBADDR + 2 * idx, idx + idx * 4);
      eeWriteShort(EE_SERVO_SWNN + 2 * idx, 0);
      eeWriteShort(EE_SERVO_SWADDR + 2 * idx, idx + 1 + idx * 4);
    }
  }


  for( idx = 0; idx < 4; idx++ ) {
    Servo[idx].config    = eeRead(EE_SERVO_CONFIG + idx);
    Servo[idx].left      = eeRead(EE_SERVO_LEFT + idx);
    Servo[idx].right     = eeRead(EE_SERVO_RIGHT + idx);
    Servo[idx].speedL    = eeRead(EE_SERVO_SPEEDL + idx);
    Servo[idx].speedR    = eeRead(EE_SERVO_SPEEDR + idx);
    Servo[idx].wantedpos = eeRead(EE_SERVO_POSITION + idx);
    Servo[idx].fbaddr    = eeReadShort(EE_SERVO_FBADDR + 2 * idx);
    Servo[idx].swnn      = eeReadShort(EE_SERVO_SWNN + 2 * idx);
    Servo[idx].swaddr    = eeReadShort(EE_SERVO_SWADDR + 2 * idx);

    if(Servo[idx].left > SERVO_MAXPOS || Servo[idx].left < SERVO_MINPOS )
      Servo[idx].left = 100;
    if(Servo[idx].right > SERVO_MAXPOS || Servo[idx].right < SERVO_MINPOS )
      Servo[idx].right = 200;
    if(Servo[idx].wantedpos > SERVO_MAXPOS || Servo[idx].wantedpos < SERVO_MINPOS )
      Servo[idx].wantedpos = SERVO_CENTERPOS;

    if( Servo[idx].speedL > 20 ) {
      Servo[idx].speedL = 20;
      eeWrite(EE_SERVO_SPEEDL + idx, Servo[idx].speedL);
    }
    if( Servo[idx].speedR > 20 ) {
      Servo[idx].speedR = Servo[idx].speedL;
      eeWrite(EE_SERVO_SPEEDR + idx, Servo[idx].speedR);
    }

    Servo[idx].position = Servo[idx].wantedpos;
    setServoRelaybits(idx);
    Servo[idx].endtime = 0;
    Servo[idx].bounceL = 0;
    Servo[idx].bounceR = 0;
    Servo[idx].pulse = Servo[idx].position;
    Servo[idx].pulse *= 5;
  }

}


// Called every 3ms.
void doLEDTimers(void) {
  if( led1timer > 0 ) {
    led1timer--;
    if( led1timer == 0 ) {
      LED1 = 0;
    }
  }

}

unsigned char checkFlimSwitch(void) {
  unsigned char val = SW;
  return !val;
}

unsigned char checkInput(unsigned char idx, unsigned char sod) {
  unsigned char ok = 1;
  if( sod ) {
    canmsg.opc = Servo[idx].position == Servo[idx].right ? OPC_ASON:OPC_ASOF;
    canmsg.d[0] = (NN_temp / 256) & 0xFF;
    canmsg.d[1] = (NN_temp % 256) & 0xFF;
    canmsg.d[2] = (Servo[idx].fbaddr / 256) & 0xFF;
    canmsg.d[3] = (Servo[idx].fbaddr % 256) & 0xFF;
    canmsg.len = 4; // data bytes
    ok = canQueue(&canmsg);
  }
  return ok;
}



void saveOutputStates(void) {
  int idx = 0;  
  for( idx = 0; idx < 4; idx++ ) {
    eeWrite(EE_SERVO_POSITION + idx, Servo[idx].position);
  }

}


static unsigned char __LED2 = 0;
void doLEDs(void) {
  if( Wait4NN || isLearning) {
    LED2 = __LED2;
    __LED2 ^= 1;
  }
  else {
    LED2 = PORT_OFF;
  }

}


static unsigned char __LED4 = 0;
void doLED250(void) {
  byte i;
  byte servoPending = FALSE;

  for( i = 0; i < 4; i++ ) {
    if( Servo[i].wantedpos != Servo[i].position ) {
      servoPending = TRUE;
      break;
    }
  }

  if( servoPending ) {
      LED4 = __LED4;
      __LED4 ^= 1;
  }
  else {
    LED4 = PORT_OFF;
    __LED4 = 1;
  }

}


void setOutput(ushort nn, ushort addr, byte on) {
  int i = 0;
  for( i = 0; i < 4; i++) {
    if( Servo[i].swaddr == addr ) {
      byte act = FALSE;
      if( NV1 & CFG_SHORTEVENTS ) {
        act = TRUE;
      }
      else if(Servo[i].swnn == nn) {
        act = TRUE;
      }

      if( act ) {
        Servo[i].wantedpos = on ? Servo[i].right:Servo[i].left;
      }
    }
  }
}
