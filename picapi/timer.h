#ifndef PICAPI_TIMER_H
#define PICAPI_TIMER_H

#include "types.h"

/*
 * Defaults to 4Mhz if not called.
 */
void initClk( uint clkMhz );

/*
 * Waits the amount of milli seconds.
 */
void waitMS(uint ms);

/*
 * Waits the amount of micro seconds.
 * The current implementation rounds it by 10.
 */
void waitUS(uint us);


#endif
