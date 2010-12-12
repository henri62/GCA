
/****************************************************************************
    Copyright (C) 2004 Alex Shepherd

    Portions Copyright (C) Digitrax Inc.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************

    IMPORTANT:

    Some of the message formats used in this code are Copyright Digitrax, Inc.
    and are used with permission as part of the EmbeddedLocoNet project. That
    permission does not extend to uses in other software products. If you wish
    to use this code, algorithm or these message formats outside of
    EmbeddedLocoNet, please contact Digitrax Inc, for specific permission.

    Note: The sale any LocoNet device hardware (including bare PCB's) that
    uses this or any other LocoNet software, requires testing and certification
    by Digitrax Inc. and will be subject to a licensing agreement.

    Please contact Digitrax Inc. for details.

*****************************************************************************

 Title :   LocoNet Buffer Source Code file
 Author:   Alex Shepherd <kiwi64ajs@sourceforge.net>
 Date:     13-Feb-2004
 Software:  AVR-GCC
 Target:    AtMega8

 DESCRIPTION
      This module provides functions that manage the receiving of
			LocoNet packets.

			As bytes are received from the LocoNet, they are stored in a circular
			buffer and after a valid packet has been received it can be read out.

			Statistics of packets and errors maintained.

			Any invalid packets that are received are discarded and the stats are
			updated approproately.

*****************************************************************************/

#include <string.h>

#ifndef __BORLANDC__
#include <avr/interrupt.h>
#endif

#include "ln_buf.h"

#define		LN_BUF_OPC_WRAP_AROUND	(byte)0x00                                 // Special character to indcate a buffer
                                                                               // wrap
#define		LN_CHECKSUM_SEED		 		(byte)0xFF

byte ln_buf_rx_buffer[25];
byte ln_buf_rx_buffer_cnt;

void initLnBuf(LnBuf * Buffer)
{
   memset(Buffer, 0, sizeof(LnBuf));
}

lnMsg                                  *recvLnMsg(LnBuf * Buffer)
{
   byte                                    newByte;
   byte                                    tempSize;
   lnMsg                                  *tempMsg;

   while (Buffer->ReadIndex != Buffer->WriteIndex)
   {
      newByte = Buffer->Buf[Buffer->ReadIndex];
      Buffer->ln_buf_process[Buffer->ln_buf_process_cnt]=newByte;
      Buffer->ln_buf_process_cnt++;

      // Check if this is the beginning of a new packet
      if (newByte & (byte) 0x80)
      {
         // if the ReadPacket index is not the same as the Read index then we have received the
         // start of the next packet without completing the previous one which is an error
         if (Buffer->ReadPacketIndex != Buffer->ReadIndex)
         {
            Buffer->Stats.RxErrors++;
         }

         Buffer->CheckSum = LN_CHECKSUM_SEED;
         Buffer->ReadPacketIndex = Buffer->ReadIndex;
         Buffer->ReadExpLen =
            ((newByte & (byte) 0x60) == (byte) 0x60) ? (byte) 0 : ((newByte & (byte) 0x60) >> (byte) 4) + (byte) 2;
      }

      // If the Expected Length is 0 and the newByte is not an Command OPC code, then it must be
      // the length byte for a variable length packet
      else if (Buffer->ReadExpLen == 0)
      {
         Buffer->ReadExpLen = newByte;
      }

      // Set the return packet pointer to NULL first
      tempMsg = NULL;

      // Advance the ReadIndex and ignore the wrap around until we have calculated the 
      // current packet length
      Buffer->ReadIndex++;

      // Calculate the current packet length
      if (Buffer->ReadIndex >= Buffer->ReadPacketIndex)
      {
         tempSize = Buffer->ReadIndex - Buffer->ReadPacketIndex;
      }
      else
      {
         tempSize = Buffer->ReadIndex + (LN_BUF_SIZE - Buffer->ReadPacketIndex);
      }

      // Check the ReadIndex for wrap around and reset if necessary
      if (Buffer->ReadIndex >= LN_BUF_SIZE)
      {
         Buffer->ReadIndex = 0;
      }

      // Do we have a complete packet
      if (tempSize == Buffer->ReadExpLen)
      {
         // Check if we have a good checksum
         if (Buffer->CheckSum == newByte)
         {
            // Set the return packet pointer
            tempMsg = (lnMsg *) Buffer->ln_buf_process;
            Buffer->Stats.RxPackets++;
            Buffer->ln_buf_process_cnt=0;
         }
         else
         {
            Buffer->Stats.RxErrors++;
            ln_buf_rx_buffer_cnt = 0;
         }

         // Whatever the case advance the ReadPacketIndex to the beginning of the
         // next packet to be received
         Buffer->ReadPacketIndex = Buffer->ReadIndex;

         if (tempMsg != NULL)
         {
            return tempMsg;
         }
      }

      // Packet not complete so add the current byte to the checksum
      Buffer->CheckSum ^= newByte;
   }

   return NULL;
}

byte getLnMsgSize(volatile lnMsg * Msg)
{
   return ((Msg->sz.command & (byte) 0x60) ==
           (byte) 0x60) ? Msg->sz.mesg_size : ((Msg->sz.command & (byte) 0x60) >> (byte) 4) + 2;
}
