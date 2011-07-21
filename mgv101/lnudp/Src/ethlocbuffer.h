
/**
 *******************************************************************************************
 * @file		EthLocBuffer.h
 * @ingroup	    EthLocBuffer
 * @defgroup	EthLocBuffer EthLocBuffer : Loconet / UDP processing. 
 * @brief		EthLocBuffer Process the data between the network interface and Loconet interface. 
 *******************************************************************************************
 */
#ifndef ETH_LOC_BUF
#define ETH_LOC_BUF

/*
 *******************************************************************************************
 * Standard include files
 *******************************************************************************************
 */
#include <inttypes.h>

/*
 *******************************************************************************************
 * Macro definitions
 *******************************************************************************************
 */

/*
 *******************************************************************************************
 * Types
 *******************************************************************************************
  */
  
typedef enum 
{ 
	up, down 
} Linkstate;
  
typedef struct
{
	uint8_t                                 command;
	uint8_t									myaddress; /* 0x0 = everyone */
} MGVCommand;  
  
typedef struct net_t
{
	uint8_t									command;
	uint8_t									myaddress;
	uint8_t									ipaddress[4];
	uint8_t     							netmask[4];
	uint8_t									macaddres[6];
	uint8_t									version;
	uint8_t									hardwarerev;
} MGVNetwork;  
  
typedef struct id_t
{
	uint8_t									command;
	uint8_t									myaddress;
	uint8_t									newaddress;
} MGVId;    
  
typedef union
{
   MGVCommand                            	query;
   MGVNetwork								net;
   MGVId									id;
   uint8_t                                 	data[18];
} MGVMsg;
  

#define MGV_MASK 		0x70
#define MGV_INFO 		0x71
#define MGV_SETNET		0x72
#define MGV_SETID		0x73

  
/*
 *******************************************************************************************
 * Variables
 *******************************************************************************************
 */

/*
 *******************************************************************************************
 * Prototypes
 *******************************************************************************************
 */

extern void                             eth_locbuffer_init(void);
extern void                             eth_locbuffer_main(void);

#endif   // ETH_LOC_BUF
