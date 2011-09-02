#include "project.h"
#include "cbusdefs.h"
#include "utils.h"
#include "commands.h"
#include "cangc2.h"
#include "io.h"


#pragma udata access VARS

void cmd_cv(void);

#pragma code APP

//
// parse_cmd()
//
// Decode the OPC and call the function to handle it.
//

void parse_cmd(void) {
  //mode_word.s_full = 0;
  switch (rx_ptr->d0) {

    case OPC_ASRQ:
    {
      int addr = rx_ptr->d3 * 256 + rx_ptr->d4;
      if( SOD == addr ) {
        checkInputs(1);
      }
      break;
    }
      
    case OPC_ACON:
    case OPC_ASON:
    {
      int addr = rx_ptr->d3 * 256 + rx_ptr->d4;
      int i = 0;
      for( i = 0; i < 16; i++) {
        if( Ports[i].addr == addr ) {
          writeOutput(i, 1);
        }
      }
      break;
    }

    case OPC_ACOF:
    case OPC_ASOF:
    {
      int addr = rx_ptr->d3 * 256 + rx_ptr->d4;
      int i = 0;
      for( i = 0; i < 16; i++) {
        if( Ports[i].addr == addr ) {
          writeOutput(i, 0);
        }
      }
      break;
    }

    case OPC_RQNPN:
      // Request to read a parameter
      if (thisNN() == 1) {
        doRqnpn((unsigned int) rx_ptr->d3);
      }
      break;

    case OPC_SNN:
    {
      if( Wait4NN ) {
        unsigned char nnH = rx_ptr->d1;
        unsigned char nnL = rx_ptr->d2;
        NN_temp = nnH * 256 + nnL;
        eeWrite(EE_NN, nnH);
        eeWrite(EE_NN+1, nnL);
        Wait4NN = 0;
        LED2 = 0;
      }
      break;
    }

    case OPC_RQNP:
      Tx1[d0] = OPC_PARAMS;
      Tx1[d1] = params[0];
      Tx1[d2] = params[1];
      Tx1[d3] = params[2];
      Tx1[d4] = params[3];
      Tx1[d5] = params[4];
      Tx1[d6] = params[5];
      Tx1[d7] = params[6];
      can_tx(8);
      break;

    case OPC_BOOT:
    // Enter bootloader mode if NN matches
      if (thisNN() == 1)
      {
        eeWrite((unsigned char)(&bootflag), 0xFF);
        Reset();
      }
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
          Tx1[d0] = OPC_NVANS;
          Tx1[d1] = (NN_temp / 256) & 0xFF;
          Tx1[d2] = (NN_temp % 256) & 0xFF;
          Tx1[d3] = nvnr;
          Tx1[d4] = NV1;
          can_tx(5);
          delay();
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

    Nop();
    rx_ptr->con = 0;
    if (can_bus_off) {
      // At least one buffer is now free
      can_bus_off = 0;
      PIE3bits.FIFOWMIE = 1;
      can_bus_on();
    }


}

void doRqnpn(unsigned int idx) {
  if (idx < 8) {
    Tx1[d0] = OPC_PARAN;
    Tx1[d3] = idx;
    Tx1[d4] = params[idx - 1];
    can_tx_nn(5);
  }
  else {
    doError(CMDERR_INV_PARAM_IDX);
  }
}

void doError(unsigned int code) {
  Tx1[d0] = OPC_CMDERR;
  Tx1[d3] = code;
  can_tx_nn(4);
}

int thisNN() {
  if ((((unsigned short) (rx_ptr->d1) << 8) + rx_ptr->d2) == NN_temp)
    return 1;
  else
    return 0;

}
