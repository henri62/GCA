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
#include "cbusdefs.h"
#include "rocrail.h"
#include "utils.h"
#include "commands.h"
#include "cangc2.h"
#include "io.h"

#pragma romdata BOOTFLAG
rom unsigned char bootflag = 0;

#pragma udata access VARS

ram unsigned char pnnCount = 0;


#pragma code APP

//
// parse_cmd()
//
// Decode the OPC and call the function to handle it.
//

unsigned char parseCmd(CANMsg *cmsg) {
    unsigned char txed = 0;
    //mode_word.s_full = 0;
    switch (cmsg->b[d0]) {

        case OPC_ASRQ:
        {
            int addr = cmsg->b[d3] * 256 + cmsg->b[d4];
            if (SOD == addr && doSOD == 0) {
                ioIdx = 0;
                doSOD = 1;
            }
            break;
        }

        case OPC_ACON:
        case OPC_ASON:
        {
            ushort nn = cmsg->b[d1] * 256 + cmsg->b[d2];
            ushort addr = cmsg->b[d3] * 256 + cmsg->b[d4];
            setOutput(nn, addr, 1);
            break;
        }

        case OPC_ACOF:
        case OPC_ASOF:
        {
            ushort nn = cmsg->b[d1] * 256 + cmsg->b[d2];
            ushort addr = cmsg->b[d3] * 256 + cmsg->b[d4];
            setOutput(nn, addr, 0);
            break;
        }

        case OPC_RQNPN:
            // Request to read a parameter
            if (thisNN(cmsg) == 1) {
                doRqnpn((unsigned int) cmsg->b[d3]);
            }
            break;

        case OPC_SNN:
        {
            if (Wait4NN) {
                unsigned char nnH = cmsg->b[d1];
                unsigned char nnL = cmsg->b[d2];
                NN_temp = nnH * 256 + nnL;
                eeWrite(EE_NN, nnH);
                eeWrite(EE_NN + 1, nnL);
                Wait4NN = 0;
                LED2 = 0;
                canmsg.b[d0] = OPC_NNACK;
                canmsg.b[d1] = nnH / 256;
                canmsg.b[d2] = nnL % 256;
                canmsg.b[dlc] = 3;
                canbusSend(&canmsg);
            }
            break;
        }

        case OPC_RQNP:
            if (Wait4NN) {
                CANMsg canmsg;
                canmsg.b[d0] = OPC_PARAMS;
                canmsg.b[d1] = params[0];
                canmsg.b[d2] = params[1];
                canmsg.b[d3] = params[2];
                canmsg.b[d4] = params[3];
                canmsg.b[d5] = params[4];
                canmsg.b[d6] = params[5];
                canmsg.b[d7] = params[6];
                canmsg.b[dlc] = 8;
                canbusSend(&canmsg);
                txed = 1;
            }
            break;

        case OPC_BOOT:
            // Enter bootloader mode if NN matches
            if (thisNN(cmsg) == 1) {
                eeWrite((unsigned char) (&bootflag), 0xFF);
                Reset();
            }
            break;

        case OPC_RTOF:
            if (NV1 & CFG_SAVEOUTPUT) {
                saveOutputStates();
            }
            break;

        case OPC_NNCLR:
            if (thisNN(cmsg) && isLearning) {
                setupIO(TRUE);
            }
            break;

        case OPC_QNN:
        {
            CANMsg canmsg;
            canmsg.b[d0] = OPC_PNN;
            canmsg.b[d1] = (NN_temp / 256) & 0xFF;
            canmsg.b[d2] = (NN_temp % 256) & 0xFF;
            canmsg.b[d3] = params[0];
            canmsg.b[d4] = params[2];
            canmsg.b[d5] = NV1;
            canmsg.b[dlc] = 6;
            canbusSend(&canmsg);
        }
            //LED2 = 1;
            txed = 1;
            break;

        case OPC_RTON:
            pnnCount = 0;
            /*
            setOutput(0,  9, (pnnCount & 0x01) ? 1:0);
            setOutput(0, 10, (pnnCount & 0x02) ? 1:0);
            setOutput(0, 11, (pnnCount & 0x04) ? 1:0);
            setOutput(0, 12, (pnnCount & 0x08) ? 1:0);
            setOutput(0, 13, (pnnCount & 0x10) ? 1:0);
            setOutput(0, 14, (pnnCount & 0x20) ? 1:0);
            setOutput(0, 15, (pnnCount & 0x40) ? 1:0);
            setOutput(0, 16, (pnnCount & 0x80) ? 1:0);
             */
            break;

        case OPC_PNN:
            pnnCount++;
            /*
            setOutput(0,  9, (pnnCount & 0x01) ? 1:0);
            setOutput(0, 10, (pnnCount & 0x02) ? 1:0);
            setOutput(0, 11, (pnnCount & 0x04) ? 1:0);
            setOutput(0, 12, (pnnCount & 0x08) ? 1:0);
            setOutput(0, 13, (pnnCount & 0x10) ? 1:0);
            setOutput(0, 14, (pnnCount & 0x20) ? 1:0);
            setOutput(0, 15, (pnnCount & 0x40) ? 1:0);
            setOutput(0, 16, (pnnCount & 0x80) ? 1:0);
             */
            break;

        case OPC_NVRD:
            if (thisNN(cmsg)) {
                CANMsg canmsg;
                byte nvnr = cmsg->b[d3];
                if (nvnr == 1) {
                    canmsg.b[d0] = OPC_NVANS;
                    canmsg.b[d1] = (NN_temp / 256) & 0xFF;
                    canmsg.b[d2] = (NN_temp % 256) & 0xFF;
                    canmsg.b[d3] = nvnr;
                    canmsg.b[d4] = NV1;
                    canmsg.b[dlc] = 5;
                    canbusSend(&canmsg);
                    txed = 1;
                } else if (nvnr < 18) {
                    canmsg.b[d0] = OPC_NVANS;
                    canmsg.b[d1] = (NN_temp / 256) & 0xFF;
                    canmsg.b[d2] = (NN_temp % 256) & 0xFF;
                    canmsg.b[d3] = nvnr;
                    canmsg.b[d4] = Ports[nvnr - 2].cfg;
                    canmsg.b[dlc] = 5;
                    canbusSend(&canmsg);
                    txed = 1;
                } else if (nvnr == 18) {
                    canmsg.b[d0] = OPC_NVANS;
                    canmsg.b[d1] = (NN_temp / 256) & 0xFF;
                    canmsg.b[d2] = (NN_temp % 256) & 0xFF;
                    canmsg.b[d3] = nvnr;
                    canmsg.b[d4] = getPortStates(0); // port status 1-8
                    canmsg.b[dlc] = 5;
                    canbusSend(&canmsg);
                    txed = 1;
                } else if (nvnr == 19) {
                    canmsg.b[d0] = OPC_NVANS;
                    canmsg.b[d1] = (NN_temp / 256) & 0xFF;
                    canmsg.b[d2] = (NN_temp % 256) & 0xFF;
                    canmsg.b[d3] = nvnr;
                    canmsg.b[d4] = getPortStates(1); // port status 9-16
                    canmsg.b[dlc] = 5;
                    canbusSend(&canmsg);
                    txed = 1;
                } else if (nvnr == 20) {
                    canmsg.b[d0] = OPC_NVANS;
                    canmsg.b[d1] = (NN_temp / 256) & 0xFF;
                    canmsg.b[d2] = (NN_temp % 256) & 0xFF;
                    canmsg.b[d3] = nvnr;
                    canmsg.b[d4] = CANID;
                    canmsg.b[dlc] = 5;
                    canbusSend(&canmsg);
                    txed = 1;
                }
            }
            break;

        case OPC_NVSET:
            if (thisNN(cmsg)) {
                byte nvnr = cmsg->b[d3];
                if (nvnr == 1) {
                    NV1 = cmsg->b[d4];
                    eeWrite(EE_NV, NV1);
                } else if (nvnr < 18) {
                    Ports[nvnr - 2].cfg = cmsg->b[d4];
                    eeWrite(EE_PORTCFG + (nvnr - 2), Ports[nvnr - 2].cfg);
                    configPort(nvnr - 2);
                } else if (nvnr == 20) {
                    CANID = cmsg->b[d4];
                    eeWrite(EE_CANID, CANID);
                }

            }
            break;

        case OPC_NNLRN:
            if (thisNN(cmsg)) {
                isLearning = TRUE;
            }
            break;

        case OPC_NNULN:
            if (thisNN(cmsg)) {
                isLearning = FALSE;
                LED2 = PORT_OFF;
            }
            break;

        case OPC_EVLRN:
            if (isLearning) {
                ushort evtnn = cmsg->b[d1] * 256 + cmsg->b[d2];
                ushort addr = cmsg->b[d3] * 256 + cmsg->b[d4];
                byte idx = cmsg->b[d5];
                byte val = cmsg->b[d6];
                if (idx < 16) {
                    Ports[idx].evtnn = evtnn;
                    Ports[idx].addr = addr;
                    eeWriteShort(EE_PORTNN + (2 * idx), evtnn);
                    eeWriteShort(EE_PORTADDR + (2 * idx), addr);
                }
                if (idx == 16) {
                    SOD = addr;
                    eeWrite(EE_SOD, addr / 256);
                    eeWrite(EE_SOD + 1, addr % 256);
                }
            }
            break;

        case OPC_NERD:
            if (thisNN(cmsg)) {
                doEV = 1;
                evIdx = 0;
                // start of day event
                {
                    CANMsg canmsg;
                    canmsg.b[d0] = OPC_ENRSP;
                    canmsg.b[d1] = (NN_temp / 256) & 0xFF;
                    canmsg.b[d2] = (NN_temp % 256) & 0xFF;
                    canmsg.b[d3] = 0;
                    canmsg.b[d4] = 0;
                    canmsg.b[d5] = SOD / 256;
                    canmsg.b[d6] = SOD % 256;
                    canmsg.b[d7] = 16;
                    canmsg.b[dlc] = 8;
                    canbusSend(&canmsg);
                }
                txed = 1;
            }
            break;


        default: break;
    }
    return txed;
}

void doRqnpn(unsigned int idx) {
    if (idx < 8) {
        CANMsg canmsg;
        canmsg.b[d0] = OPC_PARAN;
        canmsg.b[d1] = (NN_temp / 256) & 0xFF;
        canmsg.b[d2] = (NN_temp % 256) & 0xFF;
        canmsg.b[d3] = idx;
        canmsg.b[d4] = params[idx - 1];
        canmsg.b[dlc] = 5;
        canbusSend(&canmsg);
    }
}

int thisNN(CANMsg *cmsg) {
    if ((((unsigned short) (cmsg->b[d1]) << 8) + cmsg->b[d2]) == NN_temp)
        return 1;
    else
        return 0;

}

unsigned char doPortEvent(int i) {
    if (doEV) {
        CANMsg canmsg;
        canmsg.b[d0] = OPC_ENRSP;
        canmsg.b[d1] = (NN_temp / 256) & 0xFF;
        canmsg.b[d2] = (NN_temp % 256) & 0xFF;
        canmsg.b[d3] = Ports[i].evtnn / 256;
        canmsg.b[d4] = Ports[i].evtnn % 256;
        canmsg.b[d5] = Ports[i].addr / 256;
        canmsg.b[d6] = Ports[i].addr % 256;
        canmsg.b[d7] = i;
        canmsg.b[dlc] = 8;
        return canbusSend(&canmsg);
    }
    return 1;
}
