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
#include "cangc4.h"
#include "io.h"

#pragma romdata BOOTFLAG
rom unsigned char bootflag = 0;

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

unsigned char parseCmd(CANMsg *cmsg) {
    unsigned char txed = 0;
    //mode_word.s_full = 0;

    switch (cmsg->b[d0]) {

        case OPC_ASRQ:
        {
            addr = cmsg->b[d3] * 256 + cmsg->b[d4];
            if (SOD == addr && doSOD == 0) {
                ioIdx = 0;
                doSOD = 1;
            }
            break;
        }

        case OPC_ACON:
        case OPC_ASON:
        {
            nn = cmsg->b[d1] * 256 + cmsg->b[d2];
            addr = cmsg->b[d3] * 256 + cmsg->b[d4];
            setOutput(nn, addr, 1);
            break;
        }

        case OPC_ACOF:
        case OPC_ASOF:
        {
            nn = cmsg->b[d1] * 256 + cmsg->b[d2];
            addr = cmsg->b[d3] * 256 + cmsg->b[d4];
            setOutput(nn, addr, 0);
            break;
        }

        case OPC_BOOT:
            // Enter bootloader mode if NN matches
            if (thisNN(cmsg) == 1) {
                eeWrite((unsigned char) (&bootflag), 0xFF);
                Reset();
            }
            break;


        case OPC_QNN:
        {
            CANMsg canmsg;

            canmsg.b[d0] = OPC_PNN;
            canmsg.b[d1] = (NN_temp / 256) & 0xFF;
            canmsg.b[d2] = NN_temp & 0xFF;
            canmsg.b[d3] = params[0];
            canmsg.b[d4] = params[2];
            canmsg.b[d5] = NV1;
            canmsg.b[dlc] = 6;
            canbusSend(&canmsg);
            //LED2 = 1;
            txed = 1;
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
                nnH = cmsg->b[d1];
                nnL = cmsg->b[d2];
                NN_temp = nnH * 256 + nnL;
                eeWrite(EE_NN, nnH);
                eeWrite(EE_NN + 1, nnL);
                Wait4NN = 0;
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

        case OPC_RTOF:
            if (NV1 & CFG_SAVERFID) {
                byte i;
                for (i = 0; i < 8; i++) {
                    // save rfid for SoD
                    eeWrite(EE_SCANRFID + i * 5 + 0, RFID[i].data[0]);
                    eeWrite(EE_SCANRFID + i * 5 + 1, RFID[i].data[1]);
                    eeWrite(EE_SCANRFID + i * 5 + 2, RFID[i].data[2]);
                    eeWrite(EE_SCANRFID + i * 5 + 3, RFID[i].data[3]);
                    eeWrite(EE_SCANRFID + i * 5 + 4, RFID[i].data[4]);
                }
            }
            break;

        case OPC_NVRD:
            if (thisNN(cmsg)) {
                CANMsg canmsg;
                byte nvnr = cmsg->b[d3];
                if (nvnr == 1) {
                    canmsg.b[d0] = OPC_NVANS;
                    canmsg.b[d1] = (NN_temp / 256) & 0xFF;
                    canmsg.b[d2] = NN_temp & 0xFF;
                    canmsg.b[d3] = nvnr;
                    canmsg.b[d4] = NV1;
                    canmsg.b[dlc] = 5;
                    canbusSend(&canmsg);
                    txed = 1;
                } else if (nvnr == 2) {
                    canmsg.b[d0] = OPC_NVANS;
                    canmsg.b[d1] = (NN_temp / 256) & 0xFF;
                    canmsg.b[d2] = NN_temp & 0xFF;
                    canmsg.b[d3] = nvnr;
                    canmsg.b[d4] = CANID;
                    canmsg.b[dlc] = 5;
                    canbusSend(&canmsg);
                    txed = 1;
                } else if (nvnr > 2 && nvnr < 28) {
                    // 5 x 8 = 40bit Allowed RFID
                    idx = (nvnr - 3) / 5;
                    var = (nvnr - 3) % 5;
                    canmsg.b[d0] = OPC_NVANS;
                    canmsg.b[d1] = (NN_temp / 256) & 0xFF;
                    canmsg.b[d2] = NN_temp & 0xFF;
                    canmsg.b[d3] = nvnr;
                    canmsg.b[d4] = AllowedRFID[idx].data[var];
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
                } else if (nvnr == 2) {
                    CANID = cmsg->b[d4];
                    eeWrite(EE_CANID, CANID);
                } else if (nvnr > 2 && nvnr < 28) {
                    // 5 x 40bit Allowed RFID
                    byte idx = (nvnr - 3) / 5;
                    byte var = (nvnr - 3) % 5;
                    AllowedRFID[idx].data[var] = cmsg->b[d4];
                    eeWrite(EE_RFID + idx * 5 + (var), cmsg->b[d4]);
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
                nn = cmsg->b[d1] * 256 + cmsg->b[d2];
                addr = cmsg->b[d3] * 256 + cmsg->b[d4];
                idx = cmsg->b[d5];
                var = cmsg->b[d6];
                if (idx < 8) {
                    // RFID
                    RFID[idx].addr = addr;
                    eeWriteShort(EE_PORT_ADDR + 2 * idx, addr);
                } else if (idx > 7 && idx < 16) {
                    // Block
                    Sensor[idx - 8].addr = addr;
                    eeWriteShort(EE_PORT_ADDR + 2 * idx, addr);
                } else if (idx == 16) {
                    SOD = addr;
                    eeWrite(EE_SOD, addr / 256);
                    eeWrite(EE_SOD + 1, addr % 256);
                }
            }
            break;

        case OPC_NERD:
            if (thisNN(cmsg)) {
                CANMsg canmsg;
                doEV = 1;
                evIdx = 0;
                // start of day event
                canmsg.b[d0] = OPC_ENRSP;
                canmsg.b[d1] = (NN_temp / 256) & 0xFF;
                canmsg.b[d2] = NN_temp & 0xFF;
                canmsg.b[d3] = 0;
                canmsg.b[d4] = 0;
                canmsg.b[d5] = SOD / 256;
                canmsg.b[d6] = SOD % 256;
                canmsg.b[d7] = 16;
                canmsg.b[dlc] = 8;
                canbusSend(&canmsg);
                txed = 1;
            }
            break;



        default: break;
    }

    return txed;
}

void doRqnpn(unsigned int idx) {
    CANMsg canmsg;
    if (idx < 8) {
        canmsg.b[d0] = OPC_PARAN;
        canmsg.b[d1] = (NN_temp / 256) & 0xFF;
        canmsg.b[d2] = NN_temp & 0xFF;
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
    CANMsg canmsg;
    if (doEV) {
        if (i < 8) {
            canmsg.b[d0] = OPC_ENRSP;
            canmsg.b[d1] = (NN_temp / 256) & 0xFF;
            canmsg.b[d2] = NN_temp & 0xFF;
            canmsg.b[d3] = (NN_temp / 256) & 0xFF;
            canmsg.b[d4] = NN_temp & 0xFF;
            canmsg.b[d5] = RFID[i].addr / 256;
            canmsg.b[d6] = RFID[i].addr % 256;
            canmsg.b[d7] = i;
            canmsg.b[dlc] = 8;
            return canbusSend(&canmsg);
        }
        if (i > 7 && i < 16) {
            canmsg.b[d0] = OPC_ENRSP;
            canmsg.b[d1] = (NN_temp / 256) & 0xFF;
            canmsg.b[d2] = NN_temp & 0xFF;
            canmsg.b[d3] = (NN_temp / 256) & 0xFF;
            canmsg.b[d4] = NN_temp & 0xFF;
            canmsg.b[d5] = Sensor[i - 8].addr / 256;
            canmsg.b[d6] = Sensor[i - 8].addr % 256;
            canmsg.b[d7] = i;
            canmsg.b[dlc] = 8;
            return canbusSend(&canmsg);
        }
        if (i == 16) {
            canmsg.b[d0] = OPC_ENRSP;
            canmsg.b[d1] = (NN_temp / 256) & 0xFF;
            canmsg.b[d2] = NN_temp & 0xFF;
            canmsg.b[d3] = 0;
            canmsg.b[d4] = 0;
            canmsg.b[d5] = SOD / 256;
            canmsg.b[d6] = SOD % 256;
            canmsg.b[d7] = i;
            canmsg.b[dlc] = 8;
            return canbusSend(&canmsg);
        }
    }
    return 1;
}
