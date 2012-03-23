

#include "StackTsk.h"
#include "TCP.h"
#include "Helpers.h"

#include "cbuseth.h"
#include "cbus.h"
#include "commands.h"
#include "utils.h"
#include "io.h"
#include "cangc1e.h"

#define WAIT_FOR_ALL_READY

static ram CANMsg ETHMsgs[CANMSG_QSIZE];


/*
 * CBUSETH Connection Info - one for each connection.
 */
typedef struct _CBUSETH_INFO
{
    TCP_SOCKET socket;
} CBUSETH_INFO;
typedef BYTE CBUSETH_HANDLE;




static CBUSETH_INFO HCB[MAX_CBUSETH_CONNECTIONS];


static void CBusEthProcess(CBUSETH_HANDLE h);



    /* Frame ASCII format
     * :ShhhhNd0d1d2d3d4d5d6d7d; :XhhhhhhhhNd0d1d2d3d4d5d6d7d; :ShhhhR; :SB020N;
     * :S    -> S=Standard X=extended start CAN Frame
     * hhhh  -> SIDH<bit7,6,5,4=Prio bit3,2,1,0=high 4 part of ID> SIDL<bit7,6,5=low 3 part of ID>
     * Nd    -> N=normal R=RTR
     * 0d    -> OPC 2 byte HEXA
     * 1d-7d -> data 2 byte HEXA
     * ;     -> end of frame
     */

static char hexa[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
static byte msg2ASCII(CANMsg* msg, char* s) {
  byte len = getDataLen(msg->opc);
  byte i;
  byte idx = 9;
  s[0] = ':';
  s[1] = 'S';
  s[2] = '0';
  s[3] = '0';
  s[4] = '0';
  s[5] = '0';
  s[6] = 'N';
  s[7] = hexa[msg->opc >> 4];
  s[8] = hexa[msg->opc & 0x0F];
  for( i = 0; i < len; i++) {
    s[idx + i*2]     = hexa[msg->d[i] >> 4];
    s[idx + i*2 + 1] = hexa[msg->d[i] & 0x0F];
  } 
  s[idx+i*2] = ';';
  return idx+i*2+1;
}

static char hexb[] = {0,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0,0,10,11,12,13,14,15};
static byte ASCII2Msg(unsigned char* ins, byte inlen, CANMsg* msg) {
  byte len = 0;
  byte i;
  unsigned char* s = ins;
  for( i = 0; i < inlen; i++ ) {
    if( s[i] == ':' && s[i+1] == 'S' ) {
      s += i;
      break;
    }
  }
  if( i == inlen )
    return 0;

  msg->opc = (hexb[s[7]-0x30]<<4) + hexb[s[8]-0x30];
  len = getDataLen(msg->opc);
  for( i = 0; i < len; i++ ) {
    msg->d[i] = (hexb[s[9+2*i]-0x30]<<4) + hexb[s[9+2*i+1]-0x30];
  }
  msg->len = len;
  return 1;
}

void CBusEthInit(void)
{
  BYTE i;

  for ( i = 0; i <  MAX_CBUSETH_CONNECTIONS; i++ ) {
    HCB[i].socket = TCPListen(CBUSETH_PORT);
  }

  for( i = 0; i < CANMSG_QSIZE; i++ ) {
    ETHMsgs[i].status = CANMSG_FREE;
  }

}



void CBusEthServer(void)
{
  BYTE conn;
  BYTE i;

  for ( conn = 0;  conn < MAX_CBUSETH_CONNECTIONS; conn++ ) {
    CBusEthProcess(conn);
  }


  for( i = 0; i < CANMSG_QSIZE; i++ ) {
    if( ETHMsgs[i].status == CANMSG_PENDING )
      ETHMsgs[i].status = CANMSG_FREE;
  }
  for( i = 0; i < CANMSG_QSIZE; i++ ) {
    if( ETHMsgs[i].status == CANMSG_OPEN ) {
      if( CBusEthBroadcast(&ETHMsgs[i]) ) {
        ETHMsgs[i].status = CANMSG_PENDING;
      }
      break;
    }
  }

}


static void CBusEthProcess(CBUSETH_HANDLE h)
{
    BYTE cbusData[MAX_CBUSETH_CMD_LEN+1];
    CBUSETH_INFO* ph = &HCB[h];

    if ( !TCPIsConnected(ph->socket) ) {
        return;
    }


    if ( TCPIsGetReady(ph->socket) ) {
        BYTE len = 0;
        CANMsg canmsg;

        while( len < MAX_CBUSETH_CMD_LEN && TCPGet(ph->socket, &cbusData[len++]) );
        cbusData[len] = '\0';
        TCPDiscard(ph->socket);
        // TODO: Check if the message is valid.
        if( ASCII2Msg(cbusData, len, &canmsg) ) {
          canQueue(&canmsg);
          parseCmdEth(&canmsg);
        }
    }
}


byte CBusEthBroadcast(CANMsg* msg)
{

    BYTE conn;
    char s[32];
    byte len;
    byte i;
    byte wait;

#ifdef WAIT_FOR_ALL_READY
    for ( conn = 0;  conn < MAX_CBUSETH_CONNECTIONS; conn++ ) {
      CBUSETH_INFO* ph = &HCB[conn];
      if ( TCPIsConnected(ph->socket) ) {
        if( !TCPIsPutReady(ph->socket) ) {
          return 0;
        }
      }
    }
#endif
    len = msg2ASCII(msg, s);
    for ( conn = 0;  conn < MAX_CBUSETH_CONNECTIONS; conn++ ) {
      CBUSETH_INFO* ph = &HCB[conn];
      if ( TCPIsConnected(ph->socket) )
      {
        wait = 0;
        
        while( !TCPIsPutReady(ph->socket) && wait < 10 ) {
          wait++;
          delay();
        }
        
        if( TCPIsPutReady(ph->socket) ) {
          for( i = 0; i < len; i++ ) {
            TCPPut(ph->socket, s[i]);
          }
          TCPFlush(ph->socket);
        }
        else {
          LED3 = LED_OFF; /* signal error */
          led3timer = 40;
        }
      }
   }
    return 1;

}



byte ethQueue(CANMsg* msg) {
  int i = 0;
  int n = 0;
  for( i = 0; i < CANMSG_QSIZE; i++ ) {
    if( ETHMsgs[i].status == CANMSG_FREE ) {
      ETHMsgs[i].opc = msg->opc;
      ETHMsgs[i].len = msg->len;
      for( n = 0; n < 7; n++ ) {
        ETHMsgs[i].d[n] = msg->d[n];
      }
      ETHMsgs[i].status = CANMSG_OPEN;
      return 1;
    }
  }

  LED3 = LED_OFF; /* signal error */
  led3timer = 40;
  return 0;
}


