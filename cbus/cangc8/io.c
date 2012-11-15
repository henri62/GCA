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

void fc2ee(void) {


}

void ee2fc(void) {


}

// Called every 5ms.

void doLEDTimers(void) {


  if (led1timer > 0) {
    led1timer--;
    if (led1timer == 0) {
      LED1 = 0;
    }
  }

}

void doFastClock(void) {

  if (!FastClock.issync && FastClock.timer++ > (120 / (FastClock.rate ? FastClock.rate : 20))) {
    if (FastClock.rate) {
      FastClock.mins++;
      if (FastClock.mins > 59) {
        FastClock.mins = 0;
        FastClock.hours++;
        if (FastClock.hours > 23) {
          FastClock.hours = 0;
          FastClock.wday++;
          if (FastClock.wday > 7)
            FastClock.wday = 1;
          FastClock.mday++;
          if (FastClock.mday > 30) {
            FastClock.mday = 1;
            FastClock.mon++;
            if (FastClock.mon > 12)
              FastClock.mon = 1;
          }
        }
      }
    }
    FastClock.timer -= (120 / (FastClock.rate ? FastClock.rate : 20));
    displayFastClock();
    if (NV1 & CFG_ENABLE_FCLK) {
      CANMsg canmsg;

      canmsg.b[d0] = OPC_FCLK;
      canmsg.b[d1] = FastClock.mins;
      canmsg.b[d2] = FastClock.hours;
      canmsg.b[d3] = (FastClock.mon << 4) + FastClock.wday;
      canmsg.b[d4] = FastClock.rate;
      canmsg.b[d5] = FastClock.mday;
      canmsg.b[d6] = FastClock.temp;
      canmsg.b[dlc] = 7;
      canbusSend(&canmsg);
    }
  }

  if (FastClock.rate > 0 && FastClock.issync) {
    FastClock.synctime++;

    if (FastClock.synctime > ((180 / FastClock.rate) + 4)) {
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

void doLEDs(void) {
  if (Wait4NN || isLearning) {
    LED2 = __LED2;
    __LED2 ^= 1;
  } else {
    LED2 = PORT_OFF;
  }
}

void doLED250(void) {
}
