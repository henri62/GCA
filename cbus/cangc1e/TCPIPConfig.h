




#ifndef __TCPIPCONFIG_H
#define __TCPIPCONFIG_H

#include "HardwareProfile.h"

//#define STACK_USE_IP_GLEANING
#define STACK_USE_ICMP_SERVER			// Ping query and response capability
#define STACK_USE_ICMP_CLIENT			// Ping transmission capability


// =======================================================================
//   Data Storage Options
// =======================================================================

/* MPFS Configuration
 *   MPFS is automatically included when required for other
 *   applications.  If your custom application requires it
 *   otherwise, uncomment the appropriate selection.
 */
//#define STACK_USE_MPFS2

/* MPFS Storage Location
 *   If html pages are stored in internal program memory,
 *   comment both MPFS_USE_EEPROM and MPFS_USE_SPI_FLASH, then
 *   include an MPFS image (.c or .s file) in the project.
 *   If html pages are stored in external memory, uncomment the
 *   appropriate definition.
 *
 *   Supported serial flash parts include the SST25VFxxxB series.
 */
// #define MPFS_USE_EEPROM
//#define MPFS_USE_SPI_FLASH

/* EEPROM Addressing Selection
 *   If using the 1Mbit EEPROM, uncomment this line
 */
//#define USE_EEPROM_25LC1024

/* EEPROM Reserved Area
 *   Number of EEPROM bytes to be reserved before MPFS storage starts.
 *   These bytes host application configurations such as IP Address,
 *   MAC Address, and any other required variables.
 *
 *   For MPFS Classic, this setting must match the Reserved setting
 *	 on the Advanced Settings page of the MPFS2 Utility.
 */
// #define MPFS_RESERVE_BLOCK				(32)

/* MPFS File Handles
 *   Maximum number of simultaneously open MPFS2 files.
 *   For MPFS Classic, this has no effect.
 */
// #define MAX_MPFS_HANDLES				(7ul)


// =======================================================================
//   Network Addressing Options
// =======================================================================

/* Default Network Configuration
 *   These settings are only used if data is not found in EEPROM.
 *   To clear EEPROM, hold BUTTON0, reset the board, and continue
 *   holding until the LEDs flash.  Release, and reset again.
 */
#define MY_DEFAULT_HOST_NAME			"CANGC1E"

#define MY_DEFAULT_MAC_BYTE1            (0x00)	// Use the default of 00-04-A3-00-00-00
#define MY_DEFAULT_MAC_BYTE2            (0x04)	// if using an ENCX24J600, MRF24WB0M, or
#define MY_DEFAULT_MAC_BYTE3            (0xA3)	// PIC32MX6XX/7XX internal Ethernet 
#define MY_DEFAULT_MAC_BYTE4            (0x00)	// controller and wish to use the 
#define MY_DEFAULT_MAC_BYTE5            (0x00)	// internal factory programmed MAC
#define MY_DEFAULT_MAC_BYTE6            (0x00)	// address instead.

#define MY_DEFAULT_IP_ADDR_BYTE1        (192)
#define MY_DEFAULT_IP_ADDR_BYTE2        (168)
#define MY_DEFAULT_IP_ADDR_BYTE3        (0)
#define MY_DEFAULT_IP_ADDR_BYTE4        (200)

#define MY_DEFAULT_MASK_BYTE1           (255)
#define MY_DEFAULT_MASK_BYTE2           (255)
#define MY_DEFAULT_MASK_BYTE3           (255)
#define MY_DEFAULT_MASK_BYTE4           (0)

#define MY_DEFAULT_GATE_BYTE1           (192)
#define MY_DEFAULT_GATE_BYTE2           (168)
#define MY_DEFAULT_GATE_BYTE3           (0)
#define MY_DEFAULT_GATE_BYTE4           (200)

/* Transport Layer Configuration
 *   The following low level modules are automatically enabled
 *   based on module selections above.  If your custom module
 *   requires them otherwise, enable them here.
 */
#define STACK_USE_TCP
// #define STACK_USE_UDP

/* Client Mode Configuration
 *   Uncomment following line if this stack will be used in CLIENT
 *   mode.  In CLIENT mode, some functions specific to client operation
 *   are enabled.
 */
//#define STACK_CLIENT_MODE

/* TCP Socket Memory Allocation
 *   TCP needs memory to buffer incoming and outgoing data.  The
 *   amount and medium of storage can be allocated on a per-socket
 *   basis using the example below as a guide.
 */
	// Allocate how much total RAM (in bytes) you want to allocate
	// for use by your TCP TCBs, RX FIFOs, and TX FIFOs.
	#define TCP_ETH_RAM_SIZE					(4900ul)
	#define TCP_PIC_RAM_SIZE					(0ul)
	#define TCP_SPI_RAM_SIZE					(0ul)
	#define TCP_SPI_RAM_BASE_ADDRESS			(0x00)

	// Define names of socket types
	#define TCP_SOCKET_TYPES
		#define TCP_PURPOSE_GENERIC_TCP_CLIENT 0
		#define TCP_PURPOSE_GENERIC_TCP_SERVER 1
		#define TCP_PURPOSE_TELNET 2
		#define TCP_PURPOSE_FTP_COMMAND 3
		#define TCP_PURPOSE_FTP_DATA 4
		#define TCP_PURPOSE_TCP_PERFORMANCE_TX 5
		#define TCP_PURPOSE_TCP_PERFORMANCE_RX 6
		#define TCP_PURPOSE_UART_2_TCP_BRIDGE 7
		#define TCP_PURPOSE_HTTP_SERVER 8
		#define TCP_PURPOSE_DEFAULT 9
		#define TCP_PURPOSE_BERKELEY_SERVER 10
		#define TCP_PURPOSE_BERKELEY_CLIENT 11
	#define END_OF_TCP_SOCKET_TYPES

	#if defined(__TCP_C)
		// Define what types of sockets are needed, how many of
		// each to include, where their TCB, TX FIFO, and RX FIFO
		// should be stored, and how big the RX and TX FIFOs should
		// be.  Making this initializer bigger or smaller defines
		// how many total TCP sockets are available.
		//
		// Each socket requires up to 56 bytes of PIC RAM and
		// 48+(TX FIFO size)+(RX FIFO size) bytes of TCP_*_RAM each.
		//
		// Note: The RX FIFO must be at least 1 byte in order to
		// receive SYN and FIN messages required by TCP.  The TX
		// FIFO can be zero if desired.
		#define TCP_CONFIGURATION
		const struct
		{
			BYTE vSocketPurpose;
			BYTE vMemoryMedium;
			WORD wTXBufferSize;
			WORD wRXBufferSize;
		} TCPSocketInitializer[] =
		{
			//{TCP_PURPOSE_GENERIC_TCP_CLIENT, TCP_ETH_RAM, 125, 100},
			{TCP_PURPOSE_GENERIC_TCP_SERVER, TCP_ETH_RAM, 768, 128},
			{TCP_PURPOSE_GENERIC_TCP_SERVER, TCP_ETH_RAM, 768, 128},
			{TCP_PURPOSE_GENERIC_TCP_SERVER, TCP_ETH_RAM, 768, 128},
			{TCP_PURPOSE_GENERIC_TCP_SERVER, TCP_ETH_RAM, 768, 128},
			//{TCP_PURPOSE_TELNET, TCP_ETH_RAM, 200, 150},
			//{TCP_PURPOSE_TELNET, TCP_ETH_RAM, 200, 150},
			//{TCP_PURPOSE_TELNET, TCP_ETH_RAM, 200, 150},
			//{TCP_PURPOSE_FTP_COMMAND, TCP_ETH_RAM, 100, 40},
			//{TCP_PURPOSE_FTP_DATA, TCP_ETH_RAM, 0, 128},
			//{TCP_PURPOSE_TCP_PERFORMANCE_TX, TCP_ETH_RAM, 200, 1},
			//{TCP_PURPOSE_TCP_PERFORMANCE_RX, TCP_ETH_RAM, 40, 1500},
			//{TCP_PURPOSE_UART_2_TCP_BRIDGE, TCP_ETH_RAM, 256, 256},
			//{TCP_PURPOSE_HTTP_SERVER, TCP_ETH_RAM, 200, 200},
			//{TCP_PURPOSE_HTTP_SERVER, TCP_ETH_RAM, 200, 200},
			//{TCP_PURPOSE_DEFAULT, TCP_ETH_RAM, 200, 200},
			//{TCP_PURPOSE_BERKELEY_SERVER, TCP_ETH_RAM, 25, 20},
			//{TCP_PURPOSE_BERKELEY_SERVER, TCP_ETH_RAM, 25, 20},
			//{TCP_PURPOSE_BERKELEY_SERVER, TCP_ETH_RAM, 25, 20},
			//{TCP_PURPOSE_BERKELEY_CLIENT, TCP_ETH_RAM, 125, 100},
		};
		#define END_OF_TCP_CONFIGURATION
	#endif

/* UDP Socket Configuration
 *   Define the maximum number of available UDP Sockets, and whether
 *   or not to include a checksum on packets being transmitted.
 */
#define MAX_UDP_SOCKETS     (4u)
// #define UDP_USE_TX_CHECKSUM		// This slows UDP TX performance by nearly 50%, except when using the ENCX24J600 or PIC32MX6XX/7XX, which have a super fast DMA and incurs virtually no speed pentalty.

#define MAX_HTTP_CONNECTIONS (1)
#endif

