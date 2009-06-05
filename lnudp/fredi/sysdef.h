/****************************************************************************
    Copyright (C) 2006 Olaf Funke

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

******************************************************************************/
#ifndef _SYSDEF_H_
#define _SYSDEF_H_

#define wBOARD_FREDI
//#define SW_INDEX        0x0001  // Software aus Wehmingen 05
//#define SW_INDEX        0x0002  // Software aus Hotteln 05
//#define SW_INDEX        0x0003  // Software nach Braunlage 06 (22.04.06)
                                  // Bugfix: reconnect (blinking forever)
                                  //         poti version works like Fred
                                  //         increment doesn't stop anymore
//#define SW_INDEX        0x0004  // Software für Cloppenburg 06 (14.05.06)
                                  // Bugfix: ignore of speedmessage after change
                                  //         dir by dirswitch

//#define SW_INDEX        0x0005  // Software nach Cloppenburg 06 (21.05.06)
                                  //          state cleaned up.
                                  
//#define SW_INDEX        0x0006  // Software Test mit Chief (03.06.06)
                                  // Bugfix: - state THR_STATE_RECONNECT_GET_SLOT 
                                  //           was missing in error handling
                                  //         - state corrected in case of not
                                  //           sending message successfully

//#define SW_INDEX          0x0007  // Software with decoder type encoding (22.08.06)
                                  
//#define SW_INDEX          0x0008  // Software with bugfix analog functions (26.08.06)

//#define SW_INDEX            0x0100  // software for release (18.09.06)

//#define SW_INDEX            0x0101  // T.Kurz: Software for release with invertion of dir
									                    // and differend speed tables
									                    // Red led is on, when loco is stoped (25.11.06)

//#define SW_INDEX            0x0102  // software for release (04.05.07)
                                     // Bugfix: no increment steps while stop is pressed
																		 // Bugfix: red led is running on selftest again
                                  
//#define SW_INDEX            0x0103  // software for release (19.08.07)
																		 // Removed the extension of Thomas, cause of problems on 
																		 // programming FREDIs with older SW versions

//#define SW_INDEX            0x0104  // software for release (19.10.07)
																		 // adjust of LN TX routine added
																		 // fixed problems on large arrangements

#define SW_INDEX            0x0105  // software for release (09.12.07)
																		 // changed high and low byte for ID



                                  
#define SW_DAY            0x09
#define SW_MONTH          0x12
#define SW_YEAR           0x07

/******************************************************************************/
// timing
/******************************************************************************/

/******************************************************************************/
// timing
/******************************************************************************/


#define F_CPU          			  7372800		// Fredi

#define LN_TIMER_TX_RELOAD_ADJUST   106 //  14,4 us delay for FREDI


#define TIMER_TICK_FREQUENCY        1000L // 1000kHz = 1ms tick size
#define TIMER_TICK_PERIOD_MS        (1000/TIMER_TICK_FREQUENCY) // Tick Period in ms

#define TIMER_FAST	1
#define TIMER_SLOW	0


#define TIMER_TICK_FREQUENCY        1000L // 1000kHz = 1ms tick size
#define TIMER_TICK_PERIOD_MS        (1000/TIMER_TICK_FREQUENCY) // Tick Period in ms

#define TIMER_FAST	1
#define TIMER_SLOW	0


#if (TIMER_TICK_FREQUENCY==1000L)  // 1ms

  // fast timer                    // base 1ms
	#define INCREMENT_TIME				  5   // 5ms
	#define KEY_POLL_TIME       	 10   // 10ms
  
	// slow timer									   // base 100ms
	#define LED_BLINK_TIME 		      1 	// 100ms
	#define LED_SELFTEST_TIME       4   // 400ms
	#define LED_SELFTEST_DONE_TIME  1   // 100ms
	#define MESSAGE_TIME				    3   // 300ms
  #define RESPONSE_TIME          20   // 2s
  #define SPEED_TIME				    250   // 25s
	#define RELEASE_STOP_TIME       5   // 500ms

#else
	#error wrong timer tick frequency
#endif

#define LED_ON							 0 // 


/******************************************************************************/
// eeprom 
/******************************************************************************/


// use monitor in and output for monitoring
//#define USE_UART


/******************************************************************************/
// Hardware mapping
/******************************************************************************/

// PortB
#define KEYPIN_SHIFT	  PB0
#define KEYPIN_F4			  PB1
#define KEYPIN_F3			  PB2
#define KEYPIN_F2			  PB3
#define KEYPIN_F1			  PB4
#define KEYPIN_F0			  PB5

#define KEYPIN_PORT	    PORTB
#define KEYPIN_DDR	    DDRB
#define KEYPIN_PIN	    PINB

// PortC
#define ANA_SPD				  PC0
#define DIRSWITCH				PC1
#define LED_GREEN_R		  PC2
#define LED_GREEN_L		  PC3
#define LED_RED				  PC4
#define ANA_VERSION		  PC5

#define LED_PORT  		  PORTC
#define LED_DDR   		  DDRC

#define DIRSWITCH_PORT	PORTC
#define DIRSWITCH_DDR		DDRC
#define DIRSWITCH_PIN		PINC

// PortD
#define ENC_BIT_0 	    PD2
#define ENC_BIT_1 	    PD3
#define ENC_SWITCH      PD5

#define ENC_PORT  	    PORTD
#define ENC_DDR   	    DDRD
#define ENC_PIN   	    PIND


// defines for key mapping

#define KEYPIN_ALL 	  ( _BV(KEYPIN_F0) |\
										  	_BV(KEYPIN_F1) |\
										  	_BV(KEYPIN_F2) |\
										  	_BV(KEYPIN_F3) |\
										  	_BV(KEYPIN_F4) |\
										  	_BV(KEYPIN_SHIFT) )
                      
#define Key_F0 		      _BV(KEYPIN_F0)
#define Key_SHIFT	      _BV(KEYPIN_SHIFT)
#define Key_F1		      _BV(KEYPIN_F1)
#define Key_F2		      _BV(KEYPIN_F2)
#define Key_F3		      _BV(KEYPIN_F3)
#define Key_F4		      _BV(KEYPIN_F4)
#define Key_F5          (Key_F1 | Key_SHIFT)
#define Key_F6          (Key_F2 | Key_SHIFT)
#define Key_F7          (Key_F3 | Key_SHIFT)
#define Key_F8          (Key_F4 | Key_SHIFT)
#define Key_Stop        0x40
#define Key_Dir         0x80
                      
#define Key_Enc_L 	    0x0100
#define Key_Enc_R 	    0x0200
#define Key_Poti_L 	    0x0400
#define Key_Poti_R 	    0x0800

#define Key_Fredi_Inkrement 				(KEYPIN_ALL | Key_Stop | Key_Enc_L  | Key_Enc_R )
#define Key_Fredi_Inkrement_Switch 	(KEYPIN_ALL | Key_Stop | Key_Enc_L  | Key_Enc_R  | Key_Dir)
#define Key_Fredi_Poti 							(KEYPIN_ALL | Key_Stop | Key_Poti_L | Key_Poti_R |Key_Dir)

#define ENC_BITS    ( _BV(ENC_BIT_0) | _BV(ENC_BIT_1) )


/******************************************************************************/
// other defines
/******************************************************************************/

#define EVENT_1  0x01
#define EVENT_2  0x02
#define EVENT_3  0x04
#define EVENT_4  0x08
#define EVENT_5  0x10
#define EVENT_6  0x20
#define EVENT_7  0x40
#define EVENT_8  0x80

#define EVENT_KEY 		EVENT_1
#define EVENT_LOCONET	EVENT_2
#define EV_MONITOR_RX EVENT_3


/******************************************************************************/
// eeprom 
/******************************************************************************/
enum EEPROM_ADR
{
  EEPROM_ID2,                     // changed by Olaf 09.12.2007
  EEPROM_ID1,
  EEPROM_ADR_LOCO_HB,
  EEPROM_ADR_LOCO_LB,
  EEPROM_DECODER_TYPE,
  EEPROM_IMAGE,
  EEPROM_VERSION,
  EEPROM_SW_INDEX_HB,
  EEPROM_SW_INDEX_LB,
  EEPROM_SW_DAY,
  EEPROM_SW_MONTH,
  EEPROM_SW_YEAR,
  EEPROM_ADR_LAST,
};

#define  EEPROM_ID1_DEFAULT			      0x00
#define  EEPROM_ID2_DEFAULT			      0x00

#define  EEPROM_DECODER_TYPE_DEFAULT	0x00  // 28 Step decoder

#define  EEPROM_IMAGE_DEFAULT         0x55



/******************************************************************************/
// other
/******************************************************************************/

#ifndef __PBYTE_DEFINED
#define __PBYTE_DEFINED
typedef	unsigned char*		pbyte;
#endif

#ifndef __TSHORT_DEFINED
#define __TSHORT_DEFINED
typedef	signed char		  tSHORT;
typedef	signed char*		tPSHORT;
#endif

#ifndef __TBOOL_DEFINED
#define __TBOOL_DEFINED
typedef	unsigned char		  tBOOL;
typedef	unsigned char*		tPBOOL;
#endif


#ifndef __BOOLEAN_DEFINED
#define __BOOLEAN_DEFINED
#define FALSE                     0x00
#define TRUE                      0x01
#endif

#ifndef __HIBYTE_DEFINED
#define __HIBYTE_DEFINED
#define HIBYTE(a)                 (byte) ((a & 0xff00) >> 8)
#endif

#ifndef __LOBYTE_DEFINED
#define __LOBYTE_DEFINED
#define LOBYTE(a)                 (byte) ((a & 0x00ff))
#endif

#define OPC_SELFTEST			0xaf


#define BV(bit) _BV(bit)
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

#define LN_MAX_BUFFER_SIZE  240



#endif 
