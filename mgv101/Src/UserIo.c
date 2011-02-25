
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

#define USER_IO_EEP_IP_BASE_ADDRES        0                                   /**< Ip address location in EEPROM */
#define USER_IO_EEP_IP_BASE_NETMASK       4                                   /**< Netmask location in EEPROM */
#define USER_IO_EEP_IP_BASE_GATEWAY       8                                   /**< Gateway location in EEPROM */

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
 * @fn         void UserIoInit(void)
 * @brief      Init the IO pins of the jumpers and leds.
 * @return     None
 * @attention  -
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
 * @fn         void UserIoMain(void)
 * @brief      Main routine of userIo. Check if timer overrun, if yes check
 *             if a led is in blink mode.
 * @return     None
 * @attention  -
 *******************************************************************************************
 */
void UserIoMain(void)
{
   uint8_t                                 Index;

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
   }
}

/**
 *******************************************************************************************
 * @fn         void UserIoSetLed(TUserIoLed Led,TUserIoLedSet Set)
 * @brief      Set a led to a required status.
 * @param      Led Led to be set.
 * @param      Set How the led must be set.
 * @attention  -
 *******************************************************************************************
 */
void UserIoSetLed(TUserIoLed Led, TUserIoLedSet Set)
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

/**
 *******************************************************************************************
 * @fn         void UserIoIpSettingsGet(uint8_t * IpAddress, uint8_t * NetMask, uint8_t * RouterIp)
 * @brief      Read the IP Address. In future this function may be extended with reading from
 *             EEPROM.
 * @param      *IpAddress Pointer to array where IP address must be stored.
 * @param      *NetMask Pointer to array where the NetMask must be stored.
 * @param      *RouterIp Pointer to array where the RouterIp must be stored.
 * @attention  -
 *******************************************************************************************
 */
void UserIoIpSettingsGet(uint8_t * IpAddress, uint8_t * NetMask, uint8_t * RouterIp)
{
   uint8_t                                 IpAddress_1[4] = { 192, 168, 0, 200 };
   uint8_t                                 IpAddress_2[4] = { 192, 168, 1, 200 };
   uint8_t                                 IpAddress_3[4] = { 192, 168, 100, 88 };
   uint8_t                                 NetMask_1[4] = { 255, 255, 255, 0 };
   uint8_t                                 NetMask_2[4] = { 255, 255, 255, 0 };
   uint8_t                                 NetMask_3[4] = { 255, 255, 255, 0 };
   uint8_t                                 RouterIp_1[4] = { 192, 168, 0, 1 };
   uint8_t                                 RouterIp_2[4] = { 192, 168, 1, 1 };
   uint8_t                                 RouterIp_3[4] = { 192, 168, 100, 1 };

   uint8_t                                 Jumpers;
   uint8_t                                 Index;

   Jumpers = PINC & 0x07;

   switch (Jumpers)
   {
      case 7:
         memcpy(IpAddress, IpAddress_1, sizeof(IpAddress_1));
         memcpy(NetMask, NetMask_1, sizeof(NetMask_1));
         memcpy(RouterIp, RouterIp_1, sizeof(RouterIp_1));
         break;
      case 6:
         memcpy(IpAddress, IpAddress_2, sizeof(IpAddress_2));
         memcpy(NetMask, NetMask_2, sizeof(NetMask_2));
         memcpy(RouterIp, RouterIp_2, sizeof(RouterIp_2));
         break;
      case 5:
         memcpy(IpAddress, IpAddress_3, sizeof(IpAddress_3));
         memcpy(NetMask, NetMask_3, sizeof(NetMask_3));
         memcpy(RouterIp, RouterIp_3, sizeof(RouterIp_3));
         break;
      case 0:
         for (Index = 0; Index < 4; Index++)
         {
            IpAddress[Index] = eeprom_read_byte((uint8_t *) (USER_IO_EEP_IP_BASE_ADDRES + Index));
            NetMask[Index] = eeprom_read_byte((uint8_t *) (USER_IO_EEP_IP_BASE_NETMASK + Index));
            RouterIp[Index] = eeprom_read_byte((uint8_t *) (USER_IO_EEP_IP_BASE_GATEWAY + Index));
         }
         break;
      default:
         memcpy(IpAddress, IpAddress_1, sizeof(IpAddress_1));
         memcpy(NetMask, NetMask_1, sizeof(NetMask_1));
         memcpy(RouterIp, RouterIp_1, sizeof(RouterIp_1));
         break;
   }
}
