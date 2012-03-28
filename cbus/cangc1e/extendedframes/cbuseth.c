/*
 Rocrail - Model Railroad Software

 Copyright (C) Rob Versluis <r.j.versluis@rocrail.net>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


#include "StackTsk.h"
#include "TCP.h"
#include "Helpers.h"

#include "cbuseth.h"
#include "cbus.h"
#include "commands.h"
#include "utils.h"
#include "io.h"
#include "cangc1e.h"
#include "cbusdefs.h"

#define WAIT_FOR_ALL_READY

#pragma udata VARS_ETH_ARRAYS1
static far CANMsg ETHMsgs[CANMSG_QSIZE];
#pragma udata VARS_ETH_ARRAYS2
static far EXTMsg EXTMsgs[EXTMSG_QSIZE];


/*
 * CBUSETH Connection Info - one for each connection.
 */
typedef struct _CBUSETH_INFO
{
    TCP_SOCKET socket;
    unsigned char idle;
} CBUSETH_INFO;
typedef BYTE CBUSETH_HANDLE;




static CBUSETH_INFO HCB[MAX_CBUSETH_CONNECTIONS];


static byte CBusEthProcess(CBUSETH_HANDLE h);



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
  byte len = getDataLen(msg->opc, ((msg->len&0x80)?TRUE:FALSE));
  byte i;
  byte idx = 9;
  s[0] = ':';
  s[1] = ((msg->len & 0x80) ? 'Y':'S');
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

static byte extMsg2ASCII(EXTMsg* msg, char* s) {
    // extended frame
    // For an extended header, the four bytes map directly to the SIDH, SIDL, EIDH and EIDL.

    byte i;
    byte len = msg->dlc & 0x0F;
    s[0] = ':';
    s[1] = 'X';
    s[2] = hexa[msg->sidh >> 4];
    s[3] = hexa[msg->sidh & 0x0F];
    s[4] = hexa[msg->sidl >> 4];
    s[5] = hexa[msg->sidl & 0x0F];
    s[6] = hexa[msg->eidh >> 4];
    s[7] = hexa[msg->eidh & 0x0F];
    s[8] = hexa[msg->eidl >> 4];
    s[9] = hexa[msg->eidl & 0x0F];
    s[10] = 'N';
    for( i = 0; i < len; i++) {
      s[11 + i*2]     = hexa[msg->d[i] >> 4];
      s[11 + i*2 + 1] = hexa[msg->d[i] & 0x0F];
    }
    s[11+i*2] = ';';
    return 11+i*2+1;
}

/*  StrOp.fmtb( frame+1, ":%c%02X%02XN%02X;", eth?'Y':'S', (0x80 + (prio << 5) + (cid >> 3)) &0xFF, (cid << 5) & 0xFF, cmd[0] );*/
static char hexb[] = {0,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0,0,10,11,12,13,14,15};
static byte ASCII2Msg(unsigned char* ins, byte inlen, CANMsg* msg, EXTMsg* ext) {
  byte len = 0;
  byte i;
  byte type = 1;
  byte sidh, sidl;

  unsigned char* s = ins;
  for( i = 0; i < inlen; i++ ) {
    if( s[i] == ':' && ( s[i+1] == 'S' || s[i+1] == 'Y' ) ) {
      if( s[i+1] == 'Y' ) {
        type = 2;
      }
      s += i;
      break;
    }
    else if( s[i] == ':' && s[i+1] == 'X' ) {
      type = 3;
      s += i;
      break;
    }
  }
  if( i == inlen )
    return 0;

  if( type == 3 ) {
    ext->sidh = (hexb[s[2]-0x30]<<4) + hexb[s[3]-0x30];
    ext->sidl = (hexb[s[4]-0x30]<<4) + hexb[s[5]-0x30];
    ext->eidh = (hexb[s[6]-0x30]<<4) + hexb[s[7]-0x30];
    ext->eidl = (hexb[s[8]-0x30]<<4) + hexb[s[9]-0x30];
    // copying all data bytes:
    for( i = 0; i < 8 && s[10+2*i] != ';'; i++ ) {
      ext->d[i] = (hexb[s[10+2*i]-0x30]<<4) + hexb[s[10+2*i+1]-0x30];
    }
    ext->dlc = i;
  }
  else {
    sidh = (hexb[s[1]-0x30]<<4) + hexb[s[2]-0x30];
    sidl = (hexb[s[3]-0x30]<<4) + hexb[s[4]-0x30];
    CANID = (sidl >> 5 ) + ((sidh&0x0F) << 3);

    msg->opc = (hexb[s[7]-0x30]<<4) + hexb[s[8]-0x30];
    len = getDataLen(msg->opc, FALSE);
    for( i = 0; i < len; i++ ) {
      msg->d[i] = (hexb[s[9+2*i]-0x30]<<4) + hexb[s[9+2*i+1]-0x30];
    }
    msg->len = len;
  }
  return type;
}

void CBusEthInit(void)
{
  BYTE i;

  for ( i = 0; i <  MAX_CBUSETH_CONNECTIONS; i++ ) {
    HCB[i].socket = TCPListen(CBUSETH_PORT);
    HCB[i].idle = 0;
  }

  for( i = 0; i < CANMSG_QSIZE; i++ ) {
    ETHMsgs[i].status = CANMSG_FREE;
  }
  for( i = 0; i < EXTMSG_QSIZE; i++ ) {
    EXTMsgs[i].status = CANMSG_FREE;
  }

}


static byte nrClients = 0;

void CBusEthServer(void)
{
  BYTE conn;
  BYTE i;
  byte nrconn = 0;
  byte broadcasted = FALSE;

  for ( conn = 0;  conn < MAX_CBUSETH_CONNECTIONS; conn++ ) {
    if( CBusEthProcess(conn) )
      nrconn++;
  }

  if( nrconn != nrClients ) {
    CANMsg canmsg;
    canmsg.opc = 1;
    canmsg.d[0] = 0;
    canmsg.d[1] = nrconn;
    canmsg.len = 0x80 + 2;
    ethQueue(&canmsg);
    nrClients = nrconn;
  }



  for( i = 0; i < CANMSG_QSIZE; i++ ) {
    if( ETHMsgs[i].status == CANMSG_PENDING )
      ETHMsgs[i].status = CANMSG_FREE;
  }
  for( i = 0; i < CANMSG_QSIZE; i++ ) {
    if( ETHMsgs[i].status == CANMSG_OPEN ) {
      if( nrconn == 0 || CBusEthBroadcast(&ETHMsgs[i]) ) {
        ETHMsgs[i].status = CANMSG_PENDING;
        broadcasted = TRUE;
      }
      break;
    }
  }

  if( !broadcasted ) {
    for( i = 0; i < EXTMSG_QSIZE; i++ ) {
      if( EXTMsgs[i].status == CANMSG_PENDING )
        EXTMsgs[i].status = CANMSG_FREE;
    }
    for( i = 0; i < EXTMSG_QSIZE; i++ ) {
      if( EXTMsgs[i].status == CANMSG_OPEN ) {
        if( nrconn == 0 || CBusEthBroadcastExt(&EXTMsgs[i]) ) {
          EXTMsgs[i].status = CANMSG_PENDING;
          broadcasted = TRUE;
        }
        break;
      }
    }
  }

}


static byte CBusEthProcess(CBUSETH_HANDLE h)
{
    BYTE cbusData[MAX_CBUSETH_CMD_LEN+1];
    CBUSETH_INFO* ph = &HCB[h];

    if ( !TCPIsConnected(ph->socket) ) {
        ph->idle = 0;
        return FALSE;
    }


    if ( TCPIsGetReady(ph->socket) ) {
        BYTE len = 0;
        byte type;
        CANMsg canmsg;
        EXTMsg extmsg;
        ph->idle = 0;


        while( len < MAX_CBUSETH_CMD_LEN && TCPGet(ph->socket, &cbusData[len++]) );
        cbusData[len] = '\0';
        TCPDiscard(ph->socket);
        // TODO: Check if the message is valid.
        type = ASCII2Msg(cbusData, len, &canmsg, &extmsg);
        if( type > 0 ) {
          if( type < 3 ) {
            if( parseCmdEth(&canmsg, type) ) {
              canQueue(&canmsg);
            }
          }
          else if( type == 3 ) {
            canExtQueue(&extmsg);
          }
        }
    }
    else if( ph->idle > IdleTime ) {
      TCPDisconnect(ph->socket);
      ph->idle = 0;
      if( NV1 & CFG_POWEROFF_ATIDLE ) {
        CANMsg canmsg;
        canmsg.opc = OPC_RTOF;
        canmsg.len = 0;
        canQueue(&canmsg);
      }
    }

    return TRUE;
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



byte CBusEthBroadcastExt(EXTMsg* msg)
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
    len = extMsg2ASCII(msg, s);
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


byte ethExtQueue(EXTMsg* msg) {
  int i = 0;
  int n = 0;
  for( i = 0; i < EXTMSG_QSIZE; i++ ) {
    if( EXTMsgs[i].status == CANMSG_FREE ) {
      EXTMsgs[i].sidh = msg->sidh;
      EXTMsgs[i].sidl = msg->sidl;
      EXTMsgs[i].eidh = msg->eidh;
      EXTMsgs[i].eidl = msg->eidl;
      EXTMsgs[i].dlc  = msg->dlc;
      for( n = 0; n < 8; n++ ) {
        EXTMsgs[i].d[n] = msg->d[n];
      }
      EXTMsgs[i].status = CANMSG_OPEN;
      return 1;
    }
  }

  LED3 = LED_OFF; /* signal error */
  led3timer = 40;
  return 0;
}


/* called every 500ms */
void CBusEthTick(void) {
  byte conn;
  if( NV1 & CFG_IDLE_TIMEOUT ) {
    for ( conn = 0;  conn < MAX_CBUSETH_CONNECTIONS; conn++ ) {
      CBUSETH_INFO* ph = &HCB[conn];
      if( TCPIsConnected(ph->socket) ) {
        ph->idle++;
      }
    }
  }
}
