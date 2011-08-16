#ifndef __CAN_SEND_H
#define __CAN_SEND_H

extern unsigned char can_transmit_timeout;
extern unsigned char can_transmit_failed;
extern unsigned char can_bus_off;

extern void can_opc(unsigned char opc);
extern void can_tx_nn(unsigned char dlc_val);
extern void can_tx(unsigned char dlc_val);
void can_bus_on(void);
void can_debug1(unsigned char status);
void can_debug_dcc(void);

#ifdef TX1
	void sendTX1(void);
#endif

#endif
