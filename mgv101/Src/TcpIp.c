
/**
 *******************************************************************************************
 * @file 		TpcIp.c
 * @ingroup    TpcIp
 * @author		Robert Evers                                                      
 *******************************************************************************************
 */

/*
 *******************************************************************************************
 * Standard include files
 *******************************************************************************************
 */
#include "uip.h"
#include "nic.h"
#include "uip_arp.h"
#include "UserIo.h"
#include "uip_arp.h"
#include "enc28j60.h"
#include "EthLocBuffer.h"
#include "TcpIp.h"

/*
 *******************************************************************************************
 * Macro definitions
 *******************************************************************************************
 */
#define TCP_IP_BUF               ((struct uip_eth_hdr *)&uip_buf[0]) /**< uIP buffer location */
#define TCP_IP_TMR_OVERRUN        8                                  /**< Rounded timer overrun in msec */
#define TCP_IP_CNT_TIME           48 / TCP_IP_TMR_OVERRUN            /**< uIP update time */

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
uint8_t TcpIpUipTimerCounter;                   /**< uIP periodic counter. */
uint8_t TcpIpUipArpTimerCounter;                /**< uIP Arp periodic counter */
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
 * @fn	    	void TcpIpInit(void)
 * @brief   	Init the TCPIP / uIP stack.
 * @return		None
 * @attention	
 *******************************************************************************************
 */

void TcpIpInit(void)
{
   uint8_t                                 IpAddress[4];
   uint8_t                                 NetMask[4];
   uint8_t                                 RouterIp[4];

   /* Set up the network interface */
   UserIoIpSettingsGet(IpAddress, NetMask, RouterIp);
   nic_init();
   uip_init(IpAddress, NetMask, RouterIp);
   uip_arp_init();
   uip_listen(HTONS(ETH_LOC_BUFFER_BUFFER_TCP_PORT));

   TcpIpUipTimerCounter = 0;
   TcpIpUipArpTimerCounter = 0;

   TCCR0B = (1 << CS02);
}

/**
 *******************************************************************************************
 * @fn         void TcpIpMain(void)
 * @brief      Update the uIP stack.
 * @return     None
 * @attention
 *******************************************************************************************
 */
void TcpIpMain(void)
{
   uint8_t                                 Index;

   uip_len = nic_poll();
   if (uip_len == 0)
   {
      if (TIFR0 & (1 << TOV0))
      {
         TIFR0 |= (1 << TOV0);
         TcpIpUipTimerCounter++;
         // If timed out, call periodic function for each connection
         if (TcpIpUipTimerCounter > TCP_IP_CNT_TIME)
         {
            TcpIpUipTimerCounter = 0;
            for (Index = 0; Index < UIP_CONNS; Index++)
            {
               uip_periodic(Index);
               if (uip_len > 0)
               {
                  uip_arp_out();
                  nic_send();
               }
            }

            /* Call the ARP timer function every ~10 seconds. */
            if (++TcpIpUipArpTimerCounter >= 200)
            {
               uip_arp_timer();
               TcpIpUipArpTimerCounter = 0;
            }
         }
      }
   }
   else                                                                        // packet received
   {
      // process an IP packet
      if (TCP_IP_BUF->type == htons(UIP_ETHTYPE_IP))
      {
         uip_arp_ipin();
         uip_input();
         if (uip_len > 0)
         {
            uip_arp_out();
            nic_send();
         }
      }
      // process an ARP packet
      else if (TCP_IP_BUF->type == htons(UIP_ETHTYPE_ARP))
      {
         uip_arp_arpin();
         if (uip_len > 0)
            nic_send();
      }

      TcpIpUipTimerCounter = 0;
   }
}
