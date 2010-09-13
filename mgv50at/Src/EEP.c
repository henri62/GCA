
/**
 ******************************************************************************
 * @file       EEP.c
 * @ingroup    EEP
 * @author     Robert Evers
 * @attention  Routine implementation EEPROM read / write functions.
 ******************************************************************************
 * Change history   : See CVS log
 ******************************************************************************
 */

/*
 ******************************************************************************
 * Standard include files
 ******************************************************************************
 */

#include <inttypes.h>
#include <avr\io.h>
#include "EEP.h"

 /* 
  ******************************************************************************
  * Macro definitions
  ******************************************************************************
  */

#define EEPROM __attribute__ ((section (".eeprom")))       /**< EEPROM definition */
#define nop()  __asm__ __volatile__ ("nop" ::)
 /* 
  ******************************************************************************
  * Types
  ******************************************************************************
  */

/* 
 ******************************************************************************
 * Prototypes
 ******************************************************************************
 */

/*
******************************************************************************
* Routines implementation
******************************************************************************
*/

/**
******************************************************************************
* @fn	      void EEP_write_byte(uint16_t address, uint8_t data)
* @brief    Write a byte into EEPROM memory.
* @param    address EEPROM aadress to be written.
* @param    data data to be written into EEPROM.
* @return   none
* @attention	
******************************************************************************
*/

void EEP_write_byte(uint16_t address, uint8_t data)
{
   while (EECR & (1 << EEWE))
   {
      nop();
   }
   EEAR = address;
   EEDR = data;
   EECR |= (1 << EEMWE);
   EECR |= (1 << EEWE);
}

/**
******************************************************************************
* @fn       uint8_t EEP_read_byte(uint16_t address)
* @brief    Read a byte from EEPROM memory.
* @param    address EEPROM aadress to be read from.
* @return   Data read from EEPROM address
* @attention	
******************************************************************************
*/
uint8_t EEP_read_byte(uint16_t address)
{
   while (EECR & (1 << EEWE))
   {
      nop();
   }

   /* Set up address register */
   EEAR = address;
   EECR |= (1 << EERE);
   return EEDR;
}

/**
******************************************************************************
* @fn       void EEP_write_word(uint16_t address, uint16_t data)
* @brief    Write a word into EEPROM memory.
* @param    address EEPROM aadress to be written.
* @param    data data to be written into EEPROM.
* @return   none
* @attention
******************************************************************************
*/

void EEP_write_word(uint16_t address, uint16_t data)
{
   EEP_write_byte(address, data & 0xFF);
   EEP_write_byte(address, data >> 8);

}

/**
******************************************************************************
* @fn       uint16_t EEP_read_byte(uint16_t address)
* @brief    Read a word from EEPROM memory.
* @param    address EEPROM aadress to be read from.
* @return   Data read from EEPROM address
* @attention
******************************************************************************
*/
uint16_t EEP_read_word(uint16_t address)
{
   uint16_t                                Data;

   Data = EEP_read_byte(address);
   Data |= (uint16_t) (EEP_read_byte(address + 1) << 8);

   return (Data);
}
