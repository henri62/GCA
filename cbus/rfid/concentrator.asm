; MERG RFID CONCENTRATOR v1.1								APRIL 2009
; by Martin Perry M1481 
; for use with the INNOVATIONS ID12 or ID20 RFID reader (also called CORE12/20)
;=======================================================================================
; 8 CHANNEL SERIAL INPUT CODE DEVELOPED BY ROBERT AMMERMAN - see copyright notice below
; TASK LEVEL CODE DEVELOPED BY MARTIN PERRY - MERG
; ======================================================================================
;
; v1.1 April 2009 modified PIC inputs to accept inverted serial data from RFID reader pin 9
; as suggested by Gordon Hopkins
;
; Copyright (c) 2000 by Robert V Ammerman and Martin Perry MERG 2008
;
; This code may be used for any legal purpose, commercial or not. I only ask that you
; let me know if you use it and that you leave this copyright notice unchanged in 
; your source code.
;
; Software UAR(no T) code developed by and used with the permission of:
; 
;       Robert V Ammerman
;       RAm Systems
;       (contract development of high performance, high function, low-level software)
;
;======================================================================================
;
; This code is a sample UAR(no T) that receives 8 channels at 9600 baud on a 20Mhz 16F628A PIC
; The program works by handling timer interrupts at 3x the nominal bit rate and running a 
; state machine for each channel to acquire the data. 
;
; The interrupts are at the rate of:
;
;       20000000/4/174 == 28735.63 Hz.
;
; The 'perfect' rate would be 9600*3 == 28800 Hz.
;
; The bit rate error is thus about 0.22 percent.
;
; The first trick behind this code is the way the state machines are run: instead of
; handling each channel one at a time the program uses 'vertical arithmetic' to 
; process all eight channels together.
;
; The second trick is the way that the program works thru processing the eight
; input bits on the channels, accumulating them into the 'receiver shift register'
; variables, and determining when a byte has been completely received. This is done 
; using only 3 instructions per channel.
;
; Using these two tricks results in code that uses only 76 instructions per interrupt, 
; including context save and restore, but not interrupt latency. Since interrupts 
; are generated every 174 instructions this leaves about 54 percent of the CPU available 
; for 'task level' code.
;
; One important thing to note: since this code does _not_ double buffer the receiver, 
; task level code must process a received byte within 4/3 of a bit time (approximately 
; 390 task level instructions), otherwise the overrun flag will be set for that channel.
; =====================================================================================

		list		p=16F628A		; list directive to define processor
		#include	<p16F628A.inc>	; processor-specific variable definitions

		__CONFIG		h'3F62' 	; Code Protection off
									; Data Memory Code Protection off
									; Low Voltage Programming disabled
									; Brown Out Reset enabled
									; MCLR enabled
									; Power up Timer enabled
									; WatchDog Timer disabled
									; High Speed 20MHz crystal on RA6 and RA7	

	CBLOCK      0x20		; general variables
	
    	rsr_chan:8    		; Receiver shift registers for channels 0..7    
 		buff_byte:8			; Buffer byte pointers
    
; The following bitmapped variables maintain the state data for each of the 
; channels. Note that each channel will always have exactly 1 one bit set
; in these variables to indicate what state that channel is in.

        wait_start          ; Channel is waiting for start bit
        confirm_start       ; Channel is verifying that it is really in start bit
        ignore1             ; Channel is waiting for sample time
        ignore2             ; Channel is waiting for sample time
        sample              ; Channel is sampling the input state
        stop_exp            ; Channel is expecting the stop bit
    	
; The state visible to task-level code for the UART receivers
    			
        rcv_ready			; Bitmap of channels with bytes ready
        frame_err			; Bitmap of channels with framing error
        overrun_err			; Bitmap of channels with overrun error
		ch_stream			; Bitmap of channels ready to be streamed to USART

        curr_input			; The current inputs from the 8 serial ports
 
        full				; Bitmapped variable used to track when a UART shift 
							; register is full and thus starts check for the stop bit at the 
							; next sample time.

		work				; Temps used in computing the new state
        new_wait_start
        new_ignore1
							
; task level variables 

		rx_byte				; temp for incoming byte
    	buffer_ch			; temp for active Channel incoming
    	temp_ptr			; temp for incoming buffer byte pointer		
    	stream_ptr			; pointer for byte no. being streamed
    	stream_out			; buffer ch being streamed to USART
    	stream_addr			; variable to hold current stream address
    	qu_ptr				; temp work variables
    	stream_wk
 

 	ENDC
 
	CBLOCK		0x70
        
        w_save				; Save area for interrupts
        status_save

	ENDC

	CBLOCK      0xA0		; start of 5 16-byte string buffers
    
		byteA0, byteA1, byteA2, byteA3, byteA4, byteA5, byteA6, byteA7
		byteA8, byteA9, byteAA, byteAB, byteAC, byteAD, byteAE, byteAF
		byteB0, byteB1, byteB2, byteB3, byteB4, byteB5, byteB6, byteB7
		byteB8, byteB9, byteBA, byteBB, byteBC, byteBD, byteBE, byteBF
		byteC0, byteC1, byteC2, byteC3, byteC4, byteC5, byteC6, byteC7
		byteC8, byteC9, byteCA, byteCB, byteCC, byteCD, byteCE, byteCF
		byteD0, byteD1, byteD2, byteD3, byteD4, byteD5, byteD6, byteD7
		byteD8, byteD9, byteDA, byteDB, byteDC, byteDD, byteDE, byteDF
		byteE0, byteE1, byteE2, byteE3, byteE4, byteE5, byteE6, byteE7
		byteE8, byteE9, byteEA, byteEB, byteEC, byteED, byteEE, byteEF

	ENDC
	
	CBLOCK		0x120		; top 3 16-byte string buffers

		byteF0, byteF1, byteF2, byteF3, byteF4, byteF5, byteF6, byteF7
		byteF8, byteF9, byteFA, byteFB, byteFC, byteFD, byteFE, byteFF
		byteG0, byteG1, byteG2, byteG3, byteG4, byteG5, byteG6, byteG7
		byteG8, byteG9, byteGA, byteGB, byteGC, byteGD, byteGE, byteGF
		byteH0, byteH1, byteH2, byteH3, byteH4, byteH5, byteH6, byteH7
		byteH8, byteH9, byteHA, byteHB, byteHC, byteHD, byteHE, byteHF

	ENDC

;==============================================================================
; Main entry point

	org			0x00

    			goto        setup		; commence setting up the PIC

;==============================================================================
; Interrupt Service routine - the original serial input code by Ammerman used 
; Timer 0 interrupts to sample incoming serial line bits. The task level code
; adds checks for USART output streaming and tx buffer status

    org         0x04

    			movwf	w_save			; context save       
    			swapf	STATUS,W
   				movwf	status_save
    
    			btfsc	INTCON, T0IF	; timer 0 interrupted?
    			call	readinps		; yes, call serial line sampling routine
    			movf	stream_ptr, F	; test stream pointer
    			btfsc	STATUS, Z		; zero?
				goto	exitisr			; yes,
				btfsc	PIR1, TXIF		; no, test if TX buffer is empty yet				
    			call	byteout			; yes, call buffer out-stream routine
    
exitisr   		swapf   status_save, W	; no, Context restore ready for exit ISR
				movwf   STATUS
				swapf   w_save,F
				swapf   w_save,W

				retfie 

; end of ISR - ****BE CAREFUL NOT TO EXTEND ISR PAST ADDRESS 0Fh****
;==================================================================================
; Mainline code - commences with PIC setup routine

	org			0x20

setup			clrwdt					; clear wdt and prescaler (although not used in this app)
				bcf		T1CON, T1OSCEN	; disable timer 1
				bcf		INTCON, RBIE	; disable PORT B interrupts
				bsf		STATUS, RP0		; change to bank 1
				movlw	b'00001000'		; prescaler assigned to WDT so not in use for this app.
				movwf	OPTION_REG		; prime Option register
				movlw	b'10111111'		; set up PORTA, osc out, osc in, RA5=MCLR in, RA4 bank select in,
										; RA0-3 serial A-D in
				movwf	TRISA			; prime PORT A
				movlw	b'11111010'		; set up PORTB, RB4-7 serial E-H in, RB3 CTS in, RB2 TXD out
										; RB1 RXD in, RB0 RTS out
				movwf	TRISB			; prime PORT B
				movlw	b'00100110'		; set up transmit status register, TX 8 bit, async, enable tx
										; brgh high
				movwf	TXSTA			; prime tx STATUS reg
				movlw	b'10000001'		; set baud rate (BRG=129)
				movwf	SPBRG			; prime baud rate generator
				bcf		STATUS, RP0		; change back to bank 0
				movlw	b'10010000'		; enable USART serial port, 8 bit reception, continuous rx
				movwf	RCSTA			; prime rx status register		
				movlw	b'00000111'
				movwf	CMCON			; turn off comparators
				bsf		PORTB, 0		; turn off RTS

; end of PIC setup routine		
		
start			movlw   0x80			; initialize all the UART status values and buffer pointers	
        		movwf   rsr_chan+0
       			movwf   rsr_chan+1
				movwf   rsr_chan+2
				movwf   rsr_chan+3
				movwf   rsr_chan+4
				movwf   rsr_chan+5
				movwf   rsr_chan+6
				movwf   rsr_chan+7
        
				clrf	buff_byte+0
				clrf	buff_byte+1
				clrf	buff_byte+2
				clrf	buff_byte+3
				clrf	buff_byte+4
				clrf	buff_byte+5
				clrf	buff_byte+6
				clrf	buff_byte+7

				clrf    rcv_ready
				clrf    frame_err
				clrf	overrun_err
				clrf    full
			
				movlw   0xFF
				movwf   wait_start
				movwf	stream_out
				clrf	stream_ptr
				clrf	ch_stream

				clrf    confirm_start
				clrf    ignore1
				clrf    ignore2
				clrf    sample
				clrf    stop_exp

				clrf	rx_byte
				clrf	temp_ptr

				bsf     STATUS, RP0		; switch to bank 1
				bcf     OPTION_REG-0x80,T0CS               
				bsf		TXSTA, TXEN		; enable transmission
				bcf     STATUS,RP0
				movlw	0x3E			; load W with ASCII (>)				
				bcf		PORTB, 0		; send RTS to output				
txwait1			btfss	PIR1, TXIF		; tx ready?
				goto	txwait1			; no, go wait
ctswait1		btfsc	PORTB, 3		; yes, test PC CTS?
				goto	ctswait1		; no, go wait again
				movwf	TXREG			; yes, output (>) to USART
				bsf		PORTB, 0		; turn off RTS

				bsf     INTCON,T0IE		; turn on Timer 0 interrupts
				bsf     INTCON,GIE		; turn on global interrupts

; end of initialisation routines
; =======================================================================================

; =======================================================================================
; start of central looping code

forever			clrf	buffer_ch			; clear incoming channel pointer
				btfsc   rcv_ready, 0		; commence cycling round all bit maps
				call    rcv_0				; to see if any bytes are ready for buffering
				incf	buffer_ch, F		; incrementing the channel pointer as we go
				btfsc   rcv_ready,1
				call    rcv_1
				incf	buffer_ch, F    
				btfsc   rcv_ready,2
				call    rcv_2
				incf	buffer_ch, F
				btfsc   rcv_ready,3
				call    rcv_3
				incf	buffer_ch, F
				btfsc   rcv_ready,4
				call    rcv_4
				incf	buffer_ch, F
				btfsc   rcv_ready,5
				call    rcv_5
				incf	buffer_ch, F
				btfsc   rcv_ready,6
				call    rcv_6
				incf	buffer_ch, F
				btfsc   rcv_ready,7
				call    rcv_7

; ===============================================================================================
											; finished checking bitmaps, now start checking
											; if any buffers need to be sent to the USART
				movf	stream_ptr, F		; check stream pointer
				btfss	STATUS, Z			; to see if a buffer is streaming to USART 
				goto    forever				; yes, already streaming go round loop again
				movf	ch_stream, F		; no, check to see if a channel is ready to stream
				btfsc	STATUS,Z			; anything set?
				goto	forever				; no, back to inp sampling
				
				movlw	0x08				; yes, commence determination of next ch to be streamed
											; and start process
				movwf	qu_ptr				; load 8 into USART queue pointer
				movf	ch_stream, W
				movwf	stream_wk			; copy stream bit map into working variable
				clrf	ch_stream			; clear streaming flags
											; **NOTE although unlikely, this could clear unstreamed 
											; flags which have been deposited from other channels
shiftqu			rlf		stream_wk, F		; rotate once left
				btfsc	STATUS, C			; carry set?
				goto	foundch				; yes, exit loop with buffer ch no+1. in queue pointer
				decfsz	qu_ptr, F			; checked all bits?
				goto	shiftqu				; no, go round again
				goto	forever				; yes, no bits set, go back to main loop							

foundch			decf	qu_ptr, W
				movwf	stream_out			; store buffer no. to be streamed
				clrf	stream_ptr			; initialise streaming byte pointer
				
; ==============================================================================================
; INDIRECT ADDRESS CALCULATION FOR STREAMING - enter with streaming channel no. in W

				movf	stream_out, W		; refresh streaming channel no. to reset flags
				sublw	b'00000100'			; subtract from 4
				btfss	STATUS, DC			; negative?
				goto	topbank2			; yes, set address for top 3 buffers
				swapf	stream_out, W		; swap streaming channel nibbles into W 
				addlw	0xA0				; add in buffer bank
				goto	addrstor

topbank2		swapf	stream_out, F		; swap streaming channel nibbles
				movlw	b'01010000'			; set channel 5
				subwf	stream_out, W		; subtract 5 and place in W
				swapf	stream_out, F		; restore stream channel
				addlw	0x20				; add in buffer bank. IRP bit set in running code

; ===============================================================================================
				
addrstor		movwf	stream_addr			; store base stream address
				call 	byteout				; collect and send 1st USART streaming byte
											; which will make stream pointer non-zero

				goto	forever				

; end of central looping code
; ========================================================================================

; ========================================================================================
; start of serial ID12/20 reader output code

rcvchan macro   chan			

rcv_#v(chan)	movf	buff_byte+chan,W	; copy buffer byte pointer into temp variable
				movwf	temp_ptr	
				btfsc   frame_err,chan		; is byte frame error set?
				goto    ferr_#v(chan)		; yes, goto frame error routine		
				btfsc   overrun_err,chan	; no, is overrun error set?
				goto    oerr_#v(chan)		; yes. goto overrun error routine
				movf    rsr_chan+chan,W		; no, collect received byte from map variable
				goto	stor_#v(chan)

ferr_#v(chan)	bcf     frame_err,chan
				movlw	0x21				; load ASCII chr(!)
				goto	stor_#v(chan)
				
oerr_#v(chan)	bcf     overrun_err,chan
				movlw	0x3F				; load ASCII chr(?)

stor_#v(chan)	movwf	rx_byte				; lodge in output variable
				call	store_byte			; output to buffer			
				clrf	rx_byte				; clear output variable
				bcf     rcv_ready,chan
				movlw   0x80
				movwf   rsr_chan+chan
				movf	temp_ptr, W			; retrieve temp buffer byte pointer
				movwf	buff_byte+chan		; store in buffer byte variable
				sublw	0x10
				btfss	STATUS, Z			; last byte into buffer?
				return						; no, back to sampling
				bsf		ch_stream,chan		; yes, set flag in channel stream
				clrf	buff_byte+chan		; reset byte pointer for next time
				clrf	temp_ptr			; reset temporary byte pointer
				return

 endm

    	rcvchan 0
    	rcvchan 1
    	rcvchan 2
    	rcvchan 3
   	 	rcvchan 4
    	rcvchan 5
    	rcvchan 6
    	rcvchan 7

; =============================================================================
; BUFFER STORE ROUTINES

store_byte		xorlw	0x02					; is it STX?
				btfss	STATUS,Z				; zero?
				goto	etxchk					; no, go check for ETX
				movlw	0x41					; yes, set 41h for ASCII (A)
				btfss	PORTA, 4				; test if channel bank jumper is present
				addlw	0x08					; yes, increase to 49H for ASCII (I)
				addwf	buffer_ch,W				; no, ASCII CHID created
				movwf	rx_byte					; replace STX with ASCII CHID in buffer
				clrf	temp_ptr				; zero buffer byte pointer
				goto	valid_byte				; place in buffer

etxchk			movf	rx_byte, W				; retrieve from temp variable
				xorlw	0x03					; is it ETX
				btfss	STATUS, Z				; zero
				goto	valid_byte				; no, valid byte, place in buffer
				movlw	0x3E					; yes, set ASCII (>)
				movwf	rx_byte					; replace ETX with (>) in buffer

valid_byte		movf	buffer_ch, W			; get buffer channel no.
				sublw	b'00000100'				; subtract from 4
				btfss	STATUS, DC				; negative?
				goto	topbank1				; yes, set address for top 3 buffers
				swapf	buffer_ch, W			; swap buffer channel nibbles into W 
				addlw	0xA0					; add in buffer bank
				addwf	temp_ptr, W				; add in buffer byte pointer
				goto	checkout

topbank1		swapf	buffer_ch, F			; swap buffer channel nibbles
				movlw	b'01010000'				; set channel 5
				subwf	buffer_ch, W			; subtract 5 and place in W
				swapf	buffer_ch, F			; restore buffer ch
				addlw	0x20					; add in buffer bank
				addwf	temp_ptr, W				; add in buffer byte pointer
				bsf		STATUS, IRP				; set bank 2

checkout		movwf	FSR						; load address result into File Select Register
				movf	rx_byte, W				; retrieve received byte
				movwf	INDF					; store in Buffer
				bcf		STATUS, IRP				; reset to banks 0 and 1
				incf	temp_ptr, F				; increment temp buffer byte pointer
				movlw	0x10					; test for last byte
				subwf	temp_ptr, W				; zero?
				btfss	STATUS,Z				; compare
				return							; no, return to buffer routine
				movf	rx_byte, W				; yes, test for ASCII (>)
				sublw	0x3E					; compare
				btfss	STATUS,Z				; same?
				clrf	temp_ptr				; no, trash temp buffer pointer
				return							; yes, return to incoming byte routine

; end of mainline code
;==========================================================================

;==========================================================================
; BYTE STREAMING subroutine

byteout			btfss	stream_addr, 7			; check which RAM bank is needed
				bsf		STATUS, IRP				; set for bank 2
				movf	stream_addr, W			; recover base address
				movwf	FSR						; set in File Select Register
				movf	INDF, W					; get output byte
				bcf		STATUS, IRP				; clear bank 2 bit				
				bcf		PORTB, 0				; send RTS to output				
ctswait2		btfsc	PORTB, 3				; yes, test PC CTS?
				goto	ctswait2				; no, go wait again				
				movwf	TXREG					; yes, output byte to USART
				incf	stream_addr, F
				incf	stream_ptr, F
				movlw	0x10
				subwf	stream_ptr, W			; last byte in buffer?
				btfsc	STATUS, Z				
				clrf	stream_ptr				; yes, zero the stream pointer								
				bsf		PORTB, 0				; turn off RTS
				return							; no, exit routine

; =========================================================================================
				
; =========================================================================================
; INCOMING BYTE SAMPLING subroutine

readinps		bcf         INTCON,T0IF			; Clear the timer interrupt and adjust the
												; timer to correct its period

; We want a period of 174 instructions, which is 1/3 of a bit time
; So we have to add 256-174 to the counter. 
; But the counter delays updating by 2 cycles.
; So we really add 256-174+2 to get the
; desired period.

				movlw       D'256'-D'174'+D'2'
				addwf       TMR0,F

; Get the input and prepare to process it

				movf		PORTA, W		; read PORTA
				andlw		b'00001111'		; mask lower 4 bits
				movwf		curr_input		; serials A - D placed in holding variable
				movf		PORTB, W		; read PORTB
				andlw		b'11110000'		; mask upper 4 bits
				addwf		curr_input, W	; serials E - H added in W
  				movwf		curr_input		; store a copy
   				movwf       work
 
    
; Process all the inputs.

; Note that we only shift in bits for channels that are in the 'sample'
; state.
 
rcv_chan macro n

; The next instruction does two things:
;       1: It puts 'receive shift register full' status, if any, 
;       for the previous channel into the high bit of 'work'.
;       2: It puts the input data bit for the current channel into
;               carry.
 
				rrf         work,F 

; See if this channel is ready to sample

				btfsc       sample,n      ; Do we need a new bit?

; If this channel is ready to sample, the following instruction will be 
; executed to perform two functions:
;       1: It puts the input data bit into the receiver shift register.
;       2: It puts the 'receive shift register full' status into carry.

				rrf         rsr_chan+n,F    ; Yes, move it in
 endm

        rcv_chan 0
        rcv_chan 1
        rcv_chan 2
        rcv_chan 3
        rcv_chan 4
        rcv_chan 5
        rcv_chan 6
        rcv_chan 7

; At this point 'work' contains the 'receive shift register full' bits for
; channels 0..6 (in bits 1..7) and carry has the bit for channel 7.

        		rrf     work,W  ; Bring in channel 7's 
                        ;'receive shift register full' bit

; Note that the bits in WREG that correspond to channels that were not
; sampled on this cycle are garbage. Zero them out and then turn on the 
; full bits for the sampled channels that we just filled.

				andwf       sample,W        ; (can only include chans with new bits)
				iorwf       full,F          ; Turn on full bits for sampled channels

; compute the new state variables based on the old state and the 
; 'curr_input' and 'full' vectors

; Note: a stop bit is a 1, a start bit is a 0

; Here are the transitions of the state machine:
; 
;       stop_exp                -> wait_start
;                                   *set rcv_ready bits
;                                   *set frame_err bits

;       wait_start              -> INPUT==1 -> wait_start
;                                  INPUT==0 -> confirm_start

;       confirm_start           -> INPUT==1 -> wait_start 
;                                  INPUT==0 -> ignore1

;       ignore1                 -> ignore2

;       ignore2                 -> FULL==0 -> sample
;                                  FULL==1 -> stop_exp
;                                       *clear full bits

;       sample                  -> ignore1
;                                       *set overrun error bits
;                                       *store data bits
;                                       *set full bits

; Turning this around into math to compute the new values of the state
; bit vectors and 'full', 'rcv_ready', 'overrun_err' and 'frame_err' vectors 
; based on the old state bit vectors and the 'input' and 'full' vectors
; we get: 
;
;   NOTE: These comments are written assuming all the assignments happen
;       simultaneously:
;
;       confirm_start   <- (wait_start & ~input) 
;       ignore1         <- (confirm_start & ~input) | sample
;       ignore2         <- ignore1
;       sample          <- (ignore2 & ~full)
;       stop_exp        <- (ignore2 & full)
;       full            <- full & ~ignore2
;       wait_start      <- stop_exp 
;                        | (wait_start & input)
;                        | (confirm_start & input)
;       rcv_ready       <- rcv_ready | stop_exp
;       frame_err       <- frame_err | (stop_exp & ~input)
;       overrun_err     <- overrun_err | (sample & rcv_ready)

; new_wait_start = ((wait_start | confirm_start) & input) | stop_exp

 				movf    wait_start,W
				iorwf   confirm_start,W
				andwf   curr_input,W
				iorwf   stop_exp,W
				movwf   new_wait_start

; new_ignore1 = (confirm_start & ~input) | sample

				comf    curr_input,W
				andwf   confirm_start,W
				iorwf   sample,W
				movwf   new_ignore1

; overrun_err |= sample & rcv_ready

				movf    sample,W
				andwf   rcv_ready,W
				iorwf   overrun_err,F

; sample = ignore2 & ~full

				comf    full,W
				andwf   ignore2,W
				movwf   sample

; rcv_ready |= stop_exp

				movf    stop_exp,W
				iorwf   rcv_ready,F

; frame_err |= stop_exp & ~input

				comf    curr_input,W
				andwf   stop_exp,W
				iorwf   frame_err,F

; stop_exp = ignore2 & full

				movf    ignore2,W
				andwf   full,W
				movwf   stop_exp

; full = full & ~ignore2
 
				comf    ignore2,W
				andwf   full,F

; ignore2 = ignore1

				movf    ignore1,W
				movwf   ignore2

; confirm_start = wait_start & ~input

				comf    curr_input,W
				andwf   wait_start,W
				movwf   confirm_start

; wait_start = new_wait_start

				movf    new_wait_start,W
				movwf   wait_start

; ignore1 = new_ignore1
				movf    new_ignore1,W
				movwf   ignore1
				return

;===============================================================================

        end


 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
  . 
