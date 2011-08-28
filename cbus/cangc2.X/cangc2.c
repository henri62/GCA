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
#include "can_send.h"
#include "io.h"



#pragma config OSC=HSPLL, FCMEN=OFF, IESO=OFF
#pragma config PWRT=ON, BOREN=BOHW, BORV=2, WDT = OFF, WDTPS=256
#pragma config MCLRE=ON, LPT1OSC=OFF, PBADEN=OFF, DEBUG=OFF
#pragma config XINST=OFF, BBSIZ=1024, LVP=OFF, STVREN=OFF
#pragma config CP0=OFF, CP1=OFF, CPB=OFF, CPD=OFF
#pragma config WRT0=OFF, WRT1=OFF, WRTB=OFF, WRTC=OFF, WRTD=OFF
#pragma config EBTR0=OFF, EBTR1=OFF, EBTRB=OFF



ram Port Ports[16];

#pragma udata access VARS

near unsigned char  can_transmit_timeout;
near unsigned char  can_transmit_failed;
near unsigned char  can_bus_off;
near unsigned short NN_temp;
near unsigned char  Latcount;
volatile near unsigned char tmr0_reload;



#pragma romdata parameters
const rom unsigned char params[32] = {MANU_ROCRAIL, MINOR_VER, MTYP_CANGC2, EVT_NUM, EVperEVT, NV_NUM, MAJOR_VER};

#pragma romdata


void initIO(void);
void checkInputs(void);
void initCAN(void);
void resetOutputs(void);

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
  resetOutputs();

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

void initIO(void) {
  int idx = 0;

  INTCON = 0;
  EECON1 = 0;

  IPR3 = 0;					// All IRQs low priority for now
  IPR2 = 0;
  IPR1 = 0;
  PIE3 = 0b00100001;			// CAN TX error and FIFOWM interrupts
  PIE2 = 0;
  PIE1 = 0;
  INTCON3 = 0;
  INTCON2 = 0;                // Port B pullups are enabled
  INTCON = 0;
  PIR3 = 0;
  PIR2 = 0;
  PIR1 = 0;
  RCONbits.IPEN = 1;			// enable interrupt priority levels

  // Set up TMR0 for DCC bit timing with 58us period prescaler 4:1,
  // 8 bit mode
  // T0CON = 0x40;  old value
  T0CON = 0x41;
  TMR0L = 0;
  TMR0H = 0;
  INTCONbits.TMR0IE = 1;
  T0CONbits.TMR0ON = 1;
  INTCON2bits.TMR0IP = 1;

  tmr0_reload = TMR0_NORMAL;


    // Start slot timeout timer
  slot_timer = ((short long)500000)/58;  // Half second count down for 58uS interrupts

  // Set up global interrupts
  RCONbits.IPEN = 1;          // Enable priority levels on interrupts
  INTCONbits.GIEL = 1;        // Low priority interrupts allowed
  INTCONbits.GIEH = 1;        // Interrupting enabled.

  setupIO();
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
    if( (Ports[idx].cfg & 0x01) == 0x01 ) {
      unsigned char val = readInput(idx);
      if( val != Ports[idx].status ) {
        Ports[idx].status = val;
        // ToDo: Send an OPC.
        Tx1[d0] = OPC_ARST;
        can_tx(1);
        LED2 = val;
      }
    }
  }
}

void resetOutputs(void) {
  int idx = 0;
  for( idx = 0; idx < 16; idx++ ) {
    if( (Ports[idx].cfg & 0x01) == 0 ) {
        writeOutput(idx, idx%2);
    }
  }

}
