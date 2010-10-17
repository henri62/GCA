
/**
 ******************************************************************************
 * @file       MgvAxleCounter.c
 * @ingroup    MgvAxleCounter
 * @author     Robert Evers
 * @attention  Generic module for processing MgvAxleCounter data.
 ******************************************************************************
 */

/*
 ******************************************************************************
 * Standard include files
 ******************************************************************************
 */

#ifndef F_CPU
#error F_CPU not defined!
#endif

#include <avr/io.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdio.h>
#ifdef MGV_AXLE_COUNTER_DEBUG
#include "serial_mega.h"
#endif
#include "ln_buf.h"
#include "Version.h"
#include "ln_sw_uart.h"
#include "EEP_Settings.h"
#include "ln_interface.h"
#include "MgvAxleCounter.h"

/*
 *******************************************************************************************
 * Macro definitions
 *******************************************************************************************
 */

/* *INDENT-OFF* */

#define MGV_AXLE_COUNTER_LIST_MAX               8      /**< Number of axle counters */

#define MGV_AXLE_COUNTER_REPORT_TIME            1000   /**< Time (msec) after last axle change to report axle count */
#define MGV_AXLE_COUNTER_TIMER_OVERLOOP_TIME    1      /**< Timer overrun time (msec) */

#define MGV_AXLE_COUNTER_LOCONET_OPC_LISSY_REP  0xE4   /**< Loconet command counter report */
#define MGV_AXLE_COUNTER_LOCONET_CMD_RST        0xB0   /**< Loconet switch command is used to reset counters */
#define MGV_AXLE_COUNTER_LOCONET_X_PEER         0xE5   /**< Loconet XPeer command */
#define MGV_AXLE_COUNTER_X_PEER_MSG_SIZE        0x10           /**< Loconet Xpeer response number of bytes */
#define MGV_AXLE_COUNTER_LOCONET_ID             0x01   /**< ID of OPC_LISSY_REP */

#define MGV_AXLE_COUNTER_X_PEER_COMMAND_WRITE   0x01   /**< Loconet Xpeer write sv */
#define MGV_AXLE_COUNTER_X_PEER_COMMAND_READ    0x02   /**< Loconet Xpeer read sv */

#define MGV_AXLE_COUNTER_X_PEER_RESPONSE_TRY    20     /**< Number of retries for transmit xpeer response */

#define MGV_AXLE_COUNTER_X_PEER_CHANGE_BASE     0x01   /**< XPeer command to change the base address of the unit */
#define MGV_AXLE_COUNTER_X_PEER_CHANGE_COUNTER  0x02   /**< XPeer command to change address of an axle couner input */

#define MGV_AXLE_COUNTER_LOCONET_MAX_LOCONET_TX 20     /**< Max number of tries * LN_TX_RETRIES_MAX !! */
#define MGV_AXLE_COUNTER_INVALID_EEP_DATA       0xFFFF /**< Invalid EEPROM data */
#define MGV_AXLE_COUNTER_COUNT_MAX              10     /**< Number of counts for valid axle... */
#define MGV_AXLE_COUNTER_LOCONET_ACTVITY_ON     PORTB |=(1<<PB2)
#define MGV_AXLE_COUNTER_LOCONET_ACTVITY_OFF    PORTB &=~(1<<PB2)
#define MGV_ACLE_COUNTER_LOCONET_FLASH_LED_MAX  50

#define MGV_AXLE_COUNTER_COUNTER_RELOAD         113    /**< Timer reload for 1msec timer overrun */
#define MGV_AXLE_COUNTER_BASE_ADDRESS           0x0101 /**< Default base address axle counter */

/*
 *******************************************************************************************
 * Types
 *******************************************************************************************
  */

/**
 * Typedef struct for axle inputs.
 */
typedef struct
{
   uint8_t            CountRaw;                    /**< Axle 'debounce' count input */
   uint8_t            AxleCounted;                 /**< Actual cnt value is seen as axle... */
   uint16_t           AxleCnt;                     /**< Number of counted axles */
   uint16_t           AxleCntPrev;                 /**< Previous number of counted axles */
   uint16_t           Address;                     /**< Loconet assigned address */
   uint16_t           Timer;                       /**< Timer for transmit of counted axles */
}TMgvAxleCounterList;

/**
 * typedef for Axle counter module.
 */
typedef struct
{
   uint16_t             Address;                /**< Address of the axle counter unit */
   TMgvAxleCounterList  MgvAxleCounterList[MGV_AXLE_COUNTER_LIST_MAX];  /** List with axle count inputs */
}TMgvAxleCounter;
/*
 *******************************************************************************************
 * Variables
 *******************************************************************************************
 */

static LnBuf         MgvAxleCounterLocoNet ;             /**< Loconet variable */
TMgvAxleCounter      MgvAxleCounter;                     /**< The axle counter variables */
static uint8_t       MgvAxleCounterUpdate;               /**< Update info */
static uint8_t       MgvAxleCounterLoconetActivityCnt;   /**< Led flash counter */

/* *INDENT-ON* */

/*
 *******************************************************************************************
 * Routines implementation
 *******************************************************************************************
 */

/**
 *******************************************************************************************
 * @fn         SIGNAL(SIG_OVERFLOW1)
 * @brief      For each axle input check if an axle is detected. CntRaw is 'debounce'counter.
 *             AxleCnt is the actual counted values. With AxleCounted we assure one axle is not
 *             counted each time.
 * @return     None
 * @attention  -
 *******************************************************************************************
 */
SIGNAL(SIG_OVERFLOW0)
{
   uint8_t                                 Index = 0;

   /* Enable interrupts so Loconet receive is not blocked (too) long... */
   sei();

   TCNT0 = MGV_AXLE_COUNTER_COUNTER_RELOAD;

   MgvAxleCounterUpdate = 1;

   /* Now for each pin, check if axle is detected.. */
   for (Index = 0; Index < MGV_AXLE_COUNTER_LIST_MAX; Index++)
   {
#     ifdef MGV_AXLE_COUNTER_DEBUG
      /* If debugging active skip pin D1, it's in use for serial transmit */
      if (Index != 1)
      {
#     endif
         if (PIND & (1 << Index))
         {
            MgvAxleCounter.MgvAxleCounterList[Index].CountRaw++;
            if (MgvAxleCounter.MgvAxleCounterList[Index].CountRaw >= MGV_AXLE_COUNTER_COUNT_MAX)
            {
               MgvAxleCounter.MgvAxleCounterList[Index].CountRaw = MGV_AXLE_COUNTER_COUNT_MAX;
               if (MgvAxleCounter.MgvAxleCounterList[Index].AxleCounted == 0)
               {
                  MgvAxleCounter.MgvAxleCounterList[Index].AxleCnt++;
                  MgvAxleCounter.MgvAxleCounterList[Index].AxleCounted = 1;
                  if (MgvAxleCounter.MgvAxleCounterList[Index].AxleCnt == 1)
                  {
                     /* Force transmit if first axle detected */
                     MgvAxleCounter.MgvAxleCounterList[Index].Timer = MGV_AXLE_COUNTER_REPORT_TIME;
                  }
                  else
                  {
                     /* New axle detected, reset timer to reduce amount of data on bus */
                     MgvAxleCounter.MgvAxleCounterList[Index].Timer = 0;
                  }
               }
            }
         }
         else
         {
            if (MgvAxleCounter.MgvAxleCounterList[Index].CountRaw > 0)
            {
               MgvAxleCounter.MgvAxleCounterList[Index].CountRaw--;
            }
            else
            {
               MgvAxleCounter.MgvAxleCounterList[Index].AxleCounted = 0;
            }
         }
#     ifdef MGV_AXLE_COUNTER_DEBUG
      }
#     endif

   }
}

/**
 ******************************************************************************
 * @fn         void MgvAxleCounterLoconetActivityStart(void)
 * @brief      Reset the LoconetActivity counter.
 * @return     None
 * @attention  -
 ******************************************************************************
 */
void MgvAxleCounterLoconetActivityStart(void)
{
   MGV_AXLE_COUNTER_LOCONET_ACTVITY_OFF;
   MgvAxleCounterLoconetActivityCnt = 0;
   MGV_AXLE_COUNTER_LOCONET_ACTVITY_ON;
}

/**
 ******************************************************************************
 * @fn         void MgvAxleCounterLoconetActivityUpdate(void)
 * @brief      Increase counter, if counter exceeds limit switch Loconet
 *             activity led off.
 * @return     None
 * @attention  -
 ******************************************************************************
 */
void MgvAxleCounterLoconetActivityUpdate(void)
{
   MgvAxleCounterLoconetActivityCnt++;
   if (MgvAxleCounterLoconetActivityCnt > MGV_ACLE_COUNTER_LOCONET_FLASH_LED_MAX)
   {
      MgvAxleCounterLoconetActivityCnt = MGV_ACLE_COUNTER_LOCONET_FLASH_LED_MAX;
      MGV_AXLE_COUNTER_LOCONET_ACTVITY_OFF;
   }
}

/**
 ******************************************************************************
 * @fn         void MgvAxleCounterXPeerHandler(byte *DataPtr, TMgvAxleCounter *CounterPtr)
 * @brief      Process the XPeer command.
 * @param      DataPtr    Pointer to Loconet data.
 * @param      CounterPtr Axle counter data.
 * @return     None
 * @attention  -
 ******************************************************************************
 */
void MgvAxleCounterXPeerHandler(lnMsg * DataPtr, TMgvAxleCounter * CounterPtr, uint8_t Sv)
{
   uint16_t                                UnitAddress;
   uint8_t                                 LocoNetTxCnt = 0;
   uint8_t                                 ValidData = 0;
   uint8_t                                 ConfigIndex = 0;
   uint8_t                                 ConfigIndexRest = 0;
   lnMsg                                   MgvAxleCounterLocoNetSendPacket;

   /* Is the xpeer message for us ? */
   UnitAddress = ((uint16_t) DataPtr->px.d5 << 8) | DataPtr->px.dst_l;

   MgvAxleCounterLocoNetSendPacket.px.d1 = DataPtr->px.d1;
   MgvAxleCounterLocoNetSendPacket.px.d2 = Sv;
   MgvAxleCounterLocoNetSendPacket.px.d4 = 0;
   MgvAxleCounterLocoNetSendPacket.px.pxct2 = 0;

   if (DataPtr->px.dst_l == 0)
   {
      if (DataPtr->px.d1 == 2)
      {
         /* Identify */
         ValidData = 1;
         MgvAxleCounterLocoNetSendPacket.px.d5 = CounterPtr->Address >> 8;
         MgvAxleCounterLocoNetSendPacket.px.d6 = 0;
         MgvAxleCounterLocoNetSendPacket.px.d7 = CounterPtr->Address;
         MgvAxleCounterLocoNetSendPacket.px.d8 = CounterPtr->Address >> 8;
      }
   }
   else
   {
      if (((UnitAddress == CounterPtr->Address) && (ValidData == 0)) || (DataPtr->px.dst_l == 0x7F))
      {
         // Request for us..
         switch (Sv)
         {
            case 0:
               break;
            case 1:
               /* Axle counter Low Address */
               if (DataPtr->px.d1 == MGV_AXLE_COUNTER_X_PEER_COMMAND_WRITE)
               {
                  ValidData = 1;
                  CounterPtr->Address = CounterPtr->Address & 0xFF00;
                  CounterPtr->Address |= DataPtr->px.d4;

                  eeprom_write_word((uint16_t *) (EEP_SETTINGS_BASE_ADDRESS), CounterPtr->Address);

                  MgvAxleCounterLocoNetSendPacket.px.d5 = CounterPtr->Address;
                  MgvAxleCounterLocoNetSendPacket.px.d6 = 0;
                  MgvAxleCounterLocoNetSendPacket.px.d7 = 0;
                  MgvAxleCounterLocoNetSendPacket.px.d8 = DataPtr->px.d4;
               }
               break;
            case 2:
               /* Axle counter Subaddress */
               if (DataPtr->px.d1 == MGV_AXLE_COUNTER_X_PEER_COMMAND_WRITE)
               {
                  ValidData = 1;
                  CounterPtr->Address = CounterPtr->Address & 0x00FF;
                  CounterPtr->Address |= (uint16_t) (DataPtr->px.d4) << 8;
                  eeprom_write_word((uint16_t *) (EEP_SETTINGS_BASE_ADDRESS), CounterPtr->Address);

                  MgvAxleCounterLocoNetSendPacket.px.d4 = DataPtr->px.d4;
                  MgvAxleCounterLocoNetSendPacket.px.d5 = CounterPtr->Address >> 8;
                  MgvAxleCounterLocoNetSendPacket.px.d6 = 0;
                  MgvAxleCounterLocoNetSendPacket.px.d7 = 0;
                  MgvAxleCounterLocoNetSendPacket.px.d8 = DataPtr->px.d4;
               }
               break;
            default:
               if (DataPtr->px.d1 == MGV_AXLE_COUNTER_X_PEER_COMMAND_WRITE)
               {
                  if ((Sv >= 3) && (Sv <= 18))
                  {
                     /* Config data for the axle inputs. */
                     ConfigIndex = Sv - 3;
                     ConfigIndexRest = ConfigIndex % 2;
                     ConfigIndex = ConfigIndex / 2;

                     switch (ConfigIndexRest)
                     {
                        case 0:
                           ValidData = 1;
                           CounterPtr->MgvAxleCounterList[ConfigIndex].Address &= 0xFF00;
                           CounterPtr->MgvAxleCounterList[ConfigIndex].Address |= DataPtr->px.d4;
                           MgvAxleCounterLocoNetSendPacket.px.d4 = DataPtr->px.d4;
                           MgvAxleCounterLocoNetSendPacket.px.d5 = CounterPtr->Address >> 8;
                           MgvAxleCounterLocoNetSendPacket.px.d6 = DataPtr->px.d4;
                           MgvAxleCounterLocoNetSendPacket.px.d7 = DataPtr->px.d4;
                           MgvAxleCounterLocoNetSendPacket.px.d8 = DataPtr->px.d4;
                           eeprom_write_word((uint16_t *) (EEP_SETTINGS_AXLE_DATA + (ConfigIndex * 2)),
                                             CounterPtr->MgvAxleCounterList[ConfigIndex].Address);

                           break;
                        case 1:
                           /* High part of address */
                           ValidData = 1;
                           CounterPtr->MgvAxleCounterList[ConfigIndex].Address &= 0x00FF;
                           CounterPtr->MgvAxleCounterList[ConfigIndex].Address |= (uint16_t) (DataPtr->px.d4) << 8;
                           MgvAxleCounterLocoNetSendPacket.px.d4 = DataPtr->px.d4;
                           MgvAxleCounterLocoNetSendPacket.px.d5 = CounterPtr->Address >> 8;
                           MgvAxleCounterLocoNetSendPacket.px.d6 = DataPtr->px.d4;
                           MgvAxleCounterLocoNetSendPacket.px.d7 = DataPtr->px.d4;
                           MgvAxleCounterLocoNetSendPacket.px.d8 = DataPtr->px.d4;
                           eeprom_write_word((uint16_t *) (EEP_SETTINGS_AXLE_DATA + (ConfigIndex * 2)),
                                             CounterPtr->MgvAxleCounterList[ConfigIndex].Address);
                           break;
                     }
                  }
               }
               else
               {
                  if ((Sv >= 3) && (Sv <= 18))
                  {
                     ConfigIndex = Sv - 3;
                     ConfigIndexRest = ConfigIndex % 2;
                     ConfigIndex = ConfigIndex / 2;

                     switch (ConfigIndexRest)
                     {
                        case 0:
                           /* Low part of address */
                           ValidData = 1;
                           MgvAxleCounterLocoNetSendPacket.px.d5 = CounterPtr->Address >> 8;
                           MgvAxleCounterLocoNetSendPacket.px.d6 = CounterPtr->MgvAxleCounterList[ConfigIndex].Address;
                           MgvAxleCounterLocoNetSendPacket.px.d7 = CounterPtr->MgvAxleCounterList[ConfigIndex].Address;
                           MgvAxleCounterLocoNetSendPacket.px.d8 = CounterPtr->MgvAxleCounterList[ConfigIndex].Address;
                           break;

                        case 1:
                           /* High part of address */
                           ValidData = 1;
                           MgvAxleCounterLocoNetSendPacket.px.d5 = CounterPtr->Address >> 8;
                           MgvAxleCounterLocoNetSendPacket.px.d6 =
                              CounterPtr->MgvAxleCounterList[ConfigIndex].Address >> 8;
                           MgvAxleCounterLocoNetSendPacket.px.d7 =
                              CounterPtr->MgvAxleCounterList[ConfigIndex].Address >> 8;
                           MgvAxleCounterLocoNetSendPacket.px.d8 =
                              CounterPtr->MgvAxleCounterList[ConfigIndex].Address >> 8;
                           break;
                     }
                  }
               }
               break;
         }
      }
   }

   /* Transmit response if required */
   if (ValidData != 0)
   {
      MgvAxleCounterLocoNetSendPacket.px.command = MGV_AXLE_COUNTER_LOCONET_X_PEER;
      MgvAxleCounterLocoNetSendPacket.px.mesg_size = MGV_AXLE_COUNTER_X_PEER_MSG_SIZE;
      MgvAxleCounterLocoNetSendPacket.px.src = CounterPtr->Address;
      MgvAxleCounterLocoNetSendPacket.px.dst_h = 0x01;
      MgvAxleCounterLocoNetSendPacket.px.dst_l = 0x50;
      MgvAxleCounterLocoNetSendPacket.px.pxct1 = SW_MAJOR;
      MgvAxleCounterLocoNetSendPacket.px.d3 = SW_MINOR;

      while ((sendLocoNetPacket(&MgvAxleCounterLocoNetSendPacket) != LN_DONE) &&
             (LocoNetTxCnt < MGV_AXLE_COUNTER_X_PEER_RESPONSE_TRY))
      {
         LocoNetTxCnt++;
      }

      if (LocoNetTxCnt < MGV_AXLE_COUNTER_X_PEER_RESPONSE_TRY)
      {
         MgvAxleCounterLoconetActivityStart();
      }

   }
}

/**
 ******************************************************************************
 * @fn         uint8_t MgvAxleCounterLookUp(uint16_t Address, uint8_t &Index)
 * @brief      Check if received address is present in module.
 * @param      Address    Address to be checked.
 * @param      Index Location of the address if present.
 * @param      CounterPtr Axle counter data.
 * @return     0 = Not found, 1  = found.
 * @attention  -
 ******************************************************************************
 */
uint8_t MgvAxleCounterLookUp(uint16_t Address, uint8_t * Index, TMgvAxleCounter * CounterPtr)
{
   uint8_t                                 Found = 0;
   uint8_t                                 IndexLocal = 0;

   while ((IndexLocal < MGV_AXLE_COUNTER_LIST_MAX) && (Found == 0))
   {
      if (CounterPtr->MgvAxleCounterList[IndexLocal].Address == Address)
      {
         Found = 1;
      }
      else
      {
         IndexLocal++;
      }
   }
   *Index = IndexLocal;

   return (Found);
}

/**
 ******************************************************************************
 * @fn         void MgvAxleCounterUpdateCount(byte *DataPtr, TMgvAxleCounter * CounterPtr)
 * @brief      Process the get count command.
 * @param      DataPtr    Pointer to Loconet data.
 * @param      CounterPtr Axle counter data.
 * @return     None
 * @attention  -
 ******************************************************************************
 */
void MgvAxleCounterUpdateCount(TMgvAxleCounter * CounterPtr)
{
   uint8_t                                 TxMaxCnt = 0;
   LN_STATUS                               TxStatus = LN_DONE;
   uint8_t                                 Index;
   lnMsg                                   LoconetTxPacket;

#  ifdef MGV_AXLE_COUNTER_DEBUG
   char                                    DebugMsg[30];
#  endif

   for (Index = 0; Index < MGV_AXLE_COUNTER_LIST_MAX; Index++)
   {
      CounterPtr->MgvAxleCounterList[Index].Timer += MGV_AXLE_COUNTER_TIMER_OVERLOOP_TIME;
      if (CounterPtr->MgvAxleCounterList[Index].Timer > MGV_AXLE_COUNTER_REPORT_TIME)
      {
         if (CounterPtr->MgvAxleCounterList[Index].AxleCnt > CounterPtr->MgvAxleCounterList[Index].AxleCntPrev)
         {
            LoconetTxPacket.data[2] = MGV_AXLE_COUNTER_LOCONET_ID;
            LoconetTxPacket.data[3] = (CounterPtr->MgvAxleCounterList[Index].Address >> 7);
            LoconetTxPacket.data[4] = CounterPtr->MgvAxleCounterList[Index].Address & 0x7F;
            LoconetTxPacket.data[5] = (CounterPtr->MgvAxleCounterList[Index].AxleCnt >> 7);
            LoconetTxPacket.data[6] = CounterPtr->MgvAxleCounterList[Index].AxleCnt & 0x7F;
            LoconetTxPacket.data[7] = 0;
            LoconetTxPacket.sz.mesg_size = 8;                                  /* Inc checksum */
            LoconetTxPacket.sz.command = MGV_AXLE_COUNTER_LOCONET_OPC_LISSY_REP;

#           ifdef MGV_AXLE_COUNTER_DEBUG
            /* Put data on serial bus */
            sprintf(DebugMsg, "Ax %4u %u\r\n", CounterPtr->MgvAxleCounterList[Index].Address,
                    CounterPtr->MgvAxleCounterList[Index].AxleCnt);
            Serial_Buffer_Sent_Fill_Block((uint8_t *) DebugMsg);
#           endif

            MgvAxleCounterLoconetActivityStart();

            TxMaxCnt = 0;
            do
            {
               TxStatus = sendLocoNetPacket(&LoconetTxPacket);
               TxMaxCnt++;
            }
            while ((TxStatus != LN_DONE) && (TxMaxCnt < MGV_AXLE_COUNTER_LOCONET_MAX_LOCONET_TX));

            CounterPtr->MgvAxleCounterList[Index].AxleCntPrev = CounterPtr->MgvAxleCounterList[Index].AxleCnt;
         }
      }
   }
}

/**
 ******************************************************************************
 * @fn         void MgvAxleCounterResetCount(byte *DataPtr, TMgvAxleCounter * CounterPtr)
 * @brief      Process the get reset command for an axle.
 * @param      DataPtr    Pointer to Loconet data.
 * @param      CounterPtr Axle counter data.
 * @return     None
 * @attention  -
 ******************************************************************************
 */
void MgvAxleCounterResetCount(byte * DataPtr, TMgvAxleCounter * CounterPtr)
{
   uint8_t                                 Index = 0;
   uint16_t                                Address;

#  ifdef MGV_AXLE_COUNTER_DEBUG
   char                                    DebugMsg[30];
#  endif

   Address = DataPtr[1] | (uint16_t) ((DataPtr[2] & 0x0F) << 8);
   Address++;

   /* If address found, reset the applicable axle counter */
   if (MgvAxleCounterLookUp(Address, &Index, CounterPtr) != 0)
   {
      CounterPtr->MgvAxleCounterList[Index].AxleCnt = 0;
      CounterPtr->MgvAxleCounterList[Index].CountRaw = 0;
      CounterPtr->MgvAxleCounterList[Index].AxleCntPrev = 0;
      CounterPtr->MgvAxleCounterList[Index].AxleCounted = 0;

#     ifdef MGV_AXLE_COUNTER_DEBUG
      /* Put data on serial bus */
      Serial_Buffer_Sent_FillFlash(PSTR("AR : \0"));
      sprintf(DebugMsg, "%4u\r\n", Index);
      Serial_Buffer_Sent_Fill_Block((uint8_t *) DebugMsg);
#     endif
   }
}

/**
 ******************************************************************************
 * @fn         void MgvAxleCounterRcvLocoNet(lnMsg *LnPacket, TMgvAxleCounter *CounterPtr)
 * @brief      Process the content of a received message from the Loconet bus.
 * @param      LnPacket   Pointer to Loconet variable.
 * @param      CounterPtr Axle counter data.
 * @return     None
 * @attention  -
 ******************************************************************************
 */
void MgvAxleCounterRcvLocoNet(lnMsg * LnPacket, TMgvAxleCounter * CounterPtr)
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
      /* Is the command maybe for us? */
      switch (LnPacket->sz.command)
      {
         case MGV_AXLE_COUNTER_LOCONET_CMD_RST:
            MgvAxleCounterResetCount(LnPacket->data, CounterPtr);
            break;
         case MGV_AXLE_COUNTER_LOCONET_X_PEER:
            MgvAxleCounterXPeerHandler(LnPacket, CounterPtr, LnPacket->data[7]);
            break;
         default:
            break;
      }
   }
}

/**
 ******************************************************************************
 * @fn         void MgvAxleCounterInit(void)
 * @brief      Setup the hardware and other stuff for the axle counter.
 * @return     None
 * @attention  -
 ******************************************************************************
 */

void MgvAxleCounterInit(void)
{
   uint8_t                                 Index;

#  ifdef MGV_AXLE_COUNTER_DEBUG
   char                                    DebugMsg[40];
#  endif

   /* Set timer in CTC mode for 36kHz output signal for IR diode */
   TCCR2 = (1 << WGM21) | (1 << COM20) | (1 << CS21);
   OCR2 = 15;

   /* Set timer for counting the axle inputs */
   /* Set timer 0. Combined with the reload value every 1 msec a check is performed. */
   TCCR0 = (1 << CS00) | (1 << CS01);
   TIMSK |= (1 << TOIE0);

   /* Set inputs (with pull up) / outputs, PB3 for 36kHz output, PB2 for led */
   DDRB = (1 << PB2) | (1 << PB3);

   DDRD = 0;
   PORTD = 0xFF;

   MgvAxleCounterUpdate = 0;

   /* Init loconet */
   initLocoNet(&MgvAxleCounterLocoNet);

   /* Read the config from the EEPROM. Value 0xFFFF is assumed as invalid, i.e. default EEPROM data. If detected, a
    * default value will be set. */
   MgvAxleCounter.Address = eeprom_read_word((uint16_t *) EEP_SETTINGS_BASE_ADDRESS);
   if (MgvAxleCounter.Address == MGV_AXLE_COUNTER_INVALID_EEP_DATA)
   {
      MgvAxleCounter.Address = MGV_AXLE_COUNTER_BASE_ADDRESS;
   }

#  ifdef MGV_AXLE_COUNTER_DEBUG
   Serial_Buffer_Sent_FillFlash(PSTR("base address: \0"));
   sprintf(DebugMsg, "0x%04x \r\n", MgvAxleCounter.Address);
   Serial_Buffer_Sent_Fill_Block((uint8_t *) DebugMsg);
#  endif

   for (Index = 0; Index < MGV_AXLE_COUNTER_LIST_MAX; Index++)
   {
      MgvAxleCounter.MgvAxleCounterList[Index].Address =
         eeprom_read_word((uint16_t *) (EEP_SETTINGS_AXLE_DATA + (Index * 2)));
      if (MgvAxleCounter.MgvAxleCounterList[Index].Address == MGV_AXLE_COUNTER_INVALID_EEP_DATA)
      {
         MgvAxleCounter.MgvAxleCounterList[Index].Address = Index + 1;
      }

#     ifdef MGV_AXLE_COUNTER_DEBUG
      sprintf(DebugMsg, "In %02d Add %u\r\n", Index, MgvAxleCounter.MgvAxleCounterList[Index].Address);
      Serial_Buffer_Sent_Fill_Block((uint8_t *) DebugMsg);
#     endif

      MgvAxleCounter.MgvAxleCounterList[Index].Timer = 0;
      MgvAxleCounter.MgvAxleCounterList[Index].AxleCnt = 0;
      MgvAxleCounter.MgvAxleCounterList[Index].CountRaw = 0;
      MgvAxleCounter.MgvAxleCounterList[Index].AxleCntPrev = 0;
      MgvAxleCounter.MgvAxleCounterList[Index].AxleCounted = 0;
   }
}

/**
 ******************************************************************************
 * @fn         void MgvAxleCounterMain(void)
 * @brief      Verify if data is present in loconet buffer <br>
 *             and process it. Check the status of the inputs and if
 *             required transmit the status of the axle input.
 * @return     None
 * @attention  -
 ******************************************************************************
 */

void MgvAxleCounterMain(void)
{
   lnMsg                                  *RxPacket;

   /* Something received from Loconet? */
   RxPacket = recvLocoNetPacket();
   if (RxPacket)
   {
      MgvAxleCounterLoconetActivityStart();
      MgvAxleCounterRcvLocoNet(RxPacket, &MgvAxleCounter);
   }

   if (MgvAxleCounterUpdate != 0)
   {
      MgvAxleCounterUpdateCount(&MgvAxleCounter);
      MgvAxleCounterUpdate = 0;

      MgvAxleCounterLoconetActivityUpdate();
   }
}
