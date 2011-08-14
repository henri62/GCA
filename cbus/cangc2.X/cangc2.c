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


#include <p18cxxx.h>
#include "cbusdefs.h"
#include "cbus_common.h"
#include "cangc2.h"


#pragma config OSC=HSPLL, FCMEN=OFF, IESO=OFF
#pragma config PWRT=ON, BOREN=BOHW, BORV=2, WDT = OFF, WDTPS=256
#pragma config MCLRE=ON, LPT1OSC=OFF, PBADEN=OFF, DEBUG=OFF
#pragma config XINST=OFF, BBSIZ=1024, LVP=OFF, STVREN=OFF
#pragma config CP0=OFF, CP1=OFF, CPB=OFF, CPD=OFF
#pragma config WRT0=OFF, WRT1=OFF, WRTB=OFF, WRTC=OFF, WRTD=OFF
#pragma config EBTR0=OFF, EBTR1=OFF, EBTRB=OFF

#pragma udata access VARS

near unsigned char  can_transmit_timeout;
near unsigned char  can_transmit_failed;
near unsigned short NN_temp;
near unsigned char  Latcount;

#pragma romdata parameters
const rom unsigned char params[32] = {MANU_ROCRAIL, MINOR_VER, MTYP_CANGC2, EVT_NUM, EVperEVT, NV_NUM, MAJOR_VER};

#pragma romdata


/*
Port   PICbit  Connector
2      RA1     J4
3      RA3     J4
1      RA0     J4
4      RA4     J4
5      RA5     J4
6      RB0     J4
7      RB1     J4
8      RB4     J4
9      RC0     J3
10     RC1     J3
11     RC2     J3
12     RC3     J3
13     RC7     J3
14     RC6     J3
15     RC5     J3
16     RC4     J3
 */
void initIO(void) {
  unsigned char cfg = ee_read(EE_PORTCFG + 0);
  TRISAbits.TRISA0 = (cfg & 0x01) ? 1:0;
  cfg = ee_read(EE_PORTCFG + 1);
  TRISAbits.TRISA1 = (cfg & 0x01) ? 1:0;
  cfg = ee_read(EE_PORTCFG + 2);
  TRISAbits.TRISA3 = (cfg & 0x01) ? 1:0;
  cfg = ee_read(EE_PORTCFG + 3);
  TRISAbits.TRISA4 = (cfg & 0x01) ? 1:0;
  cfg = ee_read(EE_PORTCFG + 4);
  TRISAbits.TRISA5 = (cfg & 0x01) ? 1:0;
  cfg = ee_read(EE_PORTCFG + 5);
  TRISBbits.TRISB0 = (cfg & 0x01) ? 1:0;
  cfg = ee_read(EE_PORTCFG + 6);
  TRISBbits.TRISB1 = (cfg & 0x01) ? 1:0;
  cfg = ee_read(EE_PORTCFG + 7);
  TRISBbits.TRISB4 = (cfg & 0x01) ? 1:0;
  cfg = ee_read(EE_PORTCFG + 8);
  TRISCbits.TRISC0 = (cfg & 0x01) ? 1:0;
  cfg = ee_read(EE_PORTCFG + 9);
  TRISCbits.TRISC1 = (cfg & 0x01) ? 1:0;
  cfg = ee_read(EE_PORTCFG + 10);
  TRISCbits.TRISC2 = (cfg & 0x01) ? 1:0;
  cfg = ee_read(EE_PORTCFG + 11);
  TRISCbits.TRISC3 = (cfg & 0x01) ? 1:0;
  cfg = ee_read(EE_PORTCFG + 12);
  TRISCbits.TRISC7 = (cfg & 0x01) ? 1:0;
  cfg = ee_read(EE_PORTCFG + 13);
  TRISCbits.TRISC6 = (cfg & 0x01) ? 1:0;
  cfg = ee_read(EE_PORTCFG + 14);
  TRISCbits.TRISC5 = (cfg & 0x01) ? 1:0;
  cfg = ee_read(EE_PORTCFG + 15);
  TRISCbits.TRISC4 = (cfg & 0x01) ? 1:0;

  TRISBbits.TRISB6 = 0; /* LED1 */
  TRISBbits.TRISB7 = 0; /* LED2 */
  TRISAbits.TRISA2 = 1; /* Push button */
}


void initCAN(void) {
  // Setup ID
  NN_temp = DEFAULT_NN;
  Tx1[con] = 0;
  Tx1[sidh] = 0b10110000 | (FIXED_CAN_ID & 0x78) >>3;
  Tx1[sidl] = (FIXED_CAN_ID & 0x07) << 5;

  // Setup TXB0 with high priority OPC_HLT
  TXB0SIDH = 0b01110000 | (FIXED_CAN_ID & 0x78) >>3;
  TXB0SIDL = (FIXED_CAN_ID & 0x07) << 5;
  TXB0DLC = 1;
  TXB0D0 = OPC_HLT;

  // enable interrupts
  INTCONbits.GIEH = 1;
  INTCONbits.GIEL = 1;
  
  cbus_setup();
}


void main(void) {
    initIO();
    initCAN();

    return;
}
