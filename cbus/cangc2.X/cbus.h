#ifndef __CBUS_H
#define __CBUS_H

/*
 * Copyright (C) 2008 Andrew Crosland
 */

#include "project.h"
extern near unsigned char can_transmit_timeout;
extern near unsigned char can_transmit_failed;
extern near unsigned char can_bus_off;

extern void can_opc(unsigned char opc);
extern void can_tx_nn(unsigned char dlc_val);
extern void can_tx(unsigned char dlc_val);
void can_bus_on(void);
void can_debug1(unsigned char status);
void can_debug_dcc(void);

#ifdef TX1
	void sendTX1(void);
#endif

extern rom unsigned char defaultID;
extern rom unsigned char status;
extern rom unsigned short nodeID;

#ifdef HAS_EVENTS
	extern rom unsigned short ENindex;
	extern rom unsigned long ENstart[EN_NUM];
	extern rom unsigned short EVstart[EN_NUM];
#endif

extern rom unsigned char bootflag;

void cbus_setup(void);

// Receive buffer
#ifdef RX0
	extern near unsigned char Rx0[14];
#endif
#ifdef TX1
	extern near unsigned char Tx1[14];
#endif
enum bufbytes {
	con=0,
	sidh,
	sidl,
	eidh,
	eidl,
	dlc,
	d0,
	d1,
	d2,
	d3,
	d4,
	d5,
	d6,
	d7
};

#ifdef ECAN_MODE_2
unsigned char ecan_fifo_empty(void);

typedef struct {
	unsigned char con;
	unsigned char sidh;
	unsigned char sidl;
	unsigned char eidh;
	unsigned char eidl;
	unsigned char dlc;
	unsigned char d0;
	unsigned char d1;
	unsigned char d2;
	unsigned char d3;
	unsigned char d4;
	unsigned char d5;
	unsigned char d6;
	unsigned char d7;
} ecan_rx_buffer;
extern ecan_rx_buffer * rx_ptr;

#endif

#endif	// __CBUS_COMMON_H
