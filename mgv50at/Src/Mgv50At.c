
/**
 ******************************************************************************
 * @file       Mgv50At.c
 * @ingroup    Mgv50At
 * @author     Robert Evers
 * @attention  Generic module for processing MGV50AT data.
 ******************************************************************************
 */

/*
 ******************************************************************************
 * Standard include files
 ******************************************************************************
 */

#ifndef F_CPU
#define F_CPU 8000000
#endif

#include <avr/io.h>
#include <inttypes.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include "EEP_Settings.h"
#include "ln_buf.h"
#include "Version.h"
#include "ln_sw_uart.h"
#include "EEP_Settings.h"
#include "ln_interface.h"
#include "Mgv50At.h"

/*
 *******************************************************************************************
 * Macro definitions
 *******************************************************************************************
 */

/* *INDENT-OFF* */
#define MGV50AT_LOCONET_TURNOUT_COMMAND            0xB0           /**< Loconet turnout command */
#define MGV50AT_LOCONET_SENSOR_COMMAND             0xB2           /**< Loconet sensor command */
#define MGV50AT_LOCONET_X_PEER                     0xE5           /**< Loconet XPeer command */
#define MGV50AT_LOCONET_GLOBAL_POWER_OFF           0x82           /**< Loconet global power off command */
#define MGV50AT_LOCONET_GLOBAL_POWER_ON            0x83           /**< Loconet global power on command */
#define MGV50AT_LOCONET_X_PEER_MSG_SIZE            0x10           /**< Loconet Xpeer response number of bytes */
#define MGV50AT_LOCONET_X_PEER_COMMAND_WRITE       0x01           /**< Loconet Xpeer write sv */
#define MGV50AT_LOCONET_X_PEER_COMMAND_READ        0x02           /**< Loconet Xpeer read sv */
#define MGV50AT_LOCONET_X_PEER_COMMAND_MP_WRITE    0x03           /**< Loconet Xpeer MP write */
#define MGV50AT_LOCONET_X_PEER_COMMAND_MP_READ     0x04           /**< Loconet Xpeer MP write */
#define MGV50AT_LOCONET_X_PEER_GENERIC_ADDRESS     0x00           /**< Dest Low is 0, reset unit */

#define MGV50AT_LOCONET_X_PEER_RESPONSE_TRY        20             /**< Number of retries for transmit xpeer response */

#define MGV50AT_NUMBER_OF_PINS                     16             /**< Number of available pin */

#define MGV50AT_INVALID_EEP_DATA_WORD              0xFFFF         /**< Invalid EEPROM data */
#define MGV50AT_INVALID_EEP_DATA_BYTE              0xFF           /**< Invalid EEPROM data */
#define MGV50AT_CONFIG_BYTE_DEFAULT                0x1F;          /**< B0 default and input default */
#define MGV50AT_LOCO_IO_DEFAULT_ADDRESS            0x0101         /**< Default LocoIo address */

#define MGV50AT_SENSOR_TIMER_OVERFLOW_TIME         2              /**< ~2 msec timer overrun */
#define MGV50AT_SENSOR_BLOCK_CNT_SENSOR_ON         200 / MGV50AT_SENSOR_TIMER_OVERFLOW_TIME    /**< Time constant switch on */
#define MGV50AT_SENSOR_BLOCK_CNT_PULSE_ON          80 / MGV50AT_SENSOR_TIMER_OVERFLOW_TIME     /**< Time constant switch on */
#define MGV50AT_SENSOR_BLOCK_CNT_DELAY_OFF         1000 / MGV50AT_SENSOR_TIMER_OVERFLOW_TIME   /**< Time constant delayed off */
#define MGV50AT_SENSOR_BLOCK_CNT_SENSOR_OFF        10 / MGV50AT_SENSOR_TIMER_OVERFLOW_TIME     /**< Time constant off */
#define MGV50AT_BLINK_TIME                         (250 / MGV50AT_SENSOR_TIMER_OVERFLOW_TIME)  /**< Time for blinking / flashing */
#define MGV50AT_OUTPUT_TIME_PULSE_HW               (250 / MGV50AT_SENSOR_TIMER_OVERFLOW_TIME)  /**< Time for pulse output */

#define MGV50AT_LOCONET_ACTIVITY_CNT_MAX           20                   /**< On time loconet activity */
#define MGV50AT_LOCONET_ACTIVITY_ON                PORTB |= (1<<PB2);   /**< Loconet activity led on */
#define MGV50AT_LOCONET_ACTIVITY_OFF               PORTB &= ~(1<<PB2);  /**< Loconet activity led off */

#define MGV50AT_SENSOR_JUMPER                      PINB & (1<<PINB4)    /**< Transmit input status after power on yes / no */

#define PORTBDIR 0
#define PORTBOUT 1
#define PORTCDIR 2
#define PORTCOUT 3
#define PORTDDIR 4
#define PORTDOUT 5

/*
 *******************************************************************************************
 * Types
 *******************************************************************************************
  */

/**
 * Typedef union with the options for an input or output.
 */
typedef union
{
   struct
   {
      uint8_t        NotUsed_1 : 1;        /* Not used */
      uint8_t        NotUsed_2 : 1;        /* Not used */
      uint8_t        Delay : 1;            /* Delay for off */
      uint8_t        OpCode : 1;           /* Opcode for the sensor message */
      uint8_t        Block : 1;            /* Opcode for the sensor message */
      uint8_t        ButtonSwitch : 1;     /* Block or button /switch */
      uint8_t        Inverted : 1;         /* Input inverted yes / no */
      uint8_t        NotUsed_3 : 1;        /* Not used */
   } Input;

   struct
   {
      uint8_t        ContactHighStart : 1; /* Pin high at start yes / no */
      uint8_t        NotUsed : 1;          /* Input or output */
      uint8_t        PulseReset : 1;       /* SW reset of HW reset */
      uint8_t        Pulse : 1;            /* Pulse contact or normal */
      uint8_t        Blink : 1;            /* Blink or normal */
      uint8_t        Multi : 1;            /* Multi or normal */
      uint8_t        ContactBlock : 1;     /* Contact or block output */
      uint8_t        NotUsed_1 : 1;        /* Not used */
   }Output;

   uint8_t  ByteValue;                     /* Container for direct read / write */

} TMgv50AtConfigByte;

/**
 * Typedef struct for storing input data from  ports
 */
typedef struct
{
   uint8_t           PinB;                   /**< Actual debounce value inputs port B */
   uint8_t           PinC;                   /**< Actual debounce value inputs port  B */
   uint8_t           PinD;                   /**< Actual debounce value inputs port B */
}TMgv50AtSensorInput;

/**
 * Typedef enum for selected option of a pin
 */
typedef enum
{
   mgv50AtPinTypeInput = 0,                 /**< Pin set as input */
   mgv50AtPinTypeOutput                     /**< Pin set as output */
}TMgv50AtPinType;

/**
 * Typedef enum for selected option of a pin
 */
typedef enum
{
   mgv50AtOutputTypeNormal = 0,               /**< Normal output */
   mgv50AtOutputTypeBlock,                    /**< */
   mgv50AtOutputTypePulse,                    /**< */
   mgv50AtOutputTypeHwPulse,                  /**< */
   mgv50AtOutputTypeBlink                     /**< */
}TMgv50AtOutputType;

/**
 * Typedef enum for status of transmit sensor.
 */
typedef enum
{
   mgv50AtSensorTransmitOn = 0,              /**< Sensor on status transmitted */
   mgv50AtSensorTransmitOff,                 /**< Sensor off status transmitted */
   mgv50AtSensorTransmitIdle,                /**< Sensor transmit idle */
   mgv50AtSensorTransmitSkip                 /**< Skip first change after power on */
}TMgv50AtSensorTransmit;

/**
 * Typedef struct for config of a pin.
 */
typedef struct
{
   uint16_t                Address;                /**< Loconet address of the pin */
   uint8_t                 PortOut;                /**< Output address port */
   uint8_t                 PortDdr;                /**< Output address port */
   uint8_t                 PinNumber;              /**< Pin number 1 of port */
   TMgv50AtConfigByte      ConfigByte;             /**< Config byte, stored for easy response */
   uint8_t                 LocoIoGetal_2;          /**< LocoIoGetal2, stored for easy response */
   uint8_t                 LocoIoGetal_3;          /**< LocoIoGetal3, stored for easy response */
   TMgv50AtPinType         Type;                   /**< Selection type of pin, output or input */
   TMgv50AtOutputType      OutputType;             /**< The type of output */
   uint8_t                 OutputActive;           /**< Output active / set. */
   TMgv50AtSensorTransmit  Transmit;               /**< New sensor status transmitted */
   uint8_t                 *SensorIn;              /**< Pointer to sensor input value */
   uint16_t                OutputCnt;              /**< Counter for HW reset and blinking output */
   uint16_t                SensorCnt;              /**< Counter for sensor status */
   uint16_t                SensorCntMax;           /**< Counter max value for sensor status */
   uint8_t                 OpCodeCmdData[3];       /**< Extra opcode command */
}TMgv50AtConfigPin;

/**
 * Typedef struct for config of Mgv50At
 */
typedef struct
{
   uint16_t           UnitAddress;                        /**< Base address of the unit*/
   uint16_t           BlinkValue;                         /**< Blink value for flash option*/
   TMgv50AtConfigPin  Pin[MGV50AT_NUMBER_OF_PINS];        /**< Configuration if the pins*/
}TMgv50AtConfig;

/*
 *******************************************************************************************
 * Variables
 *******************************************************************************************
 */

static LnBuf                  Mgv50AtLocoNet ;            /**< Loconet variable */
static TMgv50AtSensorInput    Mgv50AtSensorInput;         /**< Holds the input values */
static TMgv50AtConfig         Mgv50AtConfig;              /**< Config of the module */
static uint8_t                Mgv50AtCheckSensors;        /**< Update the sensor info. */
static uint8_t                Mgv50AtLocoNetActivity;     /**< Counter for loconet led blink */
static lnMsg                  Mgv50AtLocoNetSendPacket;   /**< Loconet Tx buffer */

/* *INDENT-ON* */

/*
 *******************************************************************************************
 * Routines implementation
 *******************************************************************************************
 */

void Mgv50AtSetPinDirection(uint8_t Port, uint8_t Pin, TMgv50AtPinType Type)
{
   switch (Port)
   {
      case PORTBDIR:
         switch (Type)
         {
            case mgv50AtPinTypeInput:
               DDRB &= ~(1 << Pin);
               break;
            default:
               DDRB |= (1 << Pin);
               break;
         }
         break;
      case PORTCDIR:
         switch (Type)
         {
            case mgv50AtPinTypeInput:
               DDRC &= ~(1 << Pin);
               break;
            default:
               DDRC |= (1 << Pin);
               break;
         }
         break;
      case PORTDDIR:
         switch (Type)
         {
            case mgv50AtPinTypeInput:
               DDRD &= ~(1 << Pin);
               break;
            default:
               DDRD |= (1 << Pin);
               break;
         }
   }
}

void Mgv50AtSetPin(uint8_t Port, uint8_t Pin, uint8_t OnOff)
{
   switch (Port)
   {
      case PORTBOUT:
         if (OnOff == 0)
         {
            PORTB &= ~(1 << Pin);
         }
         else if (OnOff == 1)
         {
            PORTB |= (1 << Pin);
         }
         else if (OnOff == 2)
         {
            PORTB ^= (1 << Pin);
         }
         break;
      case PORTCOUT:
         if (OnOff == 0)
         {
            PORTC &= ~(1 << Pin);
         }
         else if (OnOff == 1)
         {
            PORTC |= (1 << Pin);
         }
         else if (OnOff == 2)
         {
            PORTC ^= (1 << Pin);
         }
         break;
      case PORTDOUT:
         if (OnOff == 0)
         {
            PORTD &= ~(1 << Pin);
         }
         else if (OnOff == 1)
         {
            PORTD |= (1 << Pin);
         }
         else if (OnOff == 2)
         {
            PORTD ^= (1 << Pin);
         }
         break;
   }
}

/**
 ******************************************************************************
 * @fn         SIGNAL(SIG_OVERFLOW0)
 * @brief      In- or decrease sensor input value depending on status.
 * @return     None
 * @attention  -
 ******************************************************************************
 */
SIGNAL(SIG_OVERFLOW0)
{
   uint8_t                                 Index = 0;

   // Allow interrupt of Loconet to do it's thing */
   sei();

   for (Index = 0; Index < MGV50AT_NUMBER_OF_PINS; Index++)
   {
      /* Pin used as sensor / switch !? */
      if (Mgv50AtConfig.Pin[Index].Type == mgv50AtPinTypeInput)
      {
         Mgv50AtCheckSensors = 1;
         /* If pin NOT active the it's 'off', handle it... Handle also the inverted option for an input, thats why the
          * if construction is so 'readable' */
         if (((*Mgv50AtConfig.Pin[Index].SensorIn & (1 << Mgv50AtConfig.Pin[Index].PinNumber) &&
               Mgv50AtConfig.Pin[Index].ConfigByte.Input.Inverted == 0)) ||
             ((!(*Mgv50AtConfig.Pin[Index].SensorIn & (1 << Mgv50AtConfig.Pin[Index].PinNumber)) &&
               Mgv50AtConfig.Pin[Index].ConfigByte.Input.Inverted == 1)))
         {
            if (Mgv50AtConfig.Pin[Index].SensorCnt > 0)
            {
               Mgv50AtConfig.Pin[Index].SensorCnt--;
            }
         }
         /* If pin ACTIVE the it's 'occupied / pushed', handle it... Handle also the inverted option for an input,
          * thats why the if construction is so 'readable' */
         else if (((!
                    (*Mgv50AtConfig.Pin[Index].SensorIn & (1 << Mgv50AtConfig.Pin[Index].PinNumber) &&
                     Mgv50AtConfig.Pin[Index].ConfigByte.Input.Inverted == 0)) ||
                   (((*Mgv50AtConfig.Pin[Index].SensorIn & (1 << Mgv50AtConfig.Pin[Index].PinNumber))) &&
                    Mgv50AtConfig.Pin[Index].ConfigByte.Input.Inverted == 1)))
         {
            Mgv50AtConfig.Pin[Index].SensorCnt++;
            if (Mgv50AtConfig.Pin[Index].SensorCnt >= Mgv50AtConfig.Pin[Index].SensorCntMax)
            {
               if (Mgv50AtConfig.Pin[Index].ConfigByte.Input.Delay == 0)
               {
                  Mgv50AtConfig.Pin[Index].SensorCnt = MGV50AT_SENSOR_BLOCK_CNT_DELAY_OFF;
               }
               else
               {
                  Mgv50AtConfig.Pin[Index].SensorCnt = Mgv50AtConfig.Pin[Index].SensorCntMax;
               }
            }
         }
      }
      else
      {
         /* Handle blinking and hardware pulse */
         if (Mgv50AtConfig.Pin[Index].OutputActive != 0)
         {
            switch (Mgv50AtConfig.Pin[Index].OutputType)
            {
               case mgv50AtOutputTypePulse:
               case mgv50AtOutputTypeHwPulse:
                  Mgv50AtConfig.Pin[Index].OutputCnt++;
                  if (Mgv50AtConfig.Pin[Index].OutputCnt >= MGV50AT_OUTPUT_TIME_PULSE_HW)
                  {
                     Mgv50AtConfig.Pin[Index].OutputActive = 0;
                     Mgv50AtSetPin(Mgv50AtConfig.Pin[Index].PortOut, Mgv50AtConfig.Pin[Index].PinNumber, 0);
                  }
                  break;
               case mgv50AtOutputTypeBlink:
                  Mgv50AtConfig.Pin[Index].OutputCnt++;
                  if (Mgv50AtConfig.Pin[Index].OutputCnt >= Mgv50AtConfig.BlinkValue)
                  {
                     Mgv50AtConfig.Pin[Index].OutputCnt = 0;
                     Mgv50AtSetPin(Mgv50AtConfig.Pin[Index].PortOut, Mgv50AtConfig.Pin[Index].PinNumber, 2);
                  }
                  break;
               default:
                  break;
            }

         }
      }
   }
}

/**
 ******************************************************************************
 * @fn         void Mgv50AtLocoNetLedStart(void)
 * @brief      Enable led and set timer counter to 0 for loconet activity.
 * @return     None
 * @attention  -
 ******************************************************************************
 */
void Mgv50AtLocoNetLedStart(void)
{
   MGV50AT_LOCONET_ACTIVITY_OFF;
   MGV50AT_LOCONET_ACTIVITY_ON;
   Mgv50AtLocoNetActivity = 0;
}

/**
 ******************************************************************************
 * @fn         void Mgv50AtLocoNetLedCheck(void)
 * @brief      Update counter, if exceeded switch off led.
 * @return     None
 * @attention  -
 ******************************************************************************
 */
void Mgv50AtLocoNetLedCheck(void)
{
   Mgv50AtLocoNetActivity++;
   if (Mgv50AtLocoNetActivity >= MGV50AT_LOCONET_ACTIVITY_CNT_MAX)
   {
      Mgv50AtLocoNetActivity = MGV50AT_LOCONET_ACTIVITY_CNT_MAX;
      MGV50AT_LOCONET_ACTIVITY_OFF;
   }
}

/**
 ******************************************************************************
 * @fn         void Mgv50AtUpdateEepromData(TMgv50AtConfig * ConfigPtr, uint8_t Index)
 * @brief      Update the EEPROM data of the unit, address and blink value.
 * @param      ConfigPtr Pointer to data with config settings of pins.
 * @param      Index Index for pin to be updated.
 * @return     None
 * @attention  -
 ******************************************************************************
 */
void Mgv50AtUpdateEepromDataUnit(TMgv50AtConfig * ConfigPtr)
{
   eeprom_write_word((uint16_t *) (EEP_SETTINGS_BASE_ADDRESS), ConfigPtr->UnitAddress);
   eeprom_write_word((uint16_t *) (EEP_SETTINGS_BASE_ADDRESS + 2), ConfigPtr->BlinkValue);
}

/**
 ******************************************************************************
 * @fn         void Mgv50AtUpdateEepromData(TMgv50AtConfig * ConfigPtr, uint8_t Index)
 * @brief      Update the EEPROM data of a pin.
 * @param      ConfigPtr Pointer to data with config settings of pins.
 * @param      Index Index for pin to be updated.
 * @return     None
 * @attention  -
 ******************************************************************************
 */
void Mgv50AtUpdateEepromData(TMgv50AtConfig * ConfigPtr, uint8_t Index)
{
   eeprom_write_word((uint16_t *) (EEP_SETTINGS_PIN_DATA_START + (Index * EEP_SETTINGS_PIN_DATA_SIZE)),
                     ConfigPtr->Pin[Index].Address);
   eeprom_write_byte((uint8_t *) (EEP_SETTINGS_PIN_DATA_START + (Index * EEP_SETTINGS_PIN_DATA_SIZE) + 2),
                     ConfigPtr->Pin[Index].Type);
   eeprom_write_byte((uint8_t *) (EEP_SETTINGS_PIN_DATA_START + (Index * EEP_SETTINGS_PIN_DATA_SIZE) + 3),
                     ConfigPtr->Pin[Index].ConfigByte.ByteValue);
   eeprom_write_byte((uint8_t *) (EEP_SETTINGS_PIN_DATA_START + (Index * EEP_SETTINGS_PIN_DATA_SIZE) + 4),
                     ConfigPtr->Pin[Index].LocoIoGetal_2);
   eeprom_write_byte((uint8_t *) (EEP_SETTINGS_PIN_DATA_START + (Index * EEP_SETTINGS_PIN_DATA_SIZE) + 5),
                     ConfigPtr->Pin[Index].LocoIoGetal_3);
   eeprom_write_byte((uint8_t *) (EEP_SETTINGS_PIN_DATA_START + (Index * EEP_SETTINGS_PIN_DATA_SIZE) + 6),
                     ConfigPtr->Pin[Index].OutputType);
}

/**
 ******************************************************************************
 * @fn         void Mgv50AtResetModule(void)
 * @brief      Set all values of the module to default.
 * @return     None
 * @attention  Default is all pins as input and set as block.
 ******************************************************************************
 */
void Mgv50AtResetModule(TMgv50AtConfig * ConfigPtr)
{
   uint8_t                                 Index;
   uint8_t                                 IndexOpCode;

   ConfigPtr->UnitAddress = MGV50AT_LOCO_IO_DEFAULT_ADDRESS;
   ConfigPtr->BlinkValue = MGV50AT_BLINK_TIME;

   for (Index = 0; Index < MGV50AT_NUMBER_OF_PINS; Index++)
   {

      ConfigPtr->Pin[Index].Address = Index;
      ConfigPtr->Pin[Index].Type = mgv50AtPinTypeInput;
      ConfigPtr->Pin[Index].SensorCntMax = MGV50AT_SENSOR_BLOCK_CNT_SENSOR_ON;
      ConfigPtr->Pin[Index].ConfigByte.ByteValue = MGV50AT_CONFIG_BYTE_DEFAULT;
      ConfigPtr->Pin[Index].LocoIoGetal_2 = ConfigPtr->Pin[Index].Address & 0x7F;
      ConfigPtr->Pin[Index].LocoIoGetal_3 = (ConfigPtr->Pin[Index].Address >> 9) & 0x7F;

      Mgv50AtSetPinDirection(ConfigPtr->Pin[Index].PortDdr, ConfigPtr->Pin[Index].PinNumber,
                             ConfigPtr->Pin[Index].Type);
      Mgv50AtSetPin(ConfigPtr->Pin[Index].PortOut, ConfigPtr->Pin[Index].PinNumber, 1);

      if (Index % 2)
      {
         ConfigPtr->Pin[Index].LocoIoGetal_3 |= 0x10;
      }

      for (IndexOpCode = 0; IndexOpCode < EEP_SETTINGS_OPCODE_SIZE; IndexOpCode++)
      {
         eeprom_write_byte((uint8_t *) (EEP_SETTINGS_OPCODE_BASE + (Index * EEP_SETTINGS_OPCODE_SIZE) +
                                        IndexOpCode), 0);
         Mgv50AtConfig.Pin[Index].OpCodeCmdData[IndexOpCode] = 0;
      }
   }
}

/**
 ******************************************************************************
 * @fn         void Mgv50AtReportSensor(TMgv50AtConfig * ConfigPtr)
 * @brief      Transmit the input status on the loconet bus if required. If the
 *             extra opcode is filled in this is also transmitted.
 * @param      ConfigPtr Pointer to data with config settings of pins.
 * @return     None
 * @attention  If a transmit fails, it will be retried next time.
 ******************************************************************************
 */
void Mgv50AtReportSensor(TMgv50AtConfig * ConfigPtr)
{
   lnMsg                                   LocoNetSendPacket;
   uint8_t                                 Index;

   for (Index = 0; Index < MGV50AT_NUMBER_OF_PINS; Index++)
   {
      if (ConfigPtr->Pin[Index].Type == mgv50AtPinTypeInput)
      {
         if (ConfigPtr->Pin[Index].SensorCnt >= ConfigPtr->Pin[Index].SensorCntMax)
         {
            if (ConfigPtr->Pin[Index].Transmit != mgv50AtSensorTransmitOn)
            {
               if (ConfigPtr->Pin[Index].Transmit != mgv50AtSensorTransmitSkip)
               {
                  LocoNetSendPacket.sd.mesg_size = 4;
                  if ((ConfigPtr->Pin[Index].ConfigByte.Input.OpCode) && (ConfigPtr->Pin[Index].ConfigByte.Input.Block))
                  {
                     /* Transmit the sensor status on the Loconet bus */
                     LocoNetSendPacket.data[0] = MGV50AT_LOCONET_SENSOR_COMMAND;
                     LocoNetSendPacket.data[1] = (ConfigPtr->Pin[Index].Address >> 1) & 0x7F;
                     LocoNetSendPacket.data[2] = ConfigPtr->Pin[Index].Address >> 8;
                     LocoNetSendPacket.data[2] |= 0x10;
                     if (ConfigPtr->Pin[Index].Address & 1)
                     {
                        LocoNetSendPacket.data[2] |= 0x20;
                     }
                  }
                  else
                  {
                     /* Transmit the switch request on the Loconet bus */
                     LocoNetSendPacket.data[0] = MGV50AT_LOCONET_TURNOUT_COMMAND;
                     LocoNetSendPacket.data[1] = (ConfigPtr->Pin[Index].Address) & 0x7F;
                     LocoNetSendPacket.data[2] = ConfigPtr->Pin[Index].Address >> 7;
                     LocoNetSendPacket.data[2] |= 0x10;
                  }
                  /* Transmit, if transmit not succeeded it will be performed next time.. */
                  if (sendLocoNetPacket(&LocoNetSendPacket) == LN_DONE)
                  {
                     Mgv50AtLocoNetLedStart();
                     /* Transmit the extra opcode of an input (if present) */
                     if (ConfigPtr->Pin[Index].OpCodeCmdData[0] != 0)
                     {
                        LocoNetSendPacket.data[0] = ConfigPtr->Pin[Index].OpCodeCmdData[0];
                        LocoNetSendPacket.data[1] = ConfigPtr->Pin[Index].OpCodeCmdData[1];
                        LocoNetSendPacket.data[2] = ConfigPtr->Pin[Index].OpCodeCmdData[2];
                        if (sendLocoNetPacket(&LocoNetSendPacket) == LN_DONE)
                        {
                           Mgv50AtLocoNetLedStart();
                           ConfigPtr->Pin[Index].Transmit = mgv50AtSensorTransmitOn;
                        }
                     }
                     else
                     {
                        ConfigPtr->Pin[Index].Transmit = mgv50AtSensorTransmitOn;
                     }
                  }
               }
               else
               {
                  /* Initial change after power on skipped */
                  ConfigPtr->Pin[Index].Transmit = mgv50AtSensorTransmitOn;
               }
            }
         }
         else if (ConfigPtr->Pin[Index].SensorCnt < MGV50AT_SENSOR_BLOCK_CNT_SENSOR_OFF)
         {
            if (ConfigPtr->Pin[Index].Transmit != mgv50AtSensorTransmitOff)
            {
               if (ConfigPtr->Pin[Index].Transmit != mgv50AtSensorTransmitSkip)
               {
                  LocoNetSendPacket.sd.mesg_size = 4;
                  if ((ConfigPtr->Pin[Index].ConfigByte.Input.OpCode) && (ConfigPtr->Pin[Index].ConfigByte.Input.Block))
                  {
                     /* Transmit the sensor status on the loconet bus */
                     LocoNetSendPacket.data[0] = MGV50AT_LOCONET_SENSOR_COMMAND;
                     LocoNetSendPacket.data[1] = (ConfigPtr->Pin[Index].Address >> 1) & 0x7F;
                     LocoNetSendPacket.data[2] = ConfigPtr->Pin[Index].Address >> 8;
                     if (ConfigPtr->Pin[Index].Address & 1)
                     {
                        LocoNetSendPacket.data[2] |= 0x20;
                     }
                  }
                  else
                  {
                     /* Transmit the switch request on the Loconet bus */
                     LocoNetSendPacket.data[0] = MGV50AT_LOCONET_TURNOUT_COMMAND;
                     LocoNetSendPacket.data[1] = (ConfigPtr->Pin[Index].Address) & 0x7F;
                     LocoNetSendPacket.data[2] = ConfigPtr->Pin[Index].Address >> 7;
                     LocoNetSendPacket.data[2] |= 0x30;
                  }

                  /* Transmit, if transmit not succeeded it will be performed next time.. */
                  if (sendLocoNetPacket(&LocoNetSendPacket) == LN_DONE)
                  {
                     Mgv50AtLocoNetLedStart();
                     ConfigPtr->Pin[Index].Transmit = mgv50AtSensorTransmitOff;
                  }
               }
               else
               {
                  /* Initial change after power on skipped */
                  ConfigPtr->Pin[Index].Transmit = mgv50AtSensorTransmitOff;
               }
            }
         }
      }
   }
}

/**
 ******************************************************************************
 * @fn         void Mgv50AtTurnOutHandler(byte * DataPtr, TMgv50AtConfig * ConfigPtr)
 * @brief      Handle the Loconet TurnOur command.
 * @param      DataPtr Pointer to data with Loconet data.
 * @param      ConfigPtr Pointer to data with config settings of pins.
 * @return     None
 * @attention  -
 ******************************************************************************
 */
void Mgv50AtTurnOutHandler(byte * DataPtr, TMgv50AtConfig * ConfigPtr)
{
   uint16_t                                Address;
   uint8_t                                 Index = 0;

   Address = DataPtr[1] | (uint16_t) ((DataPtr[2] & 0x0F) << 8);

   /* Check if the address is present in the module. */
   while (Index < MGV50AT_NUMBER_OF_PINS)
   {
      if (Address == ConfigPtr->Pin[Index].Address)
      {
         /* If output set it... */
         if ((ConfigPtr->Pin[Index].Type == mgv50AtPinTypeOutput) &&
             (ConfigPtr->Pin[Index].OutputType != mgv50AtOutputTypeBlock))
         {
            if ((DataPtr[2] & 0x30) == 0x10)
            {
               /* CV2 option? */
               if (ConfigPtr->Pin[Index].LocoIoGetal_3 & 0x20)
               {
                  Mgv50AtSetPin(ConfigPtr->Pin[Index].PortOut, ConfigPtr->Pin[Index].PinNumber, 0);
                  ConfigPtr->Pin[Index].OutputActive = 0;
               }
               else
               {
                  Mgv50AtSetPin(ConfigPtr->Pin[Index].PortOut, ConfigPtr->Pin[Index].PinNumber, 1);
                  ConfigPtr->Pin[Index].OutputCnt = 0;
                  ConfigPtr->Pin[Index].OutputActive = 1;
               }
            }
            else if ((DataPtr[2] & 0x30) == 0x30)
            {
               if (ConfigPtr->Pin[Index].LocoIoGetal_3 & 0x20)
               {
                  Mgv50AtSetPin(ConfigPtr->Pin[Index].PortOut, ConfigPtr->Pin[Index].PinNumber, 1);
                  ConfigPtr->Pin[Index].OutputCnt = 0;
                  ConfigPtr->Pin[Index].OutputActive = 1;
               }
               else
               {
                  Mgv50AtSetPin(ConfigPtr->Pin[Index].PortOut, ConfigPtr->Pin[Index].PinNumber, 0);
                  ConfigPtr->Pin[Index].OutputActive = 0;
               }
            }
         }
      }
      Index++;
   }
}

/**
 ******************************************************************************
 * @fn         void Mgv50AtSensorJumper(TMgv50AtConfig * ConfigPtr)
 * @brief      Reset sensor status if jumper is set.
 * @param      ConfigPtr Pointer to data with config settings of pins.
 * @return     None
 * @attention  -
 ******************************************************************************
 */
void Mgv50AtSensorJumper(TMgv50AtConfig * ConfigPtr)
{
   uint8_t                                 Index;

   if (!(MGV50AT_SENSOR_JUMPER))
   {
      for (Index = 0; Index < MGV50AT_NUMBER_OF_PINS; Index++)
      {
         if (ConfigPtr->Pin[Index].Type == mgv50AtPinTypeInput)
         {
            /* Is it a block?! */
            if (ConfigPtr->Pin[Index].ConfigByte.Input.OpCode)
            {
               ConfigPtr->Pin[Index].SensorCnt = 0;
               ConfigPtr->Pin[Index].Transmit = mgv50AtSensorTransmitSkip;
            }
         }
      }
   }
}

/**
 ******************************************************************************
 * @fn         void Mgv50AtBlockOutput(byte * DataPtr, TMgv50AtConfig * ConfigPtr)
 * @brief      If a sensor command received check if an output is set as block detector.
 *             If it is set activate or de activate the output depending on status of the
 *             sensor.
 * @param      ConfigPtr Pointer to data with config settings of pins.
 * @return     None
 * @attention  -
 ******************************************************************************
 */
void Mgv50AtBlockOutput(byte * DataPtr, TMgv50AtConfig * ConfigPtr)
{

   uint8_t                                 Index = 0;
   uint16_t                                Address;

   Address = DataPtr[1] | (uint16_t) ((DataPtr[2] & 0x0F) << 8);
   if (DataPtr[2] & 0x20)
   {
      Address++;
   }

   /* Check if the address is present in the module. */
   while (Index < MGV50AT_NUMBER_OF_PINS)
   {
      if (Address == ConfigPtr->Pin[Index].Address)
      {
         /* Address present, if block output handle it... */
         if (ConfigPtr->Pin[Index].OutputType == mgv50AtOutputTypeBlock)
         {
            if (DataPtr[2] & 0x10)
            {
               Mgv50AtSetPin(ConfigPtr->Pin[Index].PortOut, ConfigPtr->Pin[Index].PinNumber, 0);
            }
            else
            {
               Mgv50AtSetPin(ConfigPtr->Pin[Index].PortOut, ConfigPtr->Pin[Index].PinNumber, 1);
            }
         }
      }
      Index++;
   }
}

/**
 ******************************************************************************
 * @fn         void Mgv50AtXpeerRead(lnMsg * LocoNetSendPacketPtr, TMgv50AtConfig * ConfigPtr, lnMsg * DataPtr, uint8_t * ValidDataPtr)
 * @brief      Compose the response on a XPeer read command.
 * @param      LocoNetSendPacketPtr Pointer to loconet transmit data.
 * @param      ConfigPtr Pointer to data with config settings of pins.
 * @param      DataPtr Pointer to data with Loconet data.
 * @param      ValidDataPtr Transmit the response yes / no.
 * @return     None
 * @attention  -
 ******************************************************************************
 */
void Mgv50AtXpeerRead(lnMsg * LocoNetSendPacketPtr, TMgv50AtConfig * ConfigPtr, lnMsg * DataPtr, uint8_t * ValidDataPtr)
{
   uint8_t                                 ConfigIndex = 0;
   uint8_t                                 ConfigIndexRest = 0;

   if ((DataPtr->px.d2 >= 3) && (DataPtr->px.d2 <= 50))
   {
      ConfigIndex = (DataPtr->px.d2 / 3) - 1;
      ConfigIndexRest = DataPtr->px.d2 % 3;

      /* Compose response for pin options to be read */
      switch (ConfigIndexRest)
      {
         case 0:
            *ValidDataPtr = 1;
            LocoNetSendPacketPtr->px.d6 = ConfigPtr->Pin[ConfigIndex].ConfigByte.ByteValue;
            LocoNetSendPacketPtr->px.d7 = ConfigPtr->Pin[ConfigIndex].LocoIoGetal_2;
            LocoNetSendPacketPtr->px.d8 = ConfigPtr->Pin[ConfigIndex].LocoIoGetal_3;
            if (ConfigPtr->Pin[ConfigIndex].Type == mgv50AtPinTypeOutput)
            {
               LocoNetSendPacketPtr->px.pxct2 = 0x02;
            }
            break;
         case 1:
            *ValidDataPtr = 1;
            LocoNetSendPacketPtr->px.d6 = ConfigPtr->Pin[ConfigIndex].LocoIoGetal_2;
            LocoNetSendPacketPtr->px.d7 = ConfigPtr->Pin[ConfigIndex].LocoIoGetal_3;
            LocoNetSendPacketPtr->px.d8 = 0;
            LocoNetSendPacketPtr->px.pxct2 = 0;
            if (ConfigPtr->Pin[ConfigIndex].Type == mgv50AtPinTypeOutput)
            {
               LocoNetSendPacketPtr->px.pxct2 = 0x08;
            }
            break;
         case 2:
            *ValidDataPtr = 1;
            LocoNetSendPacketPtr->px.d6 = ConfigPtr->Pin[ConfigIndex].LocoIoGetal_3;
            LocoNetSendPacketPtr->px.d7 = 0;
            LocoNetSendPacketPtr->px.d8 = 0;
            LocoNetSendPacketPtr->px.pxct2 = 0;
            if (ConfigPtr->Pin[ConfigIndex].Type == mgv50AtPinTypeOutput)
            {
               LocoNetSendPacketPtr->px.pxct2 = 0x04;
            }
            break;
      }
   }
   else if ((DataPtr->px.d2 >= 51) && (DataPtr->px.d2 <= 98))
   {
      ConfigIndex = (DataPtr->px.d2 / 3) - 1 - 16;
      ConfigIndexRest = DataPtr->px.d2 % 3;

      switch (ConfigIndexRest)
      {
            /* Compose response for Opcode read */
         case 0:
            *ValidDataPtr = 1;
            LocoNetSendPacketPtr->px.d6 = ConfigPtr->Pin[ConfigIndex].OpCodeCmdData[0] & 0x7F;
            LocoNetSendPacketPtr->px.d7 = ConfigPtr->Pin[ConfigIndex].OpCodeCmdData[1] & 0x7F;
            LocoNetSendPacketPtr->px.d8 = ConfigPtr->Pin[ConfigIndex].OpCodeCmdData[2] & 0x7F;
            LocoNetSendPacketPtr->px.pxct2 = (ConfigPtr->Pin[ConfigIndex].OpCodeCmdData[0] & 0x80) ? 0x02 : 0;
            LocoNetSendPacketPtr->px.pxct2 |= (ConfigPtr->Pin[ConfigIndex].OpCodeCmdData[1] & 0x80) ? 0x04 : 0;
            LocoNetSendPacketPtr->px.pxct2 |= (ConfigPtr->Pin[ConfigIndex].OpCodeCmdData[2] & 0x80) ? 0x08 : 0;
            break;
         case 1:
            *ValidDataPtr = 1;
            LocoNetSendPacketPtr->px.d6 = ConfigPtr->Pin[ConfigIndex].OpCodeCmdData[1] & 0x7F;
            LocoNetSendPacketPtr->px.d7 = ConfigPtr->Pin[ConfigIndex].OpCodeCmdData[2] & 0x7F;
            LocoNetSendPacketPtr->px.d8 = 0;
            LocoNetSendPacketPtr->px.pxct2 = (ConfigPtr->Pin[ConfigIndex].OpCodeCmdData[1] & 0x80) ? 0x02 : 0;
            LocoNetSendPacketPtr->px.pxct2 |= (ConfigPtr->Pin[ConfigIndex].OpCodeCmdData[2] & 0x80) ? 0x04 : 0;
            break;
         case 2:
            *ValidDataPtr = 1;
            LocoNetSendPacketPtr->px.d6 = ConfigPtr->Pin[ConfigIndex].OpCodeCmdData[2] & 0x7F;
            LocoNetSendPacketPtr->px.d7 = 0;
            LocoNetSendPacketPtr->px.d8 = 0;
            LocoNetSendPacketPtr->px.pxct2 = (ConfigPtr->Pin[ConfigIndex].OpCodeCmdData[2] & 0x80) ? 0x02 : 0;
            break;
      }
   }
}

/**
 ******************************************************************************
 * @fn         void Mgv50AtLocnetXPeerResponse(lnMsg *LocoNetSendPacketPtr ,TMgv50AtConfig * ConfigPtr)
 * @brief      Transmit the response of the loconet xpeer message.
 * @param      LocoNetSendPacketPtr Pointer to data with Loconet data to be transmitted.
 * @param      ConfigPtr Pointer to data with config settings of pins.
 * @param      DataPtr Pointer to received data, used for some response items.
 * @return     None
 * @attention  -
 ******************************************************************************
 */
void Mgv50AtLocnetXPeerResponse(lnMsg * LocoNetSendPacketPtr, TMgv50AtConfig * ConfigPtr, lnMsg * DataPtr)
{
   uint8_t                                 LocoNetTxCnt = 0;

   LocoNetSendPacketPtr->px.command = MGV50AT_LOCONET_X_PEER;
   LocoNetSendPacketPtr->px.mesg_size = MGV50AT_LOCONET_X_PEER_MSG_SIZE;
   LocoNetSendPacketPtr->px.src = ConfigPtr->UnitAddress;
   LocoNetSendPacketPtr->px.dst_h = 0x01;
   LocoNetSendPacketPtr->px.dst_l = 0x50;
   LocoNetSendPacketPtr->px.pxct1 = SW_MAJOR;
   LocoNetSendPacketPtr->px.d1 = DataPtr->px.d1;
   LocoNetSendPacketPtr->px.d2 = DataPtr->px.d2;
   LocoNetSendPacketPtr->px.d3 = SW_MINOR;
   LocoNetSendPacketPtr->px.d4 = 0;

   while ((sendLocoNetPacket(LocoNetSendPacketPtr) != LN_DONE) && (LocoNetTxCnt < MGV50AT_LOCONET_X_PEER_RESPONSE_TRY))
   {
      LocoNetTxCnt++;
   }

   if (LocoNetTxCnt < MGV50AT_LOCONET_X_PEER_RESPONSE_TRY)
   {
      Mgv50AtLocoNetLedStart();
   }
}

/**
 ******************************************************************************
 * @fn         void Mgv50AtXPeerMpWrite(lnMsg * LocoNetSendPacketPtr, TMgv50AtConfig * ConfigPtr, lnMsg * DataPtr, uint8_t * ValidDataPtr)
 * @brief      Handle the MP Xpeer data.
 * @param      LocoNetSendPacketPtr Pointer to loconet transmit data.
 * @param      ConfigPtr Pointer to data with config settings of pins.
 * @param      DataPtr Pointer to data with Loconet data.
 * @param      ValidDataPtr Transmit the response yes / no.
 * @return     None
 * @attention  -
 ******************************************************************************
 */
void Mgv50AtXPeerMpWrite(lnMsg * LocoNetSendPacketPtr, TMgv50AtConfig * ConfigPtr, lnMsg * DataPtr,
                         uint8_t * ValidDataPtr)
{
   uint8_t                                 Index;
   uint16_t                                MpWriteData = 0;
   uint16_t                                MpWriteDataMask = 0;

   MpWriteData = DataPtr->px.d6;
   MpWriteData |= (uint16_t) (DataPtr->px.d8) << 8;

   MpWriteDataMask = DataPtr->px.d5;
   MpWriteDataMask |= (uint16_t) (DataPtr->px.d7) << 8;

   MpWriteData |= (DataPtr->px.pxct2 & 0x02) ? 0x0080 : 0x00;
   MpWriteData |= (DataPtr->px.pxct2 & 0x08) ? 0x8000 : 0x00;
   MpWriteDataMask |= (DataPtr->px.pxct2 & 0x01) ? 0x0080 : 0x00;
   MpWriteDataMask |= (DataPtr->px.pxct2 & 0x04) ? 0x8000 : 0x00;

   /* Now check if pin is output, if yes so then handle the mask data */
   for (Index = 0; Index < MGV50AT_NUMBER_OF_PINS; Index++)
   {
      /* If output set it if mask required to do so ... */
      if ((ConfigPtr->Pin[Index].Type == mgv50AtPinTypeOutput) &&
          (ConfigPtr->Pin[Index].OutputType != mgv50AtOutputTypeBlock))
      {
         if (MpWriteDataMask & (1 << Index))
         {
            if (MpWriteData & (1 << Index))
            {
               /* CV2 option? */
               if (ConfigPtr->Pin[Index].LocoIoGetal_3 & 0x20)
               {
                  Mgv50AtSetPin(ConfigPtr->Pin[Index].PortOut, ConfigPtr->Pin[Index].PinNumber, 0);
                  ConfigPtr->Pin[Index].OutputActive = 0;
               }
               else
               {
                  Mgv50AtSetPin(ConfigPtr->Pin[Index].PortOut, ConfigPtr->Pin[Index].PinNumber, 1);
                  ConfigPtr->Pin[Index].OutputCnt = 0;
                  ConfigPtr->Pin[Index].OutputActive = 1;
               }
            }
            else
            {
               if (ConfigPtr->Pin[Index].LocoIoGetal_3 & 0x20)
               {
                  Mgv50AtSetPin(ConfigPtr->Pin[Index].PortOut, ConfigPtr->Pin[Index].PinNumber, 1);
                  ConfigPtr->Pin[Index].OutputCnt = 0;
                  ConfigPtr->Pin[Index].OutputActive = 1;
               }
               else
               {
                  Mgv50AtSetPin(ConfigPtr->Pin[Index].PortOut, ConfigPtr->Pin[Index].PinNumber, 0);
                  ConfigPtr->Pin[Index].OutputActive = 0;
               }
            }
         }
      }
   }

   /* Compose the response */
   *ValidDataPtr = 1;
   LocoNetSendPacketPtr->px.pxct2 = DataPtr->px.pxct2;
   LocoNetSendPacketPtr->px.d5 = DataPtr->px.d5;
   LocoNetSendPacketPtr->px.d6 = DataPtr->px.d6;
   LocoNetSendPacketPtr->px.d7 = DataPtr->px.d7;
   LocoNetSendPacketPtr->px.d8 = DataPtr->px.d8;
}

/**
 ******************************************************************************
 * @fn         void Mgv50AtXPeerMpRead(lnMsg * LocoNetSendPacketPtr, TMgv50AtConfig * ConfigPtr, lnMsg * DataPtr, uint8_t * ValidDataPtr)
 * @brief      Compose the answer for the Xpeer MP Read command.
 * @param      LocoNetSendPacketPtr Pointer to loconet transmit data.
 * @param      ConfigPtr Pointer to data with config settings of pins.
 * @param      DataPtr Pointer to data with Loconet data.
 * @param      ValidDataPtr Transmit the response yes / no.
 * @return     None
 * @attention  -
 ******************************************************************************
 */
void Mgv50AtXPeerMpRead(lnMsg * LocoNetSendPacketPtr, TMgv50AtConfig * ConfigPtr, lnMsg * DataPtr,
                        uint8_t * ValidDataPtr)
{
   uint8_t                                 Index;
   uint16_t                                MpReadData = 0;
   uint16_t                                MpReadDataMask = 0;

   /* Now check if pin is output, if yes so then handle the mask data */
   for (Index = 0; Index < MGV50AT_NUMBER_OF_PINS; Index++)
   {
      /* If output set it if mask required to do so ... */
      if (ConfigPtr->Pin[Index].Type == mgv50AtPinTypeInput)
      {
         if (ConfigPtr->Pin[Index].SensorCnt >= ConfigPtr->Pin[Index].SensorCntMax)
         {
            MpReadData |= (1 << Index);
         }
      }
      else
      {
         MpReadDataMask |= (1 << Index);
      }
   }

   /* Compose the response */
   *ValidDataPtr = 1;
   LocoNetSendPacketPtr->px.d5 = MpReadDataMask & 0x7F;
   LocoNetSendPacketPtr->px.d6 = MpReadData & 0x7F;
   LocoNetSendPacketPtr->px.d7 = (MpReadDataMask >> 8) & 0x7F;
   LocoNetSendPacketPtr->px.d8 = (MpReadData >> 8) & 0x7F;

   DataPtr->px.pxct2 = (MpReadDataMask & 0x0080) ? 0x01 : 0x00;
   DataPtr->px.pxct2 = (MpReadData & 0x0080) ? 0x02 : 0x00;
   DataPtr->px.pxct2 = (MpReadDataMask & 0x8000) ? 0x04 : 0x00;
   DataPtr->px.pxct2 = (MpReadData & 0x8000) ? 0x08 : 0x00;
}

/**
 ******************************************************************************
 * @fn         void Mgv50AtXPeerHandler(lnMsg * DataPtr, TMgv50AtConfig * ConfigPtr)
 * @brief      Handle the Loconet XPEER command.
 * @param      DataPtr Pointer to data with Loconet data.
 * @param      ConfigPtr Pointer to data with config settings of pins.
 * @return     None
 * @attention  -
 ******************************************************************************
 */
void Mgv50AtXPeerHandler(lnMsg * DataPtr, TMgv50AtConfig * ConfigPtr)
{
   uint16_t                                UnitAddress;
   uint8_t                                 ValidData = 0;
   uint8_t                                 ConfigIndex = 0;
   uint8_t                                 ValidEeprom = 0;
   uint8_t                                 ConfigIndexRest = 0;

   /* Is the xpeer message for us ? */
   UnitAddress = ((uint16_t) DataPtr->px.d5 << 8) | DataPtr->px.dst_l;

   Mgv50AtLocoNetSendPacket.px.pxct2 = 0;

   switch (DataPtr->px.dst_l)
   {
      case MGV50AT_LOCONET_X_PEER_GENERIC_ADDRESS:
         switch (DataPtr->px.d1)
         {
            case MGV50AT_LOCONET_X_PEER_COMMAND_WRITE:
               /* Reset */
               Mgv50AtResetModule(ConfigPtr);
               Mgv50AtUpdateEepromDataUnit(ConfigPtr);
               Mgv50AtLocoNetSendPacket.px.d6 = 0;
               Mgv50AtLocoNetSendPacket.px.d7 = 0;
               Mgv50AtLocoNetSendPacket.px.d8 = 0;
               ValidData = 1;
               ValidEeprom = 1;
               break;
            case MGV50AT_LOCONET_X_PEER_COMMAND_READ:
               /* Identify */
               Mgv50AtLocoNetSendPacket.px.d6 = (uint8_t) ((ConfigPtr->BlinkValue / MGV50AT_BLINK_TIME) - 1) << 4;
               Mgv50AtLocoNetSendPacket.px.d7 = ConfigPtr->UnitAddress;
               Mgv50AtLocoNetSendPacket.px.d8 = ConfigPtr->UnitAddress >> 8;
               ValidData = 1;
               break;
            default:
               break;
         }
         break;
   }

   if ((UnitAddress == ConfigPtr->UnitAddress) && (ValidData == 0))
   {
      // Request for us..
      switch (DataPtr->px.d2)
      {
         case 0:
            switch (DataPtr->px.d1)
            {
               case MGV50AT_LOCONET_X_PEER_COMMAND_READ:
                  /* LocoIo Setup Read */
                  ValidData = 1;
                  Mgv50AtLocoNetSendPacket.px.d5 = ConfigPtr->UnitAddress >> 8;
                  Mgv50AtLocoNetSendPacket.px.d6 = (uint8_t) ((ConfigPtr->BlinkValue / MGV50AT_BLINK_TIME) - 1) << 4;
                  Mgv50AtLocoNetSendPacket.px.d7 = 0;
                  Mgv50AtLocoNetSendPacket.px.d8 = 0;
                  break;
               case MGV50AT_LOCONET_X_PEER_COMMAND_WRITE:
                  /* LocoIo Setup Write */
                  ValidData = 1;
                  ConfigPtr->BlinkValue = ((DataPtr->px.d4 >> 4) + 1) * MGV50AT_BLINK_TIME;
                  Mgv50AtUpdateEepromDataUnit(ConfigPtr);
                  Mgv50AtLocoNetSendPacket.px.d5 = ConfigPtr->UnitAddress >> 8;
                  Mgv50AtLocoNetSendPacket.px.d6 = 0;
                  Mgv50AtLocoNetSendPacket.px.d7 = 0;
                  /* Reject other options */
                  Mgv50AtLocoNetSendPacket.px.d8 = DataPtr->px.d4 & 0xF0;
                  break;
               default:
                  break;

            }
            break;
         case 1:
            /* LocoIo Low Address */
            if (DataPtr->px.d1 == MGV50AT_LOCONET_X_PEER_COMMAND_READ)
            {
               ValidData = 1;
               Mgv50AtLocoNetSendPacket.px.d5 = ConfigPtr->UnitAddress >> 8;
               Mgv50AtLocoNetSendPacket.px.d6 = ConfigPtr->UnitAddress;
               Mgv50AtLocoNetSendPacket.px.d7 = 0;
               Mgv50AtLocoNetSendPacket.px.d8 = 0;
            }
            else if (DataPtr->px.d1 == MGV50AT_LOCONET_X_PEER_COMMAND_WRITE)
            {
               ValidData = 1;
               ConfigPtr->UnitAddress = ConfigPtr->UnitAddress & 0xFF00;
               ConfigPtr->UnitAddress |= DataPtr->px.d4;
               Mgv50AtUpdateEepromDataUnit(ConfigPtr);
               Mgv50AtLocoNetSendPacket.px.d5 = ConfigPtr->UnitAddress >> 8;
               Mgv50AtLocoNetSendPacket.px.d6 = 0;
               Mgv50AtLocoNetSendPacket.px.d7 = 0;
               Mgv50AtLocoNetSendPacket.px.d8 = DataPtr->px.d4;
            }
            break;
         case 2:
            /* LocoIo Subaddress */
            if (DataPtr->px.d1 == MGV50AT_LOCONET_X_PEER_COMMAND_READ)
            {
               ValidData = 1;
               Mgv50AtLocoNetSendPacket.px.d5 = ConfigPtr->UnitAddress >> 8;
               Mgv50AtLocoNetSendPacket.px.d6 = ConfigPtr->UnitAddress >> 8;
               Mgv50AtLocoNetSendPacket.px.d7 = 0;
               Mgv50AtLocoNetSendPacket.px.d8 = 0;
            }
            else if (DataPtr->px.d1 == MGV50AT_LOCONET_X_PEER_COMMAND_WRITE)
            {
               ValidData = 1;
               ConfigPtr->UnitAddress = ConfigPtr->UnitAddress & 0x00FF;
               ConfigPtr->UnitAddress |= (uint16_t) (DataPtr->px.d4) << 8;
               Mgv50AtUpdateEepromDataUnit(ConfigPtr);
               Mgv50AtLocoNetSendPacket.px.d5 = ConfigPtr->UnitAddress >> 8;
               Mgv50AtLocoNetSendPacket.px.d6 = 0;
               Mgv50AtLocoNetSendPacket.px.d7 = 0;
               Mgv50AtLocoNetSendPacket.px.d8 = DataPtr->px.d4;
            }
            break;
         default:
            if (DataPtr->px.d1 == MGV50AT_LOCONET_X_PEER_COMMAND_WRITE)
            {
               if ((DataPtr->px.d2 >= 3) && (DataPtr->px.d2 <= 50))
               {
                  /* Config data for the pins */
                  ConfigIndex = (DataPtr->px.d2 / 3) - 1;
                  ConfigIndexRest = DataPtr->px.d2 % 3;

                  switch (ConfigIndexRest)
                  {
                     case 0:
                        /* Configuration byte */
                        if ((DataPtr->px.d4 & 0x03) == 0x03)
                        {
                           ConfigPtr->Pin[ConfigIndex].Type = mgv50AtPinTypeInput;
                           Mgv50AtSetPinDirection(ConfigPtr->Pin[ConfigIndex].PortDdr,
                                                  ConfigPtr->Pin[ConfigIndex].PinNumber,
                                                  ConfigPtr->Pin[ConfigIndex].Type);

                           ConfigPtr->Pin[ConfigIndex].ConfigByte.ByteValue = DataPtr->px.d4;

                           ValidData = 1;
                           ValidEeprom = 1;
                           Mgv50AtLocoNetSendPacket.px.d5 = ConfigPtr->UnitAddress >> 8;
                           Mgv50AtLocoNetSendPacket.px.d6 = 0;
                           Mgv50AtLocoNetSendPacket.px.d7 = 0;
                           Mgv50AtLocoNetSendPacket.px.d8 = DataPtr->px.d4;
                        }
                        else
                        {
                           /* Output settings */
                           ConfigPtr->Pin[ConfigIndex].Type = mgv50AtPinTypeOutput;
                           Mgv50AtSetPinDirection(ConfigPtr->Pin[ConfigIndex].PortDdr,
                                                  ConfigPtr->Pin[ConfigIndex].PinNumber,
                                                  ConfigPtr->Pin[ConfigIndex].Type);
                           ConfigPtr->Pin[ConfigIndex].ConfigByte.ByteValue = DataPtr->px.d4;
                           if (ConfigPtr->Pin[ConfigIndex].ConfigByte.Output.ContactHighStart)
                           {
                              Mgv50AtSetPin(ConfigPtr->Pin[ConfigIndex].PortOut, ConfigPtr->Pin[ConfigIndex].PinNumber,
                                            1);
                           }
                           else
                           {
                              Mgv50AtSetPin(ConfigPtr->Pin[ConfigIndex].PortOut, ConfigPtr->Pin[ConfigIndex].PinNumber,
                                            0);
                           }
                           ConfigPtr->Pin[ConfigIndex].OutputType =
                              ((DataPtr->px.d4 & 0x04) ? mgv50AtOutputTypeHwPulse :
                               (DataPtr->px.d4 & 0x08) ? mgv50AtOutputTypePulse :
                               (DataPtr->px.d4 & 0x10) ? mgv50AtOutputTypeBlink :
                               (DataPtr->px.d4 & 0x20) ? mgv50AtOutputTypeNormal :
                               (DataPtr->px.d4 & 0x40) ? mgv50AtOutputTypeBlock : mgv50AtOutputTypeNormal);

                           ValidData = 1;
                           ValidEeprom = 1;

                           Mgv50AtLocoNetSendPacket.px.d5 = ConfigPtr->UnitAddress >> 8;
                           Mgv50AtLocoNetSendPacket.px.d6 = 0;
                           Mgv50AtLocoNetSendPacket.px.d7 = 0;
                           Mgv50AtLocoNetSendPacket.px.d8 = DataPtr->px.d4;
                           Mgv50AtLocoNetSendPacket.px.pxct2 = 0x08;
                        }
                        break;
                     case 1:
                        /* LocoIo number 1 */
                        ConfigPtr->Pin[ConfigIndex].Address = DataPtr->px.d4;
                        ConfigPtr->Pin[ConfigIndex].LocoIoGetal_2 = DataPtr->px.d4;

                        if (ConfigPtr->Pin[ConfigIndex].Type == mgv50AtPinTypeInput)
                        {
                           ConfigPtr->Pin[ConfigIndex].Address = ConfigPtr->Pin[ConfigIndex].Address << 1;
                        }
                        else
                        {
                           ConfigPtr->Pin[ConfigIndex].Address = ConfigPtr->Pin[ConfigIndex].Address;
                        }

                        /* Prepare the response */
                        ValidData = 1;
                        ValidEeprom = 1;
                        Mgv50AtLocoNetSendPacket.px.d5 = ConfigPtr->UnitAddress >> 8;
                        Mgv50AtLocoNetSendPacket.px.d6 = 0;
                        Mgv50AtLocoNetSendPacket.px.d7 = 0;
                        Mgv50AtLocoNetSendPacket.px.d8 = DataPtr->px.d4;
                        break;
                     case 2:
                        /* LocoIo Number - 2 */
                        ConfigPtr->Pin[ConfigIndex].Address |= (uint16_t) (DataPtr->px.d4 & 0x0F) << 8;
                        ConfigPtr->Pin[ConfigIndex].LocoIoGetal_3 = DataPtr->px.d4;
                        if (ConfigPtr->Pin[ConfigIndex].Type == mgv50AtPinTypeInput)
                        {
                           if (DataPtr->px.d4 & (0x20))
                           {
                              ConfigPtr->Pin[ConfigIndex].Address |= 1;
                           }
                        }

                        /* Update count max for pulse or block */
                        if (Mgv50AtConfig.Pin[ConfigIndex].LocoIoGetal_3 & 0x10)
                        {
                           Mgv50AtConfig.Pin[ConfigIndex].SensorCntMax = MGV50AT_SENSOR_BLOCK_CNT_SENSOR_ON;
                        }
                        else
                        {
                           Mgv50AtConfig.Pin[ConfigIndex].SensorCntMax = MGV50AT_SENSOR_BLOCK_CNT_PULSE_ON;
                        }

                        ValidData = 1;
                        ValidEeprom = 1;
                        Mgv50AtLocoNetSendPacket.px.d5 = ConfigPtr->UnitAddress >> 8;
                        Mgv50AtLocoNetSendPacket.px.d6 = 0;
                        Mgv50AtLocoNetSendPacket.px.d7 = 0;
                        Mgv50AtLocoNetSendPacket.px.d8 = DataPtr->px.d4;
                        break;
                  }
               }
               else if ((DataPtr->px.d2 >= 51) && (DataPtr->px.d2 <= 98))
               {
                  /* Write Opcode data, if the opcode is valid yes / no is NOT checked. It's the responsibility of the
                   * user. */
                  ConfigIndex = (DataPtr->px.d2 / 3) - 1 - 16;
                  ConfigIndexRest = DataPtr->px.d2 % 3;
                  Mgv50AtConfig.Pin[ConfigIndex].OpCodeCmdData[ConfigIndexRest] = DataPtr->px.d4;
                  if (DataPtr->px.pxct1 & 0x08)
                  {
                     Mgv50AtConfig.Pin[ConfigIndex].OpCodeCmdData[ConfigIndexRest] |= 0x80;
                  }
                  eeprom_write_byte((uint8_t *) (EEP_SETTINGS_OPCODE_BASE + (ConfigIndex * EEP_SETTINGS_OPCODE_SIZE) +
                                                 ConfigIndexRest),
                                    Mgv50AtConfig.Pin[ConfigIndex].OpCodeCmdData[ConfigIndexRest]);

                  ValidData = 1;
                  Mgv50AtLocoNetSendPacket.px.d5 = ConfigPtr->UnitAddress >> 8;
                  Mgv50AtLocoNetSendPacket.px.d6 = 0;
                  Mgv50AtLocoNetSendPacket.px.d7 = 0;
                  Mgv50AtLocoNetSendPacket.px.d8 = DataPtr->px.d4;
                  Mgv50AtLocoNetSendPacket.px.pxct2 = DataPtr->px.pxct1;
               }
            }
            else
            {
               Mgv50AtXpeerRead(&Mgv50AtLocoNetSendPacket, ConfigPtr, DataPtr, &ValidData);
            }
      }
   }
   else
   {
      /* An MP command ?! */
      if ((DataPtr->px.d2 == 0) && (DataPtr->px.d3 == 0))
      {
         switch (DataPtr->px.d1)
         {
            case MGV50AT_LOCONET_X_PEER_COMMAND_MP_WRITE:
               Mgv50AtXPeerMpWrite(&Mgv50AtLocoNetSendPacket, ConfigPtr, DataPtr, &ValidData);
               break;
            case MGV50AT_LOCONET_X_PEER_COMMAND_MP_READ:
               Mgv50AtXPeerMpRead(&Mgv50AtLocoNetSendPacket, ConfigPtr, DataPtr, &ValidData);
               break;
         }
      }
   }

   /* Transmit response if required and update EEPROM data if required */
   if (ValidData != 0)
   {
      Mgv50AtLocnetXPeerResponse(&Mgv50AtLocoNetSendPacket, ConfigPtr, DataPtr);
   }

   if (ValidEeprom != 0)
   {
      Mgv50AtUpdateEepromData(ConfigPtr, ConfigIndex);
   }

}

/**
 ******************************************************************************
 * @fn	      void Mgv50AtRcvLocoNet(lnMsg *LnPacket)
 * @brief      Process the content of a received message from the loconet bus.
 * @param      LnPacket  Pointer to Loconet variable.
 * @return     None
 * @attention  -
 ******************************************************************************
 */
void Mgv50AtRcvLocoNet(lnMsg * LnPacket)
{
   uint8_t                                 OpCode;
   uint8_t                                 OpcodeFamily;
   uint8_t                                 RxLength;

   /* Check received data from Loconet */
   OpCode = (uint8_t) LnPacket->sz.command;
   OpcodeFamily = (OpCode >> 5);
   switch (OpcodeFamily)
   {
      /* *INDENT-OFF* */
      case 0b100:
         /* 2 data bytes, inc checksum */
         RxLength = 2;
         break;
      case 0b101:
         /* 4 data bytes, inc checksum */
         RxLength = 4;
         break;
      case 0b110:
         /* 6 data bytes, inc checksum */
         RxLength = 6;
         break;
      case 0b111:
         /* N data bytes, inc checksum, next octet is N */
         RxLength = LnPacket->sz.mesg_size;
         break;
      default:
         /* Unknown... */
         RxLength = 0;
         break;
      /* *INDENT-ON* */
   }

   if (RxLength != 0)
   {
      /* If something received handle it... */
      switch (LnPacket->sz.command)
      {
         case MGV50AT_LOCONET_TURNOUT_COMMAND:
            Mgv50AtTurnOutHandler(LnPacket->data, &Mgv50AtConfig);
            break;
         case MGV50AT_LOCONET_SENSOR_COMMAND:
            Mgv50AtBlockOutput(LnPacket->data, &Mgv50AtConfig);
            break;
         case MGV50AT_LOCONET_X_PEER:
            Mgv50AtXPeerHandler(LnPacket, &Mgv50AtConfig);
            break;
         case MGV50AT_LOCONET_GLOBAL_POWER_OFF:
            break;
         case MGV50AT_LOCONET_GLOBAL_POWER_ON:
            Mgv50AtSensorJumper(&Mgv50AtConfig);
            break;
         default:
            break;
      }
   }
}

/**
 ******************************************************************************
 * @fn         void Mgv50AtInit(void)
 * @brief      Init the MGV50AT module, set variables, init loconet, read and
 *             check data from EEPROM, set pins and so on.
 * @return     None
 * @attention  -
 ******************************************************************************
 */

void Mgv50AtInit(void)
{
   uint8_t                                 Index;
   uint8_t                                 OpCodeIndex;

   /* Get initial status of the inputs */
   Mgv50AtSensorInput.PinB = PINB;
   Mgv50AtSensorInput.PinC = PINC;
   Mgv50AtSensorInput.PinD = PIND;

   /* Read the config from the EEPROM. Value 0xFFFF is assumed as invalid, i.e. default EEPROM data. If detected, a
    * default value will be set. */
   Mgv50AtConfig.UnitAddress = eeprom_read_word((uint16_t *) EEP_SETTINGS_BASE_ADDRESS);
   if (Mgv50AtConfig.UnitAddress == MGV50AT_INVALID_EEP_DATA_WORD)
   {
      Mgv50AtConfig.UnitAddress = MGV50AT_LOCO_IO_DEFAULT_ADDRESS;
   }

   Mgv50AtConfig.BlinkValue = eeprom_read_word((uint16_t *) EEP_SETTINGS_BASE_ADDRESS + 2);
   if (Mgv50AtConfig.BlinkValue == MGV50AT_INVALID_EEP_DATA_WORD)
   {
      Mgv50AtConfig.BlinkValue = MGV50AT_BLINK_TIME;
   }

   /* Set location of pins and origin of pin data to make life easier during processing of sensor inputs / setting
    * outputs .... */
   Mgv50AtConfig.Pin[0].SensorIn = &Mgv50AtSensorInput.PinC;
   Mgv50AtConfig.Pin[0].PinNumber = 5;
   Mgv50AtConfig.Pin[0].PortOut = PORTCOUT;
   Mgv50AtConfig.Pin[0].PortDdr = PORTCDIR;

   Mgv50AtConfig.Pin[1].SensorIn = &Mgv50AtSensorInput.PinC;
   Mgv50AtConfig.Pin[1].PinNumber = 4;
   Mgv50AtConfig.Pin[1].PortOut = PORTCOUT;
   Mgv50AtConfig.Pin[1].PortDdr = PORTCDIR;

   Mgv50AtConfig.Pin[2].SensorIn = &Mgv50AtSensorInput.PinD;
   Mgv50AtConfig.Pin[2].PinNumber = 0;
   Mgv50AtConfig.Pin[2].PortOut = PORTDOUT;
   Mgv50AtConfig.Pin[2].PortDdr = PORTDDIR;

   Mgv50AtConfig.Pin[3].SensorIn = &Mgv50AtSensorInput.PinC;
   Mgv50AtConfig.Pin[3].PinNumber = 3;
   Mgv50AtConfig.Pin[3].PortOut = PORTCOUT;
   Mgv50AtConfig.Pin[3].PortDdr = PORTCDIR;

   Mgv50AtConfig.Pin[4].SensorIn = &Mgv50AtSensorInput.PinD;
   Mgv50AtConfig.Pin[4].PinNumber = 1;
   Mgv50AtConfig.Pin[4].PortOut = PORTDOUT;
   Mgv50AtConfig.Pin[4].PortDdr = PORTDDIR;

   Mgv50AtConfig.Pin[5].SensorIn = &Mgv50AtSensorInput.PinC;
   Mgv50AtConfig.Pin[5].PinNumber = 2;
   Mgv50AtConfig.Pin[5].PortOut = PORTCOUT;
   Mgv50AtConfig.Pin[5].PortDdr = PORTCDIR;

   Mgv50AtConfig.Pin[6].SensorIn = &Mgv50AtSensorInput.PinD;
   Mgv50AtConfig.Pin[6].PinNumber = 2;
   Mgv50AtConfig.Pin[6].PortOut = PORTDOUT;
   Mgv50AtConfig.Pin[6].PortDdr = PORTDDIR;

   Mgv50AtConfig.Pin[7].SensorIn = &Mgv50AtSensorInput.PinD;
   Mgv50AtConfig.Pin[7].PinNumber = 3;
   Mgv50AtConfig.Pin[7].PortOut = PORTDOUT;
   Mgv50AtConfig.Pin[7].PortDdr = PORTDDIR;

   Mgv50AtConfig.Pin[8].SensorIn = &Mgv50AtSensorInput.PinC;
   Mgv50AtConfig.Pin[8].PinNumber = 1;
   Mgv50AtConfig.Pin[8].PortOut = PORTCOUT;
   Mgv50AtConfig.Pin[8].PortDdr = PORTCDIR;

   Mgv50AtConfig.Pin[9].SensorIn = &Mgv50AtSensorInput.PinD;
   Mgv50AtConfig.Pin[9].PinNumber = 4;
   Mgv50AtConfig.Pin[9].PortOut = PORTDOUT;
   Mgv50AtConfig.Pin[9].PortDdr = PORTDDIR;

   Mgv50AtConfig.Pin[10].SensorIn = &Mgv50AtSensorInput.PinC;
   Mgv50AtConfig.Pin[10].PinNumber = 0;
   Mgv50AtConfig.Pin[10].PortOut = PORTCOUT;
   Mgv50AtConfig.Pin[10].PortDdr = PORTCDIR;

   Mgv50AtConfig.Pin[11].SensorIn = &Mgv50AtSensorInput.PinB;
   Mgv50AtConfig.Pin[11].PinNumber = 6;
   Mgv50AtConfig.Pin[11].PortOut = PORTBOUT;
   Mgv50AtConfig.Pin[11].PortDdr = PORTBDIR;

   Mgv50AtConfig.Pin[12].SensorIn = &Mgv50AtSensorInput.PinB;
   Mgv50AtConfig.Pin[12].PinNumber = 7;
   Mgv50AtConfig.Pin[12].PortOut = PORTBOUT;
   Mgv50AtConfig.Pin[12].PortDdr = PORTBDIR;

   Mgv50AtConfig.Pin[13].SensorIn = &Mgv50AtSensorInput.PinD;
   Mgv50AtConfig.Pin[13].PinNumber = 5;
   Mgv50AtConfig.Pin[13].PortOut = PORTDOUT;
   Mgv50AtConfig.Pin[13].PortDdr = PORTDDIR;

   Mgv50AtConfig.Pin[14].SensorIn = &Mgv50AtSensorInput.PinD;
   Mgv50AtConfig.Pin[14].PinNumber = 6;
   Mgv50AtConfig.Pin[14].PortOut = PORTDOUT;
   Mgv50AtConfig.Pin[14].PortDdr = PORTDDIR;

   Mgv50AtConfig.Pin[15].SensorIn = &Mgv50AtSensorInput.PinD;
   Mgv50AtConfig.Pin[15].PinNumber = 7;
   Mgv50AtConfig.Pin[15].PortOut = PORTDOUT;
   Mgv50AtConfig.Pin[15].PortDdr = PORTDDIR;

   /* Read the pin data from EEPROM and check if it is valid, if not set a default value. Default is input type block,
    * non inverted */
   for (Index = 0; Index < MGV50AT_NUMBER_OF_PINS; Index++)
   {
      Mgv50AtConfig.Pin[Index].Address =
         eeprom_read_word((uint16_t *) (EEP_SETTINGS_PIN_DATA_START + (Index * EEP_SETTINGS_PIN_DATA_SIZE)));
      Mgv50AtConfig.Pin[Index].Type =
         eeprom_read_byte((uint8_t *) (EEP_SETTINGS_PIN_DATA_START + (Index * EEP_SETTINGS_PIN_DATA_SIZE) + 2));
      Mgv50AtConfig.Pin[Index].ConfigByte.ByteValue =
         eeprom_read_byte((uint8_t *) (EEP_SETTINGS_PIN_DATA_START + (Index * EEP_SETTINGS_PIN_DATA_SIZE) + 3));
      Mgv50AtConfig.Pin[Index].LocoIoGetal_2 =
         eeprom_read_byte((uint8_t *) (EEP_SETTINGS_PIN_DATA_START + (Index * EEP_SETTINGS_PIN_DATA_SIZE) + 4));
      Mgv50AtConfig.Pin[Index].LocoIoGetal_3 =
         eeprom_read_byte((uint8_t *) (EEP_SETTINGS_PIN_DATA_START + (Index * EEP_SETTINGS_PIN_DATA_SIZE) + 5));
      Mgv50AtConfig.Pin[Index].OutputType =
         eeprom_read_byte((uint8_t *) (EEP_SETTINGS_PIN_DATA_START + (Index * EEP_SETTINGS_PIN_DATA_SIZE) + 6));

      if (Mgv50AtConfig.Pin[Index].Address == MGV50AT_INVALID_EEP_DATA_WORD)
      {
         Mgv50AtConfig.Pin[Index].Address = Index;
      }

      if (Mgv50AtConfig.Pin[Index].Type == MGV50AT_INVALID_EEP_DATA_BYTE)
      {
         Mgv50AtConfig.Pin[Index].Type = mgv50AtPinTypeInput;
      }

      if (Mgv50AtConfig.Pin[Index].OutputType == MGV50AT_INVALID_EEP_DATA_BYTE)
      {
         Mgv50AtConfig.Pin[Index].Type = mgv50AtOutputTypeNormal;
      }

      if (Mgv50AtConfig.Pin[Index].ConfigByte.ByteValue == MGV50AT_INVALID_EEP_DATA_BYTE)
      {
         Mgv50AtConfig.Pin[Index].ConfigByte.ByteValue = MGV50AT_CONFIG_BYTE_DEFAULT;
      }

      if (Mgv50AtConfig.Pin[Index].LocoIoGetal_2 == MGV50AT_INVALID_EEP_DATA_BYTE)
      {
         Mgv50AtConfig.Pin[Index].LocoIoGetal_2 = Mgv50AtConfig.Pin[Index].Address & 0x7F;
      }

      if (Mgv50AtConfig.Pin[Index].LocoIoGetal_3 == MGV50AT_INVALID_EEP_DATA_BYTE)
      {
         Mgv50AtConfig.Pin[Index].LocoIoGetal_3 = (Mgv50AtConfig.Pin[Index].Address >> 9) & 0x7F;
         if (Index % 2)
         {
            Mgv50AtConfig.Pin[Index].LocoIoGetal_3 |= 0x10;
         }
      }

      /* Depending on the configuration settings set pin as input or output. */
      switch (Mgv50AtConfig.Pin[Index].Type)
      {
         case mgv50AtPinTypeInput:
            Mgv50AtSetPinDirection(Mgv50AtConfig.Pin[Index].PortDdr, Mgv50AtConfig.Pin[Index].PinNumber,
                                   Mgv50AtConfig.Pin[Index].Type);
            Mgv50AtSetPin(Mgv50AtConfig.Pin[Index].PortOut, Mgv50AtConfig.Pin[Index].PinNumber, 1);

            if (Mgv50AtConfig.Pin[Index].LocoIoGetal_3 & 0x10)
            {
               Mgv50AtConfig.Pin[Index].SensorCntMax = MGV50AT_SENSOR_BLOCK_CNT_SENSOR_ON;
            }
            else
            {
               Mgv50AtConfig.Pin[Index].SensorCntMax = MGV50AT_SENSOR_BLOCK_CNT_PULSE_ON;
            }
            break;
         case mgv50AtPinTypeOutput:
            Mgv50AtSetPinDirection(Mgv50AtConfig.Pin[Index].PortDdr, Mgv50AtConfig.Pin[Index].PinNumber,
                                   Mgv50AtConfig.Pin[Index].Type);
            if (Mgv50AtConfig.Pin[Index].ConfigByte.Output.ContactHighStart)
            {
               Mgv50AtSetPin(Mgv50AtConfig.Pin[Index].PortOut, Mgv50AtConfig.Pin[Index].PinNumber, 1);
            }
            else
            {
               Mgv50AtSetPin(Mgv50AtConfig.Pin[Index].PortOut, Mgv50AtConfig.Pin[Index].PinNumber, 0);
            }
            break;
      }

      Mgv50AtConfig.Pin[Index].Transmit = mgv50AtSensorTransmitIdle;
      Mgv50AtConfig.Pin[Index].OutputActive = 0;

      /* Read opcode data */
      for (OpCodeIndex = 0; OpCodeIndex < EEP_SETTINGS_OPCODE_SIZE; OpCodeIndex++)
      {
         Mgv50AtConfig.Pin[Index].OpCodeCmdData[OpCodeIndex] =
            eeprom_read_byte((uint8_t *) (EEP_SETTINGS_OPCODE_BASE + (Index * EEP_SETTINGS_OPCODE_SIZE) + OpCodeIndex));
         if (Mgv50AtConfig.Pin[Index].OpCodeCmdData[OpCodeIndex] == MGV50AT_INVALID_EEP_DATA_BYTE)
         {
            Mgv50AtConfig.Pin[Index].OpCodeCmdData[OpCodeIndex] = 0;
         }
      }
   }

   /* Set timer for reading the sensor inputs, flashing or pulse output. */
   /* Set timer 0, timer overrun ~2,04 msec @ 8Mhz */
   TCCR0 |= (1 << CS00) | (1 << CS01);
   TIMSK |= (1 << TOIE0);

   /* Loconet activity led */
   DDRB |= (1 << PB2);

   /* Jumper input pull up */
   PORTB |= (1 << PB4);

   /* Init loconet */
   initLocoNet(&Mgv50AtLocoNet);

   Mgv50AtSensorJumper(&Mgv50AtConfig);

   /* Now lets wait some time before continuing after supply voltage is applied .. */
   for (Index = 0; Index < 50; Index++)
   {
      _delay_ms(20);
   }
}

/**
 ******************************************************************************
 * @fn         void Mgv50AtMain(void)
 * @brief      Verify if data is present in the network interface or loconet <br>
 *             interface and process it.
 * @return     None
 * @attention  -
 ******************************************************************************
 */

void Mgv50AtMain(void)
{
   lnMsg                                  *RxPacket;

   /* Something received from Loconet? */
   RxPacket = recvLocoNetPacket();
   if (RxPacket)
   {
      Mgv50AtLocoNetLedStart();
      Mgv50AtRcvLocoNet(RxPacket);
   }

   /* Read and store inputs. */
   Mgv50AtSensorInput.PinB = PINB;
   Mgv50AtSensorInput.PinC = PINC;
   Mgv50AtSensorInput.PinD = PIND;

   /* If sensors updated by timer interrupts update the sensor info */
   if (Mgv50AtCheckSensors != 0)
   {
      Mgv50AtReportSensor(&Mgv50AtConfig);
      Mgv50AtCheckSensors = 0;

      Mgv50AtLocoNetLedCheck();
   }
}
