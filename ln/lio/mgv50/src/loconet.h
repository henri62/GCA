/*
 Modelspoorgroep Venlo LocoNet Firmware

 Copyright (C) Ewout Prangsma <ewout@prangsma.net>

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

#ifndef __loconet_h
#define __loconet_h

// Protocol bit states
#define LN_SPACE                0
#define LN_MARK                 1

// Bit times
#define CD_BACKOFF              20 // bit times
#define PRIORITY_DELAY          20 // bit times 
#define NETWORK_SEIZE_DELAY     6  // bit times

#define TRANSMIT_ATTEMPTS       25

// --------------------------------------------------------
// Opcode Message Structures
// --------------------------------------------------------

typedef struct { 
        unsigned char opcode;           
        unsigned char arg1;
        unsigned char arg2;
        unsigned char chsum;
} opc_loco_adr_msg;

// OPC_INPUT_REP 
typedef struct {
        unsigned char opcode;
        unsigned char in1;
        unsigned char in2;
        unsigned char chsum;
} opc_input_rep_msg;

// OPC_SW_REP
typedef struct {
        unsigned char opcode;
        unsigned char sn1;
        unsigned char sn2;
        unsigned char chsum;
} opc_sw_rep_msg;

// OPC_SW_REQ
typedef struct {
        unsigned char opcode;
        unsigned char sw1;
        unsigned char sw2;
        unsigned char chsum;
} opc_sw_req_msg;

// OPC_PEER_XFER (16 bytes)
//  SV programming format used bu LocoIO
typedef struct {
        unsigned char opcode;
        unsigned char len; // always 0x10
        unsigned char src;
        unsigned char dst;
        unsigned char sv_type; // always 0x01
        unsigned char pxct1;
        unsigned char cmd;
        unsigned char sv_index;
        unsigned char d3;
        unsigned char sv_value;
        unsigned char pxct2;
        unsigned char sub_addr;
        unsigned char d6;
        unsigned char d7;
        unsigned char d8;        
        unsigned char chsum;
} opc_peer_xfer;

// OPC_PEER_XFER (16 bytes)
//  SV programming format 1
typedef struct {
        unsigned char opcode;
        unsigned char len; // always 0x10
        unsigned char src;
        unsigned char dst;
        unsigned char sv_type; // always 0x01
        unsigned char pxct1;
        unsigned char d1;
        unsigned char d2;
        unsigned char d3;
        unsigned char d4;
        unsigned char pxct2;
        unsigned char d5;
        unsigned char d6;
        unsigned char d7;
        unsigned char d8;        
        unsigned char chsum;
} opc_peer_xfer1;

//  SV programming format 2
typedef struct {
        unsigned char opcode;
        unsigned char len; // always 0x10
        unsigned char src;
        unsigned char sv_cmd;
        unsigned char sv_type; // always 0x01
        unsigned char svx1;
        unsigned char dst_l;
        unsigned char dst_h;
        unsigned char sv_adrl;
        unsigned char sv_adrh;
        unsigned char svx2;
        unsigned char d1;
        unsigned char d2;
        unsigned char d3;
        unsigned char d4;        
        unsigned char chsum;
} opc_peer_xfer2;

// General message
typedef union {
        opc_input_rep_msg       input_rep;
        opc_sw_rep_msg          sw_rep;
        opc_sw_req_msg          sw_req;
        opc_peer_xfer           peer_xfer;
        unsigned char           raw[16];
} ln_msg;

// --------------------------------------------------------
// Opcodes
// --------------------------------------------------------

// 2-byte
#define OPC_IDLE                0x85    // Force IDLE state
#define OPC_GPON                0x83    // Global power on
#define OPC_GPOFF               0x82    // Global power off

// 4-byte
#define OPC_LOCO_ADR            0xBF    // Request Loco address
#define OPC_SW_ACK              0xBD    // Request switch with ack function
#define OPC_SW_STATE            0xBC    // Request state of switch
#define OPC_RQ_SL_DATA          0xBB    // Request slot data
#define OPC_MOVE_SLOTS          0xBA    // Move slot src to dest
#define OPC_LINK_SLOTS          0xB9    // Link slot arg1 to slot arg2
#define OPC_UNLINK_SLOTS        0xB8    // Unlink slot arg1 from slot arg2
#define OPC_CONSIST_FUNC        0xB6    // Set func bits in a consist uplink element
#define OPC_SLOT_STAT1          0xB5    // Write slot stat1
#define OPC_LONG_ACK            0xB4    // Long acknowledge
#define OPC_INPUT_REP           0xB2    // General sensor input codes
#define OPC_SW_REP              0xB1    // Turnout sensor state report
#define OPC_SW_REQ              0xB0    // Request switch function
#define OPC_LOCO_SND            0xA2    // Set slot sound functions
#define OPC_LOCO_DIRF           0xA1    // Set slot dir, F0-4 state
#define OPC_LOCO_SPD            0xA0    // Set slot speed

// 6-byte (reserved)

// Variable bytes
#define OPC_WR_SL_DATA          0xEF    // Write slot data (10 bytes)
#define OPC_SL_RD_DATA          0xE7    // Slot data return (10 bytes)
#define OPC_PEER_XFER           0xE5    // Move 8 bytes peer to peer (16 bytes)
#define OPC_IMM_PACKET          0xED    // Send n-byte packet immediate (11 bytes)

// --------------------------------------------------------
// Flags
// --------------------------------------------------------

#define LNF_OPCODE              0x80    // D7 bit indicates an opcode.
#define IS_LN_OPCODE(x)         ((x) & LNF_OPCODE)

// Message length
#define LNF_SIZE_MASK           0x60    // Mask for getting message size
#define LNF_2BYTES              0x00    // 2-byte message
#define LNF_4BYTES              0x20    // 4-byte message
#define LNF_6BYTES              0x40    // 6-byte message
#define LNF_VAR_BYTES           0x60    // N-byte message (N is variable)

// --------------------------------------------------------
// OPC_PEER_XFER SV_CMD's
// --------------------------------------------------------

#define SV_CMD_WRITE            0x01    // Write 1 byte of data from D1
#define SV_CMD_READ             0x02    // Initiate read 1 byte of data into D1
#define SV_CMD_MASKED_WRITE     0x03    // Write 1 byte of masked data from D1. D2 contains the mask to be used.
#define SV_CMD_WRITE4           0x05    // Write 4 bytes of data from D1..D4
#define SV_CMD_READ4            0x06    // Initiate read 4 bytes of data into D1..D4
#define SV_CMD_DISCOVER         0x07    // Causes all devices to identify themselves by their MANUFACTURER_ID, DEVELOPER_ID, PRODUCT_ID and Serial Number
#define SV_CMD_IDENTIFY         0x08    // Causes an individual device to identify itself by its MANUFACTURER_ID, DEVELOPER_ID, PRODUCT_ID and Serial Number
#define SV_CMD_CHANGE_ADDR      0x09    // Changes the device address to the values specified in <DST_L> + <DST_H> in the device that matches the values specified in <SV_ADRL> + <SV_ADRH> + <D1>..<D4> that we in the reply to the Discover or Identify command issued previously
#define SV_CMD_RECONFIGURE      0x4F    // Initiates a device reconfiguration or reset so that any new device configuration becomes active

// Replies
#define SV_CMDR_WRITE           0x41    // Transfers a write response in D1
#define SV_CMDR_READ            0x42    // Transfers a read response in D1
#define SV_CMDR_MASKED_WRITE    0x43    // Transfers a masked write response in D1
#define SV_CMDR_WRITE4          0x45    // Transfers a write response in D1..D4
#define SV_CMDR_READ4           0x46    // Transfers a read response in D1..D4
#define SV_CMDR_DISCOVER        0x47    // Transfers an Discover response containing the MANUFACTURER_ID, DEVELOPER_ID, PRODUCT_ID and Serial Number
#define SV_CMDR_IDENTIFY        0x48    // Transfers an Identify response containing the MANUFACTURER_ID, DEVELOPER_ID, PRODUCT_ID and Serial Number
#define SV_CMDR_CHANGE_ADDR     0x49    // Transfers a Change Address response.
#define SV_CMDR_RECONFIGURE     0x4F    // Acknowledgement immediately prior to a device reconfiguration or reset

/*
 * Programming SV's with OPC_PEER_XFER */
 
/*
  Current addressing
  
  The PC is 01 80
  The default LocoIO is 01 81 (you will have to change this if you have more than 1 LocoIO). Digitrax has
  assigned the 01 high address for LocoIO devices.
  Note: A broadcast packet can be sent out using a value of 0 in the DSTL Destination low address
  field. This allows you to set the SV1 value to an initial value (assuming you didn’t get the PIC
  programmed from me) or to fix SV1 if you accidentally changed it to an unknown value.
  Loconet Program Packet Layout (from PC to LocoIO)
  In order to program the SV’s you use Loconet Peer to Peer messages (OPC_PEER_XFER). The syntax of
  this message is documented in the Loconet Personal Edition 1.0. This document will address the field’s
  LocoIO uses.
  
  0xE5      OP Code
  0x10      message length
  SRCL      Source low address
  DSTL      Destination low address
  DSTH      Source and Destination high address
  PXCT1
  D1        Command
  D2        Register
  D3
  D4        Data
  PXCT2
  D5
  D6
  D7
  D8
  CHK Checksum
  
  Example
  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
  E5 10 50 51 01 00 02 01 00 00 00 00 00 00 00 09
  This is sending out a read command to LocoIO # 81 (0x51) and requesting the data from SV number 1.
  
  
  Loconet Program Packet Layout (reply from LocoIO to the PC)
  
  0xE5     OP Code
  0x10     message length
  SRCL     Source low address
  DSTL     Destination low address
  DSTH     Source and Destination high address
  PXCT1    High order bit of Version
  D1       Original command
  D2       SV number requested
  D3       Lower 7 bits of LocoIO Version
  D4
  PXCT2    High order bit of requested data
  D5
  D6       Requested Data
  D7       Requested Data + 1
  D8       Requested Data + 2
  CHK Checksum
  
  Example
  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
  E5 10 51 50 01 04 02 01 07 00 08 00 51 01 40 12
  This reply is from LocoIO # 81 (0x51) and saying that SV 1 has the value 81 (0x51)

*/
#define SV_WRITE  0x01
#define SV_READ   0x02

#define LOCOIO_DSTH 0x01

#define MP_WRITE 0x03
#define MP_READ  0x04


#endif // __loconet_h
