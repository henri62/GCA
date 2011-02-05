
/**
 ******************************************************************************
 * @file       EEP_Settings.h
 * @ingroup    EEP
 * @author     Robert Evers
 * @attention  Header file for EEPROM definition and position.
 ******************************************************************************
 * Change history   : See CVS log
 ******************************************************************************
 */

#ifndef EEP_SETTING_H
#define EEP_SETTING_H

/*
 ******************************************************************************
 * Standard include files
 ******************************************************************************
 */

#include <inttypes.h>
#include <avr\eeprom.h>
#include "EEP.h"

/*
  ******************************************************************************
  * Macro definitions
  ******************************************************************************
  */

/* *INDENT-OFF* */

#define EEP_SETTINGS_BASE_ADDRESS         0     /**< Location (start) of Mgv50 Unit address */
#define EEP_SETTINGS_BLINK_ADDRESS        2     /**< Location (start) of Mgv50 blink value */
#define EEP_SETTINGS_PIN_DATA_START       4     /**< Start location pin data */
#define EEP_SETTINGS_PIN_DATA_SIZE        7     /**< Length of EEP data per pin */
#define EEP_SETTINGS_OPCODE_BASE          250   /**< Location (start) of Opcode data */
#define EEP_SETTINGS_OPCODE_SIZE          3     /**< Size of opcode data */
#define EEP_SETTINGS_INITIAL              500   /**< Initial EEPROM value yes no. */

/* *INDENT-ON* */
 /* 
  ******************************************************************************
  * Types
  ******************************************************************************
  */

/*
 ******************************************************************************
 * Variables
 ******************************************************************************
 */

 /* 
  *******************************************************************************************
  * Prototypes
  *******************************************************************************************
  */

#endif   /* EEP_SETTING_H */
