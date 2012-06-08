
/* *INDENT-OFF* */
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include "serial_mega.h"

#define TRUE 0
#define FALSE 1

volatile uint8_t gSerialOutBuffer[SERIALOUTSIZE];
volatile uint8_t *gSerialOutStart;
volatile uint8_t *gSerialOutEnd;
volatile uint8_t gSerialInBuffer[SERIALINSIZE];
volatile uint8_t *gSerialInStart;
volatile uint8_t *gSerialInEnd;

/*Interrupt vector declarations*/

/******************************************************************/
/* Function name 	: SIG_UART_DATA								  */
/* 																  */
/* Return value		: None										  */
/*																  */
/* Pre condition  	: UART present on chip						  */
/*																  */
/* Post condition	: data in sent buffer placed in UDR  		  */	
/*					  register so it will be transmitted 		  */
/*																  */
/* Remark			: None										  */
/*		   														  */
/******************************************************************/

SIGNAL (SIG_UART_TRANS)
{
	/*Sent byte if pointers are unequal*/
	if (gSerialOutStart != gSerialOutEnd)
	{
		UDR = *gSerialOutStart;
		
		gSerialOutStart++;
	
		/*Check if pointer does not exceed buffer size*/
		if ( gSerialOutStart >= (gSerialOutBuffer + SERIALOUTSIZE))
		{
			gSerialOutStart = gSerialOutBuffer;
		}	
	}
}

/******************************************************************/
/* Function name 	: SIG_UART_RECV								  */
/* 																  */
/* Return value		: None										  */
/*																  */
/* Pre condition  	: UART present on chip						  */
/*																  */
/* Post condition	: EOF counter set to zero					  */
/*					  data in UDR register placed in serial		  */	
/*					  receive buffer							  */
/*																  */
/* Remark			: None										  */
/*		   														  */
/******************************************************************/


SIGNAL (SIG_UART_RECV)
{
	uint8_t ErStat;

	ErStat = 0;

    /* First check if no receive error occurred... */
	if ((UCSRA & (1<<FE)) || (UCSRA & (1<<DOR)))
	{
	   ErStat = 1;
    }

    /* If receive error, do dummy read, else store received info 	 */
    if (ErStat)
	{
	   ErStat = UDR;
	}
	else
	{
	   /*Read data out of UDR and place in receive buffer*/
	   *gSerialInEnd = UDR;   

    	/*Increase pointer and check if overrun of buffer occurs*/
	    gSerialInEnd ++;
		if (gSerialInEnd >= (gSerialInBuffer + SERIALINSIZE))
		{
			gSerialInEnd = gSerialInBuffer;
		}
	}
}

/******************************************************************/
/* Function name 	: Serial_Init_19200()						  */
/* 																  */
/* Return value		: None										  */
/*																  */
/* Pre condition  	: UART present on chip						  */
/*																  */
/* Post condition	: UART tried to be set @19200b 8 bit, 		  */	
/* 					  1 start-,1 stopbit						  */
/*																  */
/* Remark			: None										  */
/*		   														  */
/******************************************************************/

void Serial_Init_19200_Baud(void)
{
  	gSerialOutStart = gSerialOutBuffer;					/*Pointer to write buffer*/
  	gSerialOutEnd = gSerialOutBuffer;
  	gSerialInStart = gSerialInBuffer;					/*Pointer to read buffer*/
  	gSerialInEnd = gSerialInBuffer;
  	
    UCSRA = 0;										/*no U2X, MPCM*/
  	UCSRC = (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0);	/*8 Bit */
  	UBRRL = BAUDDIVIDER19200; 						/*set baud rate*/
	UBRRH = 0;

}

/******************************************************************/
/* Function name 	: Serial_Enable_Transmit_Interrupts			  */
/* 																  */
/* Return value		: None										  */
/*																  */
/* Pre condition  	: UART present on chip						  */
/*																  */
/* Post condition	: Tx interrupt set							  */
/*																  */
/* Remark			: Interrupts active after SEI() command!      */
/*		   														  */
/******************************************************************/



void Serial_Enable_Transmit_Interrupts(void)
{
	UCSRB |= (1<<TXEN);			/*enable transmit*/
	UCSRB |= (1<<TXCIE);			/*Enable int transmit*/
}

/******************************************************************/
/* Function name 	: Serial_Buffer_Sent_Fill()					  */	
/* 																  */
/* Return value		: None										  */
/*																  */
/* Pre condition  	: UART present on chip						  */
/*																  */
/* Post condition	: Copy source into sent buffer				  */
/*																  */
/* Remark			: Source is pointer or constant string.	      */
/*					  Not written data in buffer will be 		  */
/* 					  overwritten if source is too big.			  */	
/*		   														  */
/******************************************************************/

void Serial_Buffer_Sent_Fill_Block(uint8_t *s)
{
	/*If buffer is empty, i.e. pointers equal, set first byte in transmit. 
	  This causes the UDR empty interrupt, so all data is written*/
	  
	if (gSerialOutEnd == gSerialOutStart)
	{
	    /* Now check if nothing is present in transmit buffer */
		if (UCSRA & (1<<UDRE))
		{
		   UDR=*s;     /*First byte in UDR*/
		   s++;		/*Increase source pointer*/
        }
	}
	
	/*Copy source into buffer until end of string*/
	while (*s)
	{
		/*Store data from source into buffer*/
		*gSerialOutEnd = *s;

		/*Increase pointers*/
		s++;
		gSerialOutEnd ++;
			
		/*and check if buffer does not overrun*/
		
		if (gSerialOutEnd >= (gSerialOutBuffer + SERIALOUTSIZE))
		{
			gSerialOutEnd = gSerialOutBuffer;
		}
	}
}		

/******************************************************************/
/* Function name 	: Serial_Buffer_Sent_FillFlash()			  */	
/* 																  */
/* Return value		: None										  */
/*																  */
/* Pre condition  	: UART present on chip						  */
/*																  */
/* Post condition	: Copy source in FLASH into sent buffer		  */
/*																  */
/* Remark			: Source is pointer to string in flash.	      */
/******************************************************************/

void Serial_Buffer_Sent_FillFlash(const char *s)
{
	if (gSerialOutEnd == gSerialOutStart)
	{
		/* Now check if nothing is present in transmit buffer */
		if (UCSRA & (1<<UDRE))
		{
           UDR=pgm_read_byte(s);     /*First byte in UDR*/
		   s++;		/*Increase source pointer*/
	    }
	}

	/*Copy source into buffer until end of string*/
	while (pgm_read_byte(s))
	{
		/*Store data from source into buffer*/
		*gSerialOutEnd = pgm_read_byte(s);

		/*Increase pointers*/
		s++;
		gSerialOutEnd ++;
			
		/*and check if buffer does not overrun*/
		
		if (gSerialOutEnd >= (gSerialOutBuffer + SERIALOUTSIZE))
		{
			gSerialOutEnd = gSerialOutBuffer;
		}
	}
}

/******************************************************************/
/* Function name 	: Clear_Rec_And_Sent_Buf					  */
/* 																  */
/* Return value		: None										  */
/*																  */
/* Pre condition  	: None										  */
/*																  */
/* Post condition	: Serial Sent and Receive buffer cleared 	  */
/*																  */
/* Remark			: None										  */
/*		   														  */
/******************************************************************/
  
void Clear_Rec_And_Sent_Buf(void)
{
	unsigned int  lIndex;
	
	for (lIndex = 0; lIndex < SERIALINSIZE; lIndex++)
	{
		gSerialInBuffer[lIndex] = '\0';
	}
	
	for (lIndex = 0; lIndex < SERIALOUTSIZE; lIndex++)
	{
		gSerialOutBuffer[lIndex] = '\0';
	}
	
}
/* *INDENT-ON* */
