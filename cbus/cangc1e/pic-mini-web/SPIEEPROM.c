/*********************************************************************
 *
 *               Data SPI EEPROM Access Routines
 *
 *********************************************************************
 * FileName:        SPIEEPROM.c
 * Dependencies:    Compiler.h
 *                  XEEPROM.h
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.10PL1 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * This software is owned by Microchip Technology Inc. ("Microchip") 
 * and is supplied to you for use exclusively as described in the 
 * associated software agreement.  This software is protected by 
 * software and other intellectual property laws.  Any use in 
 * violation of the software license may subject the user to criminal 
 * sanctions as well as civil liability.  Copyright 2006 Microchip
 * Technology Inc.  All rights reserved.
 *
 * This software is provided "AS IS."  MICROCHIP DISCLAIMS ALL 
 * WARRANTIES, EXPRESS, IMPLIED, STATUTORY OR OTHERWISE, NOT LIMITED 
 * TO MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
 * INFRINGEMENT.  Microchip shall in no event be liable for special, 
 * incidental, or consequential damages.
 *
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     5/20/02     Original (Rev. 1.0)
 * Howard Schlunder		9/01/04		Rewritten for SPI EEPROMs
********************************************************************/
#include "Compiler.h"
#include "XEEPROM.h"
#include "StackTsk.h"

#if !defined(MPFS_USE_EEPROM)
#error MPFS_USE_EEPROM is not defined but spieeprom.c is present
#endif

/* Hardware interface to SPI EEPROM. */
#define EEPROM_CS_TRIS		(TRISB_RB4)
#define EEPROM_CS_IO		(LATB4)
// The following SPI pins are used but are not configurable
//   RC3 is used for the SCK pin and is an output
//   RC4 is used for the SDI pin and is an input
//   RC5 is used for the SDO pin and is an output
// IMPORTANT SPI NOTE: The code in this file expects that the SPI interrupt 
//		flag (SSP1IF) be clear at all times.  If the SPI is shared with 
//		other hardware, the other code should clear the SSP1IF when it is 
//		done using the SPI.


/* Psuedo functions */
#define SPISelectEEPROM()	EEPROM_CS_IO = 0
#define SPIUnselectEEPROM()	EEPROM_CS_IO = 1

/* Atmel flash */
#define USE_ATMEL_FLASH

/* Atmel opcodes*/
#define BUFFER_1_WRITE 						0x84	// buffer 1 write 
#define BUFFER_2_WRITE 						0x87 	// buffer 2 write 
#define BUFFER_1_READ 						0x54	// buffer 1 read
#define BUFFER_2_READ 						0x56	// buffer 2 read		
#define B1_TO_MM_PAGE_PROG_WITH_ERASE 		0x83	// buffer 1 to main memory page program with built-in erase
#define B2_TO_MM_PAGE_PROG_WITH_ERASE 		0x86	// buffer 2 to main memory page program with built-in erase
#define B1_TO_MM_PAGE_PROG_WITHOUT_ERASE 	0x88	// buffer 1 to main memory page program without built-in erase
#define B2_TO_MM_PAGE_PROG_WITHOUT_ERASE 	0x89	// buffer 2 to main memory page program without built-in erase
#define MM_PAGE_PROG_THROUGH_B1 			0x82	// main memory page program through buffer 1
#define MM_PAGE_PROG_THROUGH_B2 			0x85	// main memory page program through buffer 2
#define AUTO_PAGE_REWRITE_THROUGH_B1 		0x58	// auto page rewrite through buffer 1
#define AUTO_PAGE_REWRITE_THROUGH_B2 		0x59	// auto page rewrite through buffer 2
#define MM_PAGE_TO_B1_COMP 					0x60	// main memory page compare to buffer 1
#define MM_PAGE_TO_B2_COMP 					0x61	// main memory page compare to buffer 2
#define MM_PAGE_TO_B1_XFER 					0x53	// main memory page to buffer 1 transfer
#define MM_PAGE_TO_B2_XFER 					0x55	// main memory page to buffer 2 transfer
#define	READ_STATUS_REGISTER				0xD7	// read status register
#define CONTINUOUS_ARRAY_READ				0xE8	// continuous read 
#define MAIN_MEMORY_PAGE_READ               0x52	// main page read
#define PAGE_ERASE                          0x81	// page erase


/* EEPROM opcodes */
#define READ	0b00000011	// Read data from memory array beginning at selected address
#define WRITE	0b00000010	// Write data to memory array beginning at selected address
#define WRDI	0b00000100	// Reset the write enable latch (disable write operations)
#define WREN	0b00000110	// Set the write enable latch (enable write operations)
#define RDSR	0b00000101	// Read Status register
#define WRSR	0b00000001	// Write Status register

void DoWrite(void);

WORD EEPROMAddress;
BYTE EEPROMBuffer[EEPROM_BUFFER_SIZE];
BYTE *EEPROMBufferPtr;

BYTE FLASHReadyFlag;
WORD FLASHAddress;
BYTE *FLASHBufferPtr;
BYTE FLASHBuffer[FLASH_BUFFER_SIZE];


/*********************************************************************
 * Function:        void XEEInit(unsigned char speed)
 *
 * PreCondition:    None
 *
 * Input:           speed - not used (included for compatibility only)
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Initialize SPI module to communicate to serial
 *                  EEPROM.
 *
 * Note:            Code sets SPI clock to Fosc/4.  
 ********************************************************************/
void XEEInit(unsigned char speed)
{
	#define SSPEN 0x20		// SSP Enable bit in SSP1CON1

	SPIUnselectEEPROM();
	EEPROM_CS_TRIS = 0;		// Drive SPI EEPROM chip select pin

	TRISC_RC3 = 0;			// Set RC3 (SCK) pin as an output
	TRISC_RC4 = 1;			// Make sure RC4 (SDI) pin is an input
	TRISC_RC5 = 0;			// Set RC5 (SDO) pin as an output
	SSP1CON1 = SSPEN;		// SSPEN bit is set, SPI in master mode, 
							//  IDLE state is low level, Fosc/4 clock
	SSP1IF = 0;
	SSPSTAT_CKE = 1; 		// Transmit data on rising edge of clock
	SSPSTAT_SMP = 0;		// Input sampled at middle of data output time

	FLASHReadyFlag = 1;

}


/*********************************************************************
 * Function:        XEE_RESULT XEEBeginRead(unsigned char control,
 *                                          XEE_ADDR address)
 *
 * PreCondition:    XEEInit() is already called.
 *
 * Input:           control - EEPROM control and address code.
 *                  address - Address at which read is to be performed.
 *
 * Output:          XEE_SUCCESS if successful
 *                  other value if failed.
 *
 * Side Effects:    None
 *
 * Overview:        Sets internal address counter to given address.
 *                  Puts EEPROM in sequential read mode.
 *
 * Note:            This function does not release I2C bus.
 *                  User must call XEEEndRead() when read is not longer
 *                  needed; I2C bus will released after XEEEndRead()
 *                  is called.
 ********************************************************************/
XEE_RESULT XEEBeginRead(unsigned char control, XEE_ADDR address )
{
#if defined(USE_ATMEL_FLASH)

	FLASHAddress = address;
	FLASHBufferPtr = FLASHBuffer + FLASH_BUFFER_SIZE;
	return XEE_SUCCESS;

#else

	// Save the address and emptry the contents of our local buffer
	EEPROMAddress = address;
	EEPROMBufferPtr = EEPROMBuffer + EEPROM_BUFFER_SIZE;
	return XEE_SUCCESS;

#endif
}


/*********************************************************************
 * Function:        XEE_RESULT XEERead(void)
 *
 * PreCondition:    XEEInit() && XEEBeginRead() are already called.
 *
 * Input:           None
 *
 * Output:          XEE_SUCCESS if successful
 *                  other value if failed.
 *
 * Side Effects:    None
 *
 * Overview:        Reads next byte from EEPROM; internal address
 *                  is incremented by one.
 *
 * Note:            This function does not release I2C bus.
 *                  User must call XEEEndRead() when read is not longer
 *                  needed; I2C bus will released after XEEEndRead()
 *                  is called.
 ********************************************************************/
unsigned char XEERead(void)
{

#if defined(USE_ATMEL_FLASH)

	// Check if no more bytes are left in our local buffer
	if( FLASHBufferPtr == FLASHBuffer + FLASH_BUFFER_SIZE )
	{ 
		// Get a new set of bytes
		XEEReadArray(0, FLASHAddress, FLASHBuffer, FLASH_BUFFER_SIZE);
		FLASHAddress += FLASH_BUFFER_SIZE;
		FLASHBufferPtr = FLASHBuffer;
	}

	// Return a byte from our local buffer
	return *FLASHBufferPtr++;


#else
	// Check if no more bytes are left in our local buffer
	if( EEPROMBufferPtr == EEPROMBuffer + EEPROM_BUFFER_SIZE )
	{ 
		// Get a new set of bytes
		XEEReadArray(0, EEPROMAddress, EEPROMBuffer, EEPROM_BUFFER_SIZE);
		EEPROMAddress += EEPROM_BUFFER_SIZE;
		EEPROMBufferPtr = EEPROMBuffer;
	}

	// Return a byte from our local buffer
	return *EEPROMBufferPtr++;

#endif
}

/*********************************************************************
 * Function:        XEE_RESULT XEEEndRead(void)
 *
 * PreCondition:    XEEInit() && XEEBeginRead() are already called.
 *
 * Input:           None
 *
 * Output:          XEE_SUCCESS if successful
 *                  other value if failed.
 *
 * Side Effects:    None
 *
 * Overview:        Ends sequential read cycle.
 *
 * Note:            This function ends sequential cycle that was in
 *                  progress.  It releases I2C bus.
 ********************************************************************/
XEE_RESULT XEEEndRead(void)
{
    return XEE_SUCCESS;
}


/*********************************************************************
 * Function:        XEE_RESULT XEEReadArray(unsigned char control,
 *                                          XEE_ADDR address,
 *                                          unsigned char *buffer,
 *                                          unsigned char length)
 *
 * PreCondition:    XEEInit() is already called.
 *
 * Input:           control     - EEPROM control and address code.
 *                  address     - Address from where array is to be read
 *                  buffer      - Caller supplied buffer to hold the data
 *                  length      - Number of bytes to read.
 *
 * Output:          XEE_SUCCESS if successful
 *                  other value if failed.
 *
 * Side Effects:    None
 *
 * Overview:        Reads desired number of bytes in sequential mode.
 *                  This function performs all necessary steps
 *                  and releases the bus when finished.
 *
 * Note:            None
 ********************************************************************/
XEE_RESULT XEEReadArray(unsigned char control,
                        XEE_ADDR address,
                        unsigned char *buffer,
                        unsigned char length)
{

#if defined(USE_ATMEL_FLASH)

	unsigned int addr	= 0;
	unsigned int page	= 0;

	page = address/264;
	addr = address%264;

	page<<=1;	

	// perform to transfer
	SSPSTAT_CKE = 0; 		// Data transmitted on falling edge of SCK
	SSP1CON1 = 0x30;			// perform SPI Mode 3

	SPISelectEEPROM();

	// Send READ opcode
	SSP1BUF = CONTINUOUS_ARRAY_READ;
	//SSP1BUF = MAIN_MEMORY_PAGE_READ;
	while(!SSP1IF);
	SSP1IF = 0;
	
	// 24 bit page and address
	SSP1BUF = (*((unsigned char*)&page+1));
	while(!SSP1IF);
	SSP1IF = 0;

	SSP1BUF = (((unsigned char)page&0xFE)|(*((unsigned char*)&addr+1) & 0x01));
	while(!SSP1IF);
	SSP1IF = 0;

	SSP1BUF = (unsigned char)addr;
	while(!SSP1IF);
	SSP1IF = 0;

	// 32 don't care bits
	SSP1BUF = 0;
	while(!SSP1IF);
	SSP1IF = 0;	

	SSP1BUF = 0;
	while(!SSP1IF);
	SSP1IF = 0;

	SSP1BUF = 0;
	while(!SSP1IF);
	SSP1IF = 0;

	SSP1BUF = 0;
	while(!SSP1IF);
	SSP1IF = 0;


	while(length--)
	{
		SSP1BUF = 0;
		while(!SSP1IF);
		SSP1IF = 0;
		*buffer++ = SSP1BUF;
	};
	
	SPIUnselectEEPROM();

	// perform to transfer
	SSPSTAT_CKE = 1; 		// Transmit data on rising edge of clock
	SSP1CON1 = 0x20;			// perform SPI Mode 0

	return XEE_SUCCESS;


#else

	SPISelectEEPROM();

	// Send READ opcode
	SSP1BUF = READ;
	while(!SSP1IF);
	SSP1IF = 0;
	
	// Send address
	SSP1BUF = ((WORD_VAL*)&address)->v[1];
	while(!SSP1IF);
	SSP1IF = 0;
	SSP1BUF = ((WORD_VAL*)&address)->v[0];
	while(!SSP1IF);
	SSP1IF = 0;
	
	while(length--)
	{
		SSP1BUF = 0;
		while(!SSP1IF);
		SSP1IF = 0;
		*buffer++ = SSP1BUF;
	};
	
	SPIUnselectEEPROM();
	return XEE_SUCCESS;

#endif
}


/*********************************************************************
 * Function:        XEE_RESULT XEESetAddr(unsigned char control,
 *                                        XEE_ADDR address)
 *
 * PreCondition:    XEEInit() is already called.
 *
 * Input:           control     - data EEPROM control code
 *                  address     - address to be set for writing
 *
 * Output:          XEE_SUCCESS if successful
 *                  other value if failed.
 *
 * Side Effects:    None
 *
 * Overview:        Modifies internal address counter of EEPROM.
 *
 * Note:            Unlike XEESetAddr() in xeeprom.c for I2C EEPROM 
 *					 memories, this function is used only for writing
 *					 to the EEPROM.  Reads must use XEEBeginRead(), 
 *					 XEERead(), and XEEEndRead().
 *					This function does not release the SPI bus.
 *                  User must close XEEClose() after this function
 *                   is called.
 ********************************************************************/
XEE_RESULT XEESetAddr(unsigned char control, XEE_ADDR address)
{
#if defined(USE_ATMEL_FLASH)

	FLASHReadyFlag = 1;
	FLASHAddress = address;
	FLASHBufferPtr = FLASHBuffer;
	return XEE_SUCCESS;
#else

	EEPROMAddress = address;
	EEPROMBufferPtr = EEPROMBuffer;
	return XEE_SUCCESS;
#endif
}



/*********************************************************************
 * Function:        XEE_RESULT XEEWrite(unsigned char val)
 *
 * PreCondition:    XEEInit() && XEEBeginWrite() are already called.
 *
 * Input:           val - Byte to be written
 *
 * Output:          XEE_SUCCESS
 *
 * Side Effects:    None
 *
 * Overview:        Adds a byte to the current page to be writen when
 *					XEEEndWrite() is called.
 *
 * Note:            Page boundary cannot be exceeded or the byte 
 *					to be written will be looped back to the 
 *					beginning of the page.
 ********************************************************************/
XEE_RESULT XEEWrite(unsigned char val)
{
#if defined(USE_ATMEL_FLASH)

	if(FLASHReadyFlag == 1) {
		ReadBufferFromMainMemory(FLASHAddress);
		FLASHReadyFlag = 0;
	}

	
	ByteToBuffer(val, FLASHAddress);
	FLASHAddress++;

	if((FLASHAddress%FLASH_BUFFER_SIZE) == 0) {
		WriteBufferToMainMemory(FLASHAddress-1);	
	}

	return XEE_SUCCESS;

#else
	*EEPROMBufferPtr++ = val;
	if( EEPROMBufferPtr == EEPROMBuffer + EEPROM_BUFFER_SIZE ) 
		DoWrite();

    return XEE_SUCCESS;
#endif
}


/*********************************************************************
 * Function:        XEE_RESULT XEEEndWrite(void)
 *
 * PreCondition:    XEEInit() && XEEBeginWrite() are already called.
 *
 * Input:           None
 *
 * Output:          XEE_SUCCESS if successful
 *                  other value if failed.
 *
 * Side Effects:    None
 *
 * Overview:        Instructs EEPROM to begin write cycle.
 *
 * Note:            Call this function after either page full of bytes
 *                  written or no more bytes are left to load.
 *                  This function initiates the write cycle.
 *                  User must call for XEEIsBusy() to ensure that write
 *                  cycle is finished before calling any other
 *                  routine.
 ********************************************************************/
XEE_RESULT XEEEndWrite(void)
{
#if defined(USE_ATMEL_FLASH)

	FLASHReadyFlag = 0;
	if((FLASHAddress%FLASH_BUFFER_SIZE) != 0) {
		WriteBufferToMainMemory(FLASHAddress-1);		
	}    

	return XEE_SUCCESS;

#else
	if( EEPROMBufferPtr != EEPROMBuffer )
		DoWrite();

    return XEE_SUCCESS;
#endif
}


void DoWrite(void)
{
	BYTE BytesToWrite;

#if defined(USE_ATMEL_FLASH)

#else

	// Set the Write Enable latch
	SPISelectEEPROM();
	SSP1BUF = WREN;
	while(!SSP1IF);
	SSP1IF = 0;
	SPIUnselectEEPROM();
	
	// Send WRITE opcode
	SPISelectEEPROM();
	SSP1BUF = WRITE;
	while(!SSP1IF);
	SSP1IF = 0;
	
	// Send address
	SSP1BUF = ((WORD_VAL*)&EEPROMAddress)->v[1];
	while(!SSP1IF);
	SSP1IF = 0;
	SSP1BUF = ((WORD_VAL*)&EEPROMAddress)->v[0];
	while(!SSP1IF);
	SSP1IF = 0;
	
	BytesToWrite = (BYTE)(EEPROMBufferPtr - EEPROMBuffer);
	
	EEPROMAddress += BytesToWrite;
	EEPROMBufferPtr = EEPROMBuffer;

	while(BytesToWrite--)
	{
		// Send the byte to write
		SSP1BUF = *EEPROMBufferPtr++;
		while(!SSP1IF);
		SSP1IF = 0;
	}

	// Begin the write
	SPIUnselectEEPROM();

	EEPROMBufferPtr = EEPROMBuffer;

	// Wait for write to complete
	while( XEEIsBusy(0) );

#endif

}


/*********************************************************************
 * Function:        XEE_RESULT XEEIsBusy(unsigned char control)
 *
 * PreCondition:    XEEInit() is already called.
 *
 * Input:           control     - EEPROM control and address code.
 *
 * Output:          XEE_READY if EEPROM is not busy
 *                  XEE_BUSY if EEPROM is busy
 *                  other value if failed.
 *
 * Side Effects:    None
 *
 * Overview:        Requests ack from EEPROM.
 *
 * Note:            None
 ********************************************************************/
XEE_RESULT XEEIsBusy(unsigned char control)
{

	BYTE result;

#if defined(USE_ATMEL_FLASH)

	// perform to transfer
	SSPSTAT_CKE = 0; 		// Data transmitted on falling edge of SCK
	SSP1CON1 = 0x30;			// perform SPI Mode 3

	SPISelectEEPROM();

	// Send RDSR - Read Status Register opcode
	SSP1BUF = READ_STATUS_REGISTER;
	while(!SSP1IF);
	SSP1IF = 0;

	// Get register contents
	SSP1BUF = 0;
	while(!SSP1IF);
	SSP1IF = 0;
	result = SSP1BUF;

	SPIUnselectEEPROM();

	// perform to transfer
	SSPSTAT_CKE = 1; 		// Transmit data on rising edge of clock
	SSP1CON1 = 0x20;			// perform SPI Mode 0

	if(result&0x80) return XEE_SUCCESS;
	else XEE_BUSY;

#else

	SPISelectEEPROM();
	// Send RDSR - Read Status Register opcode
	SSP1BUF = RDSR;
	while(!SSP1IF);
	SSP1IF = 0;
	
	// Get register contents
	SSP1BUF = 0;
	while(!SSP1IF);
	SSP1IF = 0;
	result = SSP1BUF;
	SPIUnselectEEPROM();

	return (((BYTE_VAL*)&result)->bits.b0) ? XEE_BUSY : XEE_SUCCESS;

#endif

}


/*********************************************************************
 * Function:        EraseSector at specify address
 *
 * PreCondition:    XEEInit() is already called.
 *
 * Input:           Address to erase
 *
 * Side Effects:    None
 *
 * Overview:        Erase sector
 *
 * Note:            None
 ********************************************************************/
void EraseSector(unsigned int address) {

#if defined(USE_ATMEL_FLASH)

	unsigned int addr	= 0;
	unsigned int page	= 0;
		
	page = address/264;
	addr = address%264;	

	page<<=1;

	// Wait for write to complete
	while( XEEIsBusy(0) );

	// perform to transfer
	SSPSTAT_CKE = 0; 		// Data transmitted on falling edge of SCK
	SSP1CON1 = 0x30;			// perform SPI Mode 3

	SPISelectEEPROM();

	//command
	SSP1BUF = PAGE_ERASE;
	while(!SSP1IF);
	SSP1IF = 0;	

	//6 reserved + 2 high address
	SSP1BUF = (*((unsigned char*)&page+1));
	while(!SSP1IF);
	SSP1IF = 0;	

	//7 low address bits + 1 don't care
	SSP1BUF = ((unsigned char)page);
	while(!SSP1IF);
	SSP1IF = 0;	

	//don't care 8 bits
	SSP1BUF = 0;
	while(!SSP1IF);
	SSP1IF = 0;	
	
	SPIUnselectEEPROM();

	// Wait for write to complete
	while( XEEIsBusy(0) );

	// perform to transfer
	SSPSTAT_CKE = 1; 		// Transmit data on rising edge of clock
	SSP1CON1 = 0x20;			// perform SPI Mode 0

#endif

}



/*********************************************************************
 * Function:        Write buffer to main memory
 *
 * PreCondition:    XEEInit() is already called.
 *
 * Input:           Address which determine page to write
 *
 * Side Effects:    None
 *
 * Overview:        Write buffer to main memory
 *
 * Note:            None
 ********************************************************************/
void WriteBufferToMainMemory(unsigned int address) {
	
#if defined(USE_ATMEL_FLASH)

	unsigned int addr	= 0;
	unsigned int page	= 0;
		
	page = address/264;
	addr = address%264;	

	page<<=1;

	// Wait for write to complete
	while( XEEIsBusy(0) );	

	// perform to transfer
	SSPSTAT_CKE = 0; 		// Data transmitted on falling edge of SCK
	SSP1CON1 = 0x30;			// perform SPI Mode 3

	SPISelectEEPROM();

	//command
	SSP1BUF = B1_TO_MM_PAGE_PROG_WITH_ERASE;
	while(!SSP1IF);
	SSP1IF = 0;	

	//6 reserved + 2 high address
	SSP1BUF = (*((unsigned char*)&page+1));
	while(!SSP1IF);
	SSP1IF = 0;	

	//7 low address bits + 1 don't care
	SSP1BUF = ((unsigned char)page);
	while(!SSP1IF);
	SSP1IF = 0;	

	//don't care 8 bits
	SSP1BUF = 0;
	while(!SSP1IF);
	SSP1IF = 0;	
	
	SPIUnselectEEPROM();

	// Wait for write to complete
	while( XEEIsBusy(0) );

	// perform to transfer
	SSPSTAT_CKE = 1; 		// Transmit data on rising edge of clock
	SSP1CON1 = 0x20;			// perform SPI Mode 0

#endif

}


/*********************************************************************
 * Function:        Read buffer from main memory
 *
 * PreCondition:    XEEInit() is already called.
 *
 * Input:           Address which determine page to read
 *
 * Side Effects:    None
 *
 * Overview:        Read buffer to main memory
 *
 * Note:            None
 ********************************************************************/
void ReadBufferFromMainMemory(unsigned int address) {

#if defined(USE_ATMEL_FLASH)	

	unsigned int addr	= 0;
	unsigned int page	= 0;
		
	page = address/264;
	addr = address%264;	

	page<<=1;

	// Wait for write to complete
	while( XEEIsBusy(0) );	

	// perform to transfer
	SSPSTAT_CKE = 0; 		// Data transmitted on falling edge of SCK
	SSP1CON1 = 0x30;			// perform SPI Mode 3

	SPISelectEEPROM();

	//command
	SSP1BUF = MM_PAGE_TO_B1_XFER;
	while(!SSP1IF);
	SSP1IF = 0;	

	//6 reserved + 2 high address
	SSP1BUF = (*((unsigned char*)&page+1));
	while(!SSP1IF);
	SSP1IF = 0;	

	//7 low address bits + 1 don't care
	SSP1BUF = ((unsigned char)page);
	while(!SSP1IF);
	SSP1IF = 0;	

	//don't care 8 bits
	SSP1BUF = 0;
	while(!SSP1IF);
	SSP1IF = 0;	
	
	SPIUnselectEEPROM();

	// Wait for write to complete
	while( XEEIsBusy(0) );

	// perform to transfer
	SSPSTAT_CKE = 1; 		// Transmit data on rising edge of clock
	SSP1CON1 = 0x20;			// perform SPI Mode 0

#endif

}

/*********************************************************************
 * Function:        Program byte to buffer
 *
 * PreCondition:    XEEInit() is already called.
 *
 * Input:           Byte to program and address 
 *
 * Side Effects:    None
 *
 * Overview:        Program byte to buffer
 *
 * Note:            None
 ********************************************************************/
void ByteToBuffer(unsigned int byte, unsigned int address) {
	
	unsigned int 	addr			= 0;
	unsigned int 	page			= 0;

	page = address/264;
	addr = address%264;	

	// Wait for write to complete
	while( XEEIsBusy(0) );	

	// perform to transfer
	SSPSTAT_CKE = 0; 		// Data transmitted on falling edge of SCK
	SSP1CON1 = 0x30;			// perform SPI Mode 3

	// Set the Write Enable latch
	SPISelectEEPROM();

	SSP1BUF = BUFFER_1_WRITE;
	while(!SSP1IF);
	SSP1IF = 0;
	
	SSP1BUF = 0;
	while(!SSP1IF);
	SSP1IF = 0;
	
	SSP1BUF = (*((unsigned char*)&addr +1));
	while(!SSP1IF);
	SSP1IF = 0;
	
	SSP1BUF = ((unsigned char)addr);
	while(!SSP1IF);
	SSP1IF = 0;

	// Send the byte to write
	SSP1BUF = byte;
	while(!SSP1IF);
	SSP1IF = 0;		
	
	SPIUnselectEEPROM();

	// Wait for write to complete
	while( XEEIsBusy(0) );

	// perform to transfer
	SSPSTAT_CKE = 1; 		// Transmit data on rising edge of clock
	SSP1CON1 = 0x20;			// perform SPI Mode 0
}

void WriteProba(void) {

	// Set the Write Enable latch
	SPISelectEEPROM();
	SSP1BUF = 0x55;
	while(!SSP1IF);
	SSP1IF = 0;
	SPIUnselectEEPROM();
}
