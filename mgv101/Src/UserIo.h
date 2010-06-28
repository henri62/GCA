
/**
 *******************************************************************************************
 * @file		UserIo.h
 * @ingroup	    UserIo
 * @defgroup	UserIo UserIo : Control of led and jumper. 
 * @brief		UserIo Routines for acces to the jumpers and the leds. 
 *******************************************************************************************
 */
#ifndef USER_IO_H
#define USER_IO_H

/*
 *******************************************************************************************
 * Standard include files
 *******************************************************************************************
 */
#include <inttypes.h>

/*
 *******************************************************************************************
 * Macro definitions
 *******************************************************************************************
 */

/**
 * Enum for the jumpers.
 */
typedef enum
{
   userIoJumper1 = 0,         /**< Jumper 1 PC0 */
   userIoJumper2,             /**< Jumper 2 PC1 */
   userIoJumper3              /**< Jumper 3 PC2 */
}TUserIoJumper;

/**
 * Enum for status of the jumpers.
 */
typedef enum
{
userIoJumperStatusOpen = 0,   /**< Jumper open */
userIoJumperStatusClosed      /**< Jumper closed */
}TUserIoJumperStatus;

/**
 * Enum for the leds.
 */
typedef enum
{
   userIoLed3 = 0,           /**< Led 3 control PB1 */
   userIoLed4,               /**< Led 4 control PC3 */
   userIoLed5,               /**< Led 5 control PC4 */
   userIoLed6,               /**< Led 6 control PC5 */
   userIoLedMax
}TUserIoLed;

/**
 * Enum for setting status of a led. 
 */
typedef enum
{
   userIoLedSetOff = 0,       /**< Led off */
   userIoLedSetOn,            /**< Led on */
   userIoLedSetBlink,         /**< Led blinking */
   userIoLedSetToggle,        /**< Led toggle */
   userIoLedSetFlash          /**< Led toggle */
}TUserIoLedSet;


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

extern void                             UserIoInit(void);
extern void                             UserIoMain(void);
extern TUserIoJumperStatus              UserIoGetJumperStatus(TUserIoJumper Jumper);
extern void                             UserIoSetLed(TUserIoLed Led, TUserIoLedSet Set);
extern void                             UserIoIpAddressGet(uint8_t * IpAddress);
extern void                             UserIoIpAddressSet(uint8_t * IpAddress);

#endif   // USER_IO_H
