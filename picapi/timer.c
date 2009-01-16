#include "timer.h"

static byte POLLING_PERIOD = 250;    //with 4Mhz processor, 250us   
static byte TMR0_PRESCALER = 1;      //gives 1:4 prescaler


void initClk( uint clkMhz ) {

/*
  PS2:PS0: Prescaler Rate Select bits
  Bit Value TMR0 Rate WDT Rate
   000      1:2         1:1
   001      1:4         1:2
   010      1:8         1:4
   011      1:16        1:8
   100      1:32        1:16
   101      1:64        1:32
   110      1:128       1:64
   111      1:256       1:128
*/
  if( clkMhz == 4 ) {
    // If the micro is running at 4Mhz, it is executing instructions at clk/4 speed, or 1MIPS.  
    // This is 1 instruction every 1us.
    POLLING_PERIOD = 250;    //with 4Mhz processor, 250us   
    TMR0_PRESCALER = 1;      //gives 1:4 prescaler
  }
  else if( clkMhz == 20 ) {
    // If the micro is running at 20Mhz, it is executing instructions at clk/4 speed, or 5MIPS.  
    // This is 5 instruction every 1us.
    POLLING_PERIOD = 156;    //with 20Mhz processor, 156 / 5 = 31.2us 
    TMR0_PRESCALER = 4;      //gives 1:32 prescaler, 32 x 31.2 = 998.4us
  }
}


void waitMS(uint ms)
{
  uint i = 0;

  //the -3 factor is to make up for overhead
  //the 0xff- factor is because the timer counts *up* to 0xff
  //do not change this
  byte TMR0_SETTING = (0xff - (POLLING_PERIOD-3));

  OPTION_REG &= 0xC0;            //turn off bottom 6 bits to configure tmr0
  OPTION_REG |= TMR0_PRESCALER;  //set prescaler
  for( i=0; i < ms; i++ )
  {
    TMR0=TMR0_SETTING;
    T0IF=0;        
    while(T0IF==0);   //wait 1000us for flag to go high
    //OK, tmr0 has overflowed, flag T0IF has gone high
   }
}


void waitUS(uint us)
{
  uint i = 0;
  //the -3 factor is to make up for overhead
  //the 0xff- factor is because the timer counts *up* to 0xff
  //do not change this
  byte TMR0_SETTING = (0xff - (POLLING_PERIOD-3));

  OPTION_REG &= 0xC0;            //turn off bottom 6 bits to configure tmr0
  OPTION_REG |= TMR0_PRESCALER;  //set prescaler
  for( i=0; i < (us/10); i++ )
  {
    TMR0=TMR0_SETTING;
    T0IF=0;        
    while(T0IF==0);   //wait 10us for flag to go high
    //OK, tmr0 has overflowed, flag T0IF has gone high
   }
}

