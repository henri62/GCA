//
// CANCMD Programmer/Command Station (C) 2009 SPROG DCC
//	web:	http://www.sprog-dcc.co.uk
//	e-mail:	sprog@sprog-dcc.co.uk
//
// This code is for a CBUS DCC programmer intended for use
// with DecoderPro. It uses a PIC18F2480 running at 32MHz.
//      	(8 MHz resonator with x4 PLL)
//
//	NOTE: This version will NOT work on early CANCMD hardware with a 
//			4 MHz resonator, you MUST change it to an 8 MHz resonator 
//
// Revision History
//
// 01/08/09		     Created from sprogII_2_6
//				k	 Modify LED state
//					 Add overload retry for mini booster
//				l	 Add AWD output, modify LED behaviour
// 24/11/10		m	 Lower BOR voltage to 2.8V
//				n	 Support for throttle Em stop
// 23/12/10 	p	 Initialize retry delay
// 06/01/11		r	 Set ave, sum, retry_delay & ovld_delay = 0 at power on
//					 during programming
//					 Wait for reset packets to complete before sampling ICCQ
//					 Remove redundant power_on/off_cycle code	
// 01/04/11		s	 Pete Brownlow -Added module paramters so can be bootloaded from FLiM config utility
//									Updated cbusdefs.h from latest CBUS specification
//									BC1a definition added ready for port to BC1a
//									Fixed loading CAN_ID so correct value loaded into TX buffer (spotted by Roger Healey)
//									Added diagnostic output signals on unused pins
// 06/04/11			Roger Healey	Initialise NN_temp to DEFAULT_NN
//		`							Support for parameter read and firmware update from FliM config utility
// 07/04/11		2a	Pete Brownlow	First release candidate for shipment with cancmd kits				
//									cbusdefs updated for release 7e of the CBUS spec (no effect on cancmd build)
// 16/4/11		2b	Pete Brownlow	Long preamble increased from 20 to 24 to ensure meeets NMRA spec and
//									to see if it will now program Tsunami decoders
// 19/04/11			Mike Bolton		Mod to 2a for 32 MHz clock  
// 19/04/11		3a	Pete Brownlow	Incorprate Mikes changes for 32MHz clock, keep slightly increased preamble of 22
//									Change to new major version no. to emphasise change in hardware required (8 MHz resonator)
// 21/04/11		3b	Mike Bolton		Bootloader modified to intialise CAN timing for 32MHz clock - source file now boot3.asm
// 26/06/11		3c	Pete Brownlow	Add CAN transmit error checking, timeout if send fails, diagnostic beeps if unable to send on CAN, 
//
// 									turn off output bridge enable during a short
// 18/07/11		3d	Pete Brownlow	Add code directives to library routines c018c and p18f2580.asm to force them to be linked above bootloader area

#include "project.h"

#pragma config OSC=HSPLL, FCMEN=OFF, IESO=OFF
#pragma config PWRT=ON, BOREN=BOHW, BORV=2, WDT = OFF, WDTPS=256
#pragma config MCLRE=ON, LPT1OSC=OFF, PBADEN=OFF, DEBUG=OFF
#pragma config XINST=OFF, BBSIZ=1024, LVP=OFF, STVREN=OFF
#pragma config CP0=OFF, CP1=OFF, CPB=OFF, CPD=OFF
#pragma config WRT0=OFF, WRT1=OFF, WRTB=OFF, WRTC=OFF, WRTD=OFF
#pragma config EBTR0=OFF, EBTR1=OFF, EBTRB=OFF

#pragma udata access VARS

//
// Flags register used for DCC packet transmission
//
volatile near union {
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
near volatile union {
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
    unsigned char byte;
} mode_word;

//
// OP_FLAGS for DCC output
//
volatile near union {
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
    unsigned char byte;
} op_flags;

//
//
//
volatile near unsigned char ovld_delay;
volatile near unsigned char imax;		// Booster mode current limit
near unsigned char ad_state;	            // A/D state machine
near unsigned char iccq;		            // Quiescent decoder current
volatile near unsigned char tmr0_reload;
near unsigned char	Latcount;
near unsigned char	BeepCount;
near unsigned char	can_transmit_timeout;
near unsigned short NN_temp;
near unsigned char LEDCanActTimer;

// dcc packet buffers for service mode programming track
// and main track
volatile near unsigned char dcc_buff_s[7];
volatile near unsigned char dcc_buff_m[7];

// Module parameters at fixed place in ROM, also used by bootloader

#pragma romdata parameters
const rom unsigned char params[7] = {MANU_MERG, MINOR_VER, MODULE_ID, EVT_NUM, EVperEVT, NV_NUM, MAJOR_VER 
};

#pragma romdata


// local function prototypes

void setup(void);
void __init(void);

/*
 * Interrupt vectors
 */
//#pragma code high_vector=0x08
#pragma code high_vector=0x808
void high_irq_errata_fix(void);
//void interrupt_at_high_vector(void)
void HIGH_INT_VECT(void)
{
    _asm
        CALL high_irq_errata_fix, 1
    _endasm
}

/*
 * See 18F2480 errata
 */
void high_irq_errata_fix(void) {
    _asm
        POP
        GOTO isr_high
    _endasm
}

//#pragma code low_vector=0x18
#pragma code low_vector=0x818
//void interrupt_at_low_vector(void)
void LOW_INT_VECT(void)
{
    _asm GOTO isr_low _endasm
}

#pragma code APP

void main(void) {
    unsigned char i;
    LEDCanActTimer = 0;
    
    setup();
    
    // Turn on main track
    op_flags.op_pwr_m = 1;

    for (i = 0; i < 5; i++) {
        Tx1[d0] = OPC_ARST;
        can_tx(1);
    }

    // Loop forever
    while (1) {
        if (dcc_flags.dcc_overload) {
            // Programming overload
            dcc_flags.dcc_overload = 0;
			OVERLOAD_PIN =0;
        } else {
            if (SWAP_OP == 0) {
                // Low power booster mode
			    if (dcc_flags.dcc_retry) {
                    // Turn power back on after retry
				    dcc_flags.dcc_retry = 0;
                    op_flags.op_pwr_m = 1;
					OVERLOAD_PIN = 0;
			    }
            }
		}

        if (((dcc_flags.dcc_reading) || (dcc_flags.dcc_writing))
            && (dcc_flags.dcc_rdy_s == 1)) {
            // iterate service mode state machine
            cv_sm();
        }

        if (dcc_flags.dcc_rdy_m) {
            // Main track output is ready for next packet
            packet_gen();
        }

        // Check for Rx packet and setup pointer to it
        if (ecan_fifo_empty() == 0) {
            // Decode the new command
            LEDCanActTimer = 2000;
            LEDCANACT = 1;
            parse_cmd();
        }

        // Handle slot & service mode timeout and beeps every half second
        if (op_flags.slot_timer) {
            for (i = 0; i < MAX_HANDLES; i++) {
                if (q_queue[i].status.valid) {
                    if (--q_queue[i].timeout == 0) {
                        q_queue[i].status.valid = 0;
                    }
                }
            }

			if (BeepCount > 0) {
				op_flags.beeping = !op_flags.beeping;
				AWD = op_flags.beeping || (retry_delay > 0);
				if (op_flags.beeping) {
					BeepCount--;
				}
			}
			else {
				op_flags.beeping = 0;
			  	if (retry_delay == 0) {
					AWD = 0;
				}
			}
            op_flags.slot_timer = 0;
        }  // slot timer flag set
    }
}


void setup(void) {
    unsigned char i;

    INTCON = 0;
    EECON1 = 0;

    //
    // setup initial values before enabling ports
    // Port A are analogue
    //
    op_flags.op_pwr_s = 0;
    op_flags.op_pwr_m = 0;
	op_flags.bus_off = 0;
	op_flags.can_transmit_failed = 0;
	op_flags.beeping = 0;
	can_transmit_timeout = 0;
	BeepCount = 0;
    retry_delay = 0;

    // Setup ports
    PORTC = PORTC_INIT;
    PORTB = PORTB_INIT;
    PORTA = PORTA_INIT;

    TRISC = PORTC_DDR;
    TRISB = PORTB_DDR;
    TRISA = PORTA_DDR;
    TRISBbits.TRISB4 = 0; /* CAN activity */
    TRISBbits.TRISB1 = 0; /* RUN indicator */
    TRISBbits.TRISB0 = 0; /* Internal booster/PT */

    DCC_EN = 1;

    cbus_setup();

    IPR3 = 0;					// All IRQs low priority for now
    IPR2 = 0;
    IPR1 = 0;
    PIE3 = 0b00100001;			// CAN TX error and FIFOWM interrupts
    PIE2 = 0;
    PIE1 = 0;
    INTCON3 = 0;
    INTCON2 = 0;                // Port B pullups are enabled
    INTCON = 0;
    PIR3 = 0;
    PIR2 = 0;
    PIR1 = 0;
    RCONbits.IPEN = 1;			// enable interrupt priority levels

    ovld_delay = 0;
    bit_flag_s = 6;			    // idle state
    bit_flag_m = 6;			    // idle state
    dcc_flags.word = 0;
    dcc_flags.dcc_rdy_m = 1;
    dcc_flags.dcc_rdy_s = 1;

    // Clear the refresh queue
    for (i = 0; i < MAX_HANDLES; i++) {
        q_queue[i].status.byte = 0;
        q_queue[i].address.addr_int = 0;
        q_queue[i].speed = 0x80;
        q_queue[i].fn1 = 0;
        q_queue[i].fn2 = 0;
        q_queue[i].fn2a = 0;
        q_queue[i].timeout = 0;
    }
    q_idx = 0;
    q_state = 0;

    // Clear the send queue
    for (i = 0; i < 16; i++) {
        s_queue[i].status.byte = 0;
        s_queue[i].d[0] = 0;
        s_queue[i].d[1] = 0;
        s_queue[i].d[2] = 0;
        s_queue[i].d[3] = 0;
        s_queue[i].d[4] = 0;
        s_queue[i].d[5] = 0;
        s_queue[i].repeat = 0;
    }
    s_head = 0;
    s_tail = 0;

    // clear the fifo receive buffers
    while (ecan_fifo_empty() == 0) {
        rx_ptr->con = 0;
    }

    mode_word.byte = 0;

    cmd_rmode();			    // read mode & current limit
    // check for magic value and set defaults if not found
    if (ee_read(EE_MAGIC) != 93)	{
        mode_word.byte = 0;
        imax = I_DEFAULT;
        cmd_wmode();            // Save default
    }

    // Bootloader interlock bit
//    if (mode_word.boot_en == 1) {
//        mode_word.boot_en = 0;
//        cmd_wmode();
//    }

    // Setup A/D - 1 i/ps with internal reference
    ADCON2 = 0b10000110;		// result right justified, Fosc/64
    ADCON1 = 0b00001110;	    // Internal Vref, AN0 analogue input
    ADCON0 = 0b00000001;	    // Channel 0, On
    ad_state = 0;
    iccq = 0;

    // Start slot timeout timer
    slot_timer = ((short long)500000)/58;  // Half second count down for 58uS interrupts

    // Set up TMR0 for DCC bit timing with 58us period prescaler 4:1,
    // 8 bit mode
   // T0CON = 0x40;  old value
    T0CON = 0x41;
    TMR0L = 0;
    TMR0H = 0;
    INTCONbits.TMR0IE = 1;
    T0CONbits.TMR0ON = 1;
    INTCON2bits.TMR0IP = 1;

    tmr0_reload = TMR0_NORMAL;
    // set 52us period if ZTC mode
    if (mode_word.ztc_mode) tmr0_reload = TMR0_ZTC;


    // Programmer state machine
    prog_state = CV_IDLE;

    // Clear current sense averager
    ave = 0;
    sum = 0;

    // Setup ID
    NN_temp = DEFAULT_NN;
    Tx1[con] = 0;
    Tx1[sidh] = 0b10110000 | (FIXED_CAN_ID & 0x78) >>3;
    Tx1[sidl] = (FIXED_CAN_ID & 0x07) << 5;

    // Setup TXB0 with high priority OPC_HLT
    TXB0SIDH = 0b01110000 | (FIXED_CAN_ID & 0x78) >>3;
    TXB0SIDL = (FIXED_CAN_ID & 0x07) << 5;
    TXB0DLC = 1;
    TXB0D0 = OPC_HLT;

    // enable interrupts
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;
}

// C intialisation - declare a copy here to avoid rts one being used which would link above the bootloader

void __init(void)

{
}
