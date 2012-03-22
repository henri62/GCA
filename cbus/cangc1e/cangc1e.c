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
#include "cbusdefs.h"
#include "rocrail.h"
#include "cbus.h"
#include "utils.h"
#include "commands.h"
#include "cangc1e.h"
#include "isr.h"
#include "io.h"
#include "project.h"
#include "eth.h"




#pragma config OSC=HSPLL, FCMEN=OFF, IESO=OFF
#pragma config PWRT=ON, BOREN=BOHW, BORV=2, WDT = OFF, WDTPS=256
#pragma config MCLRE=OFF, LPT1OSC=OFF, PBADEN=OFF, DEBUG=OFF
#pragma config XINST=OFF, BBSIZ=1024, LVP=OFF, STVREN=OFF
#pragma config CP0=OFF, CP1=OFF, CPB=OFF, CPD=OFF
#pragma config WRT0=OFF, WRT1=OFF, WRTB=OFF, WRTC=OFF, WRTD=OFF
#pragma config EBTR0=OFF, EBTR1=OFF, EBTRB=OFF


//ram Buffer CBuffer;
//ram Buffer EBuffer;

#pragma udata access VARS

near unsigned char  can_transmit_timeout;
near unsigned char  can_transmit_failed;
near unsigned char  can_bus_off;
near unsigned short NN_temp;
near unsigned char  led1timer;
near unsigned char  led2timer;
near unsigned char  led3timer;
near unsigned char ticktimer;
near unsigned char  CANID;
near unsigned char  Latcount;
near unsigned char  NV1;
near unsigned char  ioIdx;
near unsigned char  Wait4NN;
near unsigned char  isLearning;


volatile near unsigned char tmr0_reload;


  /*
    Para 1 Manufacturer number as allocated by the NMRA
    Para 2 Module version number or code
    Para 3 Module identifier
    Para 4 Number of events allowed
    Para 5 Number of event variables per event
    Para 6 Number of node variables
    Para 7 Not yet allocated.
  */

#pragma romdata parameters
const rom unsigned char params[32] = {MANU_ROCRAIL, MINOR_VER, MTYP_CANGC1E, EVT_NUM, EVperEVT, NV_NUM, MAJOR_VER};

#pragma romdata


void initIO(void);
void initCAN(void);

/*
 * Interrupt vectors
 */
#pragma code high_vector=0x08
void HIGH_INT_VECT(void)
{
    _asm GOTO isr_high _endasm
}

#pragma code low_vector=0x18
void LOW_INT_VECT(void)
{
    _asm GOTO isr_low _endasm
}


#pragma code APP
void main(void) {
  unsigned char swTrig = 0;

  lDelay();

  ioIdx = 0;
  Wait4NN = FALSE;

  NV1 = eeRead(EE_NV);

  initEth();

  initIO();

  NN_temp  = eeRead(EE_NN) * 256;
  NN_temp += eeRead(EE_NN+1);
  if( NN_temp == 0 || NN_temp == 0xFFFF )
    NN_temp = DEFAULT_NN;

  CANID = eeRead(EE_CANID);
  if( CANID == 0 || CANID == 0xFF )
    CANID = NN_temp & 0xFF;
  initCAN();

  LED3 = LED_ON; /* signal running system */

  delay();
  restoreOutputStates();
  delay();

  // Loop forever (nothing lasts forever...)
  while (1) {
    unsigned char txed = 0;
    // Check for Rx packet and setup pointer to it
    while (fifoEmpty() == 0) {
      // Decode the new command
      LED1 = LED_ON;
      led1timer = 20;
      txed = parseCmd();
    }

    if( checkInput(ioIdx) ) {
      ioIdx++;
      if( ioIdx >= 16 ) {
        ioIdx = 0;
      }
    }

    //LED2 = PORT_ON;
    canSendQ();
    //LED2 = PORT_OFF;

    doEth();

    if( checkFlimSwitch() && !swTrig ) {
      swTrig = 1;
    }
    else if( !checkFlimSwitch() && swTrig) {
      swTrig = 0;
      if( Wait4NN ) {
        Wait4NN = 0;
      }
      else {
        canmsg.opc = OPC_NNACK;
        canmsg.d[0] = NN_temp / 256;
        canmsg.d[1] = NN_temp % 256;
        canmsg.len = 2;
        canQueue(&canmsg);
        Wait4NN = 1;
      }
    }

  }

}

void initIO(void) {
  int idx = 0;

  INTCON = 0;
  EECON1 = 0;

  IPR3 = 0;					// All IRQs low priority for now
  IPR2 = 0;
  IPR1 = 0;
  PIE3 = 0;
  PIE3bits.TXB1IE = 1;
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
  //T0CON = 0x41; //or 4MHz resonat
  T0CON = 0x42; //or 8MHz resonat
  TMR0L = 0;
  TMR0H = 0;
  INTCONbits.TMR0IE = 1;
  T0CONbits.TMR0ON = 1;
  INTCON2bits.TMR0IP = 1;

  // clear the fifo receive buffers
  while (fifoEmpty() == 0) {
    rx_ptr->con = 0;
  }


  tmr0_reload = TMR0_NORMAL;


    // Start slot timeout timer
  led500ms_timer = 2000;  // 500ms
  io_timer = 200;  // 50ms
  led_timer = 16;  // 4ms
  led_timer = 8;  // 2ms

  // Set up global interrupts
  RCONbits.IPEN = 1;          // Enable priority levels on interrupts
  INTCONbits.GIEL = 1;        // Low priority interrupts allowed
  INTCONbits.GIEH = 1;        // Interrupting enabled.

  setupIO(FALSE);
}


void initCAN(void) {
  // Setup ID
  Tx1[con] = 0;
  Tx1[sidh] = 0b10110000 | (CANID & 0x78) >>3;
  Tx1[sidl] = (CANID & 0x07) << 5;

  // Setup TXB0 with high priority OPC_HLT
  TXB0SIDH = 0b01110000 | (CANID & 0x78) >>3;
  TXB0SIDL = (CANID & 0x07) << 5;
  TXB0DLC = 1;
  TXB0D0 = OPC_HLT;

  // enable interrupts
  INTCONbits.GIEH = 1;
  INTCONbits.GIEL = 1;
  
  cbusSetup();
}


