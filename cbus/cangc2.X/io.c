
#include <p18cxxx.h>
#include <stdio.h>

#include "cbus_common.h"
#include "cbusdefs.h"
#include "cangc2.h"
#include "can_send.h"
#include "io.h"


void setupIO(void) {
  int idx = 0;

  for( idx = 0; idx < 8; idx++ ) {
    Ports[idx].cfg = 0x00;
    Ports[idx].status = 0;
    Ports[idx].timedoff = 0;
    Ports[idx].timer = 0;
    Ports[idx].addr = idx + 1;
    ee_write(EE_PORTCFG + idx, Ports[idx].cfg);
  }

  for( idx = 8; idx < 16; idx++ ) {
    Ports[idx].cfg = 0x01 | 0x02;
    Ports[idx].status = 0;
    Ports[idx].timedoff = 0;
    Ports[idx].timer = 0;
    Ports[idx].addr = idx + 1;
    ee_write(EE_PORTCFG + idx, Ports[idx].cfg);
  }

  idx = 0;

  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  TRISCbits.TRISC0 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  TRISCbits.TRISC1 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  TRISCbits.TRISC2 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  TRISCbits.TRISC3 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  TRISCbits.TRISC7 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  TRISCbits.TRISC6 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  TRISCbits.TRISC5 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  TRISCbits.TRISC4 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;

  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  TRISAbits.TRISA0 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  TRISAbits.TRISA1 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  TRISAbits.TRISA3 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  TRISAbits.TRISA4 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  TRISAbits.TRISA5 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  TRISBbits.TRISB0 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  TRISBbits.TRISB4 = (Ports[idx].cfg & 0x01) ? 1:0;
  idx++;
  Ports[idx].cfg = ee_read(EE_PORTCFG + idx);
  TRISBbits.TRISB1 = (Ports[idx].cfg & 0x01) ? 1:0;


  TRISBbits.TRISB6 = 0; /* LED1 */
  TRISBbits.TRISB7 = 0; /* LED2 */
  TRISAbits.TRISA2 = 1; /* Push button */
  
}

void writeOutput(int idx, unsigned char val) {
  switch(idx) {
    case 0: PORT1 = val; break;
    case 1: PORT2 = val; break;
    case 2: PORT3 = val; break;
    case 3: PORT4 = val; break;
    case 4: PORT5 = val; break;
    case 5: PORT6 = val; break;
    case 6: PORT7 = val; break;
    case 7: PORT8 = val; break;
    case 8: PORT9 = val; break;
    case 9: PORT10 = val; break;
    case 10: PORT11 = val; break;
    case 11: PORT12 = val; break;
    case 12: PORT13 = val; break;
    case 13: PORT14 = val; break;
    case 14: PORT15 = val; break;
    case 15: PORT16 = val; break;
  }
}

unsigned char readInput(int idx) {
  unsigned char val = 0;
  switch(idx) {
    case 0: val = PORT1; break;
    case 1: val = PORT2; break;
    case 2: val = PORT3; break;
    case 3: val = PORT4; break;
    case 4: val = PORT5; break;
    case 5: val = PORT6; break;
    case 6: val = PORT7; break;
    case 7: val = PORT8; break;
    case 8: val = PORT9; break;
    case 9: val = PORT10; break;
    case 10: val = PORT11; break;
    case 11: val = PORT12; break;
    case 12: val = PORT13; break;
    case 13: val = PORT14; break;
    case 14: val = PORT15; break;
    case 15: val = PORT16; break;
  }
  return !val;
}


void doIOTimers(void) {
  int i = 0;
  for( i = 0; i < 16; i++ ) {
    if( Ports[i].timedoff ) {
      if( Ports[i].timer > 0 ) {
        Ports[i].timer--;
      }
    }
  }
}

void doTimedOff(void) {
  int i = 0;
  for( i = 0; i < 16; i++ ) {
    if( Ports[i].timedoff ) {
      if( Ports[i].timer == 0 ) {
        Ports[i].timedoff = 0;
        // Send an OPC.
        Tx1[d0] = OPC_ASOF;
        Tx1[d1] = 0;
        Tx1[d2] = 0;
        Tx1[d3] = Ports[i].addr / 256;
        Tx1[d4] = Ports[i].addr % 256;
        can_tx(5);
        LED2 = 0;
      }
    }
  }
}


void checkInputs(void) {
  int idx = 0;
  for( idx = 0; idx < 16; idx++ ) {
    if( Ports[idx].cfg & 0x01 ) {
      unsigned char val = readInput(idx);
      if( val != Ports[idx].status ) {
        Ports[idx].status = val;
        if( (Ports[idx].cfg & 0x02) && val == 0 ) {
        //if( val == 0 ) {
          Ports[idx].timer = 2;
          Ports[idx].timedoff = 1;
        }
        else {
          // Send an OPC.
          Tx1[d0] = val ? OPC_ASON:OPC_ASOF;
          Tx1[d1] = 0;
          Tx1[d2] = 0;
          Tx1[d3] = Ports[idx].addr / 256;
          Tx1[d4] = Ports[idx].addr % 256;
          can_tx(5);
          LED2 = val;
        }
      }
    }
  }
}

void resetOutputs(void) {
  int idx = 0;
  for( idx = 0; idx < 16; idx++ ) {
    if( (Ports[idx].cfg & 0x01) == 0 ) {
        writeOutput(idx, 0);
    }
  }

}
