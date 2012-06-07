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


#include "utils.h"

//#pragma code APP

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


void strToByte( unsigned char* s, unsigned char len, unsigned char* data ) {
  unsigned char i;
  for( i = 0; i < len; i+=2 ) {
    unsigned char v1 = (s[i  ] & 0x40) ? s[i  ]-0x37:s[i  ]-0x30;
    unsigned char v2 = (s[i+1] & 0x40) ? s[i+1]-0x37:s[i+1]-0x30;
    data[i/2] = (v1<<4) + v2;
  }
}

