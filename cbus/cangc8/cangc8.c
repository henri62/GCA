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

#include "project.h"
#include "cbusdefs.h"
#include "rocrail.h"
#include "canbus.h"
#include "utils.h"
#include "commands.h"
#include "cangc8.h"
#include "isr.h"
#include "display.h"
#include "io.h"



#pragma config OSC=HSPLL, FCMEN=OFF, IESO=OFF
#pragma config PWRT=ON, BOREN=BOHW, BORV=2, WDT=OFF, WDTPS=256
#pragma config MCLRE=OFF, LPT1OSC=OFF, PBADEN=OFF, DEBUG=OFF
#pragma config XINST=OFF, BBSIZ=1024, LVP=OFF, STVREN=ON
#pragma config CP0=OFF, CP1=OFF, CPB=OFF, CPD=OFF
#pragma config WRT0=OFF, WRT1=OFF, WRTB=OFF, WRTC=OFF, WRTD=OFF
#pragma config EBTR0=OFF, EBTR1=OFF, EBTRB=OFF

ram Clock FastClock;

#pragma udata VARS_MAIN_ARRAYS1
far DisplayDef DisplayA[MAXDISPLAYS];
/*
#pragma udata VARS_MAIN_ARRAYS2
far DisplayDef DisplayB[MAXDISPLAYS];
 */
#pragma udata access VARS_MAIN

near unsigned char can_transmit_timeout;
near unsigned char can_transmit_failed;
near unsigned char can_bus_off;
near unsigned short NN_temp;
near unsigned char CANID;
near unsigned char Latcount;
near unsigned char NV1;
near unsigned char led1timer;
near unsigned short fclktimer;
near unsigned char ioIdx;
near unsigned char Wait4NN;
near unsigned char isLearning;
near unsigned short SOD;
near unsigned char doSOD;
near unsigned char doEV;
near unsigned char evIdx;

volatile unsigned char __LED2 = 0;


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
const rom unsigned char params[32] = {MANU_ROCRAIL, MINOR_VER, MTYP_CANGC8, EVT_NUM, EVperEVT, NV_NUM, MAJOR_VER};

#pragma romdata BOOTFLAG
const rom unsigned char bootflag = 0;


void initIO(void);
void initCAN(void);
void initTimers(void);

/*
 * Interrupt vectors
 */
#pragma code high_vector=0x208

void HIGH_INT_VECT(void) {
    _asm GOTO writeDisplays _endasm
}

#pragma code low_vector=0x218

void LOW_INT_VECT(void) {
    _asm GOTO isr_low _endasm
}


#pragma code APP

void main(void) {
    byte i;
    byte l3 = 1;
    unsigned char swTrig = 0;

    lDelay();

    led1timer = 0;
    fclktimer = 600;
    doEV = FALSE;
    evIdx = 0;

    FastClock.mins = 0;
    FastClock.hours = 10;
    FastClock.temp = 21;
    FastClock.mday = 1;
    FastClock.mon = 1;
    FastClock.issync = FALSE;
    FastClock.synctime = 0;
    FastClock.div = 1;
    FastClock.timer = 0;
    FastClock.gotfirstsync = FALSE;

    Wait4NN = FALSE;
    isLearning = FALSE;

    NV1 = eeRead(EE_NV);

    initIO();
    initDisplays();
    initTimers();

    NN_temp = eeRead(EE_NN) * 256;
    NN_temp += eeRead(EE_NN + 1);
    if (NN_temp == 0 || NN_temp == 0xFFFF)
        NN_temp = DEFAULT_NN;

    CANID = eeRead(EE_CANID);
    if (CANID == 0 || CANID == 0xFF)
        CANID = NN_temp & 0xFF;
    initCAN();

    delay();

    setupDisplays();

    LED3 = PORT_ON; /* signal running system */


    // Loop forever (nothing lasts forever...)
    while (1) {
        CANMsg canmsg;
        unsigned char txed = 0;
        l3 ^= 1;

        // Check for Rx packet and setup pointer to it
        while (canbusRecv(&canmsg)) {
            // Decode the new command
            txed = parseCmd(&canmsg);
        }

        if (doEV && l3) {
            if (doPortEvent(evIdx)) {
                evIdx++;
                if (evIdx >= MAXDISPLAYS) {
                    evIdx = 0;
                    doEV = 0;
                }
            }
        }


        if (checkFlimSwitch() && !swTrig) {
            isLearning = FALSE;
            swTrig = 1;
        } else if (!checkFlimSwitch() && swTrig) {
            swTrig = 0;
            if (Wait4NN) {
                Wait4NN = 0;
            } else {
                canmsg.b[d0] = OPC_NNACK;
                canmsg.b[d1] = NN_temp / 256;
                canmsg.b[d2] = NN_temp % 256;
                canmsg.b[dlc] = 3;
                canbusSend(&canmsg);
                Wait4NN = 1;
            }
        }
    }
}

void initTimers(void) {
    // Start slot timeout timer
    led500ms_timer = 500; // 500ms
    io_timer = 50; // 50ms
    led_timer = 4; // 4ms

    // ***** Timer0 *****
    // 16000000/4/32/125 == 1ms.
    T0CON = 0;
    // pre scaler 2:
    T0CONbits.PSA = 0;
    T0CONbits.T0PS0 = 0;
    T0CONbits.T0PS1 = 0;
    T0CONbits.T0PS2 = 1;
    // 8 bit counter
    T0CONbits.T08BIT = 1;
    TMR0H = 0;
#if (RESCLK == 4)
    TMR0L = 256 - 125; // 4MHz resonator
#elif (RESCLK == 8)
    TMR0L = 256 - 250; // 4MHz resonator
#else
#error "Wrong Clock Value"
#endif
    // timer on
    T0CONbits.TMR0ON = 1;
    // interrupt
    INTCONbits.TMR0IE = 1;
    INTCON2bits.TMR0IP = 1;

    // ***** Timer2 *****
    T2CON = 4 << 3; // 5 post scaler
    T2CONbits.TMR2ON = 1; // Timer2 on
    T2CONbits.T2CKPS0 = 0; // 16 pre scaler = 8MHz / 16
    T2CONbits.T2CKPS1 = 1;
    TMR2 = 0; // 1 mS
#if (RESCLK == 4)
    PR2 = 50; // 4MHz resonator
#elif (RESCLK == 8)
    PR2  = 100; // 8MHz resonator
#else
#error "Wrong Clock Value"
#endif
    PIE1bits.TMR2IE = 1;
    INTCONbits.PEIE = 1;
    IPR1bits.TMR2IP = 0; // high prio


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
    INTCON = 0;
    PIR3 = 0;
    PIR2 = 0;
    PIR1 = 0;


    // Set up global interrupts
    RCONbits.IPEN = 1; // Enable priority levels on interrupts
    INTCONbits.GIE = 1; // Interrupting enabled.
    INTCONbits.GIEL = 1; // Low priority interrupts allowed
    INTCONbits.GIEH = 1; // High priority interrupts allowed

    setupIO(FALSE);
}

void initCAN(void) {

    cbusSetup();
}


