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
#include "dcc.h"

#pragma config OSC=HSPLL, IESO=OFF
#pragma config PWRT=ON, WDT=OFF, WDTPS=256
#pragma config MCLRE=OFF, DEBUG=OFF
#pragma config LVP=OFF
#pragma config CP0=OFF, CP1=OFF, CPB=OFF, CPD=OFF
#pragma config WRT0=OFF, WRT1=OFF, WRTB=OFF, WRTC=OFF, WRTD=OFF
#pragma config EBTR0=OFF, EBTR1=OFF, EBTRB=OFF

#pragma udata access VARS_MAIN_ARRAYS1
//far slot slots[MAX_SLOTS];


//#pragma udata access VARS_MAIN_1

/*
 * 
 */
void main(void) {
  setupIO();

  LED5_RUN = PORT_ON;

  while(TRUE) {
    
  }

}

