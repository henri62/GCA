#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>

#include <termio.h>
#include <termios.h>
#include "linux/serial.h"
#include <sys/io.h>


/*
#define TIOCM_LE        0x001
#define TIOCM_DTR       0x002
#define TIOCM_RTS       0x004
#define TIOCM_ST        0x008
#define TIOCM_SR        0x010
#define TIOCM_CTS       0x020
#define TIOCM_CAR       0x040
#define TIOCM_RNG       0x080
#define TIOCM_DSR       0x100
#define TIOCM_CD        TIOCM_CAR
#define TIOCM_RI        TIOCM_RNG
#define TIOCM_OUT1      0x2000
#define TIOCM_OUT2      0x4000
#define TIOCM_LOOP      0x8000
*/

/*
base + 5
LSR : Line status register (RO)

The LSR, line status register shows the current state of communication. Errors are reflected in this register. The state of the receive and transmit buffers is also available.

LSR : Line status register Bit	Description
0	Data available
1	Overrun error
2	Parity error
3	Framing error
4	Break signal received
5	THR is empty
6	THR is empty, and line is idle
7	Errornous data in FIFO
*/

/*
base + 6
MSR : Modem status register (RO)

The MSR, modem status register contains information about the four incomming modem control lines on the device. The information is split in two nibbles. The four most siginificant bits contain information about the current state of the inputs where the least significant bits are used to indicate state changes. The four LSB's are reset, each time the register is read.

MSR : Modem status register Bit	Description
0	change in Clear to send
1	change in Data set ready
2	trailing edge Ring indicator
3	change in Carrier detect
4	Clear to send
5	Data set ready
6	Ring indicator
7	Carrier detect
*/

static int last_msr = 0;
void check(int msr) {
  char *le  ="   ";
  char *st  ="   ";
  char *sr  ="   ";
  char *rts ="   ";
  char *cts ="   ";
  char *dsr ="   ";
  char *dtr ="   ";
  char *ri  ="   ";
  char *car ="   ";
  
  if( msr == last_msr )
    return;
  last_msr = msr;
  
  if(msr & TIOCM_LE)  le ="LE ";
  if(msr & TIOCM_ST)  st ="ST ";
  if(msr & TIOCM_SR)  sr ="SR ";
  if(msr & TIOCM_DSR) dsr="DSR";
  if(msr & TIOCM_RI ) ri ="RI ";
  if(msr & TIOCM_CTS) cts="CTS";
  if(msr & TIOCM_RTS) rts="RTS";
  if(msr & TIOCM_DTR) dtr="DTR";
  if(msr & TIOCM_CAR) dtr="CAR";
  printf( "[%s][%s][%s][%s][%s][%s][%s][%s][%s][%04X]\n", le,st,sr,rts,cts,dsr,dtr,ri,car, msr);
  /*printf( "msr=0x%04X dsr=0x%04X ri=0x%04X\n", msr, TIOCM_DSR, TIOCM_RI );*/
}

static int last_val = 0;
void checkport(int lsr, int msr) {
  if( (lsr+msr) != last_val ) {
    printf( "lsr[0x%02X] msr[0x%02X] %s%s%s \n", lsr,msr, lsr&0x40?"UART is empty ":"", msr&0x02?"DSR changed ":"", msr&0x04?"RI changed ":"" );
    last_val=lsr+msr;
  }
}

int main( int argc, char** argv ) {
  char* device ="/dev/ttyS0";
  int msr, mask = 1;
  int h = 0;
  int portbase = 0x3F8;
  
  if( argc >= 2 ) {
    device = argv[1];
  }
  if( argc >= 3 ) {
    portbase = atoi(argv[2]);
  }
  
  h = open( device, O_RDWR | O_TRUNC | O_NONBLOCK | O_NOCTTY  );
  if( h < 0 ) {
    printf( "h=%d errno=%d\n",h, errno );
    return -1;
  }
  
  printf( "----- Modem Status Register (%s) -----\n", device );
    
  while(1) {
    if( ioperm(portbase, 7, 1) == 0 ) {
      int l_lsr = inb(portbase+5);
      int l_msr = inb(portbase+6);
      checkport(l_lsr,l_msr);
    }
    
		{    
      int rc = ioctl( h, TIOCMGET, &msr );
      if( rc < 0 ) {
        printf( "h=%d rc=%d errno=%d\n",h, rc, errno );
        return -1;
      }
      check(msr);
 		}   
    
    usleep(100000);
/*
    rc = ioctl( h, TIOCMSET, &mask );
    mask = mask << 1;
    if(mask&0x200||mask==0) mask = 1;
*/
  };
  return 0;
}
