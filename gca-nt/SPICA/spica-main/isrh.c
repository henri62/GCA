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

#include <string.h>
#include "spica.h"
#include "io.h"
#include "loconet.h"
#include "cbus.h"
#include "utils.h"
#include "cbusdefs.h"
#include "lnconst.h"
#include "dcc.h"


static byte dcccnt = 0;
// TMR0 generates a heartbeat every 32000000/4/2/80 == 50kHz, 20us
#pragma interrupt LNDCC
void LNDCC(void) {

  // Timer0 interrupt handler
  if( INTCONbits.T0IF ) {
    byte inLN;
    INTCONbits.T0IF = 0;     // Clear interrupt flag
    TMR0L = 256 - 80 + 15;    // Reset counter with a correction of 18 cycles

    inLN = LNRX;
    BUZZER = PORT_ON;

    samplepart++;
    if( samplepart > 2 ) {
      samplepart = 0;
      //LNSCAN = PORT_ON;
    }
    
    if( mode != LN_MODE_WRITE && LNstatus == STATUS_WAITSTART && inLN == 0 ) {
      idle = 0;
      LNstatus = STATUS_CONFSTART;
    }
    else if(LNstatus != STATUS_WAITSTART) {
      if( LNstatus == STATUS_CONFSTART ) {
        if(inLN == 0)
          LNstatus = STATUS_IGN1;
        else
          LNstatus = STATUS_WAITSTART;
      }
      else if( LNstatus == STATUS_IGN1 ) {
        LNstatus = STATUS_IGN2;
      }
      else if( LNstatus == STATUS_IGN2 ) {
        LNstatus = STATUS_SAMPLE;
      }
      else if( LNstatus == STATUS_SAMPLE ) {
        if( bitcnt == 8 ) {
          if( inLN == 1 ) {
            sampledata = sample;
            if(dataready)
              overrun = TRUE;
            dataready  = TRUE;

            SampleData[writeP] = sample;
            SampleFlag |= (1 << writeP);
            writeP++;
            if( writeP > 7 )
              writeP = 0;
          }
          else {
            // No stop bit; Framing error.
            LNIndex = 0;
          }
          sample = 0;
          bitcnt = 0;
          LNstatus = STATUS_WAITSTART;
        }
        else {
          LNstatus = STATUS_IGN1;
          sample >>= 1;
          sample |= (inLN << 7);
          bitcnt++;
        }
      }
    }

    if( samplepart == 0 ) {
      if( mode == LN_MODE_READ ) {
        // End of stop bit.
        LNTX = PORT_OFF;
      }



      if( inLN == 1 && mode == LN_MODE_WRITE_REQ ) {
        idle++;
        if( mode == LN_MODE_WRITE && txtry > 20 ) {
          // Give up...
          mode = LN_MODE_READ;
          //Wait4NN = TRUE;
          //LED6_FLIM = PORT_ON;
        }
        else if( idle > (6 + (20-txtry))) {
          byte i;
          for( i = 0; i < LN_BUFFER_SIZE; i++ ) {
            if( LNBuffer[i].status == LN_STATUS_USED) {
              LNBuffer[i].status = LN_STATUS_PENDING;
              LNIndex = i;
              break;
            }
          }
          if( i < LN_BUFFER_SIZE ) {
            // Try to send...
            txtry++;
            mode = LN_MODE_WRITE;
            LNByteIndex = 0;
            LNBitIndex = 0;
            LNWrittenBit = 1;

            LED4_BUS = PORT_ON;
            ledBUStimer = 20;
          }
          else {
            // nothing todo...
            mode = LN_MODE_READ;
          }
        }
      }

      // The write...
      if( mode == LN_MODE_WRITE ) {
        // Check if the inLN equals the last send bit for collision detection.
        if( LNWrittenBit != inLN ) {
          // Collision!
          LNByteIndex = 0;
          LNBitIndex = 0;
          mode = LN_MODE_WRITE_REQ;
          //Wait4NN = TRUE;
          //LED6_FLIM = PORT_ON;
        }
        else {
          if( LNBitIndex == 0 ) {
            // Start bit
            LNWrittenBit = PORT_OFF;
            LNBitIndex++;
          }
          else if( LNBitIndex == 9 ) {
            // Stop bit
            LNWrittenBit = PORT_ON;
            LNBitIndex = 0;
            LNByteIndex++;
            if( LNBuffer[LNIndex].len == LNByteIndex ) {
              LNBuffer[LNIndex].status = LN_STATUS_FREE;
              mode = LN_MODE_READ;
            }
          }
          else {
            LNWrittenBit = (LNBuffer[LNIndex].data[LNByteIndex] >> (LNBitIndex-1)) & 0x01;
            LNBitIndex++;
          }
        }
        LNTX = LNWrittenBit ^ 0x01; // Invert?

      }


    }
    BUZZER = PORT_OFF;

    // END OF LOCONET


    // START OF DCC
    dcccnt++;
    if( dcccnt >= 3 ) {
      dcccnt = 0;
    if (SWAP_OP == 1) {
      // J7 is out
      // On-board booster is service mode output
      if (op_flags.op_pwr_s) {
        DCC_EN = 1;
        if (op_flags.op_bit_s) {
          DCC_NEG = 0;
          DCC_POS = 1;
        } else {
          DCC_POS = 0;
          DCC_NEG = 1;
        }
      } else {
        DCC_EN = 0;
        DCC_NEG = 0;
        DCC_POS = 0;
      }
    }

    else {
      // J7 is in
      // On board booster is main track output
      if (op_flags.op_pwr_m) {
        DCC_EN = 1;
        if (op_flags.op_bit_m) {
          DCC_NEG = 0;
          DCC_POS = 1;
        }
        else {
          DCC_POS = 0;
          DCC_NEG = 1;
        }
      }
      else {
        DCC_EN = 0;
        DCC_POS = 0;
        DCC_NEG = 0;
      }
    }

    // Booster output.
    if (op_flags.op_pwr_m) {
      DCC_OUT_POS = op_flags.op_bit_m;
      DCC_OUT_NEG = !op_flags.op_bit_m;
      //BOOSTER_OUT = 1;
    } else {
      DCC_OUT_POS = 0;
      DCC_OUT_NEG = 0;
      //BOOSTER_OUT = 0;
    }


    // In case mode is changed on the fly, we don't check for ack
    // when on board booster is main track output
    if (SWAP_OP == 0) {
      dcc_flags.dcc_check_ack = 0;
      // ACK_PIN = 0;
    }

    //
    // Slot timeout and other timers - every half second
    //
    if (--slot_timer == 0) {
      slot_timer = ((short long) 500000) / 58;
      op_flags.slot_timer = 1;

    }

/*
    // Determine next service mode track DCC output bit
    switch (bit_flag_s) {
      case FIRST_EDGE:
        toggle_dcc_s();
        // test for a 1 or 0 to transmit
        bit_flag_s = ((next_byte_s & 0x80) == 0x80) ? SECOND_EDGE_BIT1 : FIRST_MIDPOINT_BIT0;
        break;

      case SECOND_EDGE_BIT1:
        toggle_dcc_s();
        // fall through to next_bit

      case SECOND_MIDPOINT_BIT0:
        bit_flag_s = FIRST_EDGE; // likely to be first edge next
        next_byte_s = next_byte_s << 1; // Rotate DCC data byte for next bit
        dcc_bits_s--;
        if (dcc_bits_s != 0) {
          break;
        }
        dcc_idx_s++; // all bits done so point to next byte
        next_byte_s = dcc_buff_s[dcc_idx_s];
        dcc_bits_s = 8; // reload bit counter
        byte_cnt_s--;
        if (byte_cnt_s == 0) {
          dcc_buff_s[6]--; // no more bytes, more packets?
          if ((dcc_buff_s[6] == 0)
            || ((dcc_flags.dcc_rec_time == 0) && (dcc_flags.dcc_ack == 1))) {
            bit_flag_s = FIRST_IDLE; // no more packets or ack received
            // and not recovery time
            dcc_flags.dcc_rdy_s = 1; // buffer free
          } else {
            bit_flag_s = FIRST_PRE; // do another packet
            dcc_idx_s = 0; // reset pointer
            next_byte_s = dcc_buff_s[dcc_idx_s];
            pre_cnt_s = dcc_pre_s;
            byte_cnt_s = dcc_bytes_s;
            dcc_bits_s = 8; // and bit count
          }
        } else {
          bit_flag_s = FIRST_START; // start another byte
        }
        break;

      case SECOND_EDGE_BIT0:
        toggle_dcc_s();
        bit_flag_s = SECOND_MIDPOINT_BIT0;
        break;

      case FIRST_MIDPOINT_BIT0:
        bit_flag_s = SECOND_EDGE_BIT0;
        break;

      case SECOND_IDLE:
        toggle_dcc_s();
        bit_flag_s = FIRST_IDLE;
        // test for new packet
        if (dcc_flags.dcc_rdy_s == 0) {
          bit_flag_s = FIRST_PRE;
          dcc_idx_s = 0; // set up pointer
          next_byte_s = dcc_buff_s[dcc_idx_s];
          pre_cnt_s = dcc_pre_s;
          byte_cnt_s = dcc_bytes_s;
          dcc_bits_s = 8;
        }
        break;

      case FIRST_IDLE:
        toggle_dcc_s();
        bit_flag_s = SECOND_IDLE;
        break;

      case SECOND_PRE:
        toggle_dcc_s();
        // check if ready for start bit
        pre_cnt_s--;
        if (pre_cnt_s == 0) {
          bit_flag_s = FIRST_START;
        } else {
          bit_flag_s = FIRST_PRE;
        }
        break;

      case FIRST_PRE:
        // Could keep track of how many preamble bits have been sent with power on
        // but for now we assume we are starting from scratch
        toggle_dcc_s();
        bit_flag_s = SECOND_PRE;
        break;

      case SECOND_MID_START:
        bit_flag_s = FIRST_EDGE;
        break;

      case SECOND_START:
        toggle_dcc_s();
        bit_flag_s = SECOND_MID_START;
        break;

      case FIRST_MID_START:
        bit_flag_s = SECOND_START;
        break;

      case FIRST_START:
      default:
        toggle_dcc_s();
        bit_flag_s = FIRST_MID_START;
        break;
    }

/*
    //
    // Determine next main track DCC output bit
    //
    switch (bit_flag_m) {
      case FIRST_EDGE:
        toggle_dcc_m();
        // test for a 1 or 0 to transmit
        bit_flag_m = ((next_byte_m & 0x80) == 0x80) ? SECOND_EDGE_BIT1 : FIRST_MIDPOINT_BIT0;
        break;

      case SECOND_EDGE_BIT1:
        toggle_dcc_m();
        // fall through to next_bit

      case SECOND_MIDPOINT_BIT0:
        bit_flag_m = FIRST_EDGE; // likely to be first edge next
        next_byte_m = next_byte_m << 1; // Rotate DCC data byte for next bit
        dcc_bits_m--;
        if (dcc_bits_m != 0) {
          break;
        }

        if (dcc_idx_m == 0) {
          //DCC_PKT_PIN = 1;					// Spare output pin for sync scope on start of packet
        }

        dcc_idx_m++; // all bits done so point to next byte
        next_byte_m = dcc_buff_m[dcc_idx_m];
        dcc_bits_m = 8; // reload bit counter
        byte_cnt_m--;
        if (byte_cnt_m == 0) {
          //DCC_PKT_PIN = 0;
          dcc_buff_m[6]--; // no more bytes, more packets?
          if ((dcc_buff_m[6] == 0)) {
            bit_flag_m = FIRST_IDLE; // no more packets
            dcc_flags.dcc_rdy_m = 1; // buffer free
          } else {
            bit_flag_m = FIRST_PRE; // do another packet
            dcc_idx_m = 0; // reset pointer
            next_byte_m = dcc_buff_m[dcc_idx_m];
            pre_cnt_m = dcc_pre_m;
            byte_cnt_m = dcc_bytes_m;
            dcc_bits_m = 8; // and bit count
          }
        } else {
          bit_flag_m = FIRST_START; // start another byte
        }
        break;

      case SECOND_EDGE_BIT0:
        toggle_dcc_m();
        bit_flag_m = SECOND_MIDPOINT_BIT0;
        break;

      case FIRST_MIDPOINT_BIT0:
        bit_flag_m = SECOND_EDGE_BIT0;
        break;

      case SECOND_IDLE:
        toggle_dcc_m();
        bit_flag_m = FIRST_IDLE;
        // test for new packet
        if (dcc_flags.dcc_rdy_m == 0) {
          bit_flag_m = FIRST_PRE;
          dcc_idx_m = 0; // set up pointer
          next_byte_m = dcc_buff_m[dcc_idx_m];
          pre_cnt_m = dcc_pre_m;
          byte_cnt_m = dcc_bytes_m;
          dcc_bits_m = 8;
        }
        break;

      case FIRST_IDLE:
        toggle_dcc_m();
        bit_flag_m = SECOND_IDLE;
        break;

      case SECOND_PRE:
        toggle_dcc_m();
        // check if ready for start bit
        pre_cnt_m--;
        if (pre_cnt_m == 0) {
          bit_flag_m = FIRST_START;
        } else {
          bit_flag_m = FIRST_PRE;
        }
        break;

      case FIRST_PRE:
        // Could keep track of how many preamble bits have been sent with power on
        // but for now we assume we are starting from scratch
        toggle_dcc_m();
        bit_flag_m = SECOND_PRE;
        break;

      case SECOND_MID_START:
        bit_flag_m = FIRST_EDGE;
        break;

      case SECOND_START:
        toggle_dcc_m();
        bit_flag_m = SECOND_MID_START;
        break;

      case FIRST_MID_START:
        bit_flag_m = SECOND_START;
        break;

      case FIRST_START:
      default:
        toggle_dcc_m();
        bit_flag_m = FIRST_MID_START;
        break;
    }


/*
    //
    // service_ad
    //
    // Acquisition time is within one DCC half bit period (58us).
    //
    // Conversion time is 11 * Tad. Tad is Tosc*64 or 2.66us at 24MHz. So,
    // conversion time is also within one DCC half bit period.
    //
    // one state covers aquisition time, a second state the conversion time.
    // Historic code converts two channels even though we only use the value
    // from the current shunt) so
    // the sample period for each channel is 58us *2 *2 = 232us.
    //
    // Current sense samples on channel 4 are averaged over four sample
    // periods.
    //
    // If a potential overload condition is sensed, a timer is started to
    // check gain after 130 cycles or approx 30ms. If the overload is still present
    // in programming mode then DCC_FLAGS, DCC_OVERLOAD is set.
    //
    switch (ad_state) {
      case AD_RESET: // 0
        ad_state = ACQ_AN0;
        break;

      case ACQ_AN0:
        ad_state = CONV_AN0;
        ADCON0bits.GO = 1;
        break;

      case CONV_AN0:
        if (ADCON0bits.GO == 0) {
          an0 = ((unsigned int) ADRESH << 8) | ADRESL;
          // Averaging function used for current sense is
          // S(t) = S(t-1) - [S(t-1)/N] + VIN
          // A(t) = S(t)/N
          //
          // where S is an accumulator, N is the length of the filter (i.e.,
          // the number of samples included in the rolling average), and A is
          // the result of the averaging function.
          // The time constant of the averaging equation is given by t = N/FS,
          // where N is again the length of the filter and FS is the sampling
          // frequency.
          //
          // Re-arranged for N=4:
          // S(t) = S(t-1) - A(t-1) + Vin
          // A(t) = S(t)/4
          sum = sum - ave; // S(t) = S(t-1) - A(t-1)
          sum = sum + an0; // S(t) = S(t-1) - A(t-1) + Vin
          ave = sum >> 2; // A(t) = S(t)/4
          ADCON0 = 0b00000001; // select channel 0
          ad_state = ACQ_AN4;

          // Are we waiting for retry delay?
          if (retry_delay == 0) {
            // No, have we had a potential overload?
            if (ovld_delay == 0) {
              // No, do normal checks
              if (dcc_flags.dcc_check_ack == 1) {
                // has current increased, denoting ACK?
                if (ave >= (short) (iccq + I_ACK_DIFF)) {
                  dcc_flags.dcc_ack = 1;
                  //	ACK_PIN = 1;
                }
              } else {
                // check for dead short immediately
                // current is limited by LM317
                if ((short) I_LIMIT > an0) {
                  // not dead short, look for overload
                  if ((dcc_flags.dcc_check_ovld == 1)
                    || (SWAP_OP == 0)) { // low power booster mode
                    if (ave >= (short) imax) {
                      ovld_delay = 130; // 130*232us = 30160us delay
                    }
                  }
                } else {
                  // Dead short - shutdown immediately
                  OVERLOAD_PIN = 1;
                  if (SWAP_OP == 1) {
                    // programming mode so react immediately
                    op_flags.op_pwr_s = 0;
                    dcc_flags.dcc_overload = 1;
                    dcc_flags.dcc_check_ovld = 0;
                  } else {
                    // low power booster mode start retry delay
                    op_flags.op_pwr_m = 0;
                    retry_delay = 9 * 256; // 9 * 256 * 232us ~ 535ms
                  }
                }
              }
            } else {
              // countdown overload delay
              ovld_delay--;
              if (ovld_delay == 0) {
                // Check again
                if (ave >= (short) imax) {
                  // still overload so power off
                  OVERLOAD_PIN = 1; // Set spare output pin for scope monitoring
                  if (SWAP_OP == 1) {
                    // programming mode so react immediately
                    op_flags.op_pwr_s = 0;
                    dcc_flags.dcc_overload = 1;
                    dcc_flags.dcc_check_ovld = 0;
                  } else {
                    // low power booster mode restart retry delay
                    op_flags.op_pwr_m = 0;
                    retry_delay = 9 * 256; // 9 * 256 * 232us ~ 535ms
                  }
                }
              }
            }
          } else {
            // countdown retry delay
            retry_delay--;
            if (retry_delay == 0) {
              // Request power on again
              dcc_flags.dcc_retry = 1;
              OVERLOAD_PIN = 0;
            }
          }
        }
        break;

      case ACQ_AN4:
        ad_state = CONV_AN4;
        break;

      case CONV_AN4:
        ad_state = ACQ_AN0;
        break;

      default:
        break;
    }
*/


    }
    // END OF DCC

    BUZZER = PORT_OFF;


  }

}

