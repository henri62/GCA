

#include "StackTsk.h"
#include "TCP.h"
#include "Helpers.h"

#include "cbuseth.h"
#include "cbus.h"

static ram CANMsg CANMsgs[CANMSG_QSIZE];


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
static byte getDataLen( unsigned char OPC ) {
  if( OPC < 0x20 ) return 0;
  if( OPC < 0x40 ) return 1;
  if( OPC < 0x60 ) return 2;
  if( OPC < 0x80 ) return 3;
  if( OPC < 0xA0 ) return 4;
  if( OPC < 0xC0 ) return 5;
  if( OPC < 0xE0 ) return 6;
  return 7;
}

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
static void ASCII2Msg(unsigned char* s, CANMsg* msg) {
  byte len = 0;
  byte i;
        
  msg->opc = (hexb[s[7]]<<4) + hexb[s[8]];
  len = getDataLen(msg->opc);
  for( i = 0; i < len; i++ ) {
    msg->d[i] = (hexb[s[9+2*i]]<<4) + hexb[s[9+2*i+1]];
  }
  msg->len = len + 1;
}

void CBusEthInit(void)
{
  BYTE i;

  for ( i = 0; i <  MAX_CBUSETH_CONNECTIONS; i++ ) {
    HCB[i].socket = TCPListen(CBUSETH_PORT);
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
    if( CANMsgs[i].status == CANMSG_PENDING )
      CANMsgs[i].status = CANMSG_FREE;
  }
  for( i = 0; i < CANMSG_QSIZE; i++ ) {
    if( CANMsgs[i].status == CANMSG_OPEN ) {
      CANMsgs[i].status = CANMSG_PENDING;
      CBusEthBroadcast(&CANMsgs[i]);
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

        while( len < MAX_CBUSETH_CMD_LEN &&
               TCPGet(ph->socket, &cbusData[len++]) );
        cbusData[len] = '\0';
        TCPDiscard(ph->socket);
        // TODO: Check if the message is valid.
        ASCII2Msg(cbusData, &canmsg);
        canQueue(&canmsg);
    }
}


void CBusEthBroadcast(CANMsg* msg)
{

    BYTE conn;
    char s[32];
    byte len = msg2ASCII(msg, s);

    for ( conn = 0;  conn < MAX_CBUSETH_CONNECTIONS; conn++ ) {
      CBUSETH_INFO* ph = &HCB[conn];
      if ( !TCPIsConnected(ph->socket) )
      {
          continue;
      }
      else {
        BYTE idx = 0;
        if( TCPIsPutReady(ph->socket) ) {
          TCPPut(ph->socket, msg->opc);
          while( TCPIsPutReady(ph->socket) && idx < len ) {
            TCPPut(ph->socket, s[idx]);
            idx++;
           }
         }
        TCPFlush(ph->socket);
      }
  }
}



byte ethQueue(CANMsg* msg) {
  int i = 0;
  int n = 0;
  for( i = 0; i < CANMSG_QSIZE; i++ ) {
    if( CANMsgs[i].status == CANMSG_FREE ) {
      CANMsgs[i].opc = msg->opc;
      CANMsgs[i].len = msg->len;
      for( n = 0; n < 7; n++ ) {
        CANMsgs[i].d[n] = msg->d[n];
      }
      CANMsgs[i].status = CANMSG_OPEN;
      return 1;
    }
  }
  return 0;
}


