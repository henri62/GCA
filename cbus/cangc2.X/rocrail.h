#ifndef __ROCRAIL_H
#define __ROCRAIL_H


#define MANU_ROCRAIL 70
// Module types
#define MTYP_CANGC2  2 // 16 I/O

#define OPC_QNTP   0x11    // Query node type
#define OPC_TYPE   0x9D    // Report manufacturer and product ID.

/* Requested at 20-09-2011. (Mike & Pete)
#define OPC_QNTP   0x0F    // Query node type
#define OPC_PNTP   0x8F    // Report manufacturer and product ID.

So the reply to QNN from each node would be a new opcode PNN which would have
the following format:

0xB6 <NN hi><NN lo><manu id><module id><flags>

The manufacturer id, module id and flag byte are from the module parameters and
are as defined for RQNP and RQNPN, see Appendix 1 of spec version 7h.

With this change, NNACK will only be used in response to SNN.


 */

#endif
