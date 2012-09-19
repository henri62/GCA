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
#include "project.h"
#include "io.h"
#include "gcaeth.h"
#include "utils.h"


void InitAppConfig(void);

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
  TickInit(); // The tick is initialized in cangc1e

  /*
   * Following steps must be performed for all applications using
   * PICmicro TCP/IP Stack.
   */
  /*
   * Initialize Stack and application related NV variables.
   */
  InitAppConfig();

  StackInit();

  CBusEthInit();
}


void InitAppConfig(void) {


  if( checkFlimSwitch() || eeRead(EE_CLEAN) == 0xFF ) {
    eeWrite(EE_CLEAN, 0);
    eeWrite(EE_IPADDR + 0, MY_DEFAULT_IP_ADDR_BYTE1);
    eeWrite(EE_IPADDR + 1, MY_DEFAULT_IP_ADDR_BYTE2);
    eeWrite(EE_IPADDR + 2, MY_DEFAULT_IP_ADDR_BYTE3);
    eeWrite(EE_IPADDR + 3, MY_DEFAULT_IP_ADDR_BYTE4);

    eeWrite(EE_NETMASK + 0, MY_DEFAULT_MASK_BYTE1);
    eeWrite(EE_NETMASK + 1, MY_DEFAULT_MASK_BYTE2);
    eeWrite(EE_NETMASK + 2, MY_DEFAULT_MASK_BYTE3);
    eeWrite(EE_NETMASK + 3, MY_DEFAULT_MASK_BYTE4);

    eeWrite(EE_MACADDR + 0, MY_DEFAULT_MAC_BYTE1);
    eeWrite(EE_MACADDR + 1, MY_DEFAULT_MAC_BYTE2);
    eeWrite(EE_MACADDR + 2, MY_DEFAULT_MAC_BYTE3);
    eeWrite(EE_MACADDR + 3, MY_DEFAULT_MAC_BYTE4);
    eeWrite(EE_MACADDR + 4, MY_DEFAULT_MAC_BYTE5);
    eeWrite(EE_MACADDR + 5, MY_DEFAULT_MAC_BYTE6);
  }


    /*
     * Load default configuration into RAM.
     */
    AppConfig.MyIPAddr.v[0]     = eeRead(EE_IPADDR + 0);
    AppConfig.MyIPAddr.v[1]     = eeRead(EE_IPADDR + 1);
    AppConfig.MyIPAddr.v[2]     = eeRead(EE_IPADDR + 2);
    AppConfig.MyIPAddr.v[3]     = eeRead(EE_IPADDR + 3);

    AppConfig.MyMask.v[0]       = eeRead(EE_NETMASK + 0);
    AppConfig.MyMask.v[1]       = eeRead(EE_NETMASK + 1);
    AppConfig.MyMask.v[2]       = eeRead(EE_NETMASK + 2);
    AppConfig.MyMask.v[3]       = eeRead(EE_NETMASK + 3);

    AppConfig.MyGateway.v[0]    = AppConfig.MyMask.v[0];
    AppConfig.MyGateway.v[1]    = AppConfig.MyMask.v[1];
    AppConfig.MyGateway.v[2]    = AppConfig.MyMask.v[2];
    AppConfig.MyGateway.v[3]    = AppConfig.MyMask.v[3];

    AppConfig.MyMACAddr.v[0]    = eeRead(EE_MACADDR + 0);
    AppConfig.MyMACAddr.v[1]    = eeRead(EE_MACADDR + 1);
    AppConfig.MyMACAddr.v[2]    = eeRead(EE_MACADDR + 2);
    AppConfig.MyMACAddr.v[3]    = eeRead(EE_MACADDR + 3);
    AppConfig.MyMACAddr.v[4]    = eeRead(EE_MACADDR + 4);
    AppConfig.MyMACAddr.v[5]    = eeRead(EE_MACADDR + 5);

#if defined(STACK_USE_DHCP) || defined(STACK_USE_IP_GLEANING)
    AppConfig.Flags.bIsDHCPEnabled = TRUE;
#else
    AppConfig.Flags.bIsDHCPEnabled = FALSE;
#endif

}


void doEth(void) {
  /*
   * This task performs normal stack task including checking
   * for incoming packet, type of packet and calling
   * appropriate stack entity to process it.
   */
  StackTask();
//  StackApplications();    // no Applications defined
  CBusEthServer();
}