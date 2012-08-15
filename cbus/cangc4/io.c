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

//#pragma code IO

void setupIO(byte clr) {
  int idx = 0;

  // all digital I/O
  ADCON0 = 0x00;
  ADCON1 = 0x0F;

  TRISBbits.TRISB5 = 0;  // LED1 cbus activity
  TRISBbits.TRISB7 = 0;  // LED2 learning mode
  TRISBbits.TRISB6 = 0;  // LED3 running

  TRISAbits.TRISA2 = 1; /* SW */

  TRISCbits.TRISC0 = 1; /* RFID1 */
  TRISCbits.TRISC1 = 1; /* RFID2 */
  TRISCbits.TRISC2 = 1; /* RFID3 */
  TRISCbits.TRISC3 = 1; /* RFID4 */

  TRISCbits.TRISC4 = 1; /* RFID8 */
  TRISCbits.TRISC5 = 1; /* RFID7 */
  TRISCbits.TRISC6 = 1; /* RFID6 */
  TRISCbits.TRISC7 = 1; /* RFID5 */

  TRISAbits.TRISA0 = 1; /* SENS1 */
  TRISAbits.TRISA1 = 1; /* SENS2 */
  TRISAbits.TRISA3 = 1; /* SENS3 */
  TRISAbits.TRISA4 = 1; /* SENS4 */
  TRISAbits.TRISA5 = 1; /* SENS5 */
  TRISBbits.TRISB0 = 1; /* SENS6 */
  TRISBbits.TRISB4 = 1; /* SENS7 */
  TRISBbits.TRISB1 = 1; /* SENS8 */

  LED1   = PORT_OFF;
  LED2   = PORT_OFF;
  LED3   = PORT_OFF;

  if( checkFlimSwitch() || eeRead(EE_CLEAN) == 0xFF ) {
    eeWrite(EE_CLEAN, 0);
    for( idx = 0; idx < 16; idx++ ) {
      eeWriteShort(EE_PORT_ADDR + 2 * idx, idx + 1);
    }
    for( idx = 0; idx < 5 * 5; idx++ ) {
      eeWrite(EE_RFID + idx, 0);
    }
  }


  for( idx = 0; idx < 8; idx++ ) {
    RFID[idx].addr = eeReadShort(EE_PORT_ADDR + 2 * idx);
    Sensor[idx].addr = eeReadShort(EE_PORT_ADDR + 16 + 2 * idx);

    // read rfid for SoD
    RFID[idx].data[0] = eeRead(EE_SCANRFID + idx*5 + 0);
    RFID[idx].data[1] = eeRead(EE_SCANRFID + idx*5 + 1);
    RFID[idx].data[2] = eeRead(EE_SCANRFID + idx*5 + 2);
    RFID[idx].data[3] = eeRead(EE_SCANRFID + idx*5 + 3);
    RFID[idx].data[4] = eeRead(EE_SCANRFID + idx*5 + 4);
  }

  for( idx = 0; idx < 5; idx++ ) {
    AllowedRFID[idx].data[0] = eeRead(EE_RFID + idx * 5 + 0);
    AllowedRFID[idx].data[1] = eeRead(EE_RFID + idx * 5 + 1);
    AllowedRFID[idx].data[2] = eeRead(EE_RFID + idx * 5 + 2);
    AllowedRFID[idx].data[3] = eeRead(EE_RFID + idx * 5 + 3);
    AllowedRFID[idx].data[4] = eeRead(EE_RFID + idx * 5 + 4);
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


unsigned char sodRFID(unsigned char i) {
  if( NV1 & CFG_SAVERFID ) {
    if( RFID[i].data[0] != 0 || RFID[i].data[1] != 0 ||
        RFID[i].data[2] != 0 || RFID[i].data[3] != 0 ||
        RFID[i].data[4] != 0 )
    {
      canmsg.opc = OPC_DDES;
      canmsg.d[0] = (RFID[i].addr / 256) & 0xFF;
      canmsg.d[1] = (RFID[i].addr) & 0xFF;
      canmsg.d[2] = RFID[i].data[0];
      canmsg.d[3] = RFID[i].data[1];
      canmsg.d[4] = RFID[i].data[2];
      canmsg.d[5] = RFID[i].data[3];
      canmsg.d[6] = RFID[i].data[4];
      canmsg.len = 7; // data bytes
      return canQueue(&canmsg);
    }
  }
  return FALSE;
}

unsigned char checkInput(unsigned char idx, unsigned char sod) {
  unsigned char ok = 1;
  unsigned char val = readInput(idx);
  if( sod || val != Sensor[idx].status ) {
    Sensor[idx].status = val;
    if( !sod && val == 0 ) {
      Sensor[idx].timer = 40; // 2 seconds
      Sensor[idx].timedoff = TRUE;
    }
    else if( !sod && Sensor[idx].timedoff ) {
      Sensor[idx].timer = 40; // reload timer
      Sensor[idx].timedoff = FALSE;
    }
    else {
      // Send an OPC.
      if( sod ) {
        canmsg.opc = 0;
        if ( val ) {
          canmsg.opc = OPC_ARON;
        }
      }
      else
        canmsg.opc = val ? OPC_ASON:OPC_ASOF;
      if( canmsg.opc > 0 ) {
        canmsg.d[0] = (NN_temp / 256) & 0xFF;
        canmsg.d[1] = (NN_temp % 256) & 0xFF;
        canmsg.d[2] = (Sensor[idx].addr / 256) & 0xFF;
        canmsg.d[3] = (Sensor[idx].addr % 256) & 0xFF;
        canmsg.len = 4; // data bytes
        ok = canQueue(&canmsg);
        if( !ok ) {
          Sensor[idx].status = !Sensor[idx].status;
        }
      }
    }
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


unsigned char readInput(int idx) {
  unsigned char val = 0;
  switch(idx) {
    case 0:  val = SENS1;  break;
    case 1:  val = SENS2;  break;
    case 2:  val = SENS3;  break;
    case 3:  val = SENS4;  break;
    case 4:  val = SENS5;  break;
    case 5:  val = SENS6;  break;
    case 6:  val = SENS7;  break;
    case 7:  val = SENS8;  break;
  }
  return !val;
}

void doIOTimers(void) {
  int i = 0;
  for( i = 0; i < 8; i++ ) {
    if( Sensor[i].timedoff ) {
      if( Sensor[i].timer > 0 ) {
        Sensor[i].timer--;
      }
    }
    if( RFID[i].timedoff ) {
      if( RFID[i].timer > 0 ) {
        RFID[i].timer--;
      }
    }
  }
}


void doTimedOff(int i) {
  if( Sensor[i].timedoff ) {
    if( Sensor[i].timer == 0 ) {
      Sensor[i].timedoff = 0;
      // Send an OPC.
      canmsg.opc = OPC_ASOF;
      canmsg.d[0] = (NN_temp / 256) & 0xFF;
      canmsg.d[1] = (NN_temp % 256) & 0xFF;
      canmsg.d[2] = (Sensor[i].addr / 256) & 0xFF;
      canmsg.d[3] = (Sensor[i].addr % 256) & 0xFF;
      canmsg.len = 4;
      canQueue(&canmsg);
    }
  }
}


