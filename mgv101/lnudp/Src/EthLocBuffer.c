
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
#include <stdio.h>
#include <avr/pgmspace.h>
#include "net.h"
#include "ln_buf.h"
#include "UserIo.h"
#include "timeout.h"
#include "enc28j60.h"
#include "ln_sw_uart.h"
#include "ln_interface.h"
#include "ip_arp_udp_tcp.h"
#include "EthLocBuffer.h"

/*
 *******************************************************************************************
 * Macro definitions
 *******************************************************************************************
 */

/* *INDENT-OFF* */

#define ETH_LOC_BUFFER_BUFFER_SIZE        200                     /**< Network receive buffer */
#define ETH_LOC_BUFFER_UDP_PORT           1235                    /**< Listen / write port for UDP */
#define ETH_LOC_BUFFER_MAX_LOCONET_TX     20                      /**< Max number of tries * LN_TX_RETRIES_MAX !! */

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

static uint8_t    EthLocBufferIpDst[4]  = {224,0,0,1};                    /**< IP destination address */
static uint8_t    EthLocBufferEthBuf[ETH_LOC_BUFFER_BUFFER_SIZE+1];       /**< Network receive buffer */
static LnBuf      EthLocBufferLocoNet;                                    /**< Loconet buffer */
static    lnMsg                                   EthLocBufferLocoNetSendPacket;

/* *INDENT-ON* */

/*
 *******************************************************************************************
 * Routines implementation
 *******************************************************************************************
 */

/**
 *******************************************************************************************
 * @fn         static void EthLocBufferPingCallBack(uint8_t * DataPtr)
 * @brief      If a ping is received for us blink the led.
 * @param      DataPtr Ping data pointer.
 * @return     None
 * @attention  -
 *******************************************************************************************
 */

static void EthLocBufferPingCallBack(uint8_t * DataPtr)
{
   UserIoSetLed(userIoLed6, userIoLedSetFlash);
}

/**
 *******************************************************************************************
 * @fn         static void EthLocBufferRcvLocoNet(uint8_t * UdpDataPtr, lnMsg * LnPacket)
 * @brief      Process the content of a received message from the loconet bus.
 * @param      LnPacket  Pointer to Loconet variable.
 * @return     None
 * @attention  -
 *******************************************************************************************
 */

static void EthLocBufferRcvLocoNet(uint8_t * UdpDataPtr, lnMsg * LnPacket)
{
   uint8_t                                 OpCode;
   uint8_t                                 OpcodeFamily;
   uint8_t                                 RxLength;

   /* Blink led to indicate something is received from Loconet bus */
   UserIoSetLed(userIoLed6, userIoLedSetFlash);

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

   /* Transmit to RocRail */
   if (RxLength != 0)
   {
      send_udp(EthLocBufferEthBuf, (char *)LnPacket->data, RxLength,
               ETH_LOC_BUFFER_UDP_PORT, EthLocBufferIpDst, ETH_LOC_BUFFER_UDP_PORT);

      send_udp(EthLocBufferEthBuf, (char *)LnPacket->data, RxLength,
               ETH_LOC_BUFFER_UDP_PORT, EthLocBufferIpDst, ETH_LOC_BUFFER_UDP_PORT);
   }
}

/**
 *******************************************************************************************
 * @fn	      void EthLocBufferRcvEthernet(uint8_t *RcvData, uint16_t RcvLength)
 * @brief      Check the content of a received message from the ethernet bus.
 * @param      RcvData  Pointer to array with received data.
 * @param      RcvLength Number of received bytes.
 * @return     None
 * @attention  -
 *******************************************************************************************
 */

static void EthLocBufferRcvEthernet(uint8_t * RcvData, uint16_t RcvLength)
{
   uint8_t                                 TxMaxCnt = 0;
   LN_STATUS                               TxStatus = LN_DONE;

   if (packetloop_icmp_tcp(RcvData, RcvLength) == 0)
   {
      /* Ip packet for us? */
      if (eth_type_is_ip_and_my_ip(RcvData, RcvLength))
      {
         if (ip_arp_udp_tcp_get_source_port(RcvData) == ETH_LOC_BUFFER_UDP_PORT)
         {
            /* Blink led to indicate something is received from the network with our IP */
            UserIoSetLed(userIoLed5, userIoLedSetFlash);
            if (RcvData[IP_PROTO_P] == IP_PROTO_UDP_V)
            {
               /* If new data unequal previous data transmit it */
               if (memcmp
                   (&EthLocBufferLocoNetSendPacket.data[0], &RcvData[UDP_DATA_P],
                    RcvData[UDP_LEN_L_P] - UDP_HEADER_LEN) != 0)
               {
                  /* UDP multicast data from RocRail... Process it. */
                  memset(&EthLocBufferLocoNetSendPacket, 0, sizeof(EthLocBufferLocoNetSendPacket));
                  EthLocBufferLocoNetSendPacket.sd.mesg_size = RcvData[UDP_LEN_L_P] - UDP_HEADER_LEN;

                  /* Copy the data so it can be transmitted to the Loconet */
                  memcpy(EthLocBufferLocoNetSendPacket.data, &RcvData[UDP_DATA_P],
                         EthLocBufferLocoNetSendPacket.sd.mesg_size);

                  do
                  {
                     TxStatus = sendLocoNetPacket(&EthLocBufferLocoNetSendPacket);
                     TxMaxCnt++;
                  }
                  while ((TxStatus != LN_DONE) && (TxMaxCnt < ETH_LOC_BUFFER_MAX_LOCONET_TX));
               }
			   else
			   {
                 /* Clear buffer, data twice received. If several query's where performed 
                    only the first one is forwarded.By cleaning buffer a next command is
					forwarded... */ 			    
			     memset(&EthLocBufferLocoNetSendPacket, 0, sizeof(EthLocBufferLocoNetSendPacket));
			   }
            }
         }
      }
   }
}

/**
 *******************************************************************************************
 * @fn	      void EthLocBufferInit(void)
 * @brief      Init the ENC2860 ntwork chip and init the loconet interface.
 * @return     None
 * @attention  -
 *******************************************************************************************
 */

void EthLocBufferInit(void)
{
   uint8_t                                 EthLocBufferIp[4];
   uint8_t                                 NetMask[4];
   uint8_t                                 RouterIp[4];
   uint8_t                                 EthLocBufferMac[6] = { 0x00, 0x4D, 0x56, 0x31, 0x30, 0x31 };

   /* Set up the 2860 */
   enc28j60Init(EthLocBufferMac);
   enc28j60clkout(2);
   _delay_ms(10);

   /* Magjack leds configuration */
   enc28j60PhyWrite(PHLCON, 0x474);
   _delay_ms(10);

   /* Init the network layer */
   UserIoIpSettingsGet(EthLocBufferIp, NetMask, RouterIp);
   init_ip_arp_udp_tcp(EthLocBufferMac, EthLocBufferIp, ETH_LOC_BUFFER_UDP_PORT);
   register_ping_rec_callback(&EthLocBufferPingCallBack);

   /* Init loconet */
   initLocoNet(&EthLocBufferLocoNet);
   memset(&EthLocBufferLocoNetSendPacket, 0, sizeof(EthLocBufferLocoNetSendPacket));
   EthLocBufferLocoNetSendPacket.sd.mesg_size = 0;
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
   uint16_t                                Enc2860Rcv = 0;
   lnMsg                                  *RxPacket;

   Enc2860Rcv = enc28j60PacketReceive(ETH_LOC_BUFFER_BUFFER_SIZE, EthLocBufferEthBuf);
   if (Enc2860Rcv != 0)
   {
      /* Something received, process it */
      EthLocBufferRcvEthernet(EthLocBufferEthBuf, Enc2860Rcv);
   }

   RxPacket = recvLocoNetPacket();
   if (RxPacket)
   {
      /* Something received from Loconet interface, process it */
      EthLocBufferRcvLocoNet(EthLocBufferEthBuf, RxPacket);
   }
}
