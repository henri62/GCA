#ifndef __IO_H
#define __IO_H


#define SW      PORTAbits.RA2	// Flim switch
#define LED1    PORTBbits.RB6
#define LED2    PORTBbits.RB7

#define PORT1   PORTAbits.RA0
#define PORT2   PORTAbits.RA1
#define PORT3   PORTAbits.RA3
#define PORT4   PORTAbits.RA4
#define PORT5   PORTAbits.RA5
#define PORT6   PORTBbits.RB0
#define PORT7   PORTBbits.RB4
#define PORT8   PORTBbits.RB1

#define PORT9   PORTCbits.RC0
#define PORT10  PORTCbits.RC1
#define PORT11  PORTCbits.RC2
#define PORT12  PORTCbits.RC3
#define PORT13  PORTCbits.RC7
#define PORT14  PORTCbits.RC6
#define PORT15  PORTCbits.RC5
#define PORT16  PORTCbits.RC4


typedef struct {
  byte   cfg;
  byte   status;
  ushort addr;
  byte   timedoff;
  ushort timer;
  ushort evtnn;
  ushort evtid;
  byte   evtinv;
} Port;

extern ram Port Ports[16];


void setupIO(void);
void writeOutput(int port, unsigned char val);
unsigned char readInput(int port);
void doIOTimers(void);
void doTimedOff(void);
void checkInputs(unsigned char sod);
void resetOutputs(void);
unsigned char checkFlimSwitch(void);
void doLEDs(void);

#endif	// __IO_H