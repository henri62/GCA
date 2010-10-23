/*----------------------------------------------------------------------------
 Copyright:      Radig Ulrich  mailto: mail@ulrichradig.de
 Author:         Radig Ulrich
 Remarks:        
 known Problems: none
 Version:        24.10.2007
 Description:    Ethernet Stack

 Dieses Programm ist freie Software. Sie können es unter den Bedingungen der 
 GNU General Public License, wie von der Free Software Foundation veröffentlicht, 
 weitergeben und/oder modifizieren, entweder gemäß Version 2 der Lizenz oder 
 (nach Ihrer Option) jeder späteren Version. 

 Die Veröffentlichung dieses Programms erfolgt in der Hoffnung, 
 daß es Ihnen von Nutzen sein wird, aber OHNE IRGENDEINE GARANTIE, 
 sogar ohne die implizite Garantie der MARKTREIFE oder der VERWENDBARKEIT 
 FÜR EINEN BESTIMMTEN ZWECK. Details finden Sie in der GNU General Public License. 

 Sie sollten eine Kopie der GNU General Public License zusammen mit diesem 
 Programm erhalten haben. 
 Falls nicht, schreiben Sie an die Free Software Foundation, 
 Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA. 
------------------------------------------------------------------------------*/

#include <avr/interrupt.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "enc28j60.h"
#include "stack.h"

#define STACK_ARP_GRACIOUS_MAX           6
#define STACK_ARP_GRACIOUS_INTERVAL_TIME 250


TCP_PORT_ITEM TCP_PORT_TABLE[MAX_APP_ENTRY] = // Port-Tabelle
{
	{0,0}
};

unsigned char myip[4];
unsigned char netmask[4];
unsigned char router_ip[4];
unsigned int IP_id_counter = 0;
unsigned char eth_buffer[MTU_SIZE+1];

struct arp_table arp_entry[MAX_ARP_ENTRY];

//TCP Stack Size
//+1 damit eine Verbindung bei vollen Stack abgewiesen werden kann
struct tcp_table tcp_entry[MAX_TCP_ENTRY+1]; 

PING_STRUCT ping;

unsigned char arp_gratuitous_tmr_cnt;
unsigned char arp_gratuitous_tx_cnt;

//----------------------------------------------------------------------------
//Converts integer variables to network Byte order
unsigned int htons(unsigned int val)
{
  return HTONS(val);
}
//----------------------------------------------------------------------------
//Converts integer variables to network Byte order
unsigned long htons32(unsigned long val)
{
  return HTONS32(val);
}

/**
 ******************************************************************************
 * @fn          void stack_set_ip_settings (uint8_t *myIpPtr, uint8_t *myNetMask, uint8_t *myRouterIp)
 * @brief      Set the IP related settings.
 * @param      myIpPtr
 * @param      myNetMask
 * @param      myRouterIp
 * @return     None
 * @attention
 ******************************************************************************
 */
void stack_set_ip_settings(uint8_t * myIpPtr, uint8_t * myNetMask, uint8_t * myRouterIp)
{
   memcpy(myip, myIpPtr, sizeof(myip));
   memcpy(netmask, myNetMask, sizeof(netmask));
   memcpy(router_ip, myRouterIp, sizeof(myRouterIp));
}

//----------------------------------------------------------------------------
//Setup the network interface
void stack_init (void)
{
   eth.timer = 0;

   arp_gratuitous_tmr_cnt = 0;
   arp_gratuitous_tx_cnt = 0;

   /* NIC Initialisieren */
   enc_init();
}

//----------------------------------------------------------------------------
//Verwaltung des TCP Timers
void tcp_timer_call (void)
{
   unsigned char index;

	for (index = 0;index<MAX_TCP_ENTRY;index++)
	{
	   if (tcp_entry[index].txretry  != 0)
	   {
		if (tcp_entry[index].time == 0)
		{
			if (tcp_entry[index].ip != 0)
			{
				tcp_entry[index].time = TCP_MAX_ENTRY_TIME;
				if ((tcp_entry[index].error_count++) > MAX_TCP_ERRORCOUNT)
				{
					ETH_INT_DISABLE;
					tcp_entry[index].status =  RST_FLAG | ACK_FLAG;
					create_new_tcp_packet(0,index);
					tcp_entry[index].status =RETRY_ABORT_FLAG;
					find_and_start (index);
					ETH_INT_ENABLE;
					tcp_index_del(index);
				}
				else
				{
				   tcp_entry[index].status = RETRY_FLAG;
					find_and_start (index);
				}
			}
		}
		else
		{
			if (tcp_entry[index].time != TCP_TIME_OFF)
			{
				tcp_entry[index].time--;
			}
		}
	}
	}
}

//----------------------------------------------------------------------------
//Verwaltung des ARP Timers
void arp_timer_call (void)
{
   unsigned char a;
   unsigned char b;

	for (a = 0;a<MAX_ARP_ENTRY;a++)
	{
		if (arp_entry[a].arp_t_time == 0)
		{
			for (b = 0;b<6;b++)
			{
				arp_entry[a].arp_t_mac[b]= 0;
			}
			arp_entry[a].arp_t_ip = 0;
		}
		else
		{
			arp_entry[a].arp_t_time--;
		}
	}
}


//----------------------------------------------------------------------------
//If a link is present the gracious ARP is transmitted 5 times to announce ourself
//to the network.
void arp_gratious_timer_call (void)
{
   if (enc28j60linkup())
   {
      if (arp_gratuitous_tx_cnt < STACK_ARP_GRACIOUS_MAX)
      {
         arp_gratuitous_tmr_cnt++;
         if (arp_gratuitous_tmr_cnt > STACK_ARP_GRACIOUS_INTERVAL_TIME)
         {
            arp_gratuitous_packet();
            arp_gratuitous_tmr_cnt = 0;
            arp_gratuitous_tx_cnt++;
         }
      }
   }
   else
   {
      arp_gratuitous_tmr_cnt = 0;
      arp_gratuitous_tx_cnt = 0;
   }
}

//----------------------------------------------------------------------------
//Trägt TCP PORT/Anwendung in Anwendungsliste ein
void add_tcp_app (unsigned int port, void(*fp1)(unsigned char))
{
	unsigned char port_index = 0;
	//Freien Eintrag in der Anwendungliste suchen
	while (TCP_PORT_TABLE[port_index].port)
	{ 
		port_index++;
	}
	if (port_index >= MAX_APP_ENTRY)
	{
		return;
	}
	TCP_PORT_TABLE[port_index].port = port;
	TCP_PORT_TABLE[port_index].fp = *fp1;
	return;
}

//----------------------------------------------------------------------------
//Änderung der TCP PORT/Anwendung in Anwendungsliste
void change_port_tcp_app (unsigned int port_old, unsigned int port_new)
{
	unsigned char port_index = 0;
	//Freien Eintrag in der Anwendungliste suchen
	while (TCP_PORT_TABLE[port_index].port && TCP_PORT_TABLE[port_index].port != port_old)
	{ 
		port_index++;
	}
	if (port_index >= MAX_APP_ENTRY)
	{
		return;
	}
	TCP_PORT_TABLE[port_index].port = port_new;
	return;
}

//----------------------------------------------------------------------------
//ETH get data
void eth_get_data(void)
{
   unsigned int                            packet_length;

   if (eth.timer)
   {
      tcp_timer_call();
      arp_timer_call();
      // arp_gratious_timer_call();
      eth.timer = 0;
   }

   packet_length = enc_receive_packet(MTU_SIZE, eth_buffer);
   if (packet_length > 0)
   {
      eth_buffer[packet_length + 1] = 0;
      check_packet();
   }
}

//----------------------------------------------------------------------------
//Check Packet and call Stack for TCP or UDP
void check_packet (void)
{
    struct Ethernet_Header *ethernet;    //Pointer auf Ethernet_Header
    struct IP_Header       *ip;          //Pointer auf IP_Header
    struct TCP_Header      *tcp;         //Pointer auf TCP_Header
    struct ICMP_Header     *icmp;        //Pointer auf ICMP_Header

    ethernet = (struct Ethernet_Header *)&eth_buffer[ETHER_OFFSET];
    ip       = (struct IP_Header       *)&eth_buffer[IP_OFFSET];
    tcp      = (struct TCP_Header      *)&eth_buffer[TCP_OFFSET];
    icmp     = (struct ICMP_Header     *)&eth_buffer[ICMP_OFFSET];
    
    if(ethernet->EnetPacketType == HTONS(0x0806) )     //ARP
    {
        arp_reply(); // check arp packet request/reply
    }
    else
    {
        if( ethernet->EnetPacketType == HTONS(0x0800) )  // if IP
        {              
            if( ip->IP_Destaddr == *((unsigned long*)&myip[0]) )  // if my IP address 
            {
                arp_entry_add();  ///Refresh des ARP Eintrages
                if(ip->IP_Proto == PROT_ICMP)
                {
                    switch ( icmp->ICMP_Type )
                    {
                        case (8): //Ping reqest
                            icmp_send(ip->IP_Srcaddr,0,0,icmp->ICMP_SeqNum,icmp->ICMP_Id); 
                            ping.result |= 0x01;
                            break;
                            
                        case (0): //Ping reply
                            if ((*((unsigned long*)&ping.ip1[0])) == ip->IP_Srcaddr)
                            {
                                ping.result |= 0x01;
                            }
                            break;
                    }
                    return;
                }
                else
                {
                    if( ip->IP_Proto == PROT_TCP ) tcp_socket_process();
                }
            }
        }
    }
    return;
}

//----------------------------------------------------------------------------
//erzeugt einen ARP - Eintrag wenn noch nicht vorhanden 
void arp_entry_add (void)
{
    struct Ethernet_Header *ethernet;
    struct ARP_Header      *arp;
    struct IP_Header       *ip;
    unsigned char a;
    unsigned char b;
      
    ethernet = (struct Ethernet_Header *)&eth_buffer[ETHER_OFFSET];
    arp      = (struct ARP_Header      *)&eth_buffer[ARP_OFFSET];
    ip       = (struct IP_Header       *)&eth_buffer[IP_OFFSET];
        
    //Eintrag schon vorhanden?
    for (a = 0; a<MAX_ARP_ENTRY; a++)
    {
        if( ethernet->EnetPacketType == HTONS(0x0806) ) //If ARP
        {
            if(arp_entry[a].arp_t_ip == arp->ARP_SIPAddr)
            {
                //Eintrag gefunden Time refresh
                arp_entry[a].arp_t_time = ARP_MAX_ENTRY_TIME;
                return;
            }
        } 
        if( ethernet->EnetPacketType == HTONS(0x0800) ) //If IP
        {
            if(arp_entry[a].arp_t_ip == ip->IP_Srcaddr)
            {
                //Eintrag gefunden Time refresh
                arp_entry[a].arp_t_time = ARP_MAX_ENTRY_TIME;
                return;
            }
        }
    }
  
    //Freien Eintrag finden
    for (b = 0; b<MAX_ARP_ENTRY; b++)
    {
        if(arp_entry[b].arp_t_ip == 0)
        {
            if( ethernet->EnetPacketType == HTONS(0x0806) ) //if ARP
            {
                for(a = 0; a < 6; a++)
                {
                    arp_entry[b].arp_t_mac[a] = ethernet->EnetPacketSrc[a]; 
                }
                arp_entry[b].arp_t_ip   = arp->ARP_SIPAddr;
                arp_entry[b].arp_t_time = ARP_MAX_ENTRY_TIME;
                return;
            }
            if( ethernet->EnetPacketType == HTONS(0x0800) ) //if IP
            {
                for(a = 0; a < 6; a++)
                {
                    arp_entry[b].arp_t_mac[a] = ethernet->EnetPacketSrc[a]; 
                }
                arp_entry[b].arp_t_ip   = ip->IP_Srcaddr;
                arp_entry[b].arp_t_time = ARP_MAX_ENTRY_TIME;
                return;
            }
            return;
        }
    }
    //Eintrag konnte nicht mehr aufgenommen werden
    return;
}

//----------------------------------------------------------------------------
//Diese Routine such anhand der IP den ARP eintrag
char arp_entry_search (unsigned long dest_ip)
{
   unsigned char b;
	for ( b = 0;b<MAX_ARP_ENTRY;b++)
	{
		if(arp_entry[b].arp_t_ip == dest_ip)
		{
			return(b);
		}
	}
	return (MAX_ARP_ENTRY);
}

//----------------------------------------------------------------------------
//Diese Routine Erzeugt ein neuen Ethernetheader
void new_eth_header (unsigned char *buffer,unsigned long dest_ip)
{
   unsigned char b;
   unsigned char a;
	struct Ethernet_Header *ethernet;
	ethernet = (struct Ethernet_Header *)&buffer[ETHER_OFFSET];
	
	b = arp_entry_search (dest_ip);
	if (b != MAX_ARP_ENTRY) //Eintrag gefunden wenn ungleich
	{
		for(a = 0; a < 6; a++)
		{
			//MAC Destadresse wird geschrieben mit MAC Sourceadresse
			ethernet->EnetPacketDest[a] = arp_entry[b].arp_t_mac[a];
			//Meine MAC Adresse wird in Sourceadresse geschrieben
			ethernet->EnetPacketSrc[a] = mymac[a];
		}
	}
}

void new_eth_header_tcp (unsigned char *buffer,unsigned char Index)
{
   struct Ethernet_Header *ethernet;
   struct TCP_Header      *tcp;

   tcp = (struct TCP_Header *)&buffer[TCP_OFFSET];
   ethernet = (struct Ethernet_Header *)&buffer[ETHER_OFFSET];

   memcpy(&ethernet->EnetPacketDest[0],&tcp_entry[Index].EnetPacketDest[0],sizeof(ethernet->EnetPacketDest));
   memcpy(&ethernet->EnetPacketSrc[0],&mymac[0],sizeof(ethernet->EnetPacketSrc));
}

//----------------------------------------------------------------------------
//Diese Routine Antwortet auf ein ARP Paket
void arp_reply (void)
{
    unsigned char b;
    unsigned char a;
    struct Ethernet_Header *ethernet;
    struct ARP_Header      *arp;

    ethernet = (struct Ethernet_Header *)&eth_buffer[ETHER_OFFSET];
    arp      = (struct ARP_Header      *)&eth_buffer[ARP_OFFSET];


    if( arp->ARP_HWType  == HTONS(0x0001)  &&             // Hardware Typ:   Ethernet
        arp->ARP_PRType  == HTONS(0x0800)  &&             // Protokoll Typ:  IP
        arp->ARP_HWLen   == 0x06           &&             // Länge der Hardwareadresse: 6
        arp->ARP_PRLen   == 0x04           &&             // Länge der Protokolladresse: 4 
        arp->ARP_TIPAddr == *((unsigned long*)&myip[0])) // Für uns?
    {
        if (arp->ARP_Op == HTONS(0x0001) )                  // Request?
        {
            arp_entry_add(); 
            new_eth_header (eth_buffer, arp->ARP_SIPAddr); // Erzeugt ein neuen Ethernetheader
            ethernet->EnetPacketType = HTONS(0x0806);      // Nutzlast 0x0800=IP Datagramm;0x0806 = ARP
      
            b = arp_entry_search (arp->ARP_SIPAddr);
            if (b < MAX_ARP_ENTRY)                         // Eintrag gefunden wenn ungleich
            {
                for(a = 0; a < 6; a++)
                {
                    arp->ARP_THAddr[a] = arp_entry[b].arp_t_mac[a];
                    arp->ARP_SHAddr[a] = mymac[a];
                }
            }
      
            arp->ARP_Op      = HTONS(0x0002);                   // ARP op = ECHO  
            arp->ARP_TIPAddr = arp->ARP_SIPAddr;                // ARP Target IP Adresse 
            arp->ARP_SIPAddr = *((unsigned long *)&myip[0]);   // Meine IP Adresse = ARP Source
      
            enc_send_packet(ARP_REPLY_LEN, eth_buffer);
            return;
        }

        if ( arp->ARP_Op == HTONS(0x0002) )                    // REPLY von einem anderen Client
        {
            arp_entry_add();
        }
    }
    return;
}

//----------------------------------------------------------------------------
//Diese Routine erzeugt einen ARP Request
char arp_request (unsigned long dest_ip)
{
    unsigned char buffer[ARP_REQUEST_LEN];
    unsigned char index = 0;
    unsigned char index_tmp;
    unsigned char count;
    unsigned long a;
    unsigned long dest_ip_store;

    struct Ethernet_Header *ethernet;
    struct ARP_Header *arp;

    ethernet = (struct Ethernet_Header *)&buffer[ETHER_OFFSET];
    arp      = (struct ARP_Header      *)&buffer[ARP_OFFSET];

    dest_ip_store = dest_ip;

    if ( (dest_ip & (*((unsigned long *)&netmask[0])))==
       ((*((unsigned long *)&myip[0]))&(*((unsigned long *)&netmask[0]))) )
    {
    }
    else
    {
        dest_ip = (*((unsigned long *)&router_ip[0]));
    }

    ethernet->EnetPacketType = HTONS(0x0806);          // Nutzlast 0x0800=IP Datagramm;0x0806 = ARP
  
    new_eth_header (buffer,dest_ip);
  
    arp->ARP_SIPAddr = *((unsigned long *)&myip[0]);   // MyIP = ARP Source IP
    arp->ARP_TIPAddr = dest_ip;                         // Dest IP 
  
    for(count = 0; count < 6; count++)
    {
        arp->ARP_SHAddr[count] = mymac[count];
        arp->ARP_THAddr[count] = 0;
    }
  
    arp->ARP_HWType = HTONS(0x0001);
    arp->ARP_PRType = HTONS(0x0800);
    arp->ARP_HWLen  = 0x06;
    arp->ARP_PRLen  = 0x04;
    arp->ARP_Op     = HTONS(0x0001);

    enc_send_packet(ARP_REPLY_LEN, buffer);        //send....
  
    for(count = 0; count<20; count++)
    {
        index_tmp = arp_entry_search(dest_ip_store);
        index = arp_entry_search(dest_ip);
        if (index < MAX_ARP_ENTRY || index_tmp < MAX_ARP_ENTRY)
        {
            if (index_tmp < MAX_ARP_ENTRY) return(1);//OK
            arp_entry[index].arp_t_ip = dest_ip_store;
            return(1);//OK
        }
        for(a=0;a<10000;a++)
        {
            asm("nop");
        }
        eth_get_data();
    }
    return(0);//keine Antwort
}

//----------------------------------------------------------------------------
// Diese Routine erzeugt einen ARP gratuitous message to announce the
// device on the network without expecting a reply.
// http://en.wikipedia.org/wiki/Address_Resolution_Protocol or
// http://wiki.wireshark.org/Gratuitous_ARP for a description.

void arp_gratuitous_packet (void)
{
    unsigned char BroadcastMac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    unsigned char buffer[ARP_REQUEST_LEN];

    struct Ethernet_Header *ethernet;
    struct ARP_Header *arp;

    memset(buffer,0,sizeof(buffer));

    ethernet = (struct Ethernet_Header *)&buffer[ETHER_OFFSET];
    arp      = (struct ARP_Header      *)&buffer[ARP_OFFSET];

    // Nutzlast Datagramm;0x0806 = ARP
    ethernet->EnetPacketType = HTONS(0x0806);

    memcpy(&ethernet->EnetPacketSrc[0],&mymac[0],sizeof(mymac));
    memcpy(&ethernet->EnetPacketDest[0],&BroadcastMac[0],sizeof(BroadcastMac));
    arp->ARP_SIPAddr = *((unsigned long *)&myip[0]);
    arp->ARP_TIPAddr = *((unsigned long *)&myip[0]);
    memcpy(&arp->ARP_SHAddr[0],&mymac[0],sizeof(mymac));
    memcpy(&arp->ARP_THAddr[0],&BroadcastMac[0],sizeof(BroadcastMac));

    arp->ARP_HWType = HTONS(0x0001);
    arp->ARP_PRType = HTONS(0x0800);
    arp->ARP_HWLen  = 0x06;
    arp->ARP_PRLen  = 0x04;
    arp->ARP_Op     = HTONS(0x0001);

    enc_send_packet(ARP_REPLY_LEN, buffer);
}

//----------------------------------------------------------------------------
//Diese Routine erzeugt ein neues ICMP Packet
void icmp_send (unsigned long dest_ip, unsigned char icmp_type, 
                unsigned char icmp_code, unsigned int icmp_sn, 
                unsigned int icmp_id)
{
    unsigned int result16;  //Checksum
    struct IP_Header   *ip;
    struct ICMP_Header *icmp;

    ip   = (struct IP_Header   *)&eth_buffer[IP_OFFSET];
    icmp = (struct ICMP_Header *)&eth_buffer[ICMP_OFFSET];

    //Das ist ein Echo Reply Packet
    icmp->ICMP_Type   = icmp_type;
    icmp->ICMP_Code   = icmp_code;
    icmp->ICMP_Id     = icmp_id;
    icmp->ICMP_SeqNum = icmp_sn;
    icmp->ICMP_Cksum  = 0;
    ip->IP_Pktlen     = HTONS(0x0054);   // 0x54 = 84 
    ip->IP_Proto      = PROT_ICMP;
    make_ip_header (eth_buffer,dest_ip);

    //Berechnung der ICMP Header länge
    result16 = htons(ip->IP_Pktlen);
    result16 = result16 - ((ip->IP_Vers_Len & 0x0F) << 2);

    //pointer wird auf das erste Paket im ICMP Header gesetzt
    //jetzt wird die Checksumme berechnet
    result16 = checksum (&icmp->ICMP_Type, result16, 0);
  
    //schreibt Checksumme ins Packet
    icmp->ICMP_Cksum = htons(result16);
  
    // Sendet das erzeugte ICMP Packet
    enc_send_packet(ICMP_REPLY_LEN, eth_buffer);
}

//----------------------------------------------------------------------------
//Diese Routine erzeugt eine Cecksumme
unsigned int checksum (unsigned char *pointer,unsigned int result16,unsigned long result32)
{
	unsigned int result16_1 = 0x0000;
	unsigned char DataH;
	unsigned char DataL;
	
	//Jetzt werden alle Packete in einer While Schleife addiert
	while(result16 > 1)
	{
		//schreibt Inhalt Pointer nach DATAH danach inc Pointer
		DataH=*pointer++;

		//schreibt Inhalt Pointer nach DATAL danach inc Pointer
		DataL=*pointer++;

		//erzeugt Int aus Data L und Data H
		result16_1 = ((DataH << 8)+DataL);
		//Addiert packet mit vorherigen
		result32 = result32 + result16_1;
		//decrimiert Länge von TCP Headerschleife um 2
		result16 -=2;
	}

	//Ist der Wert result16 ungerade ist DataL = 0
	if(result16 > 0)
	{
		//schreibt Inhalt Pointer nach DATAH danach inc Pointer
		DataH=*pointer;
		//erzeugt Int aus Data L ist 0 (ist nicht in der Berechnung) und Data H
		result16_1 = (DataH << 8);
		//Addiert packet mit vorherigen
		result32 = result32 + result16_1;
	}
	
	//Komplementbildung (addiert Long INT_H Byte mit Long INT L Byte)
	result32 = ((result32 & 0x0000FFFF)+ ((result32 & 0xFFFF0000) >> 16));
	result32 = ((result32 & 0x0000FFFF)+ ((result32 & 0xFFFF0000) >> 16));	
	result16 =~(result32 & 0x0000FFFF);
	
	return (result16);
}

//----------------------------------------------------------------------------
//Diese Routine erzeugt ein IP Packet
void make_ip_header (unsigned char *buffer,unsigned long dest_ip)
{
    unsigned int result16;  //Checksum
    struct Ethernet_Header *ethernet;
    struct IP_Header       *ip;

    ethernet = (struct Ethernet_Header *)&buffer[ETHER_OFFSET];
    ip       = (struct IP_Header       *)&buffer[IP_OFFSET];

    new_eth_header(buffer, dest_ip);         //Erzeugt einen neuen Ethernetheader
    ethernet->EnetPacketType = HTONS(0x0800); //Nutzlast 0x0800=IP

    IP_id_counter++;

    ip->IP_Frag_Offset = 0x0040;  //don't fragment
    ip->IP_ttl         = 128;      //max. hops
    ip->IP_Id          = htons(IP_id_counter);
    ip->IP_Vers_Len    = 0x45;  //4 BIT Die Versionsnummer von IP,
    ip->IP_Tos         = 0;
    ip->IP_Destaddr     = dest_ip;
    ip->IP_Srcaddr     = *((unsigned long *)&myip[0]);
    ip->IP_Hdr_Cksum   = 0;

    //Berechnung der IP Header länge
    result16 = (ip->IP_Vers_Len & 0x0F) << 2;

    //jetzt wird die Checksumme berechnet
    result16 = checksum (&ip->IP_Vers_Len, result16, 0);

    //schreibt Checksumme ins Packet
    ip->IP_Hdr_Cksum = htons(result16);
    return;
}

void make_ip_header_tcp (unsigned char *buffer,unsigned long dest_ip, unsigned char Index)
{
    unsigned int result16;  //Checksum
    struct Ethernet_Header *ethernet;
    struct IP_Header       *ip;

    ethernet = (struct Ethernet_Header *)&buffer[ETHER_OFFSET];
    ip       = (struct IP_Header       *)&buffer[IP_OFFSET];

    new_eth_header_tcp(buffer, Index);         //Erzeugt einen neuen Ethernetheader
    ethernet->EnetPacketType = HTONS(0x0800); //Nutzlast 0x0800=IP

    IP_id_counter++;

    ip->IP_Frag_Offset = 0x0040;  //don't fragment
    ip->IP_ttl         = 128;      //max. hops
    ip->IP_Id          = htons(IP_id_counter);
    ip->IP_Vers_Len    = 0x45;  //4 BIT Die Versionsnummer von IP, 
    ip->IP_Tos         = 0;
    ip->IP_Destaddr     = dest_ip;
    ip->IP_Srcaddr     = *((unsigned long *)&myip[0]);
    ip->IP_Hdr_Cksum   = 0;
  
    //Berechnung der IP Header länge  
    result16 = (ip->IP_Vers_Len & 0x0F) << 2;

    //jetzt wird die Checksumme berechnet
    result16 = checksum (&ip->IP_Vers_Len, result16, 0);

    //schreibt Checksumme ins Packet
    ip->IP_Hdr_Cksum = htons(result16);
    return;
}

//----------------------------------------------------------------------------
//Diese Routine verwaltet TCP-Einträge
void tcp_entry_add (unsigned char *buffer)
{
    unsigned long result32;
    unsigned char index;

    struct TCP_Header *tcp;
    struct IP_Header  *ip;
    struct Ethernet_Header *ethernet;

    tcp = (struct TCP_Header *)&buffer[TCP_OFFSET];
    ip  = (struct IP_Header  *)&buffer[IP_OFFSET];
    ethernet = (struct Ethernet_Header *)&buffer[ETHER_OFFSET];
  
    //Eintrag schon vorhanden?
    for (index = 0;index<(MAX_TCP_ENTRY);index++)
    {
        if( (tcp_entry[index].ip       == ip->IP_Srcaddr  ) &&
            (tcp_entry[index].src_port == tcp->TCP_SrcPort)    )
        {
            //Eintrag gefunden Time refresh
            tcp_entry[index].ack_counter = tcp->TCP_Acknum;
            tcp_entry[index].seq_counter = tcp->TCP_Seqnum;
            tcp_entry[index].status      = tcp->TCP_HdrFlags;
            if ( tcp_entry[index].time != TCP_TIME_OFF )
            {
                tcp_entry[index].time = TCP_MAX_ENTRY_TIME;
            }
            result32 = htons(ip->IP_Pktlen) - IP_VERS_LEN - ((tcp->TCP_Hdrlen& 0xF0) >>2);
            result32 = result32 + htons32(tcp_entry[index].seq_counter);
            tcp_entry[index].seq_counter = htons32(result32);
            return;
        }
    }
  
    //Freien Eintrag finden
    for (index = 0;index<(MAX_TCP_ENTRY);index++)
    {
        if(tcp_entry[index].ip == 0)
        {
            tcp_entry[index].ip          = ip->IP_Srcaddr;
            tcp_entry[index].src_port    = tcp->TCP_SrcPort;
            tcp_entry[index].dest_port   = tcp->TCP_DestPort;
            tcp_entry[index].ack_counter = tcp->TCP_Acknum;
            tcp_entry[index].seq_counter = tcp->TCP_Seqnum;
            tcp_entry[index].status      = tcp->TCP_HdrFlags;
            tcp_entry[index].app_status  = 0;
            tcp_entry[index].time        = TCP_MAX_ENTRY_TIME;
            tcp_entry[index].error_count = 0;
            tcp_entry[index].first_ack   = 0;
            memcpy(tcp_entry[index].EnetPacketDest, &ethernet->EnetPacketSrc[0], sizeof(tcp_entry[index].EnetPacketDest));
        }
    }
}

//----------------------------------------------------------------------------
//Diese Routine sucht den etntry eintrag
char tcp_entry_search (unsigned long dest_ip,unsigned int SrcPort)
{
   unsigned char index;

	for (index = 0;index<MAX_TCP_ENTRY;index++)
	{
		if(	tcp_entry[index].ip == dest_ip &&
			tcp_entry[index].src_port == SrcPort)
		{
			return(index);
		}
	}
	return (MAX_TCP_ENTRY);
}

//----------------------------------------------------------------------------
//Diese Routine sucht den etntry eintrag
char tcp_entry_search_for_connection (unsigned int SrcPort, unsigned char *TcpIpIndex)
{
   unsigned char index;
   uint8_t result = 0;

   for (index = 0;index<MAX_TCP_ENTRY;index++)
   {
      if(tcp_entry[index].src_port == SrcPort)
      {
         *TcpIpIndex = index;
         result = 1;
      }
   }
   result = 0;

   return(result);
}

//----------------------------------------------------------------------------
//Diese Routine verwaltet die TCP Ports
void tcp_socket_process(void)
{
	unsigned char index = 0;
	unsigned char port_index = 0;
	unsigned long result32 = 0;

	struct TCP_Header *tcp;
	tcp = (struct TCP_Header *)&eth_buffer[TCP_OFFSET];

	struct IP_Header *ip;
	ip = (struct IP_Header *)&eth_buffer[IP_OFFSET];

	//TCP DestPort mit Portanwendungsliste durchführen
	while (TCP_PORT_TABLE[port_index].port && TCP_PORT_TABLE[port_index].port!=(htons(tcp->TCP_DestPort)))
	{ 
		port_index++;
	}
	
	// Wenn index zu gross, dann beenden keine vorhandene Anwendung für Port
	//Geht von einem Client was aus? Will eine Clientanwendung einen Port öffnen?
	if (!TCP_PORT_TABLE[port_index].port)
	{ 
		//Keine vorhandene Anwendung eingetragen! (ENDE)
		return;
	}	
	
	//Server öffnet Port
	if((tcp->TCP_HdrFlags & SYN_FLAG) && (tcp->TCP_HdrFlags & ACK_FLAG))
	{	
		//Nimmt Eintrag auf da es eine Client - Anwendung für den Port gibt
		tcp_entry_add (eth_buffer);
		//War der Eintrag erfolgreich?
		index = tcp_entry_search (ip->IP_Srcaddr,tcp->TCP_SrcPort);
		if (index >= MAX_TCP_ENTRY) //Eintrag gefunden wenn ungleich
		{
			return;
		}
	
		tcp_entry[index].time = MAX_TCP_PORT_OPEN_TIME;
		result32 = htons32(tcp_entry[index].seq_counter) + 1;
		tcp_entry[index].seq_counter = htons32(result32);
		tcp_entry[index].status =  ACK_FLAG;
		create_new_tcp_packet(0,index);
		//Server Port wurde geöffnet App. kann nun daten senden!
		tcp_entry[index].app_status = 1;
		return;
	}
	
	//Verbindungsaufbau nicht für Anwendung bestimmt
	if(tcp->TCP_HdrFlags == SYN_FLAG)
	{
		//Nimmt Eintrag auf da es eine Server - Anwendung für den Port gibt
		tcp_entry_add (eth_buffer);
		//War der Eintrag erfolgreich?
		index = tcp_entry_search (ip->IP_Srcaddr,tcp->TCP_SrcPort);
		if (index >= MAX_TCP_ENTRY) //Eintrag gefunden wenn ungleich
		{
			return;
		}
	
		tcp_entry[index].status =  ACK_FLAG | SYN_FLAG;
		create_new_tcp_packet(0,index);
		return;
	}

	//Packeteintrag im TCP Stack finden!
	index = tcp_entry_search (ip->IP_Srcaddr,tcp->TCP_SrcPort);
	
	if (index >= MAX_TCP_ENTRY) //Eintrag nicht gefunden
	{

		if(tcp->TCP_HdrFlags & FIN_FLAG || tcp->TCP_HdrFlags & RST_FLAG)
		{
			tcp_entry_add (eth_buffer);//Temporärer Indexplatz	
			result32 = htons32(tcp_entry[index].seq_counter) + 1;
			tcp_entry[index].seq_counter = htons32(result32);
			
			if (tcp_entry[index].status & FIN_FLAG)
			{
				tcp_entry[index].status = ACK_FLAG;
				create_new_tcp_packet(0,index);
			}
			tcp_index_del(index);
			return;
		}
		return;
	}


	//Refresh des Eintrages
	tcp_entry_add (eth_buffer);
	
	//Host will verbindung beenden!
	if(tcp_entry[index].status & FIN_FLAG || tcp_entry[index].status & RST_FLAG)
	{	
		result32 = htons32(tcp_entry[index].seq_counter) + 1;
		tcp_entry[index].seq_counter = htons32(result32);
		
		// Ende der Anwendung mitteilen !
		TCP_PORT_TABLE[port_index].fp(index);

      if (tcp_entry[index].status & FIN_FLAG)
      {
			tcp_entry[index].status = ACK_FLAG | FIN_FLAG;
			create_new_tcp_packet(0,index);
		}
		tcp_index_del(index);
		return;
	}
	
	//Daten für Anwendung PSH-Flag gesetzt?
	if((tcp_entry[index].status & PSH_FLAG) && 
		(tcp_entry[index].status & ACK_FLAG))
	{
		//zugehörige Anwendung ausführen
		tcp_entry[index].app_status++;	
		tcp_entry[index].status =  ACK_FLAG | PSH_FLAG;
		TCP_PORT_TABLE[port_index].fp(index); 
		return;
	}
	
	//Empfangene Packet wurde bestätigt keine Daten für Anwendung
	//z.B. nach Verbindungsaufbau (SYN-PACKET)
	if((tcp_entry[index].status & ACK_FLAG) && (tcp_entry[index].first_ack == 0))
	{
		//keine weitere Aktion
		tcp_entry[index].first_ack = 1;
	    TCP_PORT_TABLE[port_index].fp(index);
		return;
	}
	
	//Empfangsbestätigung für ein von der Anwendung gesendetes Packet (ENDE)
	if((tcp_entry[index].status & ACK_FLAG) && (tcp_entry[index].first_ack == 1))
	{
		//ACK für Verbindungs abbau
		if(tcp_entry[index].app_status == 0xFFFF)
		{
			return;
		}

		//zugehörige Anwendung ausführen
		tcp_entry[index].status =  ACK_FLAG;
		tcp_entry[index].app_status++;
		TCP_PORT_TABLE[port_index].fp(index);
		return;
	}
	return;
}

//----------------------------------------------------------------------------
//Enable the retry mechanism for transmit of data
void                                    tcp_packet_retry_tx_reset(unsigned char index)
{
   tcp_entry[index].error_count = 0;
}

//----------------------------------------------------------------------------
//Enable the retry mechanism for transmit of data
void                                    tcp_packet_retry_tx_set(unsigned char index)
{
   tcp_entry[index].txretry = 1;
   tcp_entry[index].time = TCP_MAX_ENTRY_TIME;
}

//----------------------------------------------------------------------------
//Disable the retry mechanism for transmit of data
void                                    tcp_packet_retry_tx_clear(unsigned char index)
{
   tcp_entry[index].txretry = 0;
}

//----------------------------------------------------------------------------
//Diese Routine Erzeugt ein neues TCP Packet
void create_new_tcp_packet(unsigned int data_length,unsigned char index)
{
    unsigned int  result16;
    unsigned long result32;
    unsigned int  bufferlen;

    struct TCP_Header *tcp;
    struct IP_Header  *ip;

    tcp = (struct TCP_Header *)&eth_buffer[TCP_OFFSET];
    ip  = (struct IP_Header  *)&eth_buffer[IP_OFFSET];

    tcp->TCP_SrcPort   = tcp_entry[index].dest_port;
    tcp->TCP_DestPort  = tcp_entry[index].src_port;
    tcp->TCP_UrgentPtr = 0;
    tcp->TCP_Window    = htons(MAX_WINDOWS_SIZE);
    tcp->TCP_Hdrlen    = 0x50;

    result32 = htons32(tcp_entry[index].seq_counter); 

    tcp->TCP_HdrFlags = tcp_entry[index].status;

    //Verbindung wird aufgebaut
    if(tcp_entry[index].status & SYN_FLAG)
    {
        result32++;
        // MSS-Option (siehe RFC 879) wil.
        eth_buffer[TCP_DATA_START]   = 2;
        eth_buffer[TCP_DATA_START+1] = 4;
        eth_buffer[TCP_DATA_START+2] = (MAX_WINDOWS_SIZE >> 8) & 0xff;
        eth_buffer[TCP_DATA_START+3] = MAX_WINDOWS_SIZE & 0xff;
        data_length                  = 0x04;
        tcp->TCP_Hdrlen              = 0x60;
    }
  
    tcp->TCP_Acknum = htons32(result32);
    tcp->TCP_Seqnum = tcp_entry[index].ack_counter;

    bufferlen = IP_VERS_LEN + TCP_HDR_LEN + data_length;    //IP Headerlänge + TCP Headerlänge
    ip->IP_Pktlen = htons(bufferlen);                      //Hier wird erstmal der IP Header neu erstellt
    bufferlen += ETH_HDR_LEN;
    ip->IP_Proto = PROT_TCP;
    make_ip_header_tcp (eth_buffer,tcp_entry[index].ip, index);

    tcp->TCP_Chksum = 0;

    //Berechnet Headerlänge und Addiert Pseudoheaderlänge 2XIP = 8
    result16 = htons(ip->IP_Pktlen) + 8;
    result16 = result16 - ((ip->IP_Vers_Len & 0x0F) << 2);
    result32 = result16 - 2;

    //Checksum
    result16 = checksum ((&ip->IP_Vers_Len+12), result16, result32);
    tcp->TCP_Chksum = htons(result16);

    //Send the TCP packet
    enc_send_packet(bufferlen, eth_buffer);
    //Für Retransmission
    tcp_entry[index].status = 0;
    return;
}

//----------------------------------------------------------------------------
//Diese Routine schließt einen offenen TCP-Port
void tcp_Port_close (unsigned char index)
{
	tcp_entry[index].app_status = 0xFFFF;
	tcp_entry[index].status =  ACK_FLAG | FIN_FLAG;
	create_new_tcp_packet(0,index);
	return;
}

//----------------------------------------------------------------------------
//Diese Routine findet die Anwendung anhand des TCP Ports
void find_and_start (unsigned char index)
{
    unsigned char port_index = 0;

    //Port mit Anwendung in der Liste suchen
    while ( (TCP_PORT_TABLE[port_index].port!=(htons(tcp_entry[index].dest_port))) &&
          (port_index < MAX_APP_ENTRY)                                                 )
    { 
        port_index++;
    }
    if (port_index >= MAX_APP_ENTRY) return;
  
    //zugehörige Anwendung ausführen (Senden wiederholen)
    TCP_PORT_TABLE[port_index].fp(index);
    return;
}

//----------------------------------------------------------------------------
//Diese Routine öffnet einen TCP-Port
void tcp_port_open (unsigned long dest_ip,unsigned int port_dst,unsigned int port_src)
{
	unsigned char index;
    
	ETH_INT_DISABLE;
	
	//Freien Eintrag finden
	for (index = 0;index<MAX_TCP_ENTRY;index++)
	{
		if(tcp_entry[index].ip == 0)
		{
			tcp_index_del(index);
			tcp_entry[index].ip = dest_ip;
			tcp_entry[index].src_port = port_dst;
			tcp_entry[index].dest_port = port_src;
			tcp_entry[index].ack_counter = 1234;
			tcp_entry[index].seq_counter = 2345;
			tcp_entry[index].time = MAX_TCP_PORT_OPEN_TIME;
			break;
		}
	}
	if (index >= MAX_TCP_ENTRY)
	{
		//Eintrag konnte nicht mehr aufgenommen werden
	}
	
	tcp_entry[index].status =  SYN_FLAG;
	create_new_tcp_packet(0,index);
	ETH_INT_ENABLE;
	return;
}

//----------------------------------------------------------------------------
//Diese Routine löscht einen Eintrag
void tcp_index_del (unsigned char index)
{
	if (index<MAX_TCP_ENTRY + 1)
	{
		tcp_entry[index].ip = 0;
		tcp_entry[index].src_port = 0;
		tcp_entry[index].dest_port = 0;
		tcp_entry[index].ack_counter = 0;
		tcp_entry[index].seq_counter = 0;
		tcp_entry[index].status = 0;
		tcp_entry[index].app_status = 0;
		tcp_entry[index].time = 0;
		tcp_entry[index].first_ack = 0;
	}
	return;
}

//----------------------------------------------------------------------------
//End of file: stack.c







