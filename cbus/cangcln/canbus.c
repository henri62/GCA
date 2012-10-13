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

#include <stddef.h>
#include <string.h>
#include "project.h"
#include "cangcln.h"
#include "io.h"
#include "canbus.h"

#define SW_FIFO 18

typedef struct {
  BYTE msg[14];
} CAN_PACKET;

#pragma udata VARS_FIFO_0

far CAN_PACKET Fifo0[SW_FIFO];
far BYTE Fifo0IdxW = 0;
far BYTE Fifo0IdxR = 0;

#pragma udata VARS_FIFO_1

far CAN_PACKET Fifo1[SW_FIFO];
far BYTE Fifo1IdxW = 0;
far BYTE Fifo1IdxR = 0;

#pragma udata

static BYTE* _PointBuffer(BYTE b);

void cbusSetup(void) {

  // Put module into Configuration mode.
  CANCON = 0b10000000;
  // Wait for config mode
  while (CANSTATbits.OPMODE2 == 0);

  /*
    In PICs ist 125Kbps mit folgender Einstellung bereitzustellen:

  16MHz:
  -------------
  BRGCON1: 0x03
  BRGCON2: 0xDE
  BRGCON3: 0x03
   *
   * 125KHz @ 16MHz - BRGCON1=0x7 BRGCON2=0xC9 BRGCON3=0x02
   * 125KHz @ 40MHz - BRGCON1=0xF BRGCON2=0xD1 BRGCON3=0x03
   */
  /*
   * Baud rate setting.
   * Sync segment is fixed at 1 Tq
   * Total bit time is Sync + prop + phase 1 + phase 2
   * or 16 * Tq in our case
   * So, for 125kbits/s, bit time = 8us, we need Tq = 500ns
   * Fosc is 32MHz (8MHz + PLL), Tosc is 31.25ns, so we need 1:16 prescaler
   */

  // prescaler Tq = 16/Fosc
  BRGCON1 = 0b00000111; // 8MHz resonator
  //BRGCON1 = 0b00000011; // 4MHz resonator
  // freely programmable, sample once, phase 1 = 4xTq, prop time = 7xTq
  BRGCON2 = 0b10011110;
  // Wake-up enabled, wake-up filter not used, phase 2 = 4xTq
  BRGCON3 = 0b00000011;
  // TX drives Vdd when recessive, CAN capture to CCP1 disabled
  CIOCON = 0b00100000;

  // Want ECAN mode 2 with FIFO
  ECANCON = 0b10010000; // FIFOWM = 0 (four entry left)
  // EWIN default
  BSEL0 = 0; // All buffers for receive
  // FIFO is 8 deep

  RXF0SIDL = 0b00000000; // set filter 0 to std_frame
  RXM0SIDL = 0b00001000; // enable std_frame in mask

  // set all remaining mask register 0 to
  // receive all valid messages
  RXM0SIDH = 0;
  RXM1SIDL = 0;
  RXM1SIDH = 0;

  RXM0EIDL = 0;
  RXM0EIDH = 0;
  RXM1EIDL = 0;
  RXM1EIDH = 0;

  RXFCON0 = 1; // Filter 0 enabled
  RXFCON1 = 0;

  RXFBCON0 = 0; //
  RXFBCON1 = 0;
  RXFBCON2 = 0;
  RXFBCON3 = 0;
  RXFBCON4 = 0;
  RXFBCON5 = 0;
  RXFBCON6 = 0;
  RXFBCON7 = 0;

  RXB0CON = 0b00000000; // receive valid messages
  RXB1CON = 0b00000000; // receive valid messages
  B0CON = 0b00000000; // receive valid messages
  B1CON = 0b00000000; // receive valid messages
  B2CON = 0b00000000; // receive valid messages
  B3CON = 0b00000000; // receive valid messages
  B4CON = 0b00000000; // receive valid messages
  B5CON = 0b00000000; // receive valid messages

  BIE0 = 0; // No Rx buffer interrupts
  TXBIE = 0; // No Tx buffer interrupts
  CANCON = 0;
  PIE3bits.FIFOWMIE = 1; // Fifo 4 left int
}

#define cTXbuf 3

BOOL canSend(CANMsg *msg) {

  BYTE i;
  BYTE* ptr;
  BYTE* tempPtr;
  BYTE * pb[cTXbuf];

  pb[0] = (BYTE*) & TXB2CON;
  pb[1] = (BYTE*) & TXB1CON;
  pb[2] = (BYTE*) & TXB0CON;

  for (i = 0; i < cTXbuf; i++) {
    ptr = pb[i];
    tempPtr = ptr;
    /*
     * Check to see if this buffer is empty
     */
    if (!(*ptr & 0x08)) {
      msg->b[con] = 0;
      memcpy(ptr, (void *) msg->b, msg->b[dlc] + 6);
      if (!(*tempPtr & 0x04)) {
        *tempPtr |= 0x08;
      }
      LED1_CBUSTX = PORT_ON;
      ledCBUSTXtimer = 20;
      return TRUE;
    }
  }
  return FALSE;
}

BOOL canbusSend(CANMsg *msg) {
  CANMsg canmsg;

  msg->b[sidh] = (CANID >> 3);
  msg->b[sidl] = (CANID << 5);

  while (!canSend(msg)) {
    if (COMSTATbits.TXWARN) {
      return FALSE;
    }
  }
  return TRUE;
}

BOOL canbusRecv(CANMsg *msg) {

  CANMsg *ptr;


  if (COMSTATbits.FIFOEMPTY) {

    LED2_CBUSRX = PORT_ON;
    ledCBUSRXtimer = 20;

    ptr = (CANMsg*) _PointBuffer(CANCON & 0x07);
    PIR3bits.RXBnIF = 0;
    if (COMSTATbits.RXBnOVFL) {
      COMSTATbits.RXBnOVFL = 0;
    }
    memcpy(msg->b, (void*) ptr, ptr->b[dlc] + 6);
    // Record and Clear any previous invalid message bit flag.
    if (PIR3bits.IRXIF) {
      PIR3bits.IRXIF = 0;
    }
    // Mark that this buffer is read and empty.
    *ptr->b &= 0x7f;

    return TRUE;
  }

  if (Fifo1IdxR != Fifo1IdxW) {
    memcpy(msg->b, &Fifo1[Fifo1IdxR++].msg, Fifo1[Fifo1IdxR].msg[dlc] + 6);
    if (Fifo1IdxR >= SW_FIFO) {
      Fifo1IdxR = 0;
    }
    return TRUE;
  }

  if (Fifo0IdxR != Fifo0IdxW) {
    memcpy(msg->b, &Fifo0[Fifo0IdxR++].msg, Fifo0[Fifo0IdxR].msg[dlc] + 6);
    if (Fifo0IdxR >= SW_FIFO) {
      Fifo0IdxR = 0;
    }
    return TRUE;
  }

  return FALSE;
}

void canbusFifo(void) {
  CANMsg *ptr;

  while (COMSTATbits.FIFOEMPTY) {

    LED2_CBUSRX = PORT_ON;
    ledCBUSRXtimer = 20;

    ptr = (CANMsg*) _PointBuffer(CANCON & 0x07);
    PIR3bits.RXBnIF = 0;
    if (COMSTATbits.RXBnOVFL) {
      COMSTATbits.RXBnOVFL = 0;
    }
    memcpy((void*) &Fifo0[Fifo0IdxW].msg, (void*) ptr, ptr->b[dlc] + 6);
    // Record and Clear any previous invalid message bit flag.
    if (PIR3bits.IRXIF) {
      PIR3bits.IRXIF = 0;
    }
    // Mark that this buffer is read and empty.
    *ptr->b &= 0x7f;

    Fifo0IdxW++;
    if (Fifo0IdxW >= SW_FIFO) {
      Fifo0IdxW = 0;
    }
    if (Fifo0IdxW == Fifo0IdxR) {
      if (Fifo0IdxW == 0) {
        Fifo0IdxW = SW_FIFO - 1;
      } else {
        Fifo0IdxW--;
      }
      memcpy((void*) &Fifo1[Fifo1IdxW].msg, (void*) ptr, ptr->b[dlc] + 6);
      Fifo1IdxW++;
      if (Fifo1IdxW >= SW_FIFO) {
        Fifo1IdxW = 0;
      }
      if (Fifo1IdxW == Fifo1IdxR) {
        if (Fifo1IdxW == 0) {
          Fifo1IdxW = SW_FIFO - 1;
        } else {
          Fifo1IdxW--;
        }
        break;
      }
    }
  }
}

static BYTE* _PointBuffer(BYTE b) {
  BYTE* pt;

  switch (b) {
    case 0:
      pt = (BYTE*) & RXB0CON;
      break;
    case 1:
      pt = (BYTE*) & RXB1CON;
      break;
    case 2:
      pt = (BYTE*) & B0CON;
      break;
    case 3:
      pt = (BYTE*) & B1CON;
      break;
    case 4:
      pt = (BYTE*) & B2CON;
      break;
    case 5:
      pt = (BYTE*) & B3CON;
      break;
    case 6:
      pt = (BYTE*) & B4CON;
      break;
    default:
      pt = (BYTE*) & B5CON;
      break;
  }
  return (pt);
}
