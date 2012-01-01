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
#include "cangc4.h"
#include "io.h"


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

  LED1   = PORT_OFF;
  LED2   = PORT_OFF;
  LED3   = PORT_OFF;
  LED4   = PORT_OFF;


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
    canmsg.opc = 1 ? OPC_ASON:OPC_ASOF;
    canmsg.d[0] = (NN_temp / 256) & 0xFF;
    canmsg.d[1] = (NN_temp % 256) & 0xFF;
    canmsg.d[2] = (0 / 256) & 0xFF;
    canmsg.d[3] = (0 % 256) & 0xFF;
    canmsg.len = 4; // data bytes
    ok = canQueue(&canmsg);
  }
  return ok;
}



void saveOutputStates(void) {
  int idx = 0;  
  for( idx = 0; idx < 4; idx++ ) {
    //eeWrite(EE_SERVO_POSITION + idx, Servo[idx].position);
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
}


void setOutput(ushort nn, ushort addr, byte on) {
}
