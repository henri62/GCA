#ifndef __CBUS_COMMON_H
#define __CBUS_COMMON_H

/*
 * Copyright (C) 2008 Andrew Crosland
 */

#include "project.h"

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
unsigned char ee_read(unsigned char addr);
void ee_write(unsigned char addr, unsigned char data);
unsigned short ee_read_short(unsigned char addr);
void ee_write_short(unsigned char addr, unsigned short data);
void dely(void);
void ldely(void);

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
