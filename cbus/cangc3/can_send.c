/*
 * Microchip C18 source for common CBUS routines
 *
 * Code based on Mike Bolton's canacc8 for which due credit is given.
 *
 * Modifications and conversion to C18 Copyright (C) 2009 Andrew Crosland
 *
 *	Pete Brownlow - 26/6/11 - Add checks for transmit failure and transmit timeout (handled by ISRs) to wait for transmit buffer loops
 */

#include "project.h"

#pragma code APP

#ifdef TX1
/*
 * Send simple 3-byte frame
 */
void can_opc(unsigned char opc) {
	Tx1[d0] = opc;
	can_tx_nn(3);
}

/*
 * Send a CAN frame, putting Node Number in first two data bytes
 */
void can_tx_nn(unsigned char dlc_val) {
	Tx1[d1] = NN_temp>>8;
	Tx1[d2] = NN_temp&0xFF;
	can_tx(dlc_val);
}

//
// Send a Bus On message
//
void can_bus_on(void) {
    Tx1[d0] = OPC_BON;
    can_tx(1);
}

/*
 * Send a CAN frame where data bytes have already been loaded
 */
void can_tx(unsigned char dlc_val) {
	Tx1[dlc] = dlc_val;				// data length
	Tx1[sidh] &= 0b00001111;		// clear old priority
	Tx1[sidh] |= 0b10110000;		// low priority
	Latcount = 10;
	sendTX1();			
}

/*
 * Send a debug message with one status byte
 */
void can_debug1(unsigned char status) {
	Tx1[d0] = OPC_DBG1;
	Tx1[d1] = status;
    can_tx(2);
}

#ifdef HAS_DCC
/*
 * Send a debug message with dcc packet being sent to rail
 */
void can_debug_dcc(void) {
	Tx1[d0] = 0xB8;
	Tx1[d1] = dcc_buff_m[0];
	Tx1[d2] = dcc_buff_m[1];
	Tx1[d3] = dcc_buff_m[2];
	Tx1[d4] = dcc_buff_m[3];
	Tx1[d5] = dcc_buff_m[4];
    can_tx(6);
}
#endif

/*
 * Send contents of Tx1 buffer via CAN TXB1
 */
void sendTX1(void) {
	unsigned char tx_idx;
	unsigned char * ptr_fsr1;
	unsigned char i;

	can_transmit_timeout = 2;	// half second intervals
	op_flags.can_transmit_failed = 0;

	while((TXB1CONbits.TXREQ) && (!op_flags.can_transmit_failed) && (can_transmit_timeout != 0))
		;

  LEDCanActTimer = 2000;
  LEDCANACT = 1;

    // Load TXB1
	tx_idx = 0;
	ptr_fsr1 = &TXB1CON;
	for (i=0; i<14; i++) {
		*(ptr_fsr1++) = Tx1[tx_idx++];
	}

	can_transmit_timeout = 2;	// half second intervals
	op_flags.can_transmit_failed = 0;

	while ((TXB1CONbits.TXREQ) && (!op_flags.can_transmit_failed) && (can_transmit_timeout != 0))
		;
	TXB1CONbits.TXREQ = 1;
}
		
#endif	// TX1

