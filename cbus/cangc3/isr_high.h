#ifndef __ISR_H

//
// CANCMD Programmer/Command Station (C) 2009 SPROG DCC
//	web:	http://www.sprog-dcc.co.uk
//	e-mail:	sprog@sprog-dcc.co.uk
//

#define LONG_PRE	22	// Increased from 20 - PNB 16/4/11 to meet NMRA spec 
#define NORM_PRE	14

// ISR prototype 
extern void isr_high(void);
extern void isr_low(void);

extern unsigned short an0;
extern unsigned short retry_delay;
extern unsigned short sum;
extern unsigned short ave;

extern unsigned near char bit_flag_s;
extern unsigned near char dcc_bytes_s;
extern unsigned near char dcc_pre_s;
extern unsigned near char bit_flag_m;
extern unsigned near char dcc_bytes_m;
extern unsigned near char dcc_pre_m;

extern unsigned near short long slot_timer;

#define __ISR_H
#endif
