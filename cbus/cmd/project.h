#ifndef __PROJECT_H
#define __PROJECT_H

//
// CANCMD Programmer/Command Station (C) 2009 SPROG DCC
//	web:	http://www.sprog-dcc.co.uk
//	e-mail:	sprog@sprog-dcc.co.uk
//
//	Modified (c) Pete Brownlow 31/3/11 to add status outputs on spare pins for scope debugging,
//                                 introduce conditional compiler directive for BC1a
//								   and add definitions used by node parameters
//				  Mike Bolton	19/4/11 Modified for 32 MHz clock
//				Pete Brownlow	27/6/11 Add CAN transmit error checking, beep twice if unable to transmit on CAN
//										Output bridge enable turned off during overload


// Uncomment to build for the appropriate hardware

#define CANCMD
// #define BC1a


// The device to be used

#ifdef BC1a
  #include <p18f4480.h>
#else
  #include <p18f2480.h>
#endif


// SPROG headers
#include "cancmd.h"
#include "commands.h"
#include "power_cmds.h"
#include "program_cmds.h"

// headers for this project
#include "isr_high.h"
#include "mode_cmds.h"
#include "packet_gen.h"

// for DCC debug packets
#define HAS_DCC

// CAN mode for setup
#define ECAN_MODE_2

// CBUS headers
// One Rx buffer and one Tx buffer must be defined before
// including cbus_common.h
#define TX1

// This device has a fixed CAN ID

// 06/04/11 Roger Healey - Put CANCMD constants inside #ifdef
//  					 - Add MODULE_ID as MTYP_CANCMD		

#include "cbus_common.h"
#include "can_send.h"
#include "cbusdefs7e.h"

#ifdef CANCMD
  #define MODULE_ID 	MTYP_CANCMD
#endif

#ifdef BC1a
  #define MODULE_ID		MTYP_CANBC1a
#endif

    
#define FIXED_CAN_ID 0x72
#define DEFAULT_NN 	0xFFFE
#define MAJOR_VER 	3
#define MINOR_VER 	'c'	// Minor version character

#define NV_NUM		0	// Number of node variables
#define EVT_NUM		0	// Number of event variables
#define EVperEVT	0	// Event variables per event



extern near unsigned char Latcount;
extern near unsigned short NN_temp;


// DEFINE INPUT AND OUTPUT PINS
// ??? NOTE - BC1a still wip, ddr and init not yet worked out 

//
// Port A analogue inputs and digital I/O
//

#ifdef BC1a
  // BC1a
  // RA1 is AN1 ack current sense for programming track (L6202)
  //
  #define OLOAD_DETECT	PORTAbits.RA4	// Overload detect digital input on BC1a

  #define PRTA_DDR		0xef
  #define PORTA_INIT	0x0
#else
  // CANCMD
  // RA0 is AN0 current sense
  //
  #define SW          	PORTAbits.RA2	// Flim switch
  #define AWD         	LATAbits.LATA1	// Sounder

  #define PORTA_DDR		0xf9
  #define PORTA_INIT	0x0
#endif

//
// Port B
// Note that RB2 and RB3 are canrx and cantx so not used as I/O pins
//           RB6 and RB7 are used as PGC and PGD but can also drive LEDs
//
#define LED1    		PORTBbits.RB6	// canbc1a does not have leds here,
#define LED2    		PORTBbits.RB7	// but set pins anyway for diagnostics

#ifdef BC1a
  // BC1a port b was mainly the old handsets, which are not used here
  #define PORTB_DDR		0x00
  #define PORTB_INIT	0x00
#else

  #define SWAP_OP 		PORTBbits.RB5	// Jumper for main on ext booster or on board

  #define PORTB_DDR		0x3f
  #define PORTB_INIT	0x00
#endif

//
// Port C DCC and diagnostic outputs
//
#ifdef BC1a
  #define DCC_PKT_PIN	PORTCbits.RC7		// (pin 26) For scope debug - Set during packet send (can sync scope on packet start)
  #define DCC_SVC_EN	PORTCbits.RC6		// Service track output enable
  #define DCC_SVC_POS	PORTCbits.RC5		// Service track pos DCC drive
  #define DCC_SVC_NEG	PORTCbits.RC4		// Service track neg DCC drive
  #define DCC_POS		PORTCbits.RC3	    // one side of booster H-bridge o/p   
  #define DCC_NEG		PORTCbits.RC2	    // other side o/p 
  #define OTHERFAULT 	PORTCbits.RC1		// Other fault output (turns on fault led and sounder)
  #define OVERFAULT		PORTCbits.RC0		// Overload output (turns on overlaod led and sounder)
  #define DCC_PORT		PORTC

  #define PORTC_DDR	0x10
  #define PORTC_INIT	0x0
#else //CANCMD
  #define OVERLOAD_PIN 	PORTCbits.RC7		// (pin 18) For scope debug - set bit when overload detected
  #define ISR_PIN		PORTCbits.RC6		// (pin 17) For scope debug - set bit when in high priority ISR
  #define DCC_PKT_PIN	PORTCbits.RC5		// (pin 16) For scope debug - Set during packet send (can sync scope on packet start)
  #define SHUTDOWN_N    PORTCbits.RC4
  #define DCC_OUT       PORTCbits.RC3
  #define DCC_NEG		PORTCbits.RC2	    // other side o/p 
  #define DCC_POS		PORTCbits.RC1	    // one side of booster H-bridge o/p 
  #define DCC_EN        PORTCbits.RC0
  #define DCC_PORT		PORTC

  #define PORTC_DDR	0x10
  #define PORTC_INIT	0x0
#endif

//
// Port D  (BC1a only)
//
// Note - pin 29, RD6 is connected to the clock drive output for the old
//				handsets, so should be set high impedance
//
#ifdef BC1a
  #define OPTION_J2		PORTDbits.RD3		// Option jumper (not used yet)
  #define OPTION_J3		PORTDbits.RD2		// Option jumper (not used yet)

  #define PORTD_DDR		0x10		
  #define PORTD_INIT	0x0
#endif

//
// Port E  (BC1a only)
//
#ifdef BC1a
  #define SW          	PORTEbits.RE0		// Flim switch

  #define PORTE_DDR		0x10		
  #define PORTE_INIT	0x0
#endif



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
#define TMR0_NORMAL	0x91
	// for ZTC mode we modify the bit time to 52us
	// So value is 0 - (208 - 3 - 17 - 2)/2 = -93 = 0xa3
	// Mod for 32 MHz.  was 0xA3
#define TMR0_ZTC	0x9D


#endif	// __PROJECT_H
