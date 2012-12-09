#include <delays.h>
//#include "./controlLCD.h"
#include "controlLn.h"

#define BITSET(var,bitno) ((var) |= 1 << (bitno))
#define BITCLR(var,bitno) ((var) &= ~(1 << (bitno)))


#ifndef	_XTAL_FREQ
#define	_XTAL_FREQ	20000000		/* Crystal frequency in MHz */
#endif

extern void DelayUs(unsigned char);
extern void DelayMs(unsigned char);

/**********
 ** LocoNet
 ************/

#define TMR0_FULL_BIT 80
#define TMR0_HALF_BIT 171

//extern volatile unsigned char ucUsb2LnSendingBit;
//extern volatile unsigned char ucHalfBit;

typedef volatile struct {
    unsigned bUsb2LnCheckSummTest : 1;
    unsigned bUsb2LnCheckSummOK : 1;
    unsigned bSendUsbData : 1;
    unsigned bHalfBit : 1;
    unsigned bSendUsbToLnData : 1;
    unsigned bUsb2LnSendingBit : 1;
} Usb2LnFlags_type;

extern Usb2LnFlags_type Usb2LnFlags;

typedef volatile struct Ln2UsbFlagsStruct {
    unsigned bLnToUsbData : 1;
    unsigned bLnToUsbNewBit : 1;
    unsigned bLn2UsbCheckSummTest : 1;
    unsigned bLn2UsbCheckSummOK : 1;
    unsigned bLnRecError : 1;
    unsigned bLnValue : 1;
    unsigned bNewTmr0Int : 1;
} Ln2UsbFlags_type;

extern Ln2UsbFlags_type Ln2UsbFlags;

extern volatile unsigned char ucLnBitValue;
