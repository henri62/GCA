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

unsigned char parseCmd(void) {
  canmsg.opc  = rx_ptr->d0;
  canmsg.d[0] = rx_ptr->d1;
  canmsg.d[1] = rx_ptr->d2;
  canmsg.d[2] = rx_ptr->d3;
  canmsg.d[3] = rx_ptr->d4;
  canmsg.d[4] = rx_ptr->d5;
  canmsg.d[5] = rx_ptr->d6;
  canmsg.d[6] = rx_ptr->d7;
  canmsg.len = getDataLen(canmsg.opc); // TODO: Adjust len to OPC
  ethQueue(&canmsg);

  rx_ptr->con = 0;
  if (can_bus_off) {
    // At least one buffer is now free
    can_bus_off = 0;
    PIE3bits.FIFOWMIE = 1;
  }
}


unsigned char parseCmdEth(CANMsg* p_canmsg) {
  unsigned char txed = 0;
  CANMsg canmsg;

  switch (p_canmsg->opc) {

    case OPC_QNN:
      canmsg.opc  = OPC_PNN;
      canmsg.d[0] = (NN_temp / 256) & 0xFF;
      canmsg.d[1] = (NN_temp % 256) & 0xFF;
      canmsg.d[2] = params[0];
      canmsg.d[3] = params[2];
      canmsg.d[4] = NV1;
      canmsg.len = 5;
      ethQueue(&canmsg);
      break;

    case OPC_RQNPN:
      // Request to read a parameter
      if (thisNN(p_canmsg) == 1) {
        doRqnpn((unsigned int) p_canmsg->d[2]);
      }
      break;

    case OPC_SNN:
    {
      if( Wait4NN ) {
        unsigned char nnH = p_canmsg->d[0];
        unsigned char nnL = p_canmsg->d[1];
        NN_temp = nnH * 256 + nnL;
        eeWrite(EE_NN, nnH);
        eeWrite(EE_NN+1, nnL);
        Wait4NN = 0;
      }
      break;
    }


    case OPC_RQNP:
      if( Wait4NN ) {
        canmsg.opc = OPC_PARAMS;
        canmsg.d[0] = params[0];
        canmsg.d[1] = params[1];
        canmsg.d[2] = params[2];
        canmsg.d[3] = params[3];
        canmsg.d[4] = params[4];
        canmsg.d[5] = params[5];
        canmsg.d[6] = params[6];
        canmsg.len = 7;
        ethQueue(&canmsg);
        txed = 1;
      }
      break;

    case OPC_RTOF:
      break;

    case OPC_NVRD:
      if( thisNN(p_canmsg) ) {
        byte nvnr = p_canmsg->d[2];
        if( nvnr == 1 ) {
          canmsg.opc = OPC_NVANS;
          canmsg.d[0] = (NN_temp / 256) & 0xFF;
          canmsg.d[1] = (NN_temp % 256) & 0xFF;
          canmsg.d[2] = nvnr;
          canmsg.d[3] = NV1;
          canmsg.len = 4;
          ethQueue(&canmsg);
          txed = 1;
        }
        else if( nvnr == 2 ) {
          canmsg.opc = OPC_NVANS;
          canmsg.d[0] = (NN_temp / 256) & 0xFF;
          canmsg.d[1] = (NN_temp % 256) & 0xFF;
          canmsg.d[2] = nvnr;
          canmsg.d[3] = CANID;
          canmsg.len = 4;
          ethQueue(&canmsg);
          txed = 1;
        }
        else if( nvnr > 2 && nvnr < 7) {
          canmsg.opc = OPC_NVANS;
          canmsg.d[0] = (NN_temp / 256) & 0xFF;
          canmsg.d[1] = (NN_temp % 256) & 0xFF;
          canmsg.d[2] = nvnr;
          canmsg.d[3] = eeRead(EE_IPADDR+nvnr-3);
          canmsg.len = 4;
          ethQueue(&canmsg);
          txed = 1;
        }
        else if( nvnr > 6 && nvnr < 11) {
          canmsg.opc = OPC_NVANS;
          canmsg.d[0] = (NN_temp / 256) & 0xFF;
          canmsg.d[1] = (NN_temp % 256) & 0xFF;
          canmsg.d[2] = nvnr;
          canmsg.d[3] = eeRead(EE_NETMASK+nvnr-7);
          canmsg.len = 4;
          ethQueue(&canmsg);
          txed = 1;
        }
        else if( nvnr > 10 && nvnr < 17) {
          canmsg.opc = OPC_NVANS;
          canmsg.d[0] = (NN_temp / 256) & 0xFF;
          canmsg.d[1] = (NN_temp % 256) & 0xFF;
          canmsg.d[2] = nvnr;
          canmsg.d[3] = eeRead(EE_MACADDR+nvnr-11);
          canmsg.len = 4;
          ethQueue(&canmsg);
          txed = 1;
        }
      }
      break;

    case OPC_NVSET:
      if( thisNN(p_canmsg) ) {
        byte nvnr = p_canmsg->d[2];
        if( nvnr == 1 ) {
          NV1 = p_canmsg->d[3];
          eeWrite(EE_NV, NV1);
        }
        else if( nvnr == 2 ) {
          CANID = p_canmsg->d[3];
          eeWrite(EE_CANID, CANID);
        }
        else if( nvnr > 2 && nvnr < 7) {
          eeWrite(EE_IPADDR+nvnr-3, p_canmsg->d[3]);
        }
        else if( nvnr > 6 && nvnr < 11) {
          eeWrite(EE_NETMASK+nvnr-7, p_canmsg->d[3]);
        }
        else if( nvnr > 10 && nvnr < 17) {
          eeWrite(EE_MACADDR+nvnr-11, p_canmsg->d[3]);
        }
      }
      break;

    default: break;
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
    ethQueue(&canmsg);
  }
}

int thisNN(CANMsg* p_canmsg) {
  unsigned short nn = (p_canmsg->d[0] << 8);
  nn += p_canmsg->d[1];
  if( nn == NN_temp ) {
    LED3 = LED_OFF; /* signal match */
    led3timer = 10;
    return 1;
  }
  else
    return 0;

}

