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
#include <p18cxxx.h>
#include <stdio.h>
#include "project.h"
#include "canbus.h"
#include "utils.h"
#include "cbusdefs.h"
#include "cangc2.h"
#include "io.h"

void setupIO(byte clr) {
    int idx = 0;
    byte firstTime = eeRead(EE_FIRSTTIME);

    // all digital I/O
    ADCON0 = 0x00;
    ADCON1 = 0x0F;

    TRISBbits.TRISB6 = 0; /* LED1 */
    TRISBbits.TRISB7 = 0; /* LED2 */
    TRISBbits.TRISB5 = 0; /* LED3 */
    TRISAbits.TRISA2 = 1; /* Push button */

    LED1 = 0;
    LED2 = 0;
    LED3 = 0;

    if (firstTime == 0xFF) {
        eeWrite(EE_FIRSTTIME, 0);
        clr = TRUE;
    }


    // following presets are written to eeprom if the flim switch is preshed at boot

    // preset port 1-8 as output
    for (idx = 0; idx < 8; idx++) {
        Ports[idx].cfg = PORTCFG_OUT;
        Ports[idx].status = 0;
        Ports[idx].timedoff = 0;
        Ports[idx].timer = 0;
        Ports[idx].evtnn = 0;
        Ports[idx].addr = idx + 1;
        if (clr) {
            eeWrite(EE_PORTCFG + idx, Ports[idx].cfg);
            eeWriteShort(EE_PORTNN + (2 * idx), Ports[idx].evtnn);
            eeWriteShort(EE_PORTADDR + (2 * idx), Ports[idx].addr);
        }
    }

    // preset port 9-16 as input with off delay
    for (idx = 8; idx < 16; idx++) {
        Ports[idx].cfg = PORTCFG_IN | PORTCFG_OFFDELAY;
        Ports[idx].status = 0;
        Ports[idx].timedoff = 0;
        Ports[idx].timer = 0;
        Ports[idx].evtnn = 0;
        Ports[idx].addr = idx + 1;
        if (clr) {
            eeWrite(EE_PORTCFG + idx, Ports[idx].cfg);
            eeWriteShort(EE_PORTNN + (2 * idx), Ports[idx].evtnn);
            eeWriteShort(EE_PORTADDR + (2 * idx), Ports[idx].addr);
        }
    }

    if (clr) {
        eeWrite(EE_PORTSTAT + 0, 0);
        eeWrite(EE_PORTSTAT + 1, 0);
    }

    for (idx = 0; idx < 16; idx++) {
        Ports[idx].cfg = eeRead(EE_PORTCFG + idx);
        if ((Ports[idx].cfg & PORTCFG_IO) == PORTCFG_OUT)
            Ports[idx].status = eeRead(EE_PORTSTAT + idx);
        Ports[idx].timedoff = 0;
        Ports[idx].timer = 0;
        Ports[idx].evtnn = eeReadShort(EE_PORTNN + (2 * idx));
        Ports[idx].addr = eeReadShort(EE_PORTADDR + (2 * idx));
    }

    for (idx = 0; idx < 16; idx++)
        configPort(idx);

}

void configPort(int idx) {
    // setup port 1-8
    switch (idx) {
        case 0: TRISAbits.TRISA0 = (Ports[idx].cfg & PORTCFG_IO) ? PORTCFG_IN:PORTCFG_OUT;
            break;
        case 1: TRISAbits.TRISA1 = (Ports[idx].cfg & PORTCFG_IO) ? PORTCFG_IN:PORTCFG_OUT;
            break;
        case 2: TRISAbits.TRISA3 = (Ports[idx].cfg & PORTCFG_IO) ? PORTCFG_IN:PORTCFG_OUT;
            break;
        case 3: TRISAbits.TRISA4 = (Ports[idx].cfg & PORTCFG_IO) ? PORTCFG_IN:PORTCFG_OUT;
            break;
        case 4: TRISAbits.TRISA5 = (Ports[idx].cfg & PORTCFG_IO) ? PORTCFG_IN:PORTCFG_OUT;
            break;
        case 5: TRISBbits.TRISB0 = (Ports[idx].cfg & PORTCFG_IO) ? PORTCFG_IN:PORTCFG_OUT;
            break;
        case 6: TRISBbits.TRISB4 = (Ports[idx].cfg & PORTCFG_IO) ? PORTCFG_IN:PORTCFG_OUT;
            break;
        case 7: TRISBbits.TRISB1 = (Ports[idx].cfg & PORTCFG_IO) ? PORTCFG_IN:PORTCFG_OUT;
            break;

            // setup port 9-16
        case 8: TRISCbits.TRISC0 = (Ports[idx].cfg & PORTCFG_IO) ? PORTCFG_IN:PORTCFG_OUT;
            break;
        case 9: TRISCbits.TRISC1 = (Ports[idx].cfg & PORTCFG_IO) ? PORTCFG_IN:PORTCFG_OUT;
            break;
        case 10: TRISCbits.TRISC2 = (Ports[idx].cfg & PORTCFG_IO) ? PORTCFG_IN:PORTCFG_OUT;
            break;
        case 11: TRISCbits.TRISC3 = (Ports[idx].cfg & PORTCFG_IO) ? PORTCFG_IN:PORTCFG_OUT;
            break;
        case 12: TRISCbits.TRISC7 = (Ports[idx].cfg & PORTCFG_IO) ? PORTCFG_IN:PORTCFG_OUT;
            break;
        case 13: TRISCbits.TRISC6 = (Ports[idx].cfg & PORTCFG_IO) ? PORTCFG_IN:PORTCFG_OUT;
            break;
        case 14: TRISCbits.TRISC5 = (Ports[idx].cfg & PORTCFG_IO) ? PORTCFG_IN:PORTCFG_OUT;
            break;
        case 15: TRISCbits.TRISC4 = (Ports[idx].cfg & PORTCFG_IO) ? PORTCFG_IN:PORTCFG_OUT;
            break;
    }
}

void writeOutput(int idx, unsigned char val) {
    switch (idx) {
        case 0: PORT1 = val;
            break;
        case 1: PORT2 = val;
            break;
        case 2: PORT3 = val;
            break;
        case 3: PORT4 = val;
            break;
        case 4: PORT5 = val;
            break;
        case 5: PORT6 = val;
            break;
        case 6: PORT7 = val;
            break;
        case 7: PORT8 = val;
            break;
        case 8: PORT9 = val;
            break;
        case 9: PORT10 = val;
            break;
        case 10: PORT11 = val;
            break;
        case 11: PORT12 = val;
            break;
        case 12: PORT13 = val;
            break;
        case 13: PORT14 = val;
            break;
        case 14: PORT15 = val;
            break;
        case 15: PORT16 = val;
            break;
    }
}

unsigned char readInput(int idx) {
    unsigned char val = 0;
    switch (idx) {
        case 0: val = PORT1;
            break;
        case 1: val = PORT2;
            break;
        case 2: val = PORT3;
            break;
        case 3: val = PORT4;
            break;
        case 4: val = PORT5;
            break;
        case 5: val = PORT6;
            break;
        case 6: val = PORT7;
            break;
        case 7: val = PORT8;
            break;
        case 8: val = PORT9;
            break;
        case 9: val = PORT10;
            break;
        case 10: val = PORT11;
            break;
        case 11: val = PORT12;
            break;
        case 12: val = PORT13;
            break;
        case 13: val = PORT14;
            break;
        case 14: val = PORT15;
            break;
        case 15: val = PORT16;
            break;
    }
    return !val;
}

void doLEDTimers(void) {
    if (led1timer > 0) {
        led1timer--;
        if (led1timer == 0)
            LED1 = 0;
    }
}

void doIOTimers(void) {
    int i = 0;
    for (i = 0; i < 16; i++) {
        if (Ports[i].timedoff) {
            if (Ports[i].timer > 0) {
                Ports[i].timer--;
            }
        }
    }
}

void doTimedOff(int i) {
    if (Ports[i].timedoff) {
        if (Ports[i].timer == 0) {
            Ports[i].timedoff = 0;
            if (Ports[i].cfg & PORTCFG_IN) {
                CANMsg canmsg;
                // Send an OPC.
                if (NV1 & CFG_SHORTEVENTS) {
                  canmsg.b[d0] = Ports[i].cfg & PORTCFG_INV ? OPC_ASON : OPC_ASOF;
                } else {
                  canmsg.b[d0] = Ports[i].cfg & PORTCFG_INV ? OPC_ARON : OPC_AROF;
                }
                canmsg.b[d1] = (NN_temp / 256) & 0xFF;
                canmsg.b[d2] = (NN_temp % 256) & 0xFF;
                canmsg.b[d3] = (Ports[i].addr / 256) & 0xFF;
                canmsg.b[d4] = (Ports[i].addr % 256) & 0xFF;
                canmsg.b[dlc] = 5;
                canbusSend(&canmsg);
                // check if an output is consumer of this event
                setOutput(NN_temp, Ports[i].addr, Ports[i].cfg & PORTCFG_INV ? 1 : 0);
            } else {
                writeOutput(i, 0);
            }
            //LED2 = 0;
        }
    }
}

unsigned char checkFlimSwitch(void) {
    unsigned char val = SW;
    return !val;
}

unsigned char checkInput(unsigned char idx, unsigned char sod) {
    unsigned char ok = 1;
    if ((Ports[idx].cfg & PORTCFG_IO) == PORTCFG_IN) {
        unsigned char val = readInput(idx);
        if (sod || val != Ports[idx].status) {
            Ports[idx].status = val;
            if (!sod && (Ports[idx].cfg & 0x02) && val == 0) {
                Ports[idx].timer = 40; // 2 seconds
                Ports[idx].timedoff = TRUE;
            } else if (!sod && (Ports[idx].cfg & 0x02) && Ports[idx].timedoff) {
                Ports[idx].timer = 40; // reload timer
                Ports[idx].timedoff = FALSE;
            } else {
                CANMsg canmsg;
                if ((Ports[idx].cfg & PORTCFG_INV) == PORTCFG_INV) {
                    val = !val;
                }
                // Send an OPC.
                if (sod) {
                    canmsg.b[d0] = 0;
                    if (NV1 & CFG_SOD_REPORTALL) {
                        canmsg.b[d0] = val ? OPC_ARON : OPC_AROF;
                    } else if (val) {
                        canmsg.b[d0] = OPC_ARON;
                    }
                } else if (NV1 & CFG_SHORTEVENTS) {
                    canmsg.b[d0] = val ? OPC_ASON : OPC_ASOF;
                } else {
                    canmsg.b[d0] = val ? OPC_ARON : OPC_AROF;
                }
                if (canmsg.b[d0] > 0) {
                    canmsg.b[d1] = (NN_temp / 256) & 0xFF;
                    canmsg.b[d2] = (NN_temp % 256) & 0xFF;
                    canmsg.b[d3] = (Ports[idx].addr / 256) & 0xFF;
                    canmsg.b[d4] = (Ports[idx].addr % 256) & 0xFF;
                    canmsg.b[dlc] = 5; // data bytes
                    ok = canbusSend(&canmsg);
                    if (!ok) {
                        Ports[idx].status = !Ports[idx].status;
                    }
                }
                //LED2 = val;
                //delay();
                // check if an output is consumer of this event
                setOutput(NN_temp, Ports[idx].addr, val);
            }
        }
    }

    return ok;
}

void resetOutputs(void) {
    int idx = 0;
    for (idx = 0; idx < 16; idx++) {
        if ((Ports[idx].cfg & 0x01) == 0) {
            writeOutput(idx, 0);
        }
    }
}

void saveOutputStates(void) {
    int idx = 0;
    byte o1 = 0;
    byte o2 = 0;

    for (idx = 0; idx < 8; idx++) {
        if ((Ports[idx].cfg & 0x01) == 0) {
            byte o = !readInput(idx);
            o1 += o << idx;
        }
    }
    eeWrite(EE_PORTSTAT + 0, o1);

    for (idx = 8; idx < 16; idx++) {
        if ((Ports[idx].cfg & 0x01) == 0) {
            byte o = !readInput(idx);
            o2 += o << idx;
        }
    }
    eeWrite(EE_PORTSTAT + 1, o2);


}

void restoreOutputStates(void) {
    int idx = 0;
    byte o1 = eeRead(EE_PORTSTAT + 0);
    byte o2 = eeRead(EE_PORTSTAT + 1);

    for (idx = 0; idx < 8; idx++) {
        if ((Ports[idx].cfg & 0x01) == 0) {
            byte o = (o1 >> idx) & 0x01;
            writeOutput(idx, o);
        }
    }

    for (idx = 8; idx < 16; idx++) {
        if ((Ports[idx].cfg & 0x01) == 0) {
            byte o = (o2 >> idx) & 0x01;
            writeOutput(idx, o);
        }
    }

    eeWrite(EE_PORTSTAT + 0, 0);
    eeWrite(EE_PORTSTAT + 1, 0);

}



static unsigned char __LED2 = 0;

void doLEDs(void) {
    if (Wait4NN || isLearning) {
        LED2 = __LED2;
        __LED2 ^= 1;
    }
}

byte getPortStates(int group) {
    int idx = 0;
    byte o1 = 0;
    byte o2 = 0;

    if (group == 0) {
        for (idx = 0; idx < 8; idx++) {
            byte o = 0;
            if ((Ports[idx].cfg & 0x01) == 0)
                o = !readInput(idx);
            else
                o = readInput(idx);

            o1 += o << idx;
        }
        return o1;
    }

    if (group == 1) {
        for (idx = 8; idx < 16; idx++) {
            byte o = 0;
            if ((Ports[idx].cfg & 0x01) == 0)
                o = !readInput(idx);
            else
                o = readInput(idx);
            o2 += o << (idx - 8);
        }
        return o2;
    }

    return 0;
}

void setOutput(ushort nn, ushort addr, byte on) {
    int i = 0;
    for (i = 0; i < 16; i++) {
        if ((Ports[i].cfg & 0x01) == 0) {
            if (Ports[i].addr == addr) {
                byte act = FALSE;
                if (NV1 & CFG_SHORTEVENTS) {
                    act = TRUE;
                } else if (Ports[i].evtnn == nn) {
                    act = TRUE;
                }

                if (act) {
                    if ((Ports[i].cfg & PORTCFG_INV) == PORTCFG_INV)
                        on = !on;
                    writeOutput(i, on);
                }

                if (on && act && Ports[i].cfg & PORTCFG_PULSE) {
                    byte pulsetime = (NV1 & CFG_PULSETIME_MASK);
                    Ports[i].timedoff = TRUE;
                    if (pulsetime == CFG_PULSETIME_250)
                        Ports[i].timer = 5;
                    else if (pulsetime == CFG_PULSETIME_500)
                        Ports[i].timer = 10;
                    else if (pulsetime == CFG_PULSETIME_1000)
                        Ports[i].timer = 20;
                    else if (pulsetime == CFG_PULSETIME_2000)
                        Ports[i].timer = 40;
                    if (NV1 & CFG_PULSETIME_LONG)
                      Ports[i].timer = Ports[i].timer * 6;
                }
            }
        }
    }
}
