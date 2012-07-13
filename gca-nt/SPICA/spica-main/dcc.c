
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

// CANCMD Programmer/Command Station (C) 2009 SPROG DCC
//	web:	http://www.sprog-dcc.co.uk
//	e-mail:	sprog@sprog-dcc.co.uk
//
//   Mod by Mike B for PORTC,6 up at start of ISR and down at end.
//	Pete Brownlow	27/6/11 - Change heartbeat to half second so can use for beep control and transmit timeouts as well
//							  Output bridge enable turned off during overload retry wait and when
//							  service programming track is turned off


#include "io.h"
#include "dcc.h"
#include "utils.h"


#pragma udata ISR_VARS

unsigned short an0;
unsigned short retry_delay;
unsigned short ave;
unsigned short sum;

#pragma udata DCC_VARS1

unsigned char bit_flag_s;
unsigned char bit_flag_m;
unsigned char next_byte_s;
unsigned char pre_cnt_s;
unsigned char byte_cnt_s;
unsigned char next_byte_m;
unsigned char pre_cnt_m;
unsigned char byte_cnt_m;

unsigned char dcc_pre_s;
unsigned char dcc_bits_s;
unsigned char dcc_bits_m;
unsigned char dcc_bytes_s;
unsigned char dcc_bytes_m;

unsigned char dcc_idx_s;
unsigned char dcc_idx_m;

unsigned short long slot_timer;

unsigned char awd_count;
unsigned char SWAP_OP;


//
// Flags register used for DCC packet transmission
//
volatile union {
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
volatile union {
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
volatile union {
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


// dcc packet buffers for service mode programming track
// and main track
volatile unsigned char ovld_delay;
volatile unsigned char dcc_buff_s[7];
volatile unsigned char dcc_buff_m[7];
unsigned char ad_state;
volatile unsigned char imax;
unsigned char iccq;





void initDCC(void) {

  // send an idle packet
  dcc_buff_m[0] = 0xff;
  dcc_buff_m[1] = 0;
  dcc_buff_m[2] = 0xff;
  dcc_bytes_m = 3;
  dcc_buff_m[6] = 1;                  // send once

  //idle_next = 0;
  ovld_delay = 0;
  bit_flag_s = 6;			    // idle state
  bit_flag_m = 6;			    // idle state

  dcc_flags.word = 0;
  dcc_flags.dcc_rdy_m = 1;
  dcc_flags.dcc_rdy_s = 1;
  op_flags.op_pwr_s = 1;
  op_flags.op_bit_s = 1;
  op_flags.op_pwr_m = 1;
  op_flags.op_bit_m = 1;

  SWAP_OP = 0;

  ad_state = 0;
  iccq = 0;

}
