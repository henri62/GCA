
/**
 *******************************************************************************************
 * @file       UserIo.c
 * @ingroup     UserIo                                                 
 * @attention
 *******************************************************************************************
 */

/*
 *******************************************************************************************
 * Standard include files
 *******************************************************************************************
 */
#include <string.h>
#include <inttypes.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include "UserIo.h"

/*
 *******************************************************************************************
 * Macro definitions
 *******************************************************************************************
 */

/* *INDENT-OFF* */

#define USER_IO_BLINK_RATE                1000 / USER_IO_TIMER_OVERFLOW_TIME  /**< Led blink rate is msec */
#define USER_IO_FLASH_TIME                50 / USER_IO_TIMER_OVERFLOW_TIME    /**< Led flash time */

#define USER_IO_EEP_IP_BASE_ADDRES        	0                                   /**< Ip address location in EEPROM */
#define USER_IO_EEP_IP_BASE_NETMASK       	4                                   /**< Netmask location in EEPROM */
#define USER_IO_EEP_MY_ID					8
#define USER_IO_EEP_MY_MAC					9

/*
 *******************************************************************************************
 * Types
 *******************************************************************************************
  */

/**
 * Typedef struct for led info */
typedef struct
{
   uint8_t          BlinkCnt;                           /**< Counter for led blink if active */
   TUserIoLedSet    Stat;                               /**< Set status for led */
}TUserIoLedStat;

/*
 *******************************************************************************************
 * Variables
 *******************************************************************************************
 */

TUserIoLedStat    UserIoLedStat[userIoLedMax];           /**< Led status array */

/* *INDENT-ON* */

/*
 *******************************************************************************************
 * Routines implementation
 *******************************************************************************************
 */

/**
 *******************************************************************************************
 * @fn         void userio_init(void)
 * @brief      Init the IO pins of the jumpers and leds.
 * @return     None
 * @attention  -
 *******************************************************************************************
 */
void userio_init(void)
{
   uint8_t                                 Index;

   DDRB |= (1 << PB1);
   PORTB |= (1 << PB1);
   DDRC |= (1 << PC3) | (1 << PC4) | (1 << PC5);
   PORTC |= (1 << PC0) | (1 << PC1) | (1 << PC2) | (1 << PC3) | (1 << PC4) | (1 << PC5);

   for (Index = 0; Index < userIoLedMax; Index++)
   {
      UserIoLedStat[Index].Stat = userIoLedSetOff;
   }

}

/**
 *******************************************************************************************
 * @fn         userIoJumperStatus UserIoGetJumperStatus(TUserIoJumper Jumper)
 * @brief      Read the status of a jumper.
 * @param      Jumper Jumper to read.
 * @return     userIoJumperStatus
 * @attention  -
 *******************************************************************************************
 */
TUserIoJumperStatus UserIoGetJumperStatus(TUserIoJumper Jumper)
{
   TUserIoJumperStatus                     Result = userIoJumperStatusOpen;

   switch (Jumper)
   {
      case userIoJumper1:
         if (!(PINC & (1 << PINC0)))
         {
            Result = userIoJumperStatusClosed;
         }
         break;
      case userIoJumper2:
         if (!(PINC & (1 << PINC1)))
         {
            Result = userIoJumperStatusClosed;
         }
         break;
      case userIoJumper3:
         if (!(PINC & (1 << PINC2)))
         {
            Result = userIoJumperStatusClosed;
         }
         break;
   }
   return (Result);
}

/**
 *******************************************************************************************
 * @fn         void userio_set_led(TUserIoLed Led,TUserIoLedSet Set)
 * @brief      Set a led to a required status.
 * @param      Led Led to be set.
 * @param      Set How the led must be set.
 * @attention  -
 *******************************************************************************************
 */
void userio_set_led(TUserIoLed Led, TUserIoLedSet Set)
{
   switch (Led)
   {
      case userIoLed4:
         switch (Set)
         {
            case userIoLedSetOff:
               PORTC |= (1 << PC3);
               break;
            case userIoLedSetOn:
               PORTC &= ~(1 << PC3);
               break;
            case userIoLedSetBlink:
               UserIoLedStat[userIoLed4].BlinkCnt = 0;
               break;
            case userIoLedSetToggle:
               PORTC ^= (1 << PC3);
               break;
            case userIoLedSetFlash:
               PORTC &= ~(1 << PC3);
               UserIoLedStat[userIoLed4].BlinkCnt = 0;
               break;
         }
         break;
      case userIoLed5:
         switch (Set)
         {
            case userIoLedSetOff:
               PORTC |= (1 << PC4);
               break;
            case userIoLedSetOn:
               PORTC &= ~(1 << PC4);
               break;
            case userIoLedSetBlink:
               UserIoLedStat[userIoLed5].BlinkCnt = 0;
               break;
            case userIoLedSetToggle:
               PORTC ^= (1 << PC4);
               break;
            case userIoLedSetFlash:
               PORTC &= ~(1 << PC4);
               UserIoLedStat[userIoLed5].BlinkCnt = 0;
               break;
         }
         break;
      case userIoLed6:
         switch (Set)
         {
            case userIoLedSetOff:
               PORTC |= (1 << PC5);
               break;
            case userIoLedSetOn:
               PORTC &= ~(1 << PC5);
               break;
            case userIoLedSetBlink:
               UserIoLedStat[userIoLed6].BlinkCnt = 0;
               break;
            case userIoLedSetToggle:
               PORTC ^= (1 << PC5);
               break;
            case userIoLedSetFlash:
               PORTC &= ~(1 << PC5);
               UserIoLedStat[userIoLed6].BlinkCnt = 0;
               break;
         }
         break;
      case userIoLedMax:
         break;
   }
   UserIoLedStat[Led].Stat = Set;
}

void userio_set_id(uint8_t* id)
{
	eeprom_write_byte((uint8_t *) USER_IO_EEP_MY_ID, *id);
}

void userio_set_ip(uint8_t * IpAddress, uint8_t * NetMask, uint8_t * MacAddress)
{
	uint8_t                                 Index;
	for (Index = 0; Index < 4; Index++)
	{
		eeprom_write_byte((uint8_t *) (USER_IO_EEP_IP_BASE_ADDRES + Index),IpAddress[Index]); 
		eeprom_write_byte((uint8_t *) (USER_IO_EEP_IP_BASE_NETMASK + Index), NetMask[Index]); 
	}
	for (Index = 0; Index < 6; Index++)
	{
		eeprom_write_byte((uint8_t *) (USER_IO_EEP_MY_MAC + Index),MacAddress[Index]); 
	} 	
	
}

/**
 *******************************************************************************************
 * @fn         void userio_get_settings(uint8_t * IpAddress, uint8_t * NetMask, uint8_t * MacAddress, uint8_t * MyId)
 * @brief      Read the IP Address. In future this function may be extended with reading from
 *             EEPROM.
 * @param      *IpAddress Pointer to array where IP address must be stored.
 * @param      *NetMask Pointer to array where the NetMask must be stored.
 * @attention  -
 *******************************************************************************************
 */
void userio_get_settings(uint8_t * IpAddress, uint8_t * NetMask, uint8_t * MacAddress, uint8_t * MyId)
{
	uint8_t                                 IpAddress_1[4] = { 192, 168, 0, 200 };
	uint8_t                                 IpAddress_2[4] = { 192, 168, 1, 200 };
	uint8_t                                 IpAddress_3[4] = { 192, 168, 100, 88 };
	uint8_t                                 NetMask_1[4] = { 255, 255, 255, 0 };
	uint8_t                                 NetMask_2[4] = { 255, 255, 255, 0 };
	uint8_t                                 NetMask_3[4] = { 255, 255, 255, 0 };
	uint8_t									Mac_Address[6] = { 0x00, 0x4D, 0x56, 0x31, 0x30, 0x31 };
	uint8_t                                 Jumpers;
	uint8_t                                 Index;

	*MyId = eeprom_read_byte((uint8_t *) USER_IO_EEP_MY_ID);

	Jumpers = PINC & 0x07;

	switch (Jumpers)
	{
	  case 0:
			memcpy(IpAddress, IpAddress_1, sizeof(IpAddress_1));
			memcpy(NetMask, NetMask_1, sizeof(NetMask_1));
			memcpy(MacAddress, Mac_Address, sizeof(Mac_Address));	
			break;
	  case 6:
			memcpy(IpAddress, IpAddress_2, sizeof(IpAddress_2));
			memcpy(NetMask, NetMask_2, sizeof(NetMask_2));
			memcpy(MacAddress, Mac_Address, sizeof(Mac_Address));	
			break;
	  case 5:
			memcpy(IpAddress, IpAddress_3, sizeof(IpAddress_3));
			memcpy(NetMask, NetMask_3, sizeof(NetMask_3));
			memcpy(MacAddress, Mac_Address, sizeof(Mac_Address));	
			break;
	  case 7:
	  default:
			for (Index = 0; Index < 4; Index++)
			{
				IpAddress[Index] = eeprom_read_byte((uint8_t *) (USER_IO_EEP_IP_BASE_ADDRES + Index)); 
				NetMask[Index] = eeprom_read_byte((uint8_t *) (USER_IO_EEP_IP_BASE_NETMASK + Index)); 
			}   
			
			for (Index = 0; Index < 6; Index++)
			{
				MacAddress[Index] = eeprom_read_byte((uint8_t *) (USER_IO_EEP_MY_MAC + Index)); 
			}  	
			if (IpAddress[0]== 0xFF && IpAddress[1]== 0xFF && IpAddress[2]== 0xFF && IpAddress[3]== 0xFF)
			{
				memcpy(IpAddress, IpAddress_1, sizeof(IpAddress_1));
				memcpy(NetMask, NetMask_1, sizeof(NetMask_1));	
				memcpy(MacAddress, Mac_Address, sizeof(Mac_Address));	
			}  
		break;
	}
   
 
}
