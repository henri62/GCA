   	TITLE		"Source for CAN-USB3 interface for CBUS"
; filename CAN_USBm.asm  21/02/10
; 
; Uses 4 MHz resonator and PLL for 16 MHz clock
; This interface does not have a Node Number 



; The setup timer is TMR3. This should not be used for anything else
; CAN bit rate of 125 Kbits/sec
; Standard frame only
; Uses 'Gridconnect' protocol for USB
; Works with FTDI 245AM for now.

;	

; added RUN LED switch on. 
; no full diagnostics yet.


; Doesn't use interrupts - for speed.
; Working at full speed with minimum CAN frames (no data bytes)
; 390 uSec per frame
; PC gets all frames  08/05/08
; Mod so it sends response to RTR  (FLiM compatibility)
; Fixed CAN_ID at 7E to avoid conflict with CAN_RS
; RTR response removed. Avoids possibility of buffer overflow
; Allows true sniffer capability for checking self enum. of other modules.
; FLiM should never have a CAN_ID of 7E 
; fixed bug in inloop  30/12/08  now rev d
; tested 30/12/08  OK
; rev e  bug fix in inusb for RTR.
; also sends RTR frames to the PC
; rev g. Modified rev e for bootloader. 11/10/09
; now sends standard and extended frames. CAN ID is that sent by PC.
; rev h. Adds exide bit if not sent bt PC. General cleanup
; corrected fault in Tx1con. Now cleared properly  13/01/10. Now  rev j
; version k 21/02/10
; clear RXmode at start of inusb routine to prevent RTR
; being set on first packet to CAN
; Rev m (no Rev l).  Clear COMSTAT in error routine and on setup.


;	
; Assembly options
	LIST	P=18F2480,r=hex,N=75,C=120,T=ON

	include		"p18f2480.inc"
	
	;definitions  Change these to suit hardware.
	


;set config registers

; note. there seem to be differences in the naming of the CONFIG parameters between
; versions of the p18F2480.inf files

	CONFIG	FCMEN = OFF, OSC = HSPLL, IESO = OFF
	CONFIG	PWRT = ON,BOREN = BOHW, BORV=0
	CONFIG	WDT=OFF
	CONFIG	MCLRE = ON
	CONFIG	LPT1OSC = OFF, PBADEN = OFF
	CONFIG	DEBUG = OFF
	CONFIG	XINST = OFF,LVP = OFF,STVREN = ON,CP0 = OFF
	CONFIG	CP1 = OFF, CPB = OFF, CPD = OFF,WRT0 = OFF,WRT1 = OFF, WRTB = OFF
	CONFIG 	WRTC = OFF,WRTD = OFF, EBTR0 = OFF, EBTR1 = OFF, EBTRB = OFF




;original CONFIG settings left here for reference
	
;	__CONFIG	_CONFIG1H,	B'00100110'	;oscillator HS with PLL
;	__CONFIG	_CONFIG2L,	B'00001110'	;brown out voltage and PWT	
;	__CONFIG	_CONFIG2H,	B'00000000'	;watchdog time and enable (disabled for now)
;	__CONFIG	_CONFIG3H,	B'10000000'	;MCLR enable	
;	__CONFIG	_CONFIG4L,	B'10000001'	;B'10000001'  for 	no debug
;	__CONFIG	_CONFIG5L,	B'00001111'	;code protection (off)	
;	__CONFIG	_CONFIG5H,	B'11000000'	;code protection (off)	
;	__CONFIG	_CONFIG6L,	B'00001111'	;write protection (off)	
;	__CONFIG	_CONFIG6H,	B'11100000'	;write protection (off)	
;	__CONFIG	_CONFIG7L,	B'00001111'	;table read protection (off)	
;	__CONFIG	_CONFIG7H,	B'01000000'	;boot block protection (off)


;	processor uses  4 MHz. Resonator

;********************************************************************************



;****************************************************************
;	define RAM storage
	
	CBLOCK	0		;file registers - access bank
					;interrupt stack for low priority
					;hpint uses fast stack
	W_tempL
	St_tempL
	Bsr_tempL
	PCH_tempH		;save PCH in hpint
	PCH_tempL		;save PCH in lpint (if used)
	Fsr_temp0L
	Fsr_temp0H 
	Fsr_temp1L
	Fsr_temp1H 
	Fsr_temp2L
	Fsr_hold
	Fsr_holx
	TempCANCON
	TempCANSTAT
	CanID_tmp	;temp for CAN Node ID
	IDtemph		;used in ID shuffle
	IDtempl
	
	
	
	Datmode		;flag for data waiting 
	Count		;counter for loading
	Count1
	Latcount	;latency counter

	Temp		;temps
	Temp1
					;the above variables must be in access space (00 to 5F)
					
	Buffer		;temp buffer in access bank for data
	RXbuf		;:0x1C	USB serial receive packet buffer
	RXbuf1
	RXbuf2
	RXbuf3
	RXbuf4
	RXbuf5
	RXbuf6
	RXbuf7
	RXbuf8
	RXbuf9
	RXbufA
	RXbufB
	RXbufC
	RXbufD
	RXbufE
	RxbufF
	RXbuf10
	RXbuf11
	RXbuf12
	RXbuf13
	RXbuf14
	RXbuf15
	RXbuf16
	RXbuf17
	RXbuf18
	RXbuf19
	
	

	ENDC			;ends at 5F 
	

	
	CBLOCK	h'60'	;rest of bank 0
	
	Rx0con			;start of receive packet 0
	Rx0sidh
	Rx0sidl
	Rx0eidh
	Rx0eidl
	Rx0dlc
	Rx0d0
	Rx0d1
	Rx0d2
	Rx0d3
	Rx0d4
	Rx0d5
	Rx0d6
	Rx0d7
	
	
	
	Tx1con			;start of transmit frame  1
	Tx1sidh
	Tx1sidl
	Tx1eidh
	Tx1eidl
	Tx1dlc
	Tx1d0
	Tx1d1
	Tx1d2
	Tx1d3
	Tx1d4
	Tx1d5
	Tx1d6
	Tx1d7
	
	
		


	
	
	
	
	
	;add variables to suit
	
		;****************************************************************
	;	used in ASCII to HEX and HEX to ASCII and decimal conversions
	Abyte1		;first ascii char of a hex byte for ascii input
	Abyte2		;second ascii char of a hex byte
	Hbyte1		;first ascii char of a hex byte for ascii output
	Hbyte2
	Atemp		;temp store
	Htemp
	Hexcon		;answer to ascii to hex conversion
	;*************************************************************
	;	used in USB transmit and receive
	Srbyte_n		;number of serial bytes received in packet
	Stbyte_n		;number of serial bytes to send
	TXmode		;state during serial send
	RXmode		;state during serial receive
	RXtemp		;temp store for received byte
	RXnum		;number of chars in receive string
	Datnum		;no. of data bytes in a received frame

	;**************************************************************
	
	
		
	ENDC
	
	CBLOCK	0x100
	
	TXbuf		;USB transmit packet buffer to PC
	TXbuf1
	TXbuf2
	TXbuf3
	TXbuf4
	TXbuf5
	TXbuf6
	TXbuf7
	TXbuf8
	TXbuf9
	TXbuf10
	TXbuf11
	TXbuf12
	TXbuf13
	TXbuf14	
	TXbuf15
	TXbuf16
	TXbuf17
	Txbuf18
	TXbuf19
	TXbuf20
	TXbuf21
	TXbuf22
	TXbuf23
	TXbuf24
	TXbuf25
	
	ENDC

;****************************************************************
;
;		start of program code

		ORG		0000h
		nop						;for debug
		goto	setup

		ORG		0008h
		goto	hpint			;high priority interrupt

		ORG		0018h	
		goto	lpint			;low priority interrupt


;*******************************************************************

		ORG		0020h			;start of program
;	
;
;		high priority interrupt. Used for CAN receive and transmit error.
;		Not used in USB version

hpint	movff	CANCON,TempCANCON
		movff	CANSTAT,TempCANSTAT
	
		movff	PCLATH,PCH_tempH		;save PCLATH
		clrf	PCLATH
	
		movff	FSR0L,Fsr_temp0L		;save FSR0
		movff	FSR0H,Fsr_temp0H
		movff	FSR1L,Fsr_temp1L		;save FSR1
		movff	FSR1H,Fsr_temp1H
		
		

	
		movf	TempCANSTAT,W			;Jump table
		andlw	B'00001110'
		addwf	PCL,F			;jump
		bra		back
		bra		errint			;error interrupt
		bra		back
		bra		back
		bra		back
		bra		rxb1int			;only receive interrupts used
		bra		rxb0int
		bra		back
		
rxb1int	bcf		PIR3,RXB1IF		;uses RB0 to RB1 rollover so may never use this
		
		lfsr	FSR0,Rx0con		;
		
		goto	access
		
rxb0int	bcf		PIR3,RXB0IF
	
		lfsr	FSR0,Rx0con
		
		goto	access
		
		;error routine here. Only acts on lost arbitration	
errint	movlb	.15					;change bank			
		btfss	TXB1CON,TXLARB
		bra		errbak			;not lost arb.
		movf	Latcount,F			;is it already at zero?
		bz		errbak
		decfsz	Latcount,F
		bra		errbak
		bcf		TXB1CON,TXREQ
		movlw	B'00111111'
		andwf	TXB1SIDH,F			;change priority
txagain bsf		TXB1CON,TXREQ		;try again
					
errbak	movlb	.15
		bcf		RXB1CON,RXFUL
		movlb	0
		bcf		RXB0CON,RXFUL	;ready for next
		bcf		COMSTAT,RXB0OVFL	;clear overflow flags if set
		bcf		COMSTAT,RXB1OVFL			
		bra		back1

access	movf	CANCON,W				;switch buffers
		andlw	B'11110001'
		movwf	CANCON
		movf	TempCANSTAT,W
		andlw	B'00001110'
		iorwf	CANCON
		lfsr	FSR1,RXB0CON	;this is switched bank
load	movf	POSTINC1,W
		movwf	POSTINC0
		movlw	0x6E			;end of access buffer lo byte
		cpfseq	FSR1L
		bra		load
		
		bsf		Datmode,0		;valid message frame	
		
back	bcf		RXB0CON,RXFUL	;ready for next
	
back1	clrf	PIR3			;clear all flags
		movf	CANCON,W
		andlw	B'11110001'
		iorwf	TempCANCON,W
		
		movwf	CANCON
		movff	PCH_tempH,PCLATH
		movff	Fsr_temp0L,FSR0L		;recover FSR0
		movff	Fsr_temp0H,FSR0H

		movff	Fsr_temp1L,FSR1L		;recover FSR1
		movff	Fsr_temp1H,FSR1H

		
		retfie	1				;use shadow registers




;**************************************************************
;
;
;		low priority interrupt. (if used)
;	

lpint	retfie

							
				
	
								

;*********************************************************************




main	btfsc	PIR3,RXB0IF			;fast loop for CAN detection
		bra		incan
		btfss	PORTB,0
		bra		inusb
		bra		main
			
;		move incoming CAN frame to serial output buffer

incan	btg		PORTA,0			;flag
		lfsr	FSR0,RXB0SIDH	;Rx0sidh		;start of CAN frame in RB0
		lfsr	FSR1,TXbuf		;start of serial string for PC
		movlw	":"				;serial start
		movwf	POSTINC1
		btfsc	RXB0SIDL,3		;is it extended
		bra		exide
		movlw	"S"				;standard frames only
		movwf	POSTINC1
		bra		serload
exide		movlw	"X"
		movwf	POSTINC1

serload	movf	POSTINC0,W		;get byte
		call	hexasc			;convert to acsii
		movff	Hbyte1,POSTINC1
		movff	Hbyte2,POSTINC1
		movf	POSTINC0,W		;get byte
		call	hexasc			;convert to acsii
		movff	Hbyte1,POSTINC1
		movff	Hbyte2,POSTINC1
		btfsc	RXB0SIDL,3	;is it extended
		bra		exload
		incf	FSR0L			;skip the extended ID bytes
		incf	FSR0L
		bra	serlo1
exload		movf	POSTINC0,W		;get byte
		call	hexasc			;convert to acsii
		movff	Hbyte1,POSTINC1
		movff	Hbyte2,POSTINC1
		movf	POSTINC0,W		;get byte
		call	hexasc			;convert to acsii
		movff	Hbyte1,POSTINC1
		movff	Hbyte2,POSTINC1
serlo1		movff	POSTINC0,Datnum		;get dlc byte
		btfsc	Datnum,6			;is it an RTR
		bra		rtrset
		movlw	"N"
		movwf	POSTINC1
		bra		datbyte
rtrset	movlw	"R"
		movwf	POSTINC1
		bcf		Datnum,6		;Datnum now has just the number
datbyte	tstfsz	Datnum
		bra		datload
		bra		back2			
datload	movf	POSTINC0,W		;get byte
		call	hexasc			;convert to acsii
		movff	Hbyte1,POSTINC1
		movff	Hbyte2,POSTINC1
		decfsz	Datnum
		bra		datload				
back2	movlw	";"				
		movwf	POSTINC1
		movf	FSR1L,W			;get last
		movwf	Count1			;number of bytes to send

		lfsr	FSR1,TXbuf

		bsf		PORTB,4
		movlw	B'00000000'
		movwf	TRISC			;set to output
notwr	btfsc	PORTB,1			;TXE# must be low to write to USB
		bra		notwr
					
usbwr	bsf		PORTB,4			;strobe
		movff	POSTINC1,PORTC	;output byte
		bcf		PORTB,4
		decfsz	Count1
		bra		usbwr
		movlw	B'11111111'
		movwf	TRISC			;back to inputs

		
		
		bcf		PIR3,RXB0IF
		bcf		RXB0CON,RXFUL
		bra		main
		
inusb	clrf	RXmode			;clear receive mode flags
		movlw	B'11111111'
		movwf	TRISC			;set to input
		bsf		PORTB,5			;RX strobe hi
		bcf		PORTB,5			;strobe
		
		movlw	":"
		subwf	PORTC,W
		bnz		notin			;not start of frame
		bsf		PORTB,5
		nop
		bcf		PORTB,5			;strobe
		
		movlw	"S"			;is it S (standard) or X (extended)
		subwf	PORTC,W
		bz	instd
		movlw	"X"
		subwf	PORTC,W
		bz	inext
		bra	notin			;neither so abort
instd	bcf		RXmode,3	;flag standard
		bra		instart
inext	bsf		RXmode,3	;flag extended
instart		clrf	RXnum			;byte counter
		lfsr	FSR2,RXbuf		;set to start
		bsf		PORTB,5
		nop
inloop	bcf		PORTB,5			;strobe
		
		movlw	";"				;end?
		subwf	PORTC,W
		bz		lastin
		movff	PORTC,POSTINC2  		;bug fix
		bsf		PORTB,5			;strobe up
		
		
		bra		inloop
		
lastin	movff	PORTC,POSTINC2
		bsf		PORTB,5

		lfsr	FSR2,RXbuf		;point to serial receive buffer					
		lfsr	FSR1,Tx1sidh		;point to CAN TX buffer 1  - lowest priority
txload	movff	POSTINC2,Abyte1
		movff	POSTINC2,Abyte2
		call	aschex
		movwf	POSTINC1		;put in CAN TX buffer
		movlw	"N"
		subwf	INDF2,W
		bz		txload1
		movlw	"R"
		subwf	INDF2,W
		bz		txload5
		bra		txload
txload5	bsf		RXmode,2		;flag R
txload1	incf	FSR2L			;miss N or R
		incf	FSR1L			;miss dlc
		btfsc	RXmode,3		;is it ext
		bra		txload2
		incf	FSR1L			;miss out EIDH and EIDL if not extended
		incf	FSR1L
		
txload2		movlw	";"
		subwf	INDF2,W
		bz		txdone
		
	

txload4	movff	POSTINC2,Abyte1
		movff	POSTINC2,Abyte2
		call	aschex
		movwf	POSTINC1		;put in CAN TX buffer
		incf	RXnum,F
		bra		txload2
txdone	movf	RXnum,W			;get number of data bytes
		btfsc	RXmode,2		;is it a RTR
		iorlw	B'01000000'		;set RTR bit
		movwf	Tx1dlc
		movlw	B'00001000'
		btfsc	RXmode,3		;extended?
		iorwf	Tx1sidl,F		;add extended bit
		clrf	RXmode			;ready for next
		movlw	.10
		movwf	Latcount		;ten tries at low priority
		call	sendTX1	

notin	bsf		PORTB,5
		
		goto	main


		
		
		
		
;***************************************************************************
;		main setup routine
;*************************************************************************

setup	clrf	INTCON			;no interrupts yet
		clrf	ADCON0			;turn off A/D, all digital I/O
		movlw	B'00001111'
		movwf	ADCON1
		
		;port settings will be hardware dependent. RB2 and RB3 are for CAN.
		;set S_PORT and S_BIT to correspond to port used for setup.
		;rest are hardware options
		
	
		movlw	B'00100000'		;Port A outputs except reset pin
		movwf	TRISA			;
		movlw	B'00001011'		;RB0 is CTS, RB1 is RTS,  RB2 = CANTX, RB3 = CANRX, RB4,5 are logic 
						;RB6,7 for debug and ICSP and diagnostic LEDs
		movwf	TRISB
		clrf	PORTB
		bsf		PORTB,2			;CAN recessive
		bsf		PORTB,4			;USB write strobe
		bsf		PORTB,5			;USB read strobe
		movlw	B'11111111'		;Port C  USB interface
		movwf	TRISC
		
;	next segment is essential.
		
		bsf		RCON,IPEN		;enable interrupt priority levels
		clrf	BSR				;set to bank 0
		clrf	EECON1			;no accesses to program memory	
		clrf	Datmode
		clrf	Latcount
		clrf	ECANCON			;CAN mode 0 for now
		 
		bsf		CANCON,7		;CAN to config mode
		movlw	B'00000011'		;set CAN bit rate at 125000 for now
		movwf	BRGCON1
		movlw	B'10011110'		;set phase 1 etc
		movwf	BRGCON2
		movlw	B'00000011'		;set phase 2 etc
		movwf	BRGCON3
		movlw	B'00100000'
		movwf	CIOCON			;CAN to high when off
		movlw	B'00000100'		;B'00000100'
		movwf	RXB0CON			;enable double buffer of RX0
		
		
	

		

		
mskload	lfsr	0,RXM0SIDH		;Clear masks, point to start
mskloop	clrf	POSTINC0		
		movlw	LOW RXM1EIDL+1		;end of masks
		cpfseq	FSR0L
		bra		mskloop
		
	
		

		
		movlw	B'10001000'		;Config TX0 buffer for busy frame (not used yet)
		movwf	CANCON
		movlb	.15				;buffer in bank 15
		movlw	B'00000011'
		movwf	TXB0CON			;no send yet
		movwf	TXB1CON
		clrf	TXB0SIDH
		clrf	TXB0SIDL
	
		clrf	TXB0DLC
		bsf		RXF1SIDL,3		;extended frames
		bcf		RXF0SIDL,3		;standard frames
		bcf		RXB0CON,RXM1	;frame type set by RXFnSIDL
		bcf		RXB0CON,RXM0
		bcf		RXB1CON,RXM1
		bcf		RXB1CON,RXM0
		movlb	0				;back to bank 0
		clrf	CANCON			;out of CAN setup mode
		movlw	3
		movwf	Tx1con			;set transmit priority
		
		movlw	B'00100011'
		movwf	IPR3			;high priority CAN RX and Tx error interrupts(for now)
		clrf	IPR1			;all peripheral interrupts are low priority
		clrf	IPR2
		clrf	PIE2



;next segment required
		
		
		
		clrf	INTCON2			;
		clrf	INTCON3			;
		
		clrf	Tx1eidh
		clrf	Tx1eidl
		clrf	PIR1
		clrf	PIR2
	
		movlb	.15
		bcf		RXB1CON,RXFUL
		movlb	0
		bcf		RXB0CON,RXFUL		;ready for next
		bcf		COMSTAT,RXB0OVFL	;clear overflow flags if set
		bcf		COMSTAT,RXB1OVFL
		
		clrf	PIR3			;clear all flags
		
		movlw	B'00000000'
		movwf	INTCON			;no interrupts for now
		
		movlw	.128			;clear USB receive buffer
		movwf	Count
		
flush	bcf		PORTB,5			;read strobe
		nop
		bsf		PORTB,5
		decfsz	Count
		bra		flush
		

		bcf	PORTB,6
		bsf	PORTB,7		;put run LED on.
		goto	main


		
;****************************************************************************
;		start of subroutines		

;*****************************************************************************	
;		send a busy frame - already preloaded in TX0. Not implemented yet
	
sendTX0	movlb	.15				;set to bank 1
		bsf		TXB0CON,TXREQ	;send immediately
		movlb	0				;back to bank 0	
		return
;******************************************************************	

;		Send contents of Tx1 buffer via CAN TXB1

sendTX1	movlb	.15				;check for buffer access
tx_loop	btfsc	TXB1CON,TXREQ
		bra	tx_loop
		movlb	0
	
		lfsr	FSR0,Tx1con
		lfsr	FSR1,TXB1CON
ldTX1	movf	POSTINC0,W
		movwf	POSTINC1	;load TXB1
		movlw	Tx1d7+1
		cpfseq	FSR0L
		bra		ldTX1

	
		movlb	.15				;bank 15
tx1test	btfsc	TXB1CON,TXREQ	;test if clear to send
		bra		tx1test
		bsf		TXB1CON,TXREQ	;OK so send
		
tx1done	movlb	0				;bank 0
		return					;successful send

		
		
;************************************************************************


;		converts one hex byte to two ascii bytes

hexasc	movwf	Htemp			;save hex.  comes in W
		swapf	Htemp,W
		andlw	B'00001111'		;mask
		addlw	30h
		movwf	Hbyte1
		movlw	39h
		cpfsgt	Hbyte1
		bra		nxtnib
		movlw	7
		addwf	Hbyte1,F
nxtnib	movlw	B'00001111'
		andwf	Htemp,W
		addlw	30h
		movwf	Hbyte2
		movlw	39h
		cpfsgt	Hbyte2
		bra		ascend
		movlw	7
		addwf	Hbyte2,F
ascend	return					;Hbyte1 has high nibble, Hbyte2 has low nibble


;****************************************************************************
;		converts two ascii bytes to one hex byte.
;		for consistency, ascii hi byte in Abyte1, ascii low byte in Abyte2
;		answer in Hexcon

aschex	movlw	30h
		subwf	Abyte1,F
		movlw	9
		cpfsgt	Abyte1
		bra		hinib
		movlw	7
		subwf	Abyte1,F
hinib	movf	Abyte1,W
		movwf	Hexcon
		swapf	Hexcon,F
		movlw	30h
		subwf	Abyte2,F
		movlw	9
		cpfsgt	Abyte2
		bra		hexend
		movlw	7
		subwf	Abyte2,F
hexend	movf	Abyte2,W
		iorwf	Hexcon,W
		return
	


		
;************************************************************************		

		end
