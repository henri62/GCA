
/**
 *******************************************************************************************
 * @file 		Serial_mega.c                                       
 * @ingroup     Serial
 *******************************************************************************************
 */

/*
 *******************************************************************************************
 * Standard include files
 *******************************************************************************************
 */

#include <avr\io.h>
#include <avr\pgmspace.h>
#include <avr\interrupt.h>
#include <inttypes.h>
#include "Serial_mega.h"

/* 
 *******************************************************************************************
 * Macro definitions
 *******************************************************************************************
 */

/*
 *******************************************************************************************
 * Variables
 *******************************************************************************************
 */

volatile uint8_t  gSerialOutBuffer[SERIALOUTSIZE]; /**< Tx buffer */
volatile uint8_t  *gSerialOutStart;                /**< Tx buffer start index */
volatile uint8_t  *gSerialOutEnd;                  /**< Tx bufffer end index */
volatile uint8_t  gSerialInBuffer[SERIALINSIZE];   /**< Rx Buffer */
volatile uint8_t  *gSerialInStart;                 /**< Rx buffer start index */
volatile uint8_t  *gSerialInEnd;                   /**< Rx buffer end index */

/*
 *******************************************************************************************
 * Local functions
 *******************************************************************************************
 */

/**
 *******************************************************************************************
 * @fn	    SIGNAL (SIG_USART_TRANS)   		
 * @brief   Transmit interrupt. 
 * @return	None.
 * @attention
 *******************************************************************************************
 */
SIGNAL(SIG_USART_TRANS)
{
   /* Sent byte if pointers are unequal */
   if (gSerialOutStart != gSerialOutEnd)
   {
      UDR0 = *gSerialOutStart;

      gSerialOutStart++;

      /* Check if pointer does not exceed buffer size */
      if (gSerialOutStart >= (gSerialOutBuffer + SERIALOUTSIZE))
      {
         gSerialOutStart = gSerialOutBuffer;
      }
   }
}

/**
 *******************************************************************************************
 * @fn	    SIGNAL (SIG_USART_RECV)   		
 * @brief   Receive interrupt. 
 * @return	None.
 * @attention
 *******************************************************************************************
 */
SIGNAL(SIG_USART_RECV)
{
   uint8_t                                 ErStat;

   /* First check if receive error occurred... */
   if ((UCSR0A & (1 << FE0)) || (UCSR0A & (1 << DOR0)))
   {
      ErStat = UDR0;
   }
   else
   {
      /* Read data out of UDR and place in receive buffer */
      *gSerialInEnd = UDR0;

      /* Increase pointer and check if overrun of buffer occurs */
      gSerialInEnd++;
      if (gSerialInEnd >= (gSerialInBuffer + SERIALINSIZE))
      {
         gSerialInEnd = gSerialInBuffer;
      }
   }
}

/*
 *******************************************************************************************
 * Exported functions
 *******************************************************************************************
 */

/**
 *******************************************************************************************
 * @fn	    	void Serial_Init(void)
 * @brief   	Init the UART of the Atmel to 8 bit 1 stopbit no parity. 
 * @return		None.
 * @attention 	Baudrate to be defined in Serial_mega.h 
 *******************************************************************************************
 */

void Serial_Init(void)
{
   gSerialOutStart = gSerialOutBuffer;                                         /* Pointer to write buffer */
   gSerialOutEnd = gSerialOutBuffer;
   gSerialInStart = gSerialInBuffer;
   gSerialInEnd = gSerialInBuffer;

   UCSR0A = 0;                                                                 /* no U2X, MPCM */
   UCSR0B = 0;
   UCSR0C = 0;
   UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);                                     /* 8 Bit */
   UBRR0L = BAUDDIVIDER19200;                                                  /* set baud rate */
   UBRR0H = 0;

}

/**
 *******************************************************************************************
 * @fn	    void Serial_Enable_Transmit_Interrupts(void)
 * @brief   Enable the serial transmit interrupt.   
 * @return	None.
 * @attention
 *******************************************************************************************
 */
void Serial_Enable_Transmit_Interrupts(void)
{
   UCSR0B |= (1 << TXEN0);                                                     /* enable transmit */
   UCSR0B |= (1 << TXCIE0);                                                    /* Enable int transmit */
}

/**
 *******************************************************************************************
 * @fn	    void Serial_Enable_Receive_Interrupts(void)
 * @brief   Enable the serial receive interrupt.   
 * @return	None.
 * @attention
 *******************************************************************************************
 */
void Serial_Enable_Receive_Interrupts(void)
{
   UCSR0B |= (1 << RXEN0);
   UCSR0B |= (1 << RXCIE0);
}

/**
 *******************************************************************************************
 * @fn	    void Serial_Buffer_Sent_Fill_Block(uint8_t *s)
 * @brief   Transmit an array to the serial port.
 * @param   *s Pointer to array to be transmitted.
 * @return	None.
 * @attention *2 must be NULL terminated!
 *******************************************************************************************
 */
void Serial_Buffer_Sent_Fill_Block(uint8_t * s)
{
   /* If buffer is empty, i.e. pointers equal, set first byte in transmit. This causes the UDR empty interrupt, so all 
    * data is written */

   if (gSerialOutEnd == gSerialOutStart)
   {
      /* Now check if nothing is present in transmit buffer */
      if (UCSR0A & (1 << UDRE0))
      {
         UDR0 = *s;                                                            /* First byte in UDR */
         s++;                                                                  /* Increase source pointer */
      }
   }

   /* Copy source into buffer until end of string */
   while (*s)
   {
      /* Store data from source into buffer */
      *gSerialOutEnd = *s;

      /* Increase pointers */
      s++;
      gSerialOutEnd++;

      /* and check if buffer does not overrun */

      if (gSerialOutEnd >= (gSerialOutBuffer + SERIALOUTSIZE))
      {
         gSerialOutEnd = gSerialOutBuffer;
      }
   }
}

/**
 *******************************************************************************************
 * @fn	    	void Serial_Buffer_Sent_FillFlash(const char *s)
 * @brief   	Transmit a string from progmem. 
 * @param[in]   *s String from progmem to be transmitted. 
 * @return		None.
 * @attention   String must be NULL terminated. 
 *******************************************************************************************
 */
void Serial_Buffer_Sent_FillFlash(const char *s)
{
   if (gSerialOutEnd == gSerialOutStart)
   {
      /* Now check if nothing is present in transmit buffer */
      if (UCSR0A & (1 << UDRE0))
      {
         UDR0 = pgm_read_byte(s);                                              /* First byte in UDR */
         s++;                                                                  /* Increase source pointer */
      }
   }

   /* Copy source into buffer until end of string */
   while (pgm_read_byte(s))
   {
      /* Store data from source into buffer */
      *gSerialOutEnd = pgm_read_byte(s);

      /* Increase pointers */
      s++;
      gSerialOutEnd++;

      /* and check if buffer does not overrun */

      if (gSerialOutEnd >= (gSerialOutBuffer + SERIALOUTSIZE))
      {
         gSerialOutEnd = gSerialOutBuffer;
      }
   }
}

/**
 *******************************************************************************************
 * @fn	    	uint8_t Serial_Buffer_Receive_Read_Byte(uint8_t * s)
 * @brief   	Read a character from the receive buffer. 
 * @param[in]   *s Pointer to store character if present.   
 * @return		0 = No data, 1 = data 
 * @attention    
 *******************************************************************************************
 */
uint8_t Serial_Buffer_Receive_Read_Byte(uint8_t * s)
{
   uint8_t                                 Res;

   Res = 0;

   if (gSerialInStart != gSerialInEnd)
   {
      /* Copy data , clear received data and increase pointers */
      *s = *gSerialInStart;
      Res = 1;

      gSerialInStart++;

      /* Check if readbuffer is not exceeded */
      if (gSerialInStart >= (gSerialInBuffer + SERIALINSIZE))
      {
         gSerialInStart = gSerialInBuffer;
      }
   }
   return (Res);
}
