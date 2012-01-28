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

/*
; This code is a sample UAR(no T) that receives 8 channels at 9600 baud on a 20Mhz 16F628A PIC
; The program works by handling timer interrupts at 3x the nominal bit rate and running a
; state machine for each channel to acquire the data.
;
; The interrupts are at the rate of:
;
;       20000000/4/174 == 28735.63 Hz.
 *      32000000/4/2/139 == 28776,98 Hz.
;
; The 'perfect' rate would be 9600*3 == 28800 Hz.
;
; The bit rate error is thus about 0.22 percent.
;
; The first trick behind this code is the way the state machines are run: instead of
; handling each channel one at a time the program uses 'vertical arithmetic' to
; process all eight channels together.
;
; The second trick is the way that the program works thru processing the eight
; input bits on the channels, accumulating them into the 'receiver shift register'
; variables, and determining when a byte has been completely received. This is done
; using only 3 instructions per channel.
;
; Using these two tricks results in code that uses only 76 instructions per interrupt,
; including context save and restore, but not interrupt latency. Since interrupts
; are generated every 174 instructions this leaves about 54 percent of the CPU available
; for 'task level' code.
;
; One important thing to note: since this code does _not_ double buffer the receiver,
; task level code must process a received byte within 4/3 of a bit time (approximately
; 390 task level instructions), otherwise the overrun flag will be set for that channel.

 */


#include "project.h"
#include "cangc4.h"
#include "rfid.h"

void scanRFID(void) {
  byte inc = PORTC;

}