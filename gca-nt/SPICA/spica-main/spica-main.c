/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <stdio.h>
#include <stdlib.h>

#include "io.h"
#include "main.h"
#include "spica.h"
#include "rocrail.h"
#include "cbusdefs.h"
#include "loconet.h"
#include "isr.h"
#include "cbus.h"
#include "utils.h"

#pragma config OSC=HSPLL, FCMEN=OFF, IESO=OFF
#pragma config PWRT=ON, BOREN=BOHW, BORV=2, WDT=OFF, WDTPS=256
#pragma config MCLRE=OFF, LPT1OSC=OFF, PBADEN=OFF, DEBUG=OFF
#pragma config XINST=OFF, BBSIZ=1024, LVP=OFF, STVREN=ON
#pragma config CP0=OFF, CP1=OFF, CPB=OFF, CPD=OFF
#pragma config WRT0=OFF, WRT1=OFF, WRTB=OFF, WRTC=OFF, WRTD=OFF
#pragma config EBTR0=OFF, EBTR1=OFF, EBTRB=OFF

#pragma udata access VARS_MAIN_1
near unsigned char  can_transmit_timeout;
near unsigned char  can_transmit_failed;
near unsigned char  can_bus_off;
near unsigned short NN_temp;
near unsigned char  CANID;
near unsigned char  Latcount;
near unsigned char  NV1;
near unsigned char  ledBUStimer;
near unsigned char  Wait4NN;
near unsigned char  isLearning;
volatile near unsigned char tmr0_reload;


#pragma romdata parameters
const rom unsigned char params[32] = {MANU_ROCRAIL, MINOR_VER, MTYP_SPICA, EVT_NUM, EVperEVT, NV_NUM, MAJOR_VER};


void initIO(void);
void initCAN(void);
void initTimers(void);


/*
 * Interrupt vectors
 */
#pragma code high_vector=0x08
void HIGH_INT_VECT(void)
{
    _asm GOTO scanLN _endasm
}

#pragma code low_vector=0x18
void LOW_INT_VECT(void)
{
    _asm GOTO isr_low _endasm
}



/*
 * 
 */
void main(void) {
  ledBUStimer = 0;
  Wait4NN = FALSE;
  isLearning = FALSE;

  lDelay();

  setupIO();

  initIO();
  initLN();
  initCAN();
  initTimers();

  LED2_RUN = PORT_ON;

  while(TRUE) {

    while( doLocoNet() ) {
      ;
    }
  }
}




void initIO(void) {

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



  // Set up global interrupts
  RCONbits.IPEN   = 1;        // Enable priority levels on interrupts
  INTCONbits.GIE  = 1;        // Interrupting enabled.
  INTCONbits.GIEL = 1;        // Low priority interrupts allowed
  INTCONbits.GIEH = 1;        // High priority interrupts allowed

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


void initTimers(void) {
  //tmr0_reload = TMR0_NORMAL;
    // Start slot timeout timer
  led500ms_timer = 500;  // 500ms
  io_timer = 50;  // 50ms
  led_timer = 4;  // 4ms

  // ***** Timer0 *****
  // 32000000/4/2/80 == 50kHz.
  T0CON = 0;
  // pre scaler 2:
  T0CONbits.PSA   = 0;
  T0CONbits.T0PS0 = 0;
  T0CONbits.T0PS1 = 0;
  T0CONbits.T0PS2 = 0;
  // 8 bit counter
  T0CONbits.T08BIT = 1;
  TMR0H = 0;
  TMR0L = 256 - 80;
  // timer on
  T0CONbits.TMR0ON = 1;
  // interrupt
  INTCONbits.TMR0IE  = 1;
  INTCON2bits.TMR0IP = 1;

  // ***** Timer2 *****
  T2CON = 4 << 3; // 5 post scaler
  T2CONbits.TMR2ON  = 1; // Timer2 on
  T2CONbits.T2CKPS0 = 0; // 16 pre scaler = 8MHz / 16
  T2CONbits.T2CKPS1 = 1;
  TMR2 = 0; // 1 mS
  PR2  = 100;
  PIE1bits.TMR2IE = 1;
  INTCONbits.PEIE = 1;
  IPR1bits.TMR2IP = 0; // high prio

}

