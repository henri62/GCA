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


#include "defs.h"
#include "cangc9.h"
#include "io.h"
#include "utils.h"
#include "isr.h"
#include "serial.h"

#pragma config OSC=HSPLL, IESO=OFF
#pragma config PWRT=ON, WDT=OFF, WDTPS=256
#pragma config MCLRE=OFF, DEBUG=OFF
#pragma config LVP=OFF
#pragma config CP0=OFF, CP1=OFF, CPB=OFF, CPD=OFF
#pragma config WRT0=OFF, WRT1=OFF, WRTB=OFF, WRTC=OFF, WRTD=OFF
#pragma config EBTR0=OFF, EBTR1=OFF, EBTRB=OFF


#pragma udata access VARS_MAIN_1
near byte  bidiType;
near byte timer500;
near byte timer200;
near byte timer50;
near byte timer5;


/*
 * Interrupt vectors
 */
#pragma code high_vector=0x08
void HIGH_INT_VECT(void)
{
    _asm GOTO sendSerial _endasm
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

  lDelay();

  initIO();
  initTimers();

  sDelay();

  bidiType = IN_TYPE;

  LED1_RUN = PORT_ON;

  // The main loop.
  while( TRUE ) {



  }

}

