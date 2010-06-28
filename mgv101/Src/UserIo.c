
/**
 *******************************************************************************************
 * @file 		UserIo.c                                         
 * @ingroup     UserIo                                                 
 * @attention	
 *******************************************************************************************
 */

/*
 *******************************************************************************************
 * Standard include files
 *******************************************************************************************
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include "Version.h"
#include "Enc28j60.h"
#include "Serial_mega.h"
#include "EthLocBuffer.h"
#include "UserIo.h"

/*
 *******************************************************************************************
 * Macro definitions
 *******************************************************************************************
 */

#define USER_IO_TIMER_OVERFLOW_TIME       4                                   /**< Timer overflow in msec */
#define USER_IO_BLINK_RATE                500 / USER_IO_TIMER_OVERFLOW_TIME   /**< Led blink rate is msec */
#define USER_IO_FLASH_TIME                50 / USER_IO_TIMER_OVERFLOW_TIME    /**< Led flash time */
#define USER_IO_SCREEN_UPDATE             2000 / USER_IO_TIMER_OVERFLOW_TIME  /**< User Io update of screen */

#define USER_IO_EEP_IP_ADDRES_BASE        0                                   /**< Ip address location in EEPROM */
#define USER_IO_EEP_IP_ADDRES_LENGTH      4                                   /**< Ip address length */

#define USER_IO_RCV_LENGTH                10                                  /**< Max buffer size for serial in data */
#define USER_IO_CR                        0x0D                                /**< ASCII CR */

#define USER_IO_COMMAND_IP_1              "IP1="                              /**< Change IP addres digit 1 */
#define USER_IO_COMMAND_IP_2              "IP2="                              /**< Change IP addres digit 2 */
#define USER_IO_COMMAND_IP_3              "IP3="                              /**< Change IP addres digit 3 */
#define USER_IO_COMMAND_IP_4              "IP4="                              /**< Change IP addres digit 4 */

#define USER_UI_STR_SIZE                  25                                  /**< String size for info screen */
/*
 *******************************************************************************************
 * Types
 *******************************************************************************************
  */

/**
 * Typedef struct for led info */
typedef struct
{
	uint8_t 		BlinkCnt;                    /**< Counter for led blink if active */
	TUserIoLedSet 	Stat;                     /**< Set status for led */
}TUserIoLedStat;

/*
 *******************************************************************************************
 * Variables
 *******************************************************************************************
 */

TUserIoLedStat	   UserIoLedStat[userIoLedMax];          /**< Led status array */
uint16_t          UserIoScreenUpdateCnt;                /**< Counter for update of user screen */
uint8_t           UserIoRcvBuffer[USER_IO_RCV_LENGTH];  /**< Buffer for serial in data */
uint8_t           UserIoRcvCnt;                         /**< Counter for received serial in data */

/*
 *******************************************************************************************
 * Routines implementation
 *******************************************************************************************
 */

/**
 *******************************************************************************************
 * @fn	    	void UserIoProcessSerialData(uint8_t * Data)		
 * @brief   	Check the content of the received command, and if required perform
 *              an action. 
 * @param       Data   Pointer to the array with received data. 
 * @return		None
 * @attention	- 
 *******************************************************************************************
 */
void UserIoProcessSerialData(uint8_t * Data)
{
   uint8_t                                 IpAddress[4];
   uint8_t                                 IpDigitValue;

   if (strncmp((char *)Data, (char *)USER_IO_COMMAND_IP_1, strlen((char *)USER_IO_COMMAND_IP_1)) == 0)
   {
      IpDigitValue = atoi((char *)&Data[strlen((char *)USER_IO_COMMAND_IP_1)]);
      UserIoIpAddressGet(IpAddress);
      IpAddress[0] = IpDigitValue;
      UserIoIpAddressSet(IpAddress);
   }
}

/**
 *******************************************************************************************
 * @fn	    	void UserIoInfoScreen(void)		
 * @brief   	Display some info on the serial port (ASCII terminal).
 * @return		None
 * @attention	- 
 *******************************************************************************************
 */
void UserIoInfoScreen(void)
{
   char                                    DebugMsg[USER_UI_STR_SIZE];
   uint8_t                                 IpAddress[4];
   uint32_t                                LoconetCnt;
   uint32_t                                UdpCnt;

   Serial_Buffer_Sent_FillFlash(PSTR("\x1B[2J\0"));
   Serial_Buffer_Sent_FillFlash(PSTR("MGV101 Ethernet Loconet Buffer \r\n\0"));
   Serial_Buffer_Sent_FillFlash(PSTR("\r\n\0"));
   Serial_Buffer_Sent_FillFlash(PSTR("Version               : \0"));
   sprintf(DebugMsg, "%d.%d\r\n", SW_MAJOR, SW_MINOR);
   Serial_Buffer_Sent_Fill_Block((uint8_t *) DebugMsg);
   Serial_Buffer_Sent_FillFlash(PSTR("\r\n\0"));

   Serial_Buffer_Sent_FillFlash(PSTR("IP Address            : \0"));
   UserIoIpAddressGet(IpAddress);
   sprintf(DebugMsg, "%d.%d.%d.%d \r\n", IpAddress[0], IpAddress[1], IpAddress[2], IpAddress[3]);
   Serial_Buffer_Sent_Fill_Block((uint8_t *) DebugMsg);
   Serial_Buffer_Sent_FillFlash(PSTR("ENC28J60 revision ID  : \0"));
   sprintf(DebugMsg, "0x%02X\r\n", enc28j60getrev());
   Serial_Buffer_Sent_Fill_Block((uint8_t *) DebugMsg);
   Serial_Buffer_Sent_FillFlash(PSTR("\r\n\0"));

   EthLocBufferGetReceiveCounters(&LoconetCnt, &UdpCnt);
   Serial_Buffer_Sent_FillFlash(PSTR("Received \r\n\0"));
   Serial_Buffer_Sent_FillFlash(PSTR("     UDP packages     : \0"));
   sprintf(DebugMsg, "%ld\r\n", UdpCnt);
   Serial_Buffer_Sent_Fill_Block((uint8_t *) DebugMsg);
   Serial_Buffer_Sent_FillFlash(PSTR("     Loconet packages : \0"));
   sprintf(DebugMsg, "%ld\r\n", LoconetCnt);
   Serial_Buffer_Sent_Fill_Block((uint8_t *) DebugMsg);
   Serial_Buffer_Sent_FillFlash(PSTR("\r\n\r\n\0"));
   Serial_Buffer_Sent_FillFlash(PSTR(">\0"));

   Serial_Buffer_Sent_Fill_Block(UserIoRcvBuffer);
}

/**
 *******************************************************************************************
 * @fn	    	void UserIoInit(void)		
 * @brief   	Init the IO pins of the jumpers and leds.  
 * @return		None
 * @attention	- 
 *******************************************************************************************
 */
void UserIoInit(void)
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
   /* Set timer 2, timer overrun ~4,08 msec @ 8Mhz */
   TCCR2B |= (1 << CS22) | (1 << CS20);

   UserIoRcvCnt = 0;
   UserIoScreenUpdateCnt = 0;
   memset(UserIoRcvBuffer, 0, sizeof(UserIoRcvBuffer));

}

/**
 *******************************************************************************************
 * @fn	    	userIoJumperStatus UserIoGetJumperStatus(TUserIoJumper Jumper)		
 * @brief   	Read the status of a jumper.   
 * @param       Jumper Jumper to read. 
 * @return		userIoJumperStatus
 * @attention	- 
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
 * @fn	    	void UserIoMain(void)		
 * @brief   	Main routine of userIo. Check if timer overrun, if yes check
 *              if a led is in blink mode.   
 * @return		None
 * @attention	- 
 *******************************************************************************************
 */
void UserIoMain(void)
{
   uint8_t                                 Index;
   uint8_t                                 RcvChar;

   if (TIFR2 & (1 << TOV2))
   {
      TIFR2 |= (1 << TOV2);

      for (Index = 0; Index < userIoLedMax; Index++)
      {
         if (UserIoLedStat[Index].Stat == userIoLedSetBlink)
         {
            UserIoLedStat[Index].BlinkCnt++;
            if (UserIoLedStat[Index].BlinkCnt > USER_IO_BLINK_RATE)
            {
               UserIoLedStat[Index].BlinkCnt = 0;
               switch (Index)
               {
                  case userIoLed3:
                     PORTB ^= (1 << PB1);
                     break;
                  case userIoLed4:
                     PORTC ^= (1 << PC3);
                     break;
                  case userIoLed5:
                     PORTC ^= (1 << PC4);
                     break;
                  case userIoLed6:
                     PORTC ^= (1 << PC5);
                     break;
                  case userIoLedMax:
                     break;
               }
            }
         }
         else if (UserIoLedStat[Index].Stat == userIoLedSetFlash)
         {
            UserIoLedStat[Index].BlinkCnt++;
            if (UserIoLedStat[Index].BlinkCnt > USER_IO_FLASH_TIME)
            {
               UserIoLedStat[Index].BlinkCnt = 0;
               switch (Index)
               {
                  case userIoLed3:
                     PORTB |= (1 << PB1);
                     break;
                  case userIoLed4:
                     PORTC |= (1 << PC3);
                     break;
                  case userIoLed5:
                     PORTC |= (1 << PC4);
                     break;
                  case userIoLed6:
                     PORTC |= (1 << PC5);
                     break;
                  case userIoLedMax:
                     break;
               }
            }
         }
      }

      if (UserIoGetJumperStatus(userIoJumper1) == userIoJumperStatusClosed)
      {
         /* Any data received on the serial port? */
         if (Serial_Buffer_Receive_Read_Byte(&RcvChar))
         {
            if (RcvChar != USER_IO_CR)
            {
               UserIoRcvBuffer[UserIoRcvCnt] = RcvChar;
               UserIoRcvCnt++;
               if (UserIoRcvCnt >= USER_IO_RCV_LENGTH)
               {
                  UserIoRcvCnt = 0;
                  memset(UserIoRcvBuffer, 0, sizeof(UserIoRcvBuffer));
               }
            }
            else
            {
               UserIoProcessSerialData(UserIoRcvBuffer);
               UserIoRcvCnt = 0;
               memset(UserIoRcvBuffer, 0, sizeof(UserIoRcvBuffer));
            }
         }
         /* Update info screen if time elapsed. */
         UserIoScreenUpdateCnt++;
         if (UserIoScreenUpdateCnt >= USER_IO_SCREEN_UPDATE)
         {
            UserIoInfoScreen();
            UserIoScreenUpdateCnt = 0;
         }
      }
   }
}

/**
 *******************************************************************************************
 * @fn	    	void UserIoSetLed(TUserIoLed Led,TUserIoLedSet Set)		
 * @brief   	Set a led to a required status.    
 * @param       Led Led to be set. 
 * @param		Set How the led must be set. 
 * @attention	- 
 *******************************************************************************************
 */
void UserIoSetLed(TUserIoLed Led, TUserIoLedSet Set)
{
   switch (Led)
   {
      case userIoLed3:
         switch (Set)
         {
            case userIoLedSetOff:
               PORTB |= (1 << PB1);
               break;
            case userIoLedSetOn:
               PORTB &= ~(1 << PB1);
               break;
            case userIoLedSetBlink:
               UserIoLedStat[userIoLed3].BlinkCnt = 0;
               break;
            case userIoLedSetToggle:
               PORTB ^= (1 << PB1);
               break;
            case userIoLedSetFlash:
               PORTB &= ~(1 << PB1);
               UserIoLedStat[userIoLed3].BlinkCnt = 0;
               break;
         }
         break;
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

/**
 *******************************************************************************************
 * @fn	    	void UserIoIpAddressGet(uint8_t * IpAddress)
 * @brief   	Read the stored IP address from EEPROM. If no address is stored
 *              (255.255.255.255) the default value of 192.168.0.99 will be written
 *              into EEPROM and returned.     
 * @param       *IpAddress Pointer to array where IP address must be stored.  
 * @attention	- 
 *******************************************************************************************
 */
void UserIoIpAddressGet(uint8_t * IpAddress)
{
   uint8_t                                 IpEep[4];
   uint8_t                                 IpEepInvalid[4] = { 255, 255, 255, 255 };
   uint8_t                                 IpEepDefault[4] = { 192, 168, 0, 99 };
   uint8_t                                 Index;

   // Read IP address from EEPROM
   for (Index = 0; Index < USER_IO_EEP_IP_ADDRES_LENGTH; Index++)
   {
      IpEep[Index] = eeprom_read_byte((uint8_t *) USER_IO_EEP_IP_ADDRES_BASE + Index);
   }

   // Invalid Ip value? 
   if (memcmp(IpEep, IpEepInvalid, sizeof(IpEepDefault)) == 0)
   {
      UserIoIpAddressSet(IpEepDefault);
      memcpy(IpAddress, IpEepDefault, sizeof(IpEepDefault));
   }
   else
   {
      memcpy(IpAddress, IpEep, sizeof(IpEep));
   }
}

/**
 *******************************************************************************************
 * @fn	    	void UserIoIpAddressSet(uint8_t * IpAddress)
 * @brief   	Write the IP address into EEPROM.      
 * @param       *IpAddress Pointer to array where IP address must be read from. 
 * @attention	- 
 *******************************************************************************************
 */
void UserIoIpAddressSet(uint8_t * IpAddress)
{
   uint8_t                                 Index;

   // Read IP address from EEPROM
   for (Index = 0; Index < USER_IO_EEP_IP_ADDRES_LENGTH; Index++)
   {
      eeprom_write_byte((uint8_t *) USER_IO_EEP_IP_ADDRES_BASE + Index, IpAddress[Index]);
   }
}
