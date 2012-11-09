/*
 Copyright (C) MERG CBUS

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


#include <p18cxxx.h>
#include "utils.h"

//#pragma code APP

/*
 * ee_read - read from data EEPROM
 */
unsigned char eeRead(unsigned short addr) {
  EEADR = addr % 256; /* Data Memory Address to read */
  EEADRH = addr / 256;
  EECON1bits.EEPGD = 0; /* Point to DATA memory */
  EECON1bits.CFGS = 0; /* Access program FLASH or Data EEPROM memory */
  EECON1bits.RD = 1; /* EEPROM Read */
  return EEDATA;
}

/*
 * ee_write - write to data EEPROM
 */
void eeWrite(unsigned short addr, unsigned char data) {
  INTCONbits.GIE = 0; // Disable interupts

  EECON1bits.EEPGD = 0; // Select the EEPROM memory
  EECON1bits.CFGS = 0; // Access the EEPROM memory
  EECON1bits.WREN = 1; // Enable writing
  EEADR = addr % 256; /* Data Memory Address to read */
  EEADRH = addr / 256;
  EEDATA = data; // Set the data
  EECON2 = 0x55; // Write initiate sequence
  EECON2 = 0xaa; // Write initiate sequence
  EECON1bits.WR = 1; // Start writing
  while (!PIR2bits.EEIF)
    ; // Wait for write to finish
  PIR2bits.EEIF = 0; // Clear EEIF bit

  INTCONbits.GIE = 1; // Enable interupts
}

/*
 * ee_read_short() - read a short (16 bit) word from EEPROM
 *
 * Data is stored in little endian format
 */
unsigned short eeReadShort(unsigned short addr) {
  unsigned short byte_addr = addr;
  unsigned short ret = eeRead(byte_addr++);
  ret = ret | ((unsigned short) eeRead(byte_addr) << 8);
  return ret;
}

/*
 * ee_write_short() - write a short (16 bit) data to EEPROM
 *
 * Data is stored in little endian format
 */
void eeWriteShort(unsigned short addr, unsigned short data) {
  unsigned short byte_addr = addr;
  eeWrite(byte_addr++, (unsigned char) data);
  eeWrite(byte_addr, (unsigned char) (data >> 8));
}

/*
 * A delay routine
 */
void delay(void) {
  unsigned char i, j;
  for (i = 0; i < 10; i++) { // dely     movlw    .10
    //          movwf    Count1
    for (j = 0; j <= 245; j++) // dely2    clrf    Count
      ; // dely1    decfsz    Count,F
    //          goto    dely1
    //          decfsz    Count1
  } //          bra        dely2
} //          return

/*
 * Long delay
 */
void lDelay(void) {
  unsigned char i;
  for (i = 0; i < 100; i++) { //ldely	movlw	.100
    //		movwf	Count2
    delay(); //ldely1	call	dely
    //		decfsz	Count2
  } //		bra		ldely1
  //
} //		return

void strToByte(unsigned char* s, unsigned char len, unsigned char* data) {
  unsigned char i;
  for (i = 0; i < len; i += 2) {
    unsigned char v1 = (s[i ] & 0x40) ? s[i ] - 0x37 : s[i ] - 0x30;
    unsigned char v2 = (s[i + 1] & 0x40) ? s[i + 1] - 0x37 : s[i + 1] - 0x30;
    data[i / 2] = (v1 << 4) + v2;
  }
}

