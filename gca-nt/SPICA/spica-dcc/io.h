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

#define PORT_ON 1
#define PORT_OFF 0

// LEDs
#define LED5_RUN     PORTBbits.RB0
#define LED6_PT      PORTAbits.RA4
#define OVERLOAD_PIN PORTAbits.RA4

// DCC
#define DCC_OUTA     PORTBbits.RB2
#define DCC_OUTB     PORTBbits.RB3
#define DCC_OUT_POS  PORTBbits.RB2
#define DCC_OUT_NEG  PORTBbits.RB3

// PT
#define DCC_PT_ACK    PORTAbits.RA0
#define DCC_PT_DCCA   PORTBbits.RB5
#define DCC_PT_DCCB   PORTAbits.RA2
#define DCC_PT_ENABLE PORTAbits.RA1
#define DCC_NEG       PORTBbits.RB5
#define DCC_POS       PORTAbits.RA2
#define DCC_EN        PORTAbits.RA1

#define PB2_PT        PORTBbits.RB7


void setupIO(void);


#endif
