
/**
 *******************************************************************************************
 * @file 		TcpIp.c
 * @ingroup    TcpIp
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

/* *INDENT-OFF* */
#define TCP_IP_BUF               ((struct uip_eth_hdr *)&uip_buf[0]) /**< uIP buffer location */
#define TCP_IP_TMR_OVERRUN        2                                  /**< Rounded timer overrun in msec */
#define TCP_IP_CNT_TIME           14 / TCP_IP_TMR_OVERRUN            /**< uIP update time */
#define TCP_IP_ARP_CNT_TIME       10000 / TCP_IP_CNT_TIME            /**< Arp update every ~10msec */

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
uint16_t TcpIpUipArpTimerCounter;                /**< uIP Arp periodic counter */

/* *INDENT-OFF* */
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
 * @fn         SIGNAL(SIG_OVERFLOW0)
 * @brief      Timer interrupt.
 * @return     None
 * @attention
 *******************************************************************************************
 */

SIGNAL(SIG_OVERFLOW0)
{
   sei();
   TcpIpUipTimerCounter++;
}

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

   TCCR0B = (1 << CS01) | (1<<CS00);
   TIMSK0 |= (1<<TOIE0);
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
         // If timed out, call periodic function for each connection
         if (TcpIpUipTimerCounter >= TCP_IP_CNT_TIME)
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
            if (++TcpIpUipArpTimerCounter >= TCP_IP_ARP_CNT_TIME)
            {
               uip_arp_timer();
               TcpIpUipArpTimerCounter = 0;
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
