/*****
 loconet definitions
 *****/

#define MARK  1
#define SPACE 0

#define OPC_BITS  0xE0

// 2 Byte MESSAGE opcodes
// FORMAT = <OPC>,<CKSUM>
#define OPC_2_BYTES 0x80
#define OPC_IDLE  0x85
#define OPC_GPON  0x83
#define OPC_GPOFF 0x82
#define OPC_BUSY  0x81

// 4 byte MESSAGE OPCODES
// FORMAT = <OPC>,<ARG1>,<ARG2>,<CKSUM>
// CODES 0xB8 to 0xBF have responses
#define OPC_4_BYTES      0xA0
#define OPC_LOCO_ADR     0xBF
#define OPC_SW_ACK       0xBD
#define OPC_SW_STATE     0xBC
#define OPC_RQ_SL_DATA   0xBB
#define OPC_MOVE_SLOTS   0xBA
#define OPC_LINK_SLOTS   0xB9
#define OPC_UNLINK_SLOTS 0xB8

#define OPC_CONSIST_FUNC 0xB6
#define OPC_SLOT_STAT1   0xB5
#define OPC_LONG_ACK     0xB4
#define OPC_INPUT_REP    0xB2
#define OPC_SW_REP       0xB1
#define OPC_SW_REQ       0xB0

// "A" CLASS codes
// CODES 0xA8 to 0xAF have responses
#define OPC_LOCO_SND  0xA2
#define OPC_LOCO_DIRF 0xA1
#define OPC_LOCO_SPD  0xA0

// 6 BYTES codes
#define OPC_6_BYTES       0xC0

// VARIABLE Byte MESSAGE OPCODES
// FORMAT = <OPC>,<COUNT>,<ARG2>,<ARG3>,...,<ARG(COUNT-3)>,<CKSUM>
#define OPC_VAR_BYTES    0xE0
#define OPC_WR_SL_DATA  0xEF // WRITE SLOT DATA, 10 bytes, NO return
                             // <0xEF>,<0E>,<SLOT#>,<STAT>,<ADR>,<SPD>,<DIRF>,<TRK>
                             // <SS2>,<ADR2>,<SND>,<ID1>,<ID2>,<CHK>
                             // SLOT DATA WRITE, 10 bytes data /14 byte MSG
#define OPC_SL_RD_DATA  0xE7  // SLOT DATA READ, 10 bytes, NO return
                             // <0xE7>,<0E>,<SLOT#>,<STAT>,<ADR>,<SPD>,<DIRF>,<TRK>
                             // <SS2>,<ADR2>,<SND>,<ID1>,<ID2>,<CHK>
                             // SLOT DATA READ, 10 bytes data /14 byte MSG
#if defined(__18F4550) 
   #define LN_IN_PIN  PORTBbits.RB0
   #define LN_OUT_PIN PORTBbits.RB1 
   #define LN_ACTIVITY_LED LATDbits.LATD2 
#elif defined(__18F2550)  
   #define LN_IN_PIN  PORTBbits.RB0
   #define LN_OUT_PIN PORTBbits.RB1 
   #define LN_ACTIVITY_LED LATBbits.LATB2 
#elif defined(__18F14K50)
   #define LN_IN_PIN PORTCbits.RC0
   #define LN_OUT_PIN LATCbits.LATC4
   #define LN_ACTIVITY_LED LATCbits.LATC5
#endif

extern char sendLnByte(char );
extern char sendLnMessage(char *, unsigned char );
extern char lnRecMess(char *, char *);
extern void lnCheckSummTest(char *, char );
