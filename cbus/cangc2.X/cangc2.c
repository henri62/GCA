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
#include <stdio.h>
#include "cbusdefs.h"
#include "cbus_common.h"
#include "commands.h"
#include "cangc2.h"
#include "isr_high.h"



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
near unsigned char  can_bus_off;
near unsigned short NN_temp;
near unsigned char  Latcount;
volatile near unsigned char tmr0_reload;


typedef struct {
  unsigned char cfg;
  unsigned char port;
  unsigned char status;
  //unsigned short timer;
} Port;

near Port Ports[16];

#pragma romdata parameters
const rom unsigned char params[32] = {MANU_ROCRAIL, MINOR_VER, MTYP_CANGC2, EVT_NUM, EVperEVT, NV_NUM, MAJOR_VER};

#pragma romdata


void initIO(void);
void checkInputs(void);
void initCAN(void);

/*
 * Interrupt vectors
 */
//#pragma code high_vector=0x08
#pragma code high_vector=0x808
void high_irq_errata_fix(void);
//void interrupt_at_high_vector(void)
void HIGH_INT_VECT(void)
{
    _asm
        CALL high_irq_errata_fix, 1
    _endasm
}

/*
 * See 18F2480 errata
 */
void high_irq_errata_fix(void) {
    _asm
        POP
        GOTO isr_high
    _endasm
}

//#pragma code low_vector=0x18
#pragma code low_vector=0x818
//void interrupt_at_low_vector(void)
void LOW_INT_VECT(void)
{
    _asm GOTO isr_low _endasm
}


#pragma code APP
void main(void) {
  initIO();
  initCAN();

    // Loop forever
  while (1) {
    // Check for Rx packet and setup pointer to it
    if (ecan_fifo_empty() == 0) {
      // Decode the new command
      parse_cmd();
    }
    checkInputs();
  }

}

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
  int idx = 0;

  memset(Ports, 0, sizeof(Port) * 16);

  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  Ports[idx].port = PORTAbits.RA0;
  TRISAbits.TRISA0 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  Ports[idx].port = PORTAbits.RA1;
  TRISAbits.TRISA1 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  Ports[idx].port = PORTAbits.RA3;
  TRISAbits.TRISA3 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  Ports[idx].port = PORTAbits.RA4;
  TRISAbits.TRISA4 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  Ports[idx].port = PORTAbits.RA5;
  TRISAbits.TRISA5 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  Ports[idx].port = PORTBbits.RB0;
  TRISBbits.TRISB0 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  Ports[idx].port = PORTBbits.RB1;
  TRISBbits.TRISB1 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  Ports[idx].port = PORTBbits.RB4;
  TRISBbits.TRISB4 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  Ports[idx].port = PORTCbits.RC0;
  TRISCbits.TRISC0 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  Ports[idx].port = PORTCbits.RC1;
  TRISCbits.TRISC1 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  Ports[idx].port = PORTCbits.RC2;
  TRISCbits.TRISC2 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  Ports[idx].port = PORTCbits.RC3;
  TRISCbits.TRISC3 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  Ports[idx].port = PORTCbits.RC7;
  TRISCbits.TRISC7 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  Ports[idx].port = PORTCbits.RC6;
  TRISCbits.TRISC6 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  Ports[idx].port = PORTCbits.RC5;
  TRISCbits.TRISC5 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  Ports[idx].port = PORTCbits.RC4;
  TRISCbits.TRISC4 = (Ports[idx].cfg & 0x01) ? 1:0;

  TRISBbits.TRISB6 = 0; /* LED1 */
  TRISBbits.TRISB7 = 0; /* LED2 */
  TRISAbits.TRISA2 = 1; /* Push button */

  tmr0_reload = TMR0_NORMAL;


  // Set up global interrupts
  RCONbits.IPEN = 1;          // Enable priority levels on interrupts
  INTCONbits.GIEL = 1;        // Low priority interrupts allowed
  INTCONbits.GIEH = 1;        // Interrupting enabled.
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


void checkInputs(void) {
  int idx = 0;
  for( idx = 0; idx < 16; idx++ ) {
    if( Ports[idx].cfg & 0x01 ) {
      if( Ports[idx].port != Ports[idx].status ) {
        Ports[idx].status = Ports[idx].port;
        // ToDo: Send an OPC.
      }
    }
  }
}
