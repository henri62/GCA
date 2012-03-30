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

#include <string.h>

#include "project.h"
#include "cbusdefs.h"
#include "rocrail.h"
#include "utils.h"
#include "commands.h"
#include "cangc1e.h"
#include "io.h"
#include "cbuseth.h"

ram unsigned char pnnCount = 0;

#pragma udata access VARS

#pragma code APP

//
// parse_cmd()
//
// Decode the OPC and call the function to handle it.
//

/*
 * returns TRUE if the OPC can be broadcasted.
 */
unsigned char parseCmdEth(CANMsg* p_canmsg, unsigned char frametype) {
  byte broadcast = TRUE;
  CANMsg canmsg;

  if( frametype == EXT_FRAME ) {
    return TRUE;
  }


  if( frametype == ETH_FRAME ) {
    switch (p_canmsg->b[d0]) {
      case 0:
        break;
    }
    return FALSE;
  }

  switch (p_canmsg->b[d0]) {

    case OPC_QNN:
      canmsg.b[d0]  = OPC_PNN;
      canmsg.b[d1]  = (NN_temp / 256) & 0xFF;
      canmsg.b[d2]  = (NN_temp % 256) & 0xFF;
      canmsg.b[d3]  = params[0];
      canmsg.b[d4]  = params[2];
      canmsg.b[d5]  = NV1;
      canmsg.b[dlc] = 6;
      ethQueue(&canmsg);
      break;

    case OPC_RQNPN:
      // Request to read a parameter
      if (thisNN(p_canmsg) == 1) {
        doRqnpn((unsigned int) p_canmsg->b[d3]);
        broadcast = FALSE;
      }
      break;

    case OPC_SNN:
    {
      if( Wait4NN ) {
        unsigned char nnH = p_canmsg->b[d1];
        unsigned char nnL = p_canmsg->b[d2];
        NN_temp = nnH * 256 + nnL;
        eeWrite(EE_NN, nnH);
        eeWrite(EE_NN+1, nnL);
        Wait4NN = 0;
        broadcast = FALSE;
      }
      break;
    }


    case OPC_RQNP:
      if( Wait4NN ) {
        canmsg.b[d0]  = OPC_PARAMS;
        canmsg.b[d1]  = params[0];
        canmsg.b[d2]  = params[1];
        canmsg.b[d3]  = params[2];
        canmsg.b[d4]  = params[3];
        canmsg.b[d5]  = params[4];
        canmsg.b[d6]  = params[5];
        canmsg.b[d7]  = params[6];
        canmsg.b[dlc] = 7;
        ethQueue(&canmsg);
        broadcast = FALSE;
      }
      break;

    case OPC_NVRD:
      if( thisNN(p_canmsg) ) {
        byte nvnr = p_canmsg->b[d3];
        canmsg.b[d0] = OPC_NVANS;
        canmsg.b[d1]  = (NN_temp / 256) & 0xFF;
        canmsg.b[d2]  = (NN_temp % 256) & 0xFF;
        canmsg.b[d3]  = nvnr;
        canmsg.b[dlc] = 4;

        if( nvnr == 1 ) {
          canmsg.b[d4]  = NV1;
          ethQueue(&canmsg);
          broadcast = FALSE;
        }
        else if( nvnr == 2 ) {
          canmsg.b[d4] = CANID;
          ethQueue(&canmsg);
          broadcast = FALSE;
        }
        else if( nvnr > 2 && nvnr < 7) {
          canmsg.b[d4] = eeRead(EE_IPADDR+nvnr-3);
          ethQueue(&canmsg);
          broadcast = FALSE;
        }
        else if( nvnr > 6 && nvnr < 11) {
          canmsg.b[d4] = eeRead(EE_NETMASK+nvnr-7);
          ethQueue(&canmsg);
          broadcast = FALSE;
        }
        else if( nvnr > 10 && nvnr < 17) {
          canmsg.b[d4] = eeRead(EE_MACADDR+nvnr-11);
          ethQueue(&canmsg);
          broadcast = FALSE;
        }
        else if( nvnr == 17) {
          canmsg.b[d4] = eeRead(EE_IDLETIME);
          ethQueue(&canmsg);
          broadcast = FALSE;
        }
      }
      break;

    case OPC_NVSET:
      if( thisNN(p_canmsg) ) {
        byte nvnr = p_canmsg->b[d3];
        if( nvnr == 1 ) {
          NV1 = p_canmsg->b[d4];
          eeWrite(EE_NV, NV1);
        }
        else if( nvnr == 2 ) {
          CANID = p_canmsg->b[d4];
          eeWrite(EE_CANID, CANID);
        }
        else if( nvnr > 2 && nvnr < 7) {
          eeWrite(EE_IPADDR+nvnr-3, p_canmsg->b[d4]);
        }
        else if( nvnr > 6 && nvnr < 11) {
          eeWrite(EE_NETMASK+nvnr-7, p_canmsg->b[d4]);
        }
        else if( nvnr > 10 && nvnr < 17) {
          eeWrite(EE_MACADDR+nvnr-11, p_canmsg->b[d4]);
        }
        else if( nvnr == 17 ) {
          IdleTime = p_canmsg->b[d4];
          eeWrite(EE_IDLETIME, IdleTime);
        }
        broadcast = FALSE;
      }
      break;

    default:
      break;
  }


  return broadcast;
}

void doRqnpn(unsigned int idx) {
  if (idx < 8) {
    canmsg.b[d0]  = OPC_PARAN;
    canmsg.b[d1]  = (NN_temp / 256) & 0xFF;
    canmsg.b[d2]  = (NN_temp % 256) & 0xFF;
    canmsg.b[d3]  = idx;
    canmsg.b[d4]  = params[idx - 1];
    canmsg.b[dlc] = 5;
    ethQueue(&canmsg);
  }
}



byte thisNN(CANMsg* p_canmsg) {
  if ((((unsigned short) (p_canmsg->b[d1]) << 8) + p_canmsg->b[d2]) == NN_temp) {
    LED3 = LED_OFF; /* signal match */
    led3timer = 10;
    return 1;
  }
  else
    return 0;

}

