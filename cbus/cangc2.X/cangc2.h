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


#define VERSION 1
#define EVT_NUM 16
#define EVperEVT 2
#define NV_NUM 16

// EEPROM addresses
#define EE_MAGIC 0
#define EE_NN 2
#define EE_SOD EE_NN + 2
#define EE_PORTCFG EE_SOD + 2

// values
#define MAGIC 93

extern near unsigned short NN_temp;
extern near unsigned short SOD;
extern near unsigned char Wait4NN;
extern volatile near unsigned char tmr0_reload;

#endif
