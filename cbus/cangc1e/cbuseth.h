
#ifndef CBUSETH_H
#define CBUSETH_H

#define CBUSETH_PORT            (5550)
#define MAX_CBUSETH_CONNECTIONS (4)
#define MAX_CBUSETH_CMD_LEN     (50)

#include "cbus.h"

void CBusEthTick(void);
void CBusEthInit(void);
void CBusEthServer(void);
unsigned char CBusEthBroadcast(CANMsg* msg);
unsigned char ethQueue(CANMsg* msg);


#endif
