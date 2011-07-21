
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

/* *INDENT-OFF* */

#define USER_IO_TIMER_OVERFLOW_TIME       4         /**< Timer overflow in msec */

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
   userIoLed4 = 0,           /**< Led 4 control PC3 */
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

extern uint16_t           UserIoTcpIpLinkCnt;
/* *INDENT-ON* */

/*
 *******************************************************************************************
 * Prototypes
 *******************************************************************************************
 */

extern void                             userio_init(void);
extern TUserIoJumperStatus              UserIoGetJumperStatus(TUserIoJumper Jumper);
extern void                             userio_set_led(TUserIoLed Led, TUserIoLedSet Set);
extern void                             userio_get_settings(uint8_t * IpAddress, uint8_t * NetMask, uint8_t * MacAddress, uint8_t * MyId);
extern void                             userio_set_id(uint8_t * newId);
extern void 							userio_set_ip(uint8_t * IpAddress, uint8_t * NetMask, uint8_t * MacAddress);
#endif   // USER_IO_H
