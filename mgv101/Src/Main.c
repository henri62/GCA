
/**
 *******************************************************************************************
 * @file 		main.c                                         
 * @ingroup     Main
 * @defgroup	Main Main : Main appliocation of the MGV101 Ethernet Loconet buffer.  
 * @author		Robert Evers                                                      
 *******************************************************************************************
 */

/*
 *******************************************************************************************
 * Standard include files
 *******************************************************************************************
 */
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "UserIo.h"
#include "enc28j60.h"
#include "EthLocBuffer.h"
#include "serial_mega.h"

#ifndef F_CPU
# 	error F_CPU NOT DEFINED!!!
#endif

/*
 *******************************************************************************************
 * Macro definitions
 *******************************************************************************************
 */

/*
 *******************************************************************************************
 * Types
 *******************************************************************************************
  */

/*
 *******************************************************************************************
 * Variables
 *******************************************************************************************
 */

/*
 *******************************************************************************************
 * Prototypes
 *******************************************************************************************
 */

/*
 *******************************************************************************************
 * Routines implementation
 *******************************************************************************************
 */

/**
 *******************************************************************************************
 * @fn	    	void main(void)		
 * @brief   	Main routine of MGV101 Ethernet Loconet  application.
 * @return		None
 * @attention	
 *******************************************************************************************
 */

int main(void)
{
   /* Set run led */
   UserIoInit();
   UserIoSetLed(userIoLed3, userIoLedSetBlink);

   if (UserIoGetJumperStatus(userIoJumper1) == userIoJumperStatusClosed)
   {
      /* Init serial port for userio screen */
      Serial_Init();
      Serial_Enable_Transmit_Interrupts();
      Serial_Enable_Receive_Interrupts();
   }

   /* Init the Ethernet Loconet interface */
   EthLocBufferInit();

   sei();

   while (1)
   {
      UserIoMain();
      EthLocBufferMain();
   }
}
