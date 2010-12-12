
/**
 ******************************************************************************************
 * @file       LoconetTxBuffer.c
 * @ingroup    LoconetTxBuffer
 * @author		Robert Evers                                                      
 * @attention
 ******************************************************************************************
 */

/*
 ******************************************************************************************
 * Standard include files
 ******************************************************************************************
 */
#include <string.h>
#include <inttypes.h>
#include "ln_interface.h"
#include "LoconetTxBuffer.h"

/*
 *******************************************************************************************
 * Macro definitions
 *******************************************************************************************
 */
#define LOCONET_TX_BUFFER_DATA_SIZE            16     /**< Data size */
#define LOCONET_TX_BUFFER_SIZE                 10     /**< Buffer size */

/*
 *******************************************************************************************
 * Types
 *******************************************************************************************
  */

/**
 * Typedef struct for Loconet transmit buffer
 */
typedef struct
{
   uint8_t                                 Data[LOCONET_TX_BUFFER_DATA_SIZE];    /**< One entry of the buffer */
   uint8_t                                 DataLength;                           /**< Number of bytes in entry of buffer to transmit */
} TLoconetTxBuffer;

/*
 *******************************************************************************************
 * Variables
 *******************************************************************************************
 */

lnMsg                                   LoconetTxBuffer[LOCONET_TX_BUFFER_SIZE]; /**< Tx buffer */
uint8_t                                 LoconetTxBufferIndexStart;               /**< Tx buffer index start */
uint8_t                                 LoconetTxBufferIndexEnd;                 /**< Tx buffer index end */

/*
 *******************************************************************************************
 * Prototypes
 *******************************************************************************************
 */

/*
 *******************************************************************************************
 * Routines implementation
 *******************************************************************************************
 */

/**
 *******************************************************************************************
 * @fn         void LoconetTxBufferInit(void)
 * @brief      Init the Loconet buffer module.
 * @return     None
 * @attention	
 *******************************************************************************************
 */
void LoconetTxBufferInit(void)
{
   memset(LoconetTxBuffer, 0, sizeof(LoconetTxBuffer));
   LoconetTxBufferIndexEnd = 0;
   LoconetTxBufferIndexStart = 0;
}

/**
 ******************************************************************************
 * @fn         uint8_t LoconetTxBufferBufferSpace(void)
 * @brief      Calculate free space in Loconet Tx buffer
 * @return     0 (no space) .. LOCONET_TX_BUFFER_SIZE (max space)
 * @attention  it's assumed if end en start index are equal the buffer is
 *             empty. If start overruns end this is a false assumption...
 *             For mow, live with it.
 ******************************************************************************
 */
uint8_t LoconetTxBufferBufferSpace(void)
{
   uint8_t                                 Result;

   if (LoconetTxBufferIndexEnd == LoconetTxBufferIndexStart)
   {
      Result = LOCONET_TX_BUFFER_SIZE;
   }
   else if (LoconetTxBufferIndexEnd > LoconetTxBufferIndexStart)
   {
      Result = LoconetTxBufferIndexEnd - LoconetTxBufferIndexStart;
   }
   else
   {
      Result = LoconetTxBufferIndexStart - LoconetTxBufferIndexEnd;
   }

   return (Result);
}

/**
 *******************************************************************************************
 * @fn         void LoconetTxBufferMain(void)
 * @brief      Main the Loconet buffer module. If data is present it is tried to be
 *             transmitted on the Loconet bus.
 * @return     None
 * @attention
 *******************************************************************************************
 */
void LoconetTxBufferMain(void)
{
   if (LoconetTxBufferIndexStart != LoconetTxBufferIndexEnd)
   {
      if (sendLocoNetPacket(&LoconetTxBuffer[LoconetTxBufferIndexStart]) == LN_DONE)
      {
         LoconetTxBufferIndexStart++;
         LoconetTxBufferIndexStart %= LOCONET_TX_BUFFER_SIZE;
      }
   }
}

/**
 *******************************************************************************************
 * @fn         uint8_t LoconetTxBufferSet(lnMsg *DataPtr)
 * @brief      Copy data for Loconet bus into the transmit buffer.
 * @param      DataPtr Complete Loconet message.
 * @return     None
 * @attention
 *******************************************************************************************
 */
uint8_t LoconetTxBufferSet(lnMsg * DataPtr)
{
   uint8_t                                 Result = 0;

   if (LoconetTxBufferBufferSpace())
   {
      memcpy(&LoconetTxBuffer[LoconetTxBufferIndexEnd], DataPtr, sizeof(LoconetTxBuffer[LoconetTxBufferIndexEnd]));
      LoconetTxBufferIndexEnd++;
      LoconetTxBufferIndexEnd %= LOCONET_TX_BUFFER_SIZE;
      Result = 1;
   }
   else
   {
      /* Keep trying to transmit if buffer is full.... */
      LoconetTxBufferMain();
   }

   return (Result);
}
