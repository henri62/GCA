
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
#include <avr/interrupt.h>
#include "net.h"
#include "Serial.h"
#include "ln_buf.h"
#include "userio.h"
#include "timeout.h"
#include "version.h"
#include "enc28j60.h"
#include "ln_sw_uart.h"
#include "ln_interface.h"
#include "ip_arp_udp_tcp.h"
#include "ethlocbuffer.h"
#include "ringbuffer.h"


/*
 *******************************************************************************************
 * Macro definitions
 *******************************************************************************************
 */

/* *INDENT-OFF* */

#define ETH_LOC_BUFFER_BUFFER_SIZE        200                     /**< Network receive buffer */
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

static Linkstate	link_state;
static uint8_t		ip_address[4];
static uint8_t		ip_netmask[4];
static uint8_t		mac_address[6];

static uint8_t    	mc_address[4]  = {224,0,0,1};                    
static uint16_t		mc_port = 1235;

static uint8_t    	eth_buffer[ETH_LOC_BUFFER_BUFFER_SIZE+1];       /**< Network receive buffer */
static LnBuf      	loco_buffer;                                    /**< Loconet buffer */
static lnMsg   		loco_net_message;
static MGVMsg		mgv_config_packet;
static uint8_t      eth_send_buffer[64];
static RingBuffer_t ring_buffer;
static uint8_t		unit_id;

/* *INDENT-ON* */

/*
 *******************************************************************************************
 * Routines implementation
 *******************************************************************************************
 */

/**
 *******************************************************************************************
 * @fn         static void send_to_rocrail()
 * @brief      Send a loconet message from the udp buffer onto the network
 * @return     None
 * @attention  -
 *******************************************************************************************
 */
 
static void send_to_rocrail(void)
{
	uint8_t                                 rxlength;
	uint8_t									i = 0;
	uint8_t									tmpMsg[16]; // 1 locomsg

	if (RingBuffer_IsEmpty(&ring_buffer))
		return;

	/* read length of message */
	rxlength = RingBuffer_Remove(&ring_buffer);

	/* read x bytes from the buffer */
	for (i=0; i<rxlength;i++)
	{
		tmpMsg[i] = RingBuffer_Remove(&ring_buffer);
	}
	
	/* transmit data to enc28j60 */
	send_udp(eth_buffer, (char*)tmpMsg, rxlength, mc_port, mc_address, mc_port);
}

/**
 *******************************************************************************************
 * @fn         static void eth_locbuffer_rcv_loconet(uint8_t * UdpDataPtr, lnMsg * LnPacket)
 * @brief      Process the content of a received message from the loconet bus.
 * @param      LnPacket  Pointer to Loconet variable.
 * @return     None
 * @attention  -
 *******************************************************************************************
 */

static void receive_from_loconet(uint8_t * UdpDataPtr, lnMsg * LnPacket)
{
   uint8_t                                 OpCode;
   uint8_t                                 OpcodeFamily;
   uint8_t                                 rxlength;
   uint8_t									i = 0;
     
   /* Check received data from Loconet */
   OpCode = (uint8_t) LnPacket->sz.command;

   OpcodeFamily = (OpCode >> 5);
   switch (OpcodeFamily)
   {
      /* *INDENT-OFF* */
      case 0b100:
         /* 2 data bytes, inc checksum */
         rxlength = 2;
         break;
      case 0b101:
         /* 4 data bytes, inc checksum */
         rxlength = 4;
         break;
      case 0b110:
         /* 6 data bytes, inc checksum */
         rxlength = 6;
         break;
      case 0b111:
         /* N data bytes, inc checksum, next octet is N */
         rxlength = LnPacket->sz.mesg_size;
         break;
      default:
         /* Unknown... */
         rxlength = 0;
         break;
      /* *INDENT-ON* */
   }

   /* Add to local udp send buffer */
	if (rxlength == 0)
	{
		return;
	}
	
	RingBuffer_Insert( &ring_buffer, rxlength );
	while (i<rxlength)
	{
		if (RingBuffer_IsFull(&ring_buffer))
		{
			/* transmit 1 message to make space */
		   send_to_rocrail();
		} 		
		RingBuffer_Insert( &ring_buffer, LnPacket->data[i] );
		i++;
	}
	
}

/**
 *******************************************************************************************
 * @fn	      void broadcast_netpacket(void)
 * @brief      Create a net info packet and add it to the transmit buffer
 * @return     None
 * @attention  -
 *******************************************************************************************
 */
static void broadcast_netpacket(void)
{
	uint8_t hardware_version = enc28j60getrev();
	uint8_t my_version = (SW_MINOR & ~0xF0) | (SW_MAJOR << 4 & 0xF0);

	RingBuffer_Insert( &ring_buffer, 18 );
	RingBuffer_Insert( &ring_buffer, MGV_INFO );
	RingBuffer_Insert( &ring_buffer, unit_id );
	RingBuffer_Insert( &ring_buffer, ip_address[0] );
	RingBuffer_Insert( &ring_buffer, ip_address[1] );
	RingBuffer_Insert( &ring_buffer, ip_address[2] );
	RingBuffer_Insert( &ring_buffer, ip_address[3] );
	
	RingBuffer_Insert( &ring_buffer, ip_netmask[0] );
	RingBuffer_Insert( &ring_buffer, ip_netmask[1] );
	RingBuffer_Insert( &ring_buffer, ip_netmask[2] );
	RingBuffer_Insert( &ring_buffer, ip_netmask[3] );
	
	RingBuffer_Insert( &ring_buffer, mac_address[0] );
	RingBuffer_Insert( &ring_buffer, mac_address[1] );
	RingBuffer_Insert( &ring_buffer, mac_address[2] );
	RingBuffer_Insert( &ring_buffer, mac_address[3] );
	RingBuffer_Insert( &ring_buffer, mac_address[4] );
	RingBuffer_Insert( &ring_buffer, mac_address[5] );
	
	RingBuffer_Insert( &ring_buffer, my_version );
	RingBuffer_Insert( &ring_buffer, hardware_version );	

}


/**
 *******************************************************************************************
 * @fn	      void process_mgv_packet(mgvMsg * message)
 * @brief      Process mgv101 configuration packet
 * @param      message  Pointer to mgvMsg struct
 * @return     None
 * @attention  -
 *******************************************************************************************
 */
static void process_mgv_packet(MGVMsg * message)
{
	switch (message->query.command)
	{
		case MGV_INFO:
			if (message->query.myaddress != 0x0)
				return;
		
			broadcast_netpacket();

		break;
		case MGV_SETNET:		
			if ( (message->query.myaddress == unit_id) )
			{					
				userio_set_ip(message->net.ipaddress, message->net.netmask, message->net.macaddres);	
			}		
		break;
		case MGV_SETID:
			if ( (message->query.myaddress == unit_id) )
			{		
				unit_id = message->id.newaddress;
				userio_set_id(&unit_id);
				return; 
			}		
		break;
	}
}

/**
 *******************************************************************************************
 * @fn	      uint8_t is_valid_multicast_packet(uint8_t * buf, uint16_t len)
 * @brief      Check if ethernet packet is valid and matches our multicast group address
 * @param      buf  Pointer to packet buffer
 * @param      len Length of the buffer
 * @return     0/1
 * @attention  -
 *******************************************************************************************
 */
uint8_t is_valid_multicast_packet(uint8_t * buf, uint16_t len)
{
	uint8_t	i = 0;
	if (len < 42)
	{
	  return 0;
	}
	if (buf[ETH_TYPE_H_P] != ETHTYPE_IP_H_V || buf[ETH_TYPE_L_P] != ETHTYPE_IP_L_V)
	{
	  return 0;
	}
	if (buf[IP_HEADER_LEN_VER_P] != 0x45)
	{
	  // must be IP V4 and 20 byte header
	  return 0;
	}
	while (i < 4)
	{
	  if (buf[IP_DST_P + i] != mc_address[i])
	  {
		 return 0;
	  }
	  i++;
	}
	
	return 1;
}

/**
 *******************************************************************************************
 * @fn	      void receive_from_ethernet(uint8_t *RcvData, uint16_t RcvLength)
 * @brief      Check the content of a received message from the ethernet bus.
 * @param      RcvData  Pointer to array with received data.
 * @param      RcvLength Number of received bytes.
 * @return     None
 * @attention  -
 *******************************************************************************************
 */

static void receive_from_ethernet(uint8_t * RcvData, uint16_t RcvLength)
{
	uint8_t                                 TxMaxCnt = 0;
	uint8_t 								PacketLength = 0;
	LN_STATUS                               TxStatus = LN_DONE;

	/* ICMP packet? */
	if (packetloop_icmp_tcp(RcvData, RcvLength) != 0)
	{
		return;
	}
   
	/* Ip packet for us? */
	if (is_valid_multicast_packet(RcvData, RcvLength) == 0)
	{
		return;
	}
	
	if (ip_arp_udp_tcp_get_source_port(RcvData) != mc_port)
	{
		return;
	}
	
	if (RcvData[IP_PROTO_P] != IP_PROTO_UDP_V)
	{
		return;
	}

	PacketLength = RcvData[UDP_LEN_L_P] - UDP_HEADER_LEN;
	
	if( RcvData[UDP_DATA_P] & (uint8_t)0x80 )
	{
		/* UDP multicast data from RocRail... Process it. */
		memset(&loco_net_message, 0, sizeof(loco_net_message));
		loco_net_message.sd.mesg_size = PacketLength;

		/* Copy the data so it can be transmitted to the Loconet */
		memcpy(loco_net_message.data, &RcvData[UDP_DATA_P], loco_net_message.sd.mesg_size);
	
		do
		{
			TxStatus = sendLocoNetPacket(&loco_net_message);
			TxMaxCnt++;
		}
		while ((TxStatus != LN_DONE) && (TxMaxCnt < ETH_LOC_BUFFER_MAX_LOCONET_TX));	
	}

	// Mgv 101 specific packet
	if( RcvData[UDP_DATA_P] & (uint8_t)MGV_MASK)
	{
		memset(&mgv_config_packet, 0, sizeof(mgv_config_packet));
		memcpy(mgv_config_packet.data, &RcvData[UDP_DATA_P], RcvLength>16?16:RcvLength);
		process_mgv_packet(&mgv_config_packet);
	}

}

/**
 *******************************************************************************************
 * @fn	      void eth_locbuffer_init(void)
 * @brief      Init the ENC2860 ntwork chip and init the loconet interface.
 * @return     None
 * @attention  -
 *******************************************************************************************
 */

void eth_locbuffer_init(void)
{
	
	/* Init the network layer */
	userio_get_settings(ip_address, ip_netmask, mac_address, &unit_id);
      					   

	/* Set up the 2860 */
	enc28j60Init(mac_address);
	enc28j60clkout(2);
	_delay_ms(10);

	/* Magjack leds configuration */
	enc28j60PhyWrite(PHLCON, 0x474);
	_delay_ms(10);

	init_ip_arp_udp_tcp(mac_address, ip_address, mc_port);
	
	/* Init udp transmit buffer */
	RingBuffer_InitBuffer(&ring_buffer, eth_send_buffer, sizeof(eth_send_buffer));   

	/* Init loconet */
	initLocoNet(&loco_buffer);
	memset(&loco_net_message, 0, sizeof(loco_net_message));
	loco_net_message.sd.mesg_size = 0;
	
	link_state = down;
	
}

/**
 *******************************************************************************************
 * @fn	      void eth_locbuffer_main(void)
 * @brief      Verify if data is present in the network interface or locnet <br>
 *             interface and process it.
 * @return     None
 * @attention  -
 *******************************************************************************************
 */

void eth_locbuffer_main(void)
{
	uint16_t                                Enc2860Rcv = 0;
	lnMsg                                  *RxPacket;

	if (link_state == down && enc28j60linkup() == 1)
	{
		link_state = up;
		broadcast_netpacket();
	} else if (link_state == up && enc28j60linkup() == 0)
	{
		link_state = down;
	}

	Enc2860Rcv = enc28j60PacketReceive(ETH_LOC_BUFFER_BUFFER_SIZE, eth_buffer);
	if (Enc2860Rcv != 0)
	{
		userio_set_led(userIoLed4, userIoLedSetOn);
		receive_from_ethernet(eth_buffer, Enc2860Rcv);
		userio_set_led(userIoLed4, userIoLedSetOff);
	}

	RxPacket = recvLocoNetPacket();
	if (RxPacket)
	{
		userio_set_led(userIoLed5, userIoLedSetOn); 
		receive_from_loconet(eth_buffer, RxPacket);
		userio_set_led(userIoLed5, userIoLedSetOff); 
	}

	if (!RingBuffer_IsEmpty(&ring_buffer))
	{
		userio_set_led(userIoLed6, userIoLedSetOn); 
		send_to_rocrail();
		userio_set_led(userIoLed6, userIoLedSetOff); 
	}      

	
}
