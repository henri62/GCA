/*
 Rocrail - Model Railroad Software

 Copyright (C) Rob Versluis <r.j.versluis@rocrail.net>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3
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
/*
RB4/RX	 Read BiDi at 120 or 250kBaud
RB0	 Current detection, with 1.5s delay, and trigger for reading BiDi
RA0	 Software UART for sending data to the CAN-GC4
RA1	 LED1 5V
RA2	 LED2 Current detection
RA3	 LED3 BiDi scanned
RA4	 LED4 Data send to CAN-GC4
RB2	 Jumper for Lissy/RailCom
RA6/7	 8MHz Resonator
*/

#define LED1_RUN  PORTAbits.RA1
#define LED2_OCC  PORTAbits.RA2
#define LED3_BIDI PORTAbits.RA3
#define LED4_SER  PORTAbits.RA4

#define IN_TYPE   PORTBbits.RB2
#define IN_OCC    PORTBbits.RB0

#define OUT_SER   PORTAbits.RA0

#define PORT_ON  1
#define PORT_OFF 0


void initIO(void);

#endif

