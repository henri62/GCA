/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

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


#ifndef __IO_H
#define __IO_H

#define TRUE  1
#define FALSE 0
#define PORT_ON 1
#define PORT_OFF 0

// LEDs
#define LED1_PT      PORTBbits.RB0 // Accept programming commands
#define LED2_RUN     PORTBbits.RB1 // Also used for FLiM
#define LED3_BOOSTER PORTAbits.RA5 // Rail power ON/OFF
#define LED4_BUS     PORTBbits.RB4 // CBUS & LocoNet activity
#define LED5_ACK     PORTBbits.RB5 //

// Buttons
#define PB1_POWER    PORTCbits.RC5 // Track power
#define PB2_PT       PORTCbits.RC4 // Use of internal booster: Track/PT
#define PB3_FLIM     PORTCbits.RC3

// LocoNet
#define LNTX         PORTCbits.RC1
#define LNRX         PORTCbits.RC2

// CAN
#define CTX          PORTBbits.RB2
#define CRX          PORTBbits.RB3

// Buzzer
#define BUZZER       PORTCbits.RC0


// DCC
#define DCC_OUT_POS  PORTCbits.RC6
#define DCC_OUT_NEG  PORTCbits.RC7

// PT
#define DCC_PT_ACK    PORTAbits.RA0
#define DCC_NEG       PORTAbits.RA1
#define DCC_POS       PORTAbits.RA2
#define DCC_EN        PORTAbits.RA3



void setupIO(void);
void doLEDTimers(void);
void doLED250(void);
void doLEDs(void);


#endif
