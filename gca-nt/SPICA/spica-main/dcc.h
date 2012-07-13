/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


#ifndef __DCC_H
#define __DCC_H

#include "spica.h"

//
// Flags register used for DCC packet transmission
//
extern volatile union {
    struct {
        unsigned dcc_rdy_s:1;		    // set if Tx ready for a new packet
        unsigned dcc_long_pre:1;	// set forces long preamble
        unsigned dcc_retry:1;
        unsigned dcc_ack:1;
        unsigned dcc_overload:1;	// set if overload detected
        unsigned dcc_check_ack:1;
        unsigned dcc_check_ovld:1;
        unsigned :1;
        unsigned dcc_rdy_m:1;
        unsigned dcc_reading:1;
        unsigned dcc_writing:1;
        unsigned dcc_cv_no_ack:1;
        unsigned dcc_rec_time:1;
        unsigned :1;
        unsigned dcc_em_stop:1;
        unsigned :1;
    } ;
    unsigned int word;
} dcc_flags;

//
// MODE_WORD flags
//
extern volatile union {
    struct {
        unsigned boot_en:1;
        unsigned :1;
        unsigned s_full:1;
        unsigned :1;
        unsigned :1;
        unsigned ztc_mode:1;	// ZTC compatibility mode
        unsigned direct_byte:1;
        unsigned :1;
    } ;
    unsigned char mbyte;
} mode_word;

//
// OP_FLAGS for DCC output
//
extern volatile union {
    struct {
        unsigned op_pwr_s:1;
        unsigned op_bit_s:1;
        unsigned op_pwr_m:1;
        unsigned op_bit_m:1;
        unsigned bus_off:1;
        unsigned slot_timer:1;
        unsigned can_transmit_failed:1;
        unsigned beeping:1;
    } ;
    unsigned char mbyte;
} op_flags;



typedef struct _slot {
  byte addrl;
  byte addrh;
  byte speed;
  byte fn[4];
} slot;

#define MAX_SLOTS 32
extern slot slots[MAX_SLOTS];


// 14 Clear Timer interrupt flag
// Reload TMR0 for interrupt every 58us
// Tcyc = 250ns @ 16MHz
// Interrupt every 58/(.250) = 232 Tcyc
// Value loaded into TMR0 needs to be adjusted for:
// - TMR0 interrupt latency (3 Tcyc)
// - Number of instructions from interrupt to reload
// - TMR0 inhibit after reload (2 Tcyc with 2:1 prescaler)
// Prescaler = 2:1
// So value is 0 - (232 - 3 - 17 - 2)/2 = -105 = 0x97
// Modified for 32 MHz clock  by Mike B. value was 0x97
#define TMR0_DCC 0x91

#define I_ACK_DIFF 5	// No. steps for additional 60ma ACK pulse
#define I_OVERLOAD 24
#define I_DEFAULT 96
#define I_LIMIT 768

//
// DCC bit state machine macros
//
#define FIRST_EDGE 0
#define SECOND_EDGE_BIT1 1
#define SECOND_MIDPOINT_BIT0 2
#define SECOND_EDGE_BIT0 3
#define FIRST_MIDPOINT_BIT0 4
#define SECOND_IDLE 5
#define FIRST_IDLE 6
#define SECOND_PRE 7
#define FIRST_PRE 8
#define SECOND_MID_START 9
#define SECOND_START 10
#define FIRST_MID_START 11
#define FIRST_START 12

//
// ADC state machine macros
//
#define AD_RESET 0
#define ACQ_AN0 1
#define CONV_AN0 2
#define ACQ_AN4 3
#define CONV_AN4 4


#define dcc_pre_m   14

//
// toggle_dcc()
//
// Called from ISR to toggle the internal DCC value.
//
#define toggle_dcc_s() op_flags.op_bit_s=!op_flags.op_bit_s
#define toggle_dcc_m() op_flags.op_bit_m=!op_flags.op_bit_m


extern volatile unsigned char dcc_buff_s[7];
extern volatile unsigned char dcc_buff_m[7];
extern unsigned char ad_state;
extern volatile unsigned char ovld_delay;
volatile extern unsigned char imax;
extern unsigned char iccq;
extern unsigned char SWAP_OP;
extern unsigned short long slot_timer;

void doDCC(void);
void initDCC(void);

#endif
