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
#include "canbus.h"
#include "utils.h"
#include "cbusdefs.h"
#include "cangc8.h"
#include "io.h"
#include "display.h"


// #pragma code IO

void setupIO(byte clr) {
    int idx = 0;

    // all digital I/O
    ADCON0 = 0x00;
    ADCON1 = 0x0F;

    TRISBbits.TRISB6 = 0; // LED1 cbus activity
    TRISBbits.TRISB7 = 0; // LED2 learning mode
    TRISBbits.TRISB5 = 0; // LED3 running

    TRISAbits.TRISA2 = 1; /* SW */

    LED1 = PORT_OFF;
    LED2 = PORT_OFF;
    LED3 = PORT_OFF;

    if (checkFlimSwitch() || eeRead(EE_CLEAN) == 0xFF) {
        eeWrite(EE_CLEAN, 0);
        NV1 = CFG_SHORT_EVENTS;
        eeWrite(EE_NV, NV1);

        for (idx = 0; idx < MAXDISPLAYS; idx++) {
            eeWriteShort(EE_PORT_ADDR + 2 * idx, idx + 1);
            eeWrite(EE_PORT_CONF + idx, 0x09); // Contrast
        }
    }


    for (idx = 0; idx < MAXDISPLAYS; idx++) {
        DisplayA[idx].config = eeRead(EE_PORT_CONF + idx);
        DisplayA[idx].addr = eeReadShort(EE_PORT_ADDR + 2 * idx);
    }
}

// Called every 5ms.

void doLEDTimers(void) {


    if (led1timer > 0) {
        led1timer--;
        if (led1timer == 0) {
            LED1 = 0;
        }
    }

    if ( fclktimer > 0 ) {
        fclktimer--;
        if (fclktimer == 0) {
            displayFastClock();
        }
    }

    if (!FastClock.issync && FastClock.timer++ > (199 * 60 / FastClock.div)) {
        FastClock.mins++;
        if (FastClock.mins > 59) {
            FastClock.mins = 0;
            FastClock.hours++;
            if (FastClock.hours > 23) {
                FastClock.hours = 0;
                // TODO Date
            }
        }
        FastClock.timer -= (199 * 60 / FastClock.div);
        displayFastClock();
    }

    if (FastClock.div > 0 && FastClock.issync) {
        FastClock.synctime++;

        if (FastClock.synctime > (220 * 60 / FastClock.div)) {
            FastClock.issync = FALSE;
            FastClock.timer = FastClock.synctime;
            displayFastClock();
        }
    }
}

unsigned char checkFlimSwitch(void) {
    unsigned char val = SW;
    return !val;
}

unsigned char sodRFID(unsigned char i) {
    return FALSE;
}

void saveOutputStates(void) {
    int idx = 0;
    for (idx = 0; idx < 4; idx++) {
        //eeWrite(EE_SERVO_POSITION + idx, Servo[idx].position);
    }

}

void doLEDs(void) {
    if (Wait4NN || isLearning) {
        LED2 = __LED2;
        __LED2 ^= 1;
    } else {
        LED2 = PORT_OFF;
    }
}


static unsigned char __LED4 = 0;

void doLED250(void) {
}
