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
    //BUZZER = PORT_OFF;

    // END OF LOCONET


    // START OF DCC
    dcccnt++;
    if( dcccnt >= 3 ) {
      dcccnt = 0;
      
    // Booster output.
    if (op_flags.op_pwr_m) {
      DCC_OUT_POS = op_flags.op_bit_m;
      DCC_OUT_NEG = !op_flags.op_bit_m;
    }
    else {
      DCC_OUT_POS = 0;
      DCC_OUT_NEG = 0;
    }


    //
    // Determine next main track DCC output bit
    //
    switch (bit_flag_m) {
      case FIRST_EDGE:
        toggle_dcc_m();
        // test for a 1 or 0 to transmit
        bit_flag_m = (next_byte_m & 0x80) ? SECOND_EDGE_BIT1 : FIRST_MIDPOINT_BIT0;
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

        dcc_idx_m++; // all bits done so point to next byte
        next_byte_m = dcc_buff_m[dcc_idx_m];
        dcc_bits_m = 8; // reload bit counter
        byte_cnt_m--;
        bit_flag_m = FIRST_START; // start another byte
        if (byte_cnt_m == 0) {
          //DCC_PKT_PIN = 0;
          dcc_buff_m[6]--; // no more bytes, more packets?
          if ((dcc_buff_m[6] == 0)) {
            bit_flag_m = FIRST_IDLE; // no more packets
            dcc_flags.dcc_rdy_m = 1; // buffer free
          }
          else {
            bit_flag_m = FIRST_PRE; // do another packet
            dcc_idx_m = 0; // reset pointer
            next_byte_m = dcc_buff_m[dcc_idx_m];
            pre_cnt_m = dcc_pre_m;
            byte_cnt_m = dcc_bytes_m;
            dcc_bits_m = 8; // and bit count
          }
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
        bit_flag_m = FIRST_PRE;
        if (pre_cnt_m == 0) {
          bit_flag_m = FIRST_START;
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




    }
    // END OF DCC

    BUZZER = PORT_OFF;


  }

}

