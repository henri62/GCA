#include "specific.h"
#include "controlLn.h"

char sendLnByte(char cInByte)
{

    char cSendBitIdx = -1;
    char cSendByte;
    char  cSendBit;
    char  cCheckBit;
    char cReturnValue = 0x0B;
//    unsigned char ucShiftValue = 0; 
	
    Usb2LnFlags.bHalfBit = 0;

    INTCONbits.TMR0IE = 0;    // stop the timer 
    INTCONbits.TMR0IF = 0;    // clear interrupt memory
    TMR0L =171;               // 29.833 uS; 
    cSendByte = ~cInByte;
//    ucShiftValue = 0; 
    
    cSendBit = 1;
    cCheckBit = 0;
    cReturnValue = 0x0B; 
       
    for(cSendBitIdx = 0; (cSendBitIdx < 10) && (cReturnValue == 0x0B); cSendBitIdx++)
    {
       if(cSendBitIdx == 0) // start bit
       {
          INTCONbits.TMR0IF = 0;    // clear interrupt memory
          INTCONbits.TMR0IE = 1;    // start the timer
       }

       LN_OUT_PIN = cSendBit;	     // b0 (lsb)
       Usb2LnFlags.bUsb2LnSendingBit = 1;

       do {
          Nop();
       }while (Usb2LnFlags.bUsb2LnSendingBit) ;  //fisrst half of the start bit
	
       if(LN_IN_PIN != cCheckBit)  // received bit != check bit
       {
          cReturnValue = cSendBitIdx;
       }
	   
       if(cSendBitIdx<8) // data bits bit
       {
          cSendBit = (cSendByte >> cSendBitIdx) & 0x01;
          cCheckBit = (cInByte >> cSendBitIdx) & 0x01;
//          ucShiftValue++; 
       }
       
       if(cSendBitIdx==8) // stop bits bit
       {
          cSendBit = 0;
          cCheckBit = 1;
       }
       
       Usb2LnFlags.bUsb2LnSendingBit = 1;
       do {
	      Nop();
       }while (Usb2LnFlags.bUsb2LnSendingBit) ;  //start bit is transmited
   } // for(cSendBitIdx = -1

   if(cReturnValue != 0x0B) // transmission error => send at least 13 0 bits
   {
      for(cSendBitIdx = 0; cSendBitIdx < 15; cSendBitIdx++)
      {
          if(cSendBitIdx == -1) // start bit
          {
            cSendBit = 1;
            cCheckBit == 0;
          }

         LN_OUT_PIN = cSendBit;	     // b0 (lsb)
         Usb2LnFlags.bUsb2LnSendingBit = 1;

         do {
            Nop();
         }while (Usb2LnFlags.bUsb2LnSendingBit) ;  //fisrst half of the error mark bit

         cSendBit = 1; // profita de pauza si pregateste bitul urmator
         cCheckBit = 0; // profita de pauza si pregateste bitul urmator

         Usb2LnFlags.bUsb2LnSendingBit = 1;
         do {
	     Nop();
         }while (Usb2LnFlags.bUsb2LnSendingBit) ;  //start bit is transmited
      } // for(cSendBitIdx = 0
   }

   INTCONbits.TMR0IE = 0;    // stop the timer 
   INTCONbits.TMR0IF = 0;    // clear interrupt memory

   return cReturnValue;
}

char sendLnMessage(char *cInBytes, unsigned char ucMessLen)
{
   unsigned char ucIdx;
   char cRetVal = 0x0B;
//   char cTempByte;
   
   INTCONbits.INT0IE=0; //deactiveaza intreruperea RB0   
   for(ucIdx = 0; (ucIdx<ucMessLen) && (cRetVal == 0x0B); ucIdx++)
   {
//	   cTempByte = *cInBytes;
	   cRetVal = sendLnByte(*cInBytes);
	   *cInBytes++;
   } 
   
   INTCONbits.INT0IF=0; // flagurile sunt active si cand INTE=0, asa ca la activarea lui INTE genereaza intrerupere falsa
   INTCONbits.INT0IE=1; //activeaza intreruperea RB0
   return cRetVal;	
}

void sendLn15ZeroBits(void)
{
    ;
}

/**********char lnRecByte(char *cRecByte)**********************
 *
 * Function used to receive one byte from the LN network.
 * The receive process starts when (in idle mode) a negative edge is detected
 * at the LN input (INT0 interrupt) - begining of a start bit. After this edge:
 * - the INT0 interrupt is deactivated;
 * - the timer 0 is programed to overflor after 30 us (middle of the start bit);
 *   after first timer 0 interrupt, the timer 0 is programed to overflow after
 *   60 us (bit duration).
 * - the timer 0 interrupt is activated
 * - each time the timer 0 overflows, the bNewLnBit flag is setted.
 *    - if the bit number is>=1 && <=8, the bit is shifted in the received byte.
 *    - if the bit number is == 9 (stop bit), it is checked if it is 1.
 *       if yes => byte OK, if no => error => drop all message bytes
 *
 * - use lnSendByte() to send each byte of message, if no transmission errors
 *
 * Parameters: - pointer to the byte save variable
 * Return value: - error code
 ******************************/
 
char lnRecByte(char *cRecByte)
{
    char cBitIdx=0;
    char cRetVal = 0;
    char cRecByteLoc = 0;
    char cByteNotComplete = 1;
    
    do
    {
       if (Ln2UsbFlags.bLnToUsbNewBit)  // the bit timer overflows (60 us)
       {
         Ln2UsbFlags.bLnToUsbNewBit = 0;
         if(cBitIdx > 0 && cBitIdx < 9)
         {
            cRecByteLoc += (ucLnBitValue << (cBitIdx-1));
         }
         cBitIdx++;
         if(cBitIdx == 10)
         {
            if(ucLnBitValue)
            {
               cRetVal = 0;
               cBitIdx = 0;
               cByteNotComplete = 0;
               INTCONbits.TMR0IE = 0; //dezactiveaza TMR0
               INTCONbits.TMR0IF = 0; //dezactiveaza TMR0
               INTCONbits.INT0IF=0; // flagurile sunt active si cand INTE=0, asa ca la activarea lui INTE genereaza intrerupere falsa
               INTCONbits.INT0IE=1; //activeaza intreruperea RB0
            }   
            else
               cRetVal = -1;
         }
       } //if (Ln2UsbFlags.bLnToUsbNewBit)
    } while(((cRetVal == 0) && (cByteNotComplete)) /*|| (cBitIdx < 20)*/); //all 10 bits for comm OK or 20 reset bit for fail
    
    *cRecByte = cRecByteLoc;

    return cRetVal;
}

char lnRecMess(char *cRecMess, char *cMessLenght)
{
    char cByteIdx=0;
    char cMessageLengthLoc = 2;
    char cCommStatus;
    char cLnOpc;
    char cRecLocal = 0;
    char cBitIdx=0;
    char cRetVal = 0;
    
    cCommStatus = 0;

    for(cByteIdx = 0; (cByteIdx<cMessageLengthLoc) && (cCommStatus==0); /*cByteIdx++*/)
    {
	    cRecMess[cByteIdx] = 0;
	    cRetVal = 0;
	    
        if(Ln2UsbFlags.bLnToUsbData)
        {
           Ln2UsbFlags.bLnToUsbData=0;
           
	       cCommStatus = lnRecByte(&cRecMess[cByteIdx]);
           
           if (cByteIdx == 1)
           {
              cLnOpc = cRecMess[0] & OPC_BITS;
              switch (cLnOpc)
              {
                case OPC_2_BYTES : cMessageLengthLoc = 2;
                                   *cMessLenght = 2;
                                   break;
                case OPC_4_BYTES : cMessageLengthLoc = 4;
                                   *cMessLenght = 4;
                                   break;
                case OPC_6_BYTES : cMessageLengthLoc = 6;
                                   *cMessLenght = 6;
                                   break;
                case OPC_VAR_BYTES : cMessageLengthLoc = cRecMess[cByteIdx];
                                    *cMessLenght = cRecMess[cByteIdx];
                                     break;
              } //switch
           } //if (cByteIdx == 1)  
        
           cByteIdx++;
           
       } //if(Ln2UsbFlags.bLnToUsbData)          
    } //for(cByteIdx = 0; 
    
       
    INTCONbits.TMR0IE = 0; //dezactiveaza TMR0
    INTCONbits.INT0IF=0; // flagurile sunt active si cand INTE=0, asa ca la activarea lui INTE genereaza intrerupere falsa
    INTCONbits.INT0IE=1; //activeaza intreruperea RB0
    return cCommStatus;
}

void lnCheckSummTest(char *cMessage, char cMesLen)
{
	unsigned char ucIdx = 0;
	char cLn2UsbCheckSummXor = 0;
	
	Ln2UsbFlags.bLn2UsbCheckSummOK = 0;
	
	for (ucIdx=0; ucIdx < cMesLen; ucIdx++)
	{
       cLn2UsbCheckSummXor ^= cMessage[ucIdx];    
    }
	if(cLn2UsbCheckSummXor == 0xFF)
	{
	   Ln2UsbFlags.bLn2UsbCheckSummOK = 1;
	}   
}

	



