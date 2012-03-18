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


#include "cangc1e.h"
#include "eth.h"
/*
 * This is used by other stack elements.
 * Main application must define this and initialize it with
 * proper values.
 */
APP_CONFIG AppConfig;

void initEth(void) {
    /*
   * Initialize all stack related components.
   * Following steps must be performed for all applications using
   * PICmicro TCP/IP Stack.
   */
  TickInit();

  /*
   * Following steps must be performed for all applications using
   * PICmicro TCP/IP Stack.
   */
  //MPFSInit();

  /*
   * Initialize Stack and application related NV variables.
   */
  //InitAppConfig();


	// Disable DHCP
	//SetConfig();


}
