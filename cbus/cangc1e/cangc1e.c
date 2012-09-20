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
#include <TCPIP Stack/Tick.h>

#include "project.h"
#include "cbusdefs.h"
#include "rocrail.h"
#include "canbus.h"
#include "utils.h"
#include "commands.h"
#include "cangc1e.h"
#include "isr.h"
#include "io.h"
#include "eth.h"
#include "gcaeth.h"




#pragma config OSC=HSPLL, FCMEN=OFF, IESO=OFF
#pragma config PWRT=ON, BOREN=BOHW, BORV=2, WDT = OFF, WDTPS=256
#pragma config MCLRE=OFF, LPT1OSC=OFF, PBADEN=OFF, DEBUG=OFF
#pragma config XINST=OFF, BBSIZ=1024, LVP=OFF, STVREN=ON
#pragma config CP0=OFF, CP1=OFF, CPB=OFF, CPD=OFF
#pragma config WRT0=OFF, WRT1=OFF, WRTB=OFF, WRTC=OFF, WRTD=OFF
#pragma config EBTR0=OFF, EBTR1=OFF, EBTRB=OFF


unsigned short NN_temp;
unsigned char NV1;
unsigned char CANID;
unsigned char Wait4NN;
unsigned char led1timer;
unsigned char led2timer;
unsigned char led3timer;
unsigned char IdleTime;
unsigned char cmdticker;
unsigned char isLearning;
unsigned char maxcanq;
unsigned char maxethq;

volatile unsigned short tmr1_reload;


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

/*
 * Interrupt vectors
 */
#pragma code high_vector=0x08

void HIGH_INT_VECT(void) {
    _asm GOTO isr_high _endasm
}

#pragma code low_vector=0x18

void LOW_INT_VECT(void) {
    _asm GOTO isr_low _endasm
}


#pragma code APP

void main(void) {
    unsigned char swTrig = 0;

    Wait4NN = FALSE;
    isLearning = FALSE;
    maxcanq = 0;
    maxethq = 0;
    cmdticker = 0;
    IdleTime = 120;
    doEthTick = FALSE;

    lDelay();

    NV1 = eeRead(EE_NV);
    if (NV1 == 0xFF) {
        eeWrite(EE_NV, 0);
        NV1 = 0;
        IdleTime = 120; // 120 * 500ms = 60 sec.
        eeWrite(EE_IDLETIME, IdleTime);
    }

    initIO();

    setupIO(FALSE);

    initEth();

    NN_temp = eeRead(EE_NN) * 256;
    NN_temp += eeRead(EE_NN + 1);
    if (NN_temp == 0 || NN_temp == 0xFFFF)
        NN_temp = DEFAULT_NN;

    CANID = eeRead(EE_CANID);
    if (CANID == 0 || CANID == 0xFF)
        CANID = NN_temp & 0xFF;

    cbusSetup();

    LED3 = LED_ON; /* signal running system */

    // Loop forever (nothing lasts forever...)
    while (1) {
        CANMsg cmsg;
        BYTE rsend;

        rsend = FALSE;
        // Check for Rx
        while (canbusRecv(&cmsg)) {
            if (CBusEthBroadcast(&cmsg)) {
                rsend = TRUE;
                break;
            }
        }

        if (doEthTick) {
            doEthTick = FALSE;
            CBusEthTick();
        }

        doEth();

        if (rsend) {
           if ( !CBusEthBroadcast(&cmsg)) {
               LED3 = LED_ON;
           }
        }


        if (checkFlimSwitch() && !swTrig) {
            swTrig = 1;
        } else if (!checkFlimSwitch() && swTrig) {
            swTrig = 0;
            if (Wait4NN) {
                Wait4NN = 0;
            } else {
                CANMsg canmsg;
                canmsg.b[sidh] = (CANID >> 3);
                canmsg.b[sidl] = (CANID << 5);
                canmsg.b[d0] = OPC_NNACK;
                canmsg.b[d1] = NN_temp / 256;
                canmsg.b[d2] = NN_temp % 256;
                canmsg.b[dlc] = 3;
                CBusEthBroadcast(&canmsg);
                Wait4NN = 1;
            }
        }
    }
}

void initIO(void) {
    int idx = 0;


    INTCON = 0;
    EECON1 = 0;

    IPR3 = 0; // All IRQs low priority for now
    IPR2 = 0;
    IPR1 = 0;
    PIE3 = 0;
    PIE2 = 0;
    PIE1 = 0;
    INTCON3 = 0;
    INTCON2 = 0; // Port B pullups are enabled
    PIR3 = 0;
    PIR2 = 0;
    PIR1 = 0;
    RCONbits.IPEN = 1; // enable interrupt priority levels

    T1CON = 0b00110000;
    IPR1bits.TMR1IP = 1; // high_priority
    PIE1bits.TMR1IE = 1; // enable int

    tmr1_reload = TMR1_NORMAL;
    TMR1H = tmr1_reload / 256;
    TMR1L = tmr1_reload % 256;

    T1CONbits.TMR1ON = 1; // start timer

    // Start slot timeout timer
    led500ms_timer = 2; // 500ms

    // Set up global interrupts
    RCONbits.IPEN = 1; // Enable priority levels on interrupts
    INTCONbits.GIEL = 1; // Low priority interrupts allowed
    INTCONbits.GIEH = 1; // Interrupting enabled.
}
