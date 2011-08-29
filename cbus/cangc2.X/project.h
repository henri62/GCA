#ifndef __PROJECT_H
#define __PROJECT_H

#define CANGC2

#include <p18f2480.h>

// CAN mode for setup
#define ECAN_MODE_2

// CBUS headers
// One Rx buffer and one Tx buffer must be defined before
// including cbus_common.h
#define TX1

// This device has a fixed CAN ID

// 06/04/11 Roger Healey - Put CANCMD constants inside #ifdef
//  					 - Add MODULE_ID as MTYP_CANCMD		

#include "cbus_common.h"

    
#define FIXED_CAN_ID 0x72
#define DEFAULT_NN   0xFFEE


extern near unsigned char Latcount;
extern near unsigned short NN_temp;
extern const rom unsigned char params[32];

// DEFINE INPUT AND OUTPUT PINS
// ??? NOTE - BC1a still wip, ddr and init not yet worked out 

//
// Port A analogue inputs and digital I/O
//

// RA0 is AN0 current sense
//
#define TMR0_NORMAL	0x91

#endif	// __PROJECT_H
