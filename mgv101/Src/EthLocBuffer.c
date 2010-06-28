
/**
 *******************************************************************************************
 * @file 		EthLocBuffer.c                                         
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
#include <avr/pgmspace.h>
#include "net.h"
#include "UserIo.h"
#include "timeout.h"
#include "enc28j60.h"
#include "ln_sw_uart.h"
#include "serial_mega.h"
#include "ip_arp_udp_tcp.h"
#include "EthLocBuffer.h"

/*
 *******************************************************************************************
 * Macro definitions
 *******************************************************************************************
 */

#define ETH_LOC_BUFFER_BUFFER_SIZE  200                     /**< Network receive buffer */
#define ETH_LOC_BUFFER_UDP_PORT     1235                    /**< Listen / write port for UDP */

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

static uint8_t    EthLocBufferMac[6] = {0x00,0x23,0x8b,0x13,0xaa,0x6e};   /**< Mac Address */
static uint8_t    EthLocBufferIp[4];                                      /**< IP Address */
static uint8_t    EthLocBufferIpDst[4]  = {224,0,0,1};                    /**< IP destination address */
static uint8_t    EthLocBufferEthBuf[ETH_LOC_BUFFER_BUFFER_SIZE+1];       /**< Network receive buffer */
static LnBuf      EthLocBufferLocoNet;                                    /**< Loconet buffer */
static uint32_t   EthLocBufferRcvCntUdp;                                  /**< Counter for received UDP data */
static uint32_t   EthLocBufferRcvCntLoconet;                              /**< Counter for received loconet data */

/*
 *******************************************************************************************
 * Routines implementation
 *******************************************************************************************
 */

/**
 *******************************************************************************************
 * @fn	    	static void EthLocBufferPingCallBack(uint8_t * DataPtr)	
 * @brief   	If a ping is received for us blink the led. 
 * @param       DataPtr Ping data pointer.  
 * @return		None
 * @attention	- 
 *******************************************************************************************
 */

static void EthLocBufferPingCallBack(uint8_t * DataPtr)
{
   UserIoSetLed(userIoLed5, userIoLedSetFlash);
}

/**
 *******************************************************************************************
 * @fn	    	static void EthLocBufferRcvLocoNet(uint8_t * UdpDataPtr, lnMsg * LnPacket)
 * @brief   	Process the content of a received message from the loconet bus.
 * @param       LnPacket  Pointer to Loconet variable. 
 * @return		None
 * @attention	- 
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

   /* Copy the data so it can be transmitted to RocRail */
   if (RxLength != 0)
   {
      send_udp(EthLocBufferEthBuf, (char *)LnPacket->data, RxLength,
               ETH_LOC_BUFFER_UDP_PORT, EthLocBufferIpDst, ETH_LOC_BUFFER_UDP_PORT);

      EthLocBufferRcvCntLoconet++;
   }
}

/**
 *******************************************************************************************
 * @fn	    	void EthLocBufferRcvEthernet(uint8_t *RcvData, uint16_t RcvLength)	
 * @brief   	Check the content of a received message from the ethernet bus.
 * @param       RcvData  Pointer to array with received data. 
 * @param       RcvLength Number of received bytes.  
 * @return		None
 * @attention	- 
 *******************************************************************************************
 */

static void EthLocBufferRcvEthernet(uint8_t * RcvData, uint16_t RcvLength)
{
   lnMsg                                   LocoNetSendPacket;
   uint16_t                                RcvPort = 0;

   if (packetloop_icmp_tcp(RcvData, RcvLength) == 0)
   {
      /* Ip packet for us? */
      if (eth_type_is_ip_and_my_ip(RcvData, RcvLength))
      {
         RcvPort = ip_arp_udp_tcp_get_source_port(RcvData);

         if (RcvPort == ETH_LOC_BUFFER_UDP_PORT)
         {
            /* Blink led to indicate something is received from the network */
            UserIoSetLed(userIoLed5, userIoLedSetFlash);
            if (RcvData[IP_PROTO_P] == IP_PROTO_UDP_V)
            {
               /* UDP multicast data from RocRail... Process it. */
               memset(&LocoNetSendPacket, 0, sizeof(LocoNetSendPacket));
               LocoNetSendPacket.sd.mesg_size = RcvData[UDP_LEN_L_P] - UDP_HEADER_LEN;

               /* Copy the data so it can be transmitted to the Loconet */
               EthLocBufferRcvCntUdp++;
               memcpy(LocoNetSendPacket.data, &RcvData[UDP_DATA_P], LocoNetSendPacket.sd.mesg_size);
               sendLocoNetPacket(&LocoNetSendPacket);
            }
         }
      }
   }
}

/**
 *******************************************************************************************
 * @fn	    	void EthLocBufferInit(void)		
 * @brief   	Init the ENC2860 ntwork chip and init the loconet interface. 
 * @return		None
 * @attention	- 
 *******************************************************************************************
 */

void EthLocBufferInit(void)
{
   /* Set up the 2860 */
   enc28j60Init(EthLocBufferMac);
   enc28j60clkout(2);
   _delay_ms(1);

   /* Magjack leds configuration */
   enc28j60PhyWrite(PHLCON, 0x476);
   _delay_ms(1);

   /* Init the network layer */
   UserIoIpAddressGet(EthLocBufferIp);
   init_ip_arp_udp_tcp(EthLocBufferMac, EthLocBufferIp, ETH_LOC_BUFFER_UDP_PORT);
   register_ping_rec_callback(&EthLocBufferPingCallBack);

   /* Init loconet */
   initLocoNet(&EthLocBufferLocoNet);

   /* Set used variables to default */
   EthLocBufferRcvCntUdp = 0;
   EthLocBufferRcvCntLoconet = 0;
}

/**
 *******************************************************************************************
 * @fn	    	void EthLocBufferMain(void)		
 * @brief   	Verify if data is present in the network interface or locnet <br>
 *              interface and process it.  
 * @return		None
 * @attention	- 
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

/**
 *******************************************************************************************
 * @fn	    	void EthLocBufferGetReceiveCounters(uint32_t * LoconetCnt, uint32_t * UdpCnt)
 * @brief   	Receive actual counters of received data.
 * @param       LoconetCnt   Received messages from the loconet bus. 
 * @param       UdpCnt       Received messages from Rocrail / UDP .  
 * @return		None
 * @attention	- 
 *******************************************************************************************
 */

void EthLocBufferGetReceiveCounters(uint32_t * LoconetCnt, uint32_t * UdpCnt)
{
   *UdpCnt = EthLocBufferRcvCntUdp;
   *LoconetCnt = EthLocBufferRcvCntLoconet;
}
