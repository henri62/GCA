//
// CANCMD Programmer/Command Station (C) 2009 SPROG DCC
//	web:	http://www.sprog-dcc.co.uk
//	e-mail:	sprog@sprog-dcc.co.uk
//

#include "project.h"
#include "isr_high.h"
#include "cangc2.h"
#include "can_send.h"

#pragma udata access VARS
near unsigned short long slot_timer;

#pragma code APP

//
// Interrupt Service Routine
//
// TMR0 generates a heartbeat every 58uS to generate timing for the DCC bit
// stream. If no DCC packet to be transmitted then preamble is generated.
//
// One bit half cycle is 58usec. Zero bit half cycle is 116usec.
//
// A/D readings are stored in RAM.
//
#pragma interrupt isr_high
void isr_high(void) {
    // 13 clocks to get here after interrupt
    INTCONbits.T0IF = 0;
    TMR0L = tmr0_reload;

    //
    // TMR0 is enabled all the time and we send a continuous preamble
    // as required for booster operation. The power may, however, be off
    // resulting in no actual DCC ooutput
    //
    // Outputs are toggled here to set the output to the state
    // determined during the previous interrupt. This ensures the output
    // always toggles at the same time relative to the interrupt, regardless
    // of the route taken through the switch() statement for bit generation.
    //
    // This hardware does not use MOSFETs and does not need shoot-through
    // protection delay
    //


    //
    // Slot timeout and other timers - every half second
    //
    if (--slot_timer == 0) {
        slot_timer = ((short long)500000)/58;

      if (can_transmit_timeout != 0) {
        --can_transmit_timeout;
      }
    }
}
