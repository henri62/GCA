#ifndef _APP_H_
#define _APP_H_

/*****************************************************************************
*  "A Very Simple Application" from the uIP 0.9 documentation
*****************************************************************************/

/* UIP_APPSTATE_SIZE: The size of the application-specific state
   stored in the uip_conn structure. (not used, but has to at least be one) */
#define UIP_APPSTATE_SIZE 1

#include "uip.h"
#include "EthLocBuffer.h"

#define FS_STATISTICS 0

#define UIP_APPCALL     EthLocBufferTcpRcvEthernet

#endif
