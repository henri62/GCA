/*
 Rocrail - Model Railroad Software

 Copyright (C) Rob Versluis <r.j.versluis@rocrail.net>

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

#ifndef __FY_H
#define __FY_H

#define MAX_TRACKS    11  /* adjust value to the number of FY tracks */


#define cl_puls       RA0
#define cw_ccw        RA1
#define power         RA2
#define buttonNext    RA3    /* input */
#define displayH      RA4
#define displayL      RA5

#define display       PORTB  /* bits 3 2 1 0; 7 segement nibble */
#define dataIn        PORTB  /* bits 7 6 5 4; new position nibble */

#define microBegin    RC0    /* begin position switch */
#define microEnd      RC1    /* eind position switch */
#define newPos        RC2    /* new position is set in dataIn */
#define ledOK         RC3    /* UI LED */
#define buttonSave    RC4
#define buttonLeft    RC5
#define buttonRight   RC6
#define buttonSetup   RC7

/* values for target */
#define BUTTON_ON     0
#define BUTTON_OFF    1

#define INPUT_ON     1
#define INPUT_OFF    0

#define STEP_RIGHT    0
#define STEP_LEFT     1

#define POWER_ON      0
#define POWER_OFF     1

#define KEEPPOWER_ON  1
#define KEEPPOWER_OFF 0

#define LEDOK_ON      1
#define LEDOK_OFF     0

#define STATE_STARTUP 1
#define STATE_INIT    2
#define STATE_SETUP   3
#define STATE_OPERATE 4


#define ADDR_KEEPPOWER 0x32

#define ERR_OK                 0
#define ERR_TIMEOUT_AUTOMODE   1
#define ERR_TIMEOUT_BUTTONSAVE 2
#define ERR_TIMEOUT_BUTTONNEXT 2




#endif 
