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


#include <GenericTypeDefs.h>
#include "project.h"
#include "isr.h"
#include "cangc2.h"
#include "io.h"

#pragma udata access VARS
near unsigned short led500ms_timer;
near unsigned short io_timer;
near unsigned short led_timer;

#pragma code APP

//
// Interrupt Service Routine
//
// TMR0 generates a heartbeat every 250uS.
//
#pragma interrupt isr_high

void isr_high(void) {
    INTCONbits.T0IF = 0;
    TMR0L = tmr0_reload;

    //
    // I/O timeout - 5ms
    //
    if (--led_timer == 0) {
        led_timer = 20;
        doLEDTimers();
    }

    //
    // I/O timeout - 50ms
    //
    if (--io_timer == 0) {
        io_timer = 200;
        doIOTimers();
    }

    //
    // Timer 500ms
    //
    if (--led500ms_timer == 0) {
        led500ms_timer = 2000;
        doLEDs();

    }

}


//
// Low priority interrupt. Used for CAN receive.
//
#pragma interruptlow isr_low

void isr_low(void) {

    if (PIR3bits.FIFOWMIF == 1) {
        PIR3bits.FIFOWMIF = 0;
        canbusFifo();
    }

    PIR3 = 0; // clear interrupts
}
