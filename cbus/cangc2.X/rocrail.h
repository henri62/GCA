#ifndef __ROCRAIL_H
#define __ROCRAIL_H


#define MANU_ROCRAIL 70
// Module types
#define MTYP_CANGC2  2 // 16 I/O

#define OPC_PNN    0xB6    // <NN hi><NN lo><manu id><module id><flags>

/* Requested at 20-09-2011. (Mike & Pete)
    So the reply to QNN from each node would be a new opcode PNN which would have
    the following format:

    #define OPC_PNN    0xB6     // <NN hi><NN lo><manu id><module id><flags>

    The manufacturer id, module id and flag byte are from the module parameters and
    are as defined for RQNP and RQNPN, see Appendix 1 of spec version 7h.

    With this change, NNACK will only be used in response to SNN.
*/

#endif
