
/**
 *******************************************************************************************
 * @file 		Serial_mega.h                                         
 * @ingroup     Serial 
 * @defgroup	Serial Serial : Routines for the UART. 
 * @brief		UART routines for interrupt driven serial communication. 
 *******************************************************************************************
 */

#ifndef SERIAL_MEGA_H
#define SERIAL_MEGA_H

/*
 *******************************************************************************************
 * Standard include files
 *******************************************************************************************
 */
#include <inttypes.h>
#include <avr\pgmspace.h>

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

#define  XTAL              F_CPU          /**< CPU frequency */
#define BAUD19200          19200          /**< Desired baud rate */
#define BAUDDIVIDER19200   (unsigned int) (XTAL / BAUD19200 / 16- 0.5)+1  /**< Baudrate setting */
#define SERIALOUTSIZE      300            /**< Serial output buffer size */
#define SERIALINSIZE       10             /**< Serial input buffer size */

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

extern void                             Serial_Init(void);
extern void                             Serial_Enable_Transmit_Interrupts(void);
extern void                             Serial_Buffer_Sent_Fill_Block(uint8_t * s);
extern void                             Serial_Buffer_Sent_FillFlash(const char *s);
extern void                             Serial_Enable_Receive_Interrupts(void);
extern uint8_t                          Serial_Buffer_Receive_Read_Byte(uint8_t * s);

#endif
