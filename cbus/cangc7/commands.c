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


#include "project.h"
#include "cbusdefs.h"
#include "rocrail.h"
#include "utils.h"
#include "commands.h"
#include "cangc7.h"
#include "io.h"

ram unsigned char pnnCount = 0;

#pragma udata access VARS

#pragma code APP

//
// parse_cmd()
//
// Decode the OPC and call the function to handle it.
//

static unsigned char __LED1 = 0;
unsigned char parseCmd(void) {
  unsigned char txed = 0;
  //mode_word.s_full = 0;
  switch (rx_ptr->d0) {

    case OPC_FCLK:
      // fast clock:
      /*
      cmd[0] = OPC_FCLK;
      cmd[1] = mins;
      cmd[2] = hours;
      cmd[3] = wday;
      cmd[4] = div;
      cmd[5] = 0;
      cmd[6] = 0;
      */
      FastClock.mins   = rx_ptr->d1;
      FastClock.hours  = rx_ptr->d2;
      FastClock.wday   = rx_ptr->d3;
      FastClock.div    = rx_ptr->d4;
      FastClock.issync = TRUE;

      //LED1 = __LED1;
      __LED1 ^= 1;
      break;

    case OPC_RQNPN:
      // Request to read a parameter
      if (thisNN() == 1) {
        doRqnpn((unsigned int) rx_ptr->d3);
      }
      break;

    case OPC_SNN:
    {
      unsigned char nnH = rx_ptr->d1;
      unsigned char nnL = rx_ptr->d2;
      NN_temp = nnH * 256 + nnL;
      eeWrite(EE_NN, nnH);
      eeWrite(EE_NN+1, nnL);
      LED2 = 0;
      break;
    }

    case OPC_RQNP:
      canmsg.opc = OPC_PARAMS;
      canmsg.d[0] = params[0];
      canmsg.d[1] = params[1];
      canmsg.d[2] = params[2];
      canmsg.d[3] = params[3];
      canmsg.d[4] = params[4];
      canmsg.d[5] = params[5];
      canmsg.d[6] = params[6];
      canmsg.len = 7;
      canQueue(&canmsg);
      txed = 1;
      break;

    case OPC_RTOF:
      if( NV1 & CFG_SAVEOUTPUT ) {
        saveOutputStates();
      }
      break;

    case OPC_NVRD:
      if( thisNN() ) {
        byte nvnr = rx_ptr->d3;
        if( nvnr == 1 ) {
          canmsg.opc = OPC_NVANS;
          canmsg.d[0] = (NN_temp / 256) & 0xFF;
          canmsg.d[1] = (NN_temp % 256) & 0xFF;
          canmsg.d[2] = nvnr;
          canmsg.d[3] = NV1;
          canmsg.len = 4;
          canQueue(&canmsg);
          txed = 1;
        }
      }
      break;

    case OPC_NVSET:
      if( thisNN() ) {
        byte nvnr = rx_ptr->d3;
        if( nvnr == 1 ) {
          NV1 = rx_ptr->d4;
          eeWrite(EE_NV, NV1);
        }
      }
      break;

    default: break;
  }

    rx_ptr->con = 0;
    if (can_bus_off) {
      // At least one buffer is now free
      can_bus_off = 0;
      PIE3bits.FIFOWMIE = 1;
    }

    return txed;
}

void doRqnpn(unsigned int idx) {
  if (idx < 8) {
    canmsg.opc = OPC_PARAN;
    canmsg.d[0] = (NN_temp / 256) & 0xFF;
    canmsg.d[1] = (NN_temp % 256) & 0xFF;
    canmsg.d[2] = idx;
    canmsg.d[3] = params[idx - 1];
    canmsg.len = 4;
    canQueue(&canmsg);
  }
}

int thisNN() {
  if ((((unsigned short) (rx_ptr->d1) << 8) + rx_ptr->d2) == NN_temp)
    return 1;
  else
    return 0;

}

