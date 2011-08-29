/*
 * Microchip C18 source for common CBUS routines
 *
 * Code based on Mike Bolton's canacc8 for which due credit is given.
 *
 * Modifications and conversion to C18 Copyright (C) 2009 Andrew Crosland

 * Changed for 8 MHz resonator to double the clock. Mike Bolton  19/04/11
 */

#include "project.h"

#pragma romdata EEPROM
// Ensure newly programmed part is in virgin state
rom unsigned char defaultID = 0x7F;
rom unsigned char status = 0;
rom unsigned short nodeID = 0;

#ifdef HAS_EVENTS
rom unsigned short ENindex = 0;
rom event ENstart[EN_NUM];
rom unsigned short EVstart[EN_NUM];
#endif

#pragma romdata BOOTFLAG
rom unsigned char bootflag = 0;

#pragma code APP

/*
 * Common CBUS CAN setup
 */
void cbus_setup(void) {
  CANCON = 0b10000000; // CAN to config mode
  while (CANSTATbits.OPMODE2 == 0)
    // Wait for config mode
    ;

#ifdef ECAN_MODE_2
  // Want ECAN mode 2 with FIFO
  ECANCON = 0b10110000; // FIFOWM = 1 (one entry)
  // EWIN default
  BSEL0 = 0; // All buffers to receive so
  // FIFO is 8 deep
  RXB0CON = 0b00000000; // receive valid messages
#else
  ECANCON = 0b00010000; // EWIN default
  RXB0CON = 0b00100100; // Receive valid packets, double buffer RXB0
#endif

  /*
   * Baud rate setting.
   * Sync segment is fixed at 1 Tq
   * Total bit time is Sync + prop + phase 1 + phase 2
   * or 16 * Tq in our case
   * So, for 125kbits/s, bit time = 8us, we need Tq = 500ns
   * Fosc is 32MHz (8MHz + PLL), Tosc is 31.25ns, so we need 1:16 prescaler
   */
  // prescaler Tq = 16/Fosc
  BRGCON1 = 0b00000011;
  // freely programmable, sample once, phase 1 = 4xTq, prop time = 7xTq
  BRGCON2 = 0b10011110;
  // Wake-up enabled, wake-up filter not used, phase 2 = 4xTq
  BRGCON3 = 0b00000011;
  // TX drives Vdd when recessive, CAN capture to CCP1 disabled
  CIOCON = 0b00100000;

  RXFCON0 = 1; // Only filter 0 enabled
  RXFCON1 = 0;
  SDFLC = 0;

  // Setup masks so all filter bits are ignored apart from EXIDEN
  RXM0SIDH = 0;
  RXM0SIDL = 0x08;
  RXM0EIDH = 0;
  RXM0EIDL = 0;
  RXM1SIDH = 0;
  RXM1SIDL = 0x08;
  RXM1EIDH = 0;
  RXM1EIDL = 0;

  // Set filter 0 for standard ID only to reject bootloader messages
  RXF0SIDL = 0x80;

  // Link all filters to RXB0 - maybe only neccessary to link 1
  RXFBCON0 = 0;
  RXFBCON1 = 0;
  RXFBCON2 = 0;
  RXFBCON3 = 0;
  RXFBCON4 = 0;
  RXFBCON5 = 0;
  RXFBCON6 = 0;
  RXFBCON7 = 0;

  // Link all filters to mask 0
  MSEL0 = 0;
  MSEL1 = 0;
  MSEL2 = 0;
  MSEL3 = 0;

  BIE0 = 0; // No Rx buffer interrupts - assume FIFOWM will interrupt
  TXBIE = 0; // No Tx buffer interrupts

  // Clear RXFUL bits
  RXB0CON = 0;
  RXB1CON = 0;
  B0CON = 0;
  B1CON = 0;
  B2CON = 0;
  B3CON = 0;
  B4CON = 0;
  B5CON = 0;

  CANCON = 0; // Out of CAN setup mode
}

#pragma udata access VARS
#ifdef RX0
// Receive buffer
near unsigned char Rx0[14];
#endif

#ifdef TX1
// Transmit buffers
near unsigned char Tx1[14];
#endif

#pragma code APP

/*
 * ee_read - read from data EEPROM
 */
unsigned char ee_read(unsigned char addr) {
  EEADR = addr; /* Data Memory Address to read */
  EECON1bits.EEPGD = 0; /* Point to DATA memory */
  EECON1bits.CFGS = 0; /* Access program FLASH or Data EEPROM memory */
  EECON1bits.RD = 1; /* EEPROM Read */
  return EEDATA;
}

/*
 * ee_write - write to data EEPROM
 */
void ee_write(unsigned char addr, unsigned char data) {
  EEADR = addr;
  EEDATA = data;
  EECON1bits.EEPGD = 0; /* Point to DATA memory */
  EECON1bits.CFGS = 0; /* Access program FLASH or Data EEPROM memory */
  EECON1bits.WREN = 1; /* Enable writes */
  INTCONbits.GIE = 0; /* Disable Interrupts */
  EECON2 = 0x55;
  EECON2 = 0xAA;
  EECON1bits.WR = 1;
  _asm nop
  nop _endasm
  INTCONbits.GIE = 1; /* Enable Interrupts */
  while (!PIR2bits.EEIF)
    ;
  PIR2bits.EEIF = 0;
  EECON1bits.WREN = 0; /* Disable writes */
}

/*
 * ee_read_short() - read a short (16 bit) word from EEPROM
 *
 * Data is stored in little endian format
 */
unsigned short ee_read_short(unsigned char addr) {
  unsigned char byte_addr = addr;
  unsigned short ret = ee_read(byte_addr++);
  ret = ret | ((unsigned short) ee_read(byte_addr) << 8);
  return ret;
}

/*
 * ee_write_short() - write a short (16 bit) data to EEPROM
 *
 * Data is stored in little endian format
 */
void ee_write_short(unsigned char addr, unsigned short data) {
  unsigned char byte_addr = addr;
  ee_write(byte_addr++, (unsigned char) data);
  ee_write(byte_addr, (unsigned char) (data >> 8));
}

/*
 * A delay routine
 */
void dely(void) {
  unsigned char i, j;
  for (i = 0; i < 10; i++) { // dely     movlw    .10
    //          movwf    Count1
    for (j = 0; j <= 245; j++) // dely2    clrf    Count
      ; // dely1    decfsz    Count,F
    //          goto    dely1
    //          decfsz    Count1
  } //          bra        dely2
} //          return

/*
 * Long delay
 */
void ldely(void) {
  unsigned char i;
  for (i = 0; i < 100; i++) { //ldely	movlw	.100
    //		movwf	Count2
    dely(); //ldely1	call	dely
    //		decfsz	Count2
  } //		bra		ldely1
  //
} //		return

#ifdef ECAN_MODE_2
#pragma udata
ecan_rx_buffer * rx_ptr;

//
// ecan_fifo_empty()
//
// Check if ECAN FIFO has a valid receive buffer available and
// preload the pointer to it
//

unsigned char ecan_fifo_empty(void) {
  switch (CANCON & 0b00000111) {
    case 0:
      rx_ptr = (ecan_rx_buffer *) & RXB0CON;
      break;

    case 1:
      rx_ptr = (ecan_rx_buffer *) & RXB1CON;
      break;

    default:
      // Remaining buffers are 16 bytes long starting at
      // pointer value 2
      rx_ptr = (ecan_rx_buffer *) (&B0CON + (((CANCON - 2) & 0b00000111) << 4));
      break;

  }

  if (rx_ptr->con & 0x80) {
    // current buffer is not empty
    return 0;
  } else {
    return 1;
  }
}

#endif

