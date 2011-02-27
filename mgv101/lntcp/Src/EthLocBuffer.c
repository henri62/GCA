
/**
 *******************************************************************************************
 * @file        EthLocBuffer.c
 * @ingroup     EthLocBuffer
 * @attention
 *******************************************************************************************
 */

/*
 *******************************************************************************************
 * Standard include files
 *******************************************************************************************
 */

#include <inttypes.h>
#include <string.h>
#include "UserIo.h"
#include "ln_buf.h"
#include "uip_arp.h"
#include "ln_interface.h"
#include "EthLocBuffer.h"

/*
 *******************************************************************************************
 * Macro definitions
 *******************************************************************************************
 */

/* *INDENT-OFF* */

#define ETH_LOC_BUFFER_MAX_LOCONET_TX     20          /**< Max number of tries * LN_TX_RETRIES_MAX !! */

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


static LnBuf      EthLocBufferLocoNet;                /**< Loconet buffer */
static uint8_t    EthLocBufferTcpContinue;            /**< Process next Loconet message */
static uint8_t    EthLocBufferTcpIpRocRail;           /**< Index for TcpIp transmit */
static lnMsg      EthLocBufferTcpLoconetData;         /**< Copy of last received Loconet data for retry on TCPIP */
static uint8_t    EthLocBufferUipTimerCounter;        /**< Timer counter for uip handling */
static uint8_t    EthLocBufferUipArpTimerCounter;     /**< Timer counter for uip Arp handling */
static uint8_t    EthLocBufferTcpLoconetDataLength;   /**< Copy of last received Loconet data for retry on TCPIP */
/* *INDENT-ON* */

/*
 *******************************************************************************************
 * Routines implementation
 *******************************************************************************************
 */

/**
 *******************************************************************************************
 * @fn         static void EthLocBufferTransmitGlobalPowerOff(void)
 * @brief      Transmit Global PowerOff on the Loconet bus.
 * @return     None
 * @attention  -
 *******************************************************************************************
 */
static void EthLocBufferTransmitGlobalPowerOff(void)
{
   LN_STATUS                               TxStatus = LN_DONE;
   lnMsg                                   LocoNetSendPacket;
   uint8_t                                 TxMaxCnt = 0;

   EthLocBufferTcpContinue = 255;
   EthLocBufferTcpIpRocRail = 255;

   /* Transmit Power Off on Loconet */
   LocoNetSendPacket.sz.mesg_size = 2;
   LocoNetSendPacket.data[0] = 0x82;
   LocoNetSendPacket.data[1] = 0x7D;

   do
   {
      TxStatus = sendLocoNetPacket(&LocoNetSendPacket);
      TxMaxCnt++;
   }
   while ((TxStatus != LN_DONE) && (TxMaxCnt < ETH_LOC_BUFFER_MAX_LOCONET_TX));

   /* Blink TCPIP led indicating no connection is present to RocRail */
   UserIoSetLed(userIoLed4, userIoLedSetOff);
   UserIoSetLed(userIoLed5, userIoLedSetOff);
   UserIoSetLed(userIoLed4, userIoLedSetBlink);
   UserIoSetLed(userIoLed5, userIoLedSetBlink);
}

/**
 *******************************************************************************************
 * @fn         static void EthLocBufferRcvLocoNet(lnMsg * LnPacket)
 * @brief      Process the content of a received message from the Loconet bus and
 *             prepare it to put it on the TCPIP connection.
 * @param      LnPacket  Pointer to Loconet variable.
 * @return     None
 * @attention  -
 *******************************************************************************************
 */
static void EthLocBufferRcvLocoNet(lnMsg * LnPacket)
{
   uint8_t                                 OpCode;
   uint8_t                                 RxLength;
   uint8_t                                 OpcodeFamily;

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

   /* Copy the data so it can be transmitted to RocRail */
   if (RxLength != 0)
   {
      EthLocBufferTcpContinue = 0;
      EthLocBufferTcpLoconetDataLength = RxLength;
      memcpy(EthLocBufferTcpLoconetData.data, (char *)LnPacket->data, EthLocBufferTcpLoconetDataLength);
   }
}

/**
 *******************************************************************************************
 * @fn         void EthLocBufferProcessLoconet(void)
 * @brief      If Loconet data is received from the Loconet bus process it.
 *             if no connection is present to RocRail just flash the led and discard the
 *             data, else try to transmit the data to RocRail.
 * @return     None
 * @attention  -
 *******************************************************************************************
 */
void EthLocBufferProcessLoconet(void)
{
   lnMsg                                  *RxPacket;

   /* If no TCPIP connection discard all received Loconet data */
   if (EthLocBufferTcpIpRocRail == 255)
   {
      RxPacket = recvLocoNetPacket();
      if (RxPacket)
      {
         UserIoSetLed(userIoLed6, userIoLedSetFlash);
      }
   }
   else if (EthLocBufferTcpContinue == 1)
   {
      RxPacket = recvLocoNetPacket();
      if (RxPacket)
      {
         /* Something received from Loconet interface, process it */
         UserIoSetLed(userIoLed6, userIoLedSetFlash);
         EthLocBufferRcvLocoNet(RxPacket);
      }
   }
}

/**
 *******************************************************************************************
 * @fn	      void EthLocBufferInit(void)
 * @brief      Init the ENC2860 network chip and init the loconet interface.
 * @return     None
 * @attention  -
 *******************************************************************************************
 */

void EthLocBufferInit(void)
{
   /* Init loconet */
   initLocoNet(&EthLocBufferLocoNet);

   /* Set used variables to initial values */
   EthLocBufferTcpContinue = 0;
   EthLocBufferTcpIpRocRail = 255;
   EthLocBufferUipTimerCounter = 0;
   EthLocBufferUipArpTimerCounter = 0;
   EthLocBufferTcpLoconetDataLength = 0;
}

/**
 *******************************************************************************************
 * @fn	      void EthLocBufferMain(void)
 * @brief      Verify if data is present in the network interface or locnet <br>
 *             interface and process it.
 * @return     None
 * @attention  -
 *******************************************************************************************
 */

void EthLocBufferMain(void)
{
   EthLocBufferProcessLoconet();
}

/**
 *******************************************************************************************
 * @fn         void EthLocBufferTcpRcvEthernet(void)
 * @brief      The stack uses calls this function to perform network activities.
 * @return     None
 *******************************************************************************************
 */

void EthLocBufferTcpRcvEthernet(void)
{
   uint8_t                                 TxMaxCnt = 0;
   uint8_t                                 OpCode = 0;
   uint8_t                                 OpcodeFamily = 0;
   LN_STATUS                               TxStatus = LN_DONE;
   lnMsg                                   LocoNetSendPacket;
   lnMsg                                  *RxPacket;
   //char                                    DebugStr[20];

   if (uip_timedout() || uip_aborted() || uip_closed() || uip_closed())
   {
      /* Connection ended */
      EthLocBufferTransmitGlobalPowerOff();
   }

   if (uip_connected())
   {
      /* If connection to RocRail present allow forwarding of Loconet data from Loconet bus to TCPIP. */
      EthLocBufferTcpContinue = 1;
      EthLocBufferTcpIpRocRail = 1;
      UserIoSetLed(userIoLed5, userIoLedSetOff);
   }

   if (uip_acked())
   {
      //SerialTransmit("Ack\r\n");
      RxPacket = recvLocoNetPacket();
      if (RxPacket)
      {
         /* Something received from Loconet interface, process it */
         // UserIoSetLed(userIoLed6, userIoLedSetFlash);
         EthLocBufferRcvLocoNet(RxPacket);
         if (EthLocBufferTcpContinue == 0)
         {
            EthLocBufferTcpContinue = 2;
            uip_send(EthLocBufferTcpLoconetData.data, EthLocBufferTcpLoconetDataLength);
         }
      }
      else
      {
         EthLocBufferTcpContinue = 1;
      }
   }

   if (uip_rexmit())
   {
      /* Retransmit last transmitted data */
      uip_send(EthLocBufferTcpLoconetData.data, EthLocBufferTcpLoconetDataLength);
      //SerialTransmit("Ret\r\n");
   }

   if (uip_newdata())
   {
      /* Check if received data is Loconet format */
      if (uip_datalen() != 0)
      {
         OpCode = (uint8_t) uip_appdata[0];
         OpcodeFamily = (OpCode >> 5);
         switch (OpcodeFamily)
         {
           /* *INDENT-OFF* */
           case 0b100:
              /* 2 data bytes, inc checksum */
              LocoNetSendPacket.sz.mesg_size = 2;
              break;
           case 0b101:
              /* 4 data bytes, inc checksum */
              LocoNetSendPacket.sz.mesg_size = 4;
              break;
           case 0b110:
              /* 6 data bytes, inc checksum */
              LocoNetSendPacket.sz.mesg_size = 6;
              break;
           case 0b111:
              /* N data bytes, inc checksum, next octet is N */
              LocoNetSendPacket.sz.mesg_size = (uint8_t) uip_appdata[1];
              break;
           default:
              /* Unknown... */
              LocoNetSendPacket.sz.mesg_size = 0;
              break;
           /* *INDENT-ON* */
         }

         if (LocoNetSendPacket.sz.mesg_size > 0)
         {
            /* Blink led to indicate Loconet data is received from RocRail */
            UserIoSetLed(userIoLed5, userIoLedSetFlash);

            /* Copy the data so it can be transmitted to the Loconet */
            memcpy(LocoNetSendPacket.data, (uint8_t *) uip_appdata, LocoNetSendPacket.sz.mesg_size);

            do
            {
               TxStatus = sendLocoNetPacket(&LocoNetSendPacket);
               TxMaxCnt++;
            }
            while ((TxStatus != LN_DONE) && (TxMaxCnt < ETH_LOC_BUFFER_MAX_LOCONET_TX));

//            sprintf(DebugStr, "%02x %02x %02x %02x\r\n ", uip_appdata[0], uip_appdata[1], uip_appdata[2],
//                    uip_appdata[3]);
            //SerialTransmit(DebugStr);
         }
      }

      //SerialTransmit("Rx\r\n");
      if (EthLocBufferTcpContinue == 2)
      {
         /* A message is received but no ack on previous transmitted message. For some reason uIP does not detect this
          * or assumes an error... So retransmit.. And this situation occurs sometimes when MGV101 transmits data and
          * RR also transmits data... */
         //SerialTransmit("Rx rt\r\n");
         uip_send(EthLocBufferTcpLoconetData.data, EthLocBufferTcpLoconetDataLength);
      }
   }

   if (uip_poll())
   {
      /* If Loconet data present from Loconet bus transmit it... */
      if (EthLocBufferTcpContinue == 0)
      {
         //SerialTransmit("Tx\r\n");
         EthLocBufferTcpContinue = 2;
         uip_send(EthLocBufferTcpLoconetData.data, EthLocBufferTcpLoconetDataLength);
      }
   }
}
