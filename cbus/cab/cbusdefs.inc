
; CBUS definitions for use in PIC assembler

; (c) Pete Brownlow  2011-2012

; These definitions taken from CBUS specifications document rev 8a

; Pete Brownlow 6/4/11 - Original from spec version 7e 
; Pete Brownlow 7/6/11 - Update to spec version 7f, add new module type codes
; Pete Brownlow 4/7/11 - Update to spec version 7g.
; Pete Brownlow 14/8/11 - Updated to spec ver 7h
; Pete Brownlow 20/2/12 - Updated to spec ver 8a, Rocrail and animated modeller module types added


; NOTE: To display this file with everything correctly lined up set your editor or IDE tab stop to 4

;   This program is free software: you can redistribute it and/or modify
;   it under the terms of the GNU General Public License as published by
;   the Free Software Foundation, version 3 of the License, as set out
;   at <http:;www.gnu.org/licenses/>.
;
;   This program is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
;   See the GNU General Public License for more details.
;
;   As set out in the GNU General Public License, you must retain and acknowledge
;   the statements above relating to copyright and licensing. You must also
;   state clearly any modifications made.  Please therefore retain this header
;   and add documentation of any changes you make. If you distribute a changed
;   version, you must make those changes publicly available.
;
;   The GNU license requires that if you distribute this software, changed or
;   unchanged, or software which includes code from this software, including
;   the supply of hardware that incorporates this software, you MUST either
;   include the source code or a link to a location where you make the source
;   publicly available. The best way to make your changes publicly available is
;   via the MERG online resources.  See <www.merg.co.uk> 


; Manufacturers

MANU_MERG		equ	.165	; http:;www.merg.co.uk
MANU_ROCRAIL	equ	.70		; http:;www.rocrail.net
MANU_SPECTRUM	equ	.80		; http:;animatedmodeler.com  (Spectrum Engineering)

; MERG Module types

MTYP_SLIM       equ 0   ; default type code for SLiM nodes
MTYP_CANACC4    equ 1	; Solenoid point driver
MTYP_CANACC5    equ 2	; Motorised point driver
MTYP_CANACC8    equ 3	; 8 digital outputs
MTYP_CANACE3    equ 4	; Control panel switch/button encoder
MTYP_CANACE8C   equ 5	; 8 digital inputs
MTYP_CANLED     equ 6	; 64 led driver
MTYP_CANLED64   equ 7	; 64 led driver (multi leds per event)
MTYP_CANACC4_2  equ 8	; 12v version of CANACC4  
MTYP_CANCAB     equ 9	; CANCAB hand throttle   
MTYP_CANCMD     equ .10	; CANCMD command station
MTYP_CANSERVO	equ	.11	; 8 servo driver (on canacc8 or similar hardware)
MTYP_CANBC		equ .12	; BC1a command station
MTYP_CANRPI		equ	.13	; RPI and RFID interface
MTYP_CANTTCA	equ	.14	; Turntable controller (turntable end)
MTYP_CANTTCB	equ	.15 ; Turntable controller (control panel end)
MTYP_CANHS		equ	.16	; Handset controller for old BC1a type handsets

; Rocrail Module types

MTYP_CANGC1		equ	 1	; RS232 PC interface
MTYP_CANGC2  	equ	 2	; 16 I/O
MTYP_CANGC3		equ	 3	; Command station (derived from cancmd)
MTYP_CANGC4		equ	 4	; 8 channel RFID reader
MTYP_CANGC5		equ	 5	; Cab for fixed panels (derived from cancab)
MTYP_CANGC6		equ	 6	; 4 channel servo controller
MTYP_CANGC7		equ	 7	; Fast clock module

; Spectrum Engineering Animated Modeller module types

MTYP_AMCTRLR	equ  1	; Animation controller (firmware derived from cancmd)
MTYP_DUALCAB	equ	 2	; Dual cab based on cancab


;  CBUS opcodes list 

; Packets with no data bytes

OPC_ACK     EQU 0x00    ; General ack
OPC_NAK     EQU 0x01    ; General nak
OPC_HLT     EQU	0x02    ; Bus Halt
OPC_BON     EQU	0x03    ; Bus on
OPC_TOF     EQU	0x04    ; Track off
OPC_TON     EQU	0x05    ; Track on
OPC_ESTOP   EQU	0x06    ; Track stopped
OPC_ARST    EQU	0x07    ; System reset
OPC_RTOF    EQU	0x08    ; Request track off
OPC_RTON    EQU	0x09    ; Request track on
OPC_RESTP   EQU	0x0a    ; Request emergency stop all
OPC_RSTAT   EQU 0x0c    ; Request node status
OPC_QNN     EQU 0x0d    ; Query nodes

OPC_RQNP    EQU 0x10    ; Read node parameters
OPC_RQMN	EQU 0x11	; Request name of module type

; Packets with 1 data byte

OPC_STAT    EQU 0x20    ; Command station status report
OPC_KLOC    EQU 0x21    ; Release engine by handle
OPC_QLOC    EQU	0x22    ; Query engine by handle
OPC_DKEEP	EQU 0x23	; Keep alive for cab

OPC_DBG1    EQU 0x30    ; Debug message with 1 status byte
OPC_EXTC    EQU 0x3F    ; Extended opcode

; Packets with 2 data bytes

OPC_RLOC    EQU 0x40    ; Request session for loco
OPC_QCON	EQU 0x41	; Query consist
OPC_SNN     EQU	0x42    ; Set node number
OPC_STMOD   EQU	0x44    ; Set Throttle mode
OPC_PCON    EQU	0x45    ; Consist loco
OPC_KCON    EQU	0x46    ; De-consist loco
OPC_DSPD    EQU	0x47    ; Loco speed/dir
OPC_DFLG    EQU 0x48    ; Set engine flags
OPC_DFNON	EQU 0x49	; Loco function on
OPC_DFNOF	EQU 0x4A	; Loco function off
OPC_SSTAT   EQU	0x4C    ; Service mode status

OPC_RQNN	EQU	0x50    ; Request Node number in setup mode
OPC_NNREL	EQU	0x51    ; Node number release
OPC_NNACK	EQU	0x52    ; Node number acknowledge
OPC_NNLRN	EQU	0x53    ; Set learn mode
OPC_NNULN	EQU	0x54    ; Release learn mode
OPC_NNCLR	EQU	0x55    ; Clear all events
OPC_NNEVN	EQU	0x56    ; Read available event slots
OPC_NERD    EQU 0x57    ; Read all stored events
OPC_RQEVN   EQU 0x58    ; Read number of stored events
OPC_WRACK	EQU 0x59	; Write acknowledge
OPC_RQDAT	EQU	0x5A	; Request node data event
OPC_RQDDS	EQU 0x5B    ; Request short data frame
OPC_BOOT	EQU	0x5C    ; Put node into boot mode
OPC_EXTC1   EQU 0x5F    ; Extended opcode with 1 data byte

; Packets with 3 data bytes

OPC_DFUN    EQU	0x60    ; Set engine functions
OPC_GLOC	EQU 0x61	; Get loco (with support for steal/share)
OPC_ERR     EQU	0x63    ; Command station error
OPC_CMDERR  EQU 0x6F    ; Errors from nodes during config

OPC_EVNLF	EQU	0x70    ; Event slots left response
OPC_NVRD	EQU	0x71    ; Request read of node variable
OPC_NENRD   EQU 0x72    ; Request read stored event by index
OPC_RQNPN	EQU	0x73	; Request read module parameters
OPC_NUMEV   EQU 0x74    ; Number of events stored response
OPC_EXTC2   EQU 0x7F    ; Extended opcode with 2 data bytes

; Packets with 4 data bytes

OPC_RDCC3   EQU	0x80    ; 3 byte DCC packet
OPC_WCVO    EQU	0x82    ; Write CV byte Ops mode by handle
OPC_WCVB    EQU	0x83    ; Write CV bit Ops mode by handle
OPC_QCVS    EQU	0x84    ; Read CV
OPC_PCVS    EQU	0x85    ; Report CV

OPC_ACON	EQU	0x90	; on event
OPC_ACOF	EQU	0x91	; off event
OPC_AREQ	EQU	0x92    ; Accessory request event
OPC_ARON	EQU 0x93    ; Accessory response event on
OPC_AROF    EQU 0x94    ; Accessory response event off
OPC_EVULN	EQU	0x95    ; Unlearn event
OPC_NVSET	EQU	0x96    ; Set a node variable
OPC_NVANS	EQU	0x97    ; Node variable value response
OPC_ASON	EQU	0x98    ; Short event on
OPC_ASOF	EQU	0x99    ; Short event off
OPC_ASRQ	EQU	0x9A    ; Short request event
OPC_PARAN   EQU 0x9B    ; Single node parameter response
OPC_REVAL   EQU 0x9C    ; Request read of event variable
OPC_ARSON	EQU	0x9D	; Accessory short response on event
OPC_ARSOF	EQU	0x9E	; Accessory short response off event
OPC_EXTC3   EQU 0x9F    ; Extended opcode with 3 data bytes

; Packets with 5 data bytes

OPC_RDCC4   EQU	0xA0    ; 4 byte DCC packet
OPC_WCVS    EQU	0xA2    ; Write CV service mode

OPC_ACON1   EQU 0xB0    ; On event with one data byte
OPC_ACOF1   EQU 0xB1    ; Off event with one data byte
OPC_REQEV	EQU	0xB2    ; Read event variable in learn mode
OPC_ARON1	EQU	0xB3	; Accessory on response (1 data byte)
OPC_AROF1  	EQU	0xB4    ; Accessory off response (1 data byte)
OPC_NEVAL   EQU 0xB5    ; Event variable by index read response
OPC_PNN		EQU	0xB6	; Response to QNN
OPC_ASON1	EQU	0xB8	; Accessory short on with 1 data byte
OPC_ASOF1	EQU	0xB9	; Accessory short off with 1 data byte
OPC_ARSON1	EQU	0xBD	; Short response event on with one data byte
OPC_ARSOF1  EQU	0xBE	; Short response event off with one data byte
OPC_EXTC4   EQU 0xBF    ; Extended opcode with 4 data bytes

; Packets with 6 data bytes

OPC_RDCC5   EQU	0xC0    ; 5 byte DCC packet
OPC_WCVOA   EQU	0xC1    ; Write CV ops mode by address
OPC_FCLK	EQU	0xCF	; Fast clock

OPC_ACON2   EQU 0xD0    ; On event with two data bytes
OPC_ACOF2   EQU 0xD1    ; Off event with two data bytes
OPC_EVLRN	EQU	0xd2    ; Teach event
OPC_EVANS	EQU	0xd3    ; Event variable read response in learn mode
OPC_ARON2   EQU 0xD4    ; Accessory on response 
OPC_AROF2   EQU 0xD5    ; Accessory off response
OPC_ASON2	EQU	0xD8	; Accessory short on with 2 data bytes
OPC_ASOF2	EQU	0xD9	; Accessory short off with 2 data bytes
OPC_ARSON2	EQU	0xDD	; Short response event on with two data bytes
OPC_ARSOF2  EQU	0xDE	; Short response event off with two data bytes
OPC_EXTC5   EQU 0xDF    ; Extended opcode with 5 data bytes

; Packets with 7 data bytes

OPC_RDCC6  	EQU	0xE0    ; 6 byte DCC packets
OPC_PLOC	EQU	0xE1    ; Loco session report
OPC_NAME	EQU 0xE2    ; Module name response
OPC_PARAMS	EQU	0xEF    ; Node parameters response

OPC_ACON3   EQU 0xF0    ; On event with 3 data bytes
OPC_ACOF3   EQU 0xF1    ; Off event with 3 data bytes
OPC_ENRSP	EQU	0xF2    ; Read node events response
OPC_ARON3   EQU 0xF3    ; Accessory on response 
OPC_AROF3   EQU 0xF4    ; Accessory off response
OPC_EVLRNI	EQU	0xF5    ; Teach event using event indexing
OPC_ACDAT   EQU	0xF6    ; Accessory data event: 5 bytes of node data (eg: RFID)
OPC_ARDAT	EQU	0xF7	; Accessory data response
OPC_ASON3	EQU	0xF8	; Accessory short on with 3 data bytes
OPC_ASOF3	EQU	0xF9	; Accessory short off with 3 data bytes
OPC_DDES	EQU	0xFA	; Short data frame aka device data event (device id plus 5 data bytes)
OPC_DDRS	EQU	0xFB	; Short data frame response aka device data response
OPC_ARSON3	EQU	0xFD	; Short response event on with 3 data bytes
OPC_ARSOF3  EQU	0xFE	; Short response event off with 3 data bytes
OPC_EXTC6	EQU	0xFF	; Extended opcode with 6 data byes


; Modes for STMOD

TMOD_SPD_MASK   		EQU	3
TMOD_SPD_128    		EQU	0
TMOD_SPD_14     		EQU	1
TMOD_SPD_28I    		EQU	2
TMOD_SPD_28     		EQU	3

; Error codes for OPC_ERR

ERR_LOCO_STACK_FULL     EQU	1
ERR_LOCO_ADDR_TAKEN     EQU	2
ERR_SESSION_NOT_PRESENT EQU	3
ERR_CONSIST_EMPTY		EQU	4
ERR_LOCO_NOT_FOUND    	EQU	5
ERR_CMD_RX_BUF_OFLOW	EQU	6
ERR_INVALID_REQUEST		EQU 7
ERR_SESSION_CANCELLED	EQU 8

; Status codes for OPC_SSTAT

SSTAT_NO_ACK    		EQU	1
SSTAT_OVLD      		EQU	2
SSTAT_WR_ACK    		EQU	3
SSTAT_BUSY      		EQU	4
SSTAT_CV_ERROR  		EQU	5

; Error codes for OPC_CMDERR

CMDERR_INV_CMD			EQU	1
CMDERR_NOT_LRN			EQU	2
CMDERR_NOT_SETUP		EQU	3
CMDERR_TOO_MANY_EVENTS	EQU	4
; CMDERR_NO_EV			EQU	5		NOW RESERVED
CMDERR_INV_EV_IDX		EQU	6
CMDERR_INVALID_EVENT	EQU	7
; CMDERR_INV_EN_IDX		EQU	8		NOW RESERVED
CMDERR_INV_PARAM_IDX	EQU 9
CMDERR_INV_NV_IDX		EQU	10
CMDERR_INV_EV_VALUE		EQU	11
CMDERR_INV_NV_VALUE		EQU	12

; Parameter index numbers (readable by OPC_RQNPN, returned in OPC_PARAN)

PAR_MANU				EQU	1	; Manufacturer id
PAR_MINVER				EQU 2	; Minor version letter
PAR_MTYP				EQU 3	; Module type code				
PAR_EVTNUM				EQU 4	; Number of events supported
PAR_EVNUM				EQU	5	; Event variables per event
PAR_NVNUM				EQU 6	; Number of Node variables
PAR_MAJVER				EQU 7	; Major version number
PAR_FLAGS				EQU 8	; Node flags

; Flags in PAR_FLAGS

PF_CONSUMER				equ	1
PF_PRODUCER				equ	2
PF_COMBI				equ	3
PF_FLiM					equ 4


