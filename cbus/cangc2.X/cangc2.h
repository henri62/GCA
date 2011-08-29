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

#ifndef __CANGC2_H
#define __CANGC2_H


#define MAJOR_VER 1
#define MINOR_VER 0
#define EVT_NUM 16
#define EVperEVT 2
#define NV_NUM 16

// EEPROM addresses
#define EE_MAGIC 0
#define EE_PORTCFG 2
#define EE_NN EE_PORTCFG + 16

// values
#define MAGIC 93

#define LED1 PORTBbits.RB6
#define LED2 PORTBbits.RB7


#define PORT1 PORTCbits.RC0
#define PORT2 PORTCbits.RC1
#define PORT3 PORTCbits.RC2
#define PORT4 PORTCbits.RC3
#define PORT5 PORTCbits.RC7
#define PORT6 PORTCbits.RC6
#define PORT7 PORTCbits.RC5
#define PORT8 PORTCbits.RC4

#define PORT9  PORTAbits.RA0
#define PORT10 PORTAbits.RA1
#define PORT11 PORTAbits.RA3
#define PORT12 PORTAbits.RA4
#define PORT13 PORTAbits.RA5
#define PORT14 PORTBbits.RB0
#define PORT15 PORTBbits.RB4
#define PORT16 PORTBbits.RB1



extern near unsigned short NN_temp;
extern volatile near unsigned char tmr0_reload;

#endif
