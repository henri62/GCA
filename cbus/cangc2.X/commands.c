//
// CANCMD Programmer/Command Station (C) 2009 SPROG DCC
//	web:	http://www.sprog-dcc.co.uk
//	e-mail:	sprog@sprog-dcc.co.uk
//

// 06/04/11 Roger Healey - Modify code for OPC_BOOT
//						 - add thisNN routine
//						 - add code for OPC_RQNPN
//						 - add doError for command error response
#include "project.h"
#include "cbusdefs.h"
#include "commands.h"


#pragma udata access VARS

void cmd_cv(void);

#pragma code APP

//
// parse_cmd()
//
// Decode the OPC and call the function to handle it.
//
void parse_cmd(void) {
    //mode_word.s_full = 0;
    switch(rx_ptr->d0) {

        case OPC_RQNPN:
        	// Request to read a parameter
            if (thisNN() == 1)
	        {
		        doRqnpn((unsigned int)rx_ptr->d3);
		    }
		    break;

        default: break;
    }
}

void doRqnpn(unsigned int idx)
{
	if (idx < 8)
	{
		Tx1[d0] = OPC_PARAN;
		Tx1[d3] = idx;
		Tx1[d4] = params[idx-1];
		can_tx_nn(5);
	}
	else
	{
	doError(CMDERR_INV_PARAM_IDX);
	}
}

void doError(unsigned int code)
{
		Tx1[d0] = OPC_CMDERR;
		Tx1[d3] = code;
		can_tx_nn(4);
}

int thisNN()
{
 	if ((((unsigned short)(rx_ptr->d1)<<8) + rx_ptr->d2) == NN_temp)
	 	return 1;
	 else
	 	return 0;

}	
