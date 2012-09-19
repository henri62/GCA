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


#ifndef __HARDWARE_PROFILE_H
#define __HARDWARE_PROFILE_H

#include <p18f2585.h>

#define GetSystemClock()               (32000000ul)
#define GetInstructionClock()          (GetSystemClock()/4)
#define GetPeripheralClock()           (GetSystemClock()/4)    // Should be GetSystemClock()/4 for PIC18

// ENC28J60 I/O pins
#define ENC_RST_TRIS            (TRISAbits.RA4)
#define ENC_RST_IO              (LATAbits.LATA4)
#define ENC_CS_TRIS             (TRISAbits.RA5)
#define ENC_CS_IO               (LATAbits.LATA5)

#define ENC_SCK_TRIS            (TRISCbits.RC3)
#define ENC_SDI_TRIS            (TRISCbits.RC4)
#define ENC_SDO_TRIS            (TRISCbits.RC5)
#define ENC_SPI_IF              (PIR1bits.SSPIF)
#define ENC_SSPBUF              (SSPBUF)
#define ENC_SPISTAT             (SSPSTAT)
#define ENC_SPISTATbits         (SSPSTATbits)
#define ENC_SPICON1             (SSPCON1)
#define ENC_SPICON1bits         (SSPCON1bits)
#define ENC_SPICON2             (SSPCON2)



#endif	// __HARDWARE_PROFILE_H
