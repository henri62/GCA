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
#include "cangcln.h"
#include "io.h"
#include "loconet.h"



#pragma udata access VARS_CMD
near unsigned char pnnCount;
near byte idx;
near byte var;
near ushort nn;
near ushort addr;
near byte nnH;
near byte nnL;

//#pragma code CMD

//
// parse_cmd()
//
// Decode the OPC and call the function to handle it.
//

unsigned char parseCmd(void) {
  unsigned char txed = 0;
  //mode_word.s_full = 0;

  if( !(NV1 & CFG_READONLY)) {
    send2LocoNet();
  }

  switch (rx_ptr->d0) {

    case OPC_ASRQ:
    {
      addr = rx_ptr->d3 * 256 + rx_ptr->d4;
      if( SOD == addr && doSOD == 0) {
        ioIdx = 0;
        doSOD = 1;
      }
      break;
    }

    case OPC_QNN:
      canmsg.opc  = OPC_PNN;
      canmsg.d[0] = (NN_temp / 256) & 0xFF;
      canmsg.d[1] = NN_temp & 0xFF;
      canmsg.d[2] = params[0];
      canmsg.d[3] = params[2];
      canmsg.d[4] = NV1;
      canmsg.len = 5;
      canQueue(&canmsg);
      //LED2 = 1;
      txed = 1;
      break;

    case OPC_RQNPN:
      // Request to read a parameter
      if (thisNN() == 1) {
        doRqnpn((unsigned int) rx_ptr->d3);
      }
      break;

    case OPC_SNN:
    {
      if( Wait4NN ) {
        nnH = rx_ptr->d1;
        nnL = rx_ptr->d2;
        NN_temp = nnH * 256 + nnL;
        eeWriteShort(EE_NN, NN_temp);
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
        canQueue(&canmsg);
        txed = 1;
      }
      break;

    case OPC_NVRD:
      if( thisNN() ) {
        byte nvnr = rx_ptr->d3;
        if( nvnr == 1 ) {
          canmsg.opc = OPC_NVANS;
          canmsg.d[0] = (NN_temp / 256) & 0xFF;
          canmsg.d[1] = NN_temp & 0xFF;
          canmsg.d[2] = nvnr;
          canmsg.d[3] = NV1;
          canmsg.len = 4;
          canQueue(&canmsg);
          txed = 1;
        }
        else if( nvnr == 2 ) {
          canmsg.opc = OPC_NVANS;
          canmsg.d[0] = (NN_temp / 256) & 0xFF;
          canmsg.d[1] = NN_temp & 0xFF;
          canmsg.d[2] = nvnr;
          canmsg.d[3] = CANID;
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
        else if( nvnr == 2 ) {
          CANID = rx_ptr->d4;
          eeWrite(EE_CANID, CANID);
        }
      }
      break;


    case OPC_NNLRN:
      if( thisNN() ) {
        isLearning = TRUE;
      }
      break;

    case OPC_NNULN:
      if( thisNN() ) {
        isLearning = FALSE;
        LED6_FLIM = PORT_OFF;
      }
      break;

    case OPC_EVLRN:
      if( isLearning ) {
        nn = rx_ptr->d1 * 256 + rx_ptr->d2;
        addr  = rx_ptr->d3 * 256 + rx_ptr->d4;
        idx = rx_ptr->d5;
        var = rx_ptr->d6;

        if( idx == 0 ) {
          SOD = addr;
          eeWriteShort(EE_SOD, addr);
        }
        else if( idx == 1 ) {
          SWStart = addr;
          eeWriteShort(EE_SWSTART, addr);
        }
        else if( idx == 2 ) {
          SWEnd = addr;
          eeWriteShort(EE_SWEND, addr);
        }
        else if( idx == 3 ) {
          FBStart = addr;
          eeWriteShort(EE_FBSTART, addr);
        }
        else if( idx == 4 ) {
          FBEnd = addr;
          eeWriteShort(EE_FBEND, addr);
        }
      }
      break;

    case OPC_NERD:
      if( thisNN() ) {
        doEV = 1;
        evIdx = 0;
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
    canmsg.d[1] = NN_temp & 0xFF;
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

unsigned char doEvent(int i ) {
  if( doEV ) {
    if( i == 0 ) {
      canmsg.opc = OPC_ENRSP;
      canmsg.d[0] = (NN_temp / 256) & 0xFF;
      canmsg.d[1] = NN_temp & 0xFF;
      canmsg.d[2] = 0;
      canmsg.d[3] = 0;
      canmsg.d[4] = SOD / 256;
      canmsg.d[5] = SOD % 256;
      canmsg.d[6] = i;
      canmsg.len = 7;
      return canQueue(&canmsg);
    }
    if( i == 1 ) {
      canmsg.opc = OPC_ENRSP;
      canmsg.d[0] = (NN_temp / 256) & 0xFF;
      canmsg.d[1] = NN_temp & 0xFF;
      canmsg.d[2] = 0;
      canmsg.d[3] = 0;
      canmsg.d[4] = SWStart / 256;
      canmsg.d[5] = SWStart % 256;
      canmsg.d[6] = i;
      canmsg.len = 7;
      return canQueue(&canmsg);
    }
    if( i == 2 ) {
      canmsg.opc = OPC_ENRSP;
      canmsg.d[0] = (NN_temp / 256) & 0xFF;
      canmsg.d[1] = NN_temp & 0xFF;
      canmsg.d[2] = 0;
      canmsg.d[3] = 0;
      canmsg.d[4] = SWEnd / 256;
      canmsg.d[5] = SWEnd % 256;
      canmsg.d[6] = i;
      canmsg.len = 7;
      return canQueue(&canmsg);
    }
    if( i == 3 ) {
      canmsg.opc = OPC_ENRSP;
      canmsg.d[0] = (NN_temp / 256) & 0xFF;
      canmsg.d[1] = NN_temp & 0xFF;
      canmsg.d[2] = 0;
      canmsg.d[3] = 0;
      canmsg.d[4] = FBStart / 256;
      canmsg.d[5] = FBStart % 256;
      canmsg.d[6] = i;
      canmsg.len = 7;
      return canQueue(&canmsg);
    }
    if( i == 4 ) {
      canmsg.opc = OPC_ENRSP;
      canmsg.d[0] = (NN_temp / 256) & 0xFF;
      canmsg.d[1] = NN_temp & 0xFF;
      canmsg.d[2] = 0;
      canmsg.d[3] = 0;
      canmsg.d[4] = FBEnd / 256;
      canmsg.d[5] = FBEnd % 256;
      canmsg.d[6] = i;
      canmsg.len = 7;
      return canQueue(&canmsg);
    }
  }
  return 1;
}
