#ifndef __IO_H
#define __IO_H


#define SW      PORTAbits.RA2	// Flim switch
#define LED1    PORTBbits.RB6
#define LED2    PORTBbits.RB7


#define PORT1   PORTCbits.RC0
#define PORT2   PORTCbits.RC1
#define PORT3   PORTCbits.RC2
#define PORT4   PORTCbits.RC3
#define PORT5   PORTCbits.RC7
#define PORT6   PORTCbits.RC6
#define PORT7   PORTCbits.RC5
#define PORT8   PORTCbits.RC4

#define PORT9   PORTAbits.RA0
#define PORT10  PORTAbits.RA1
#define PORT11  PORTAbits.RA3
#define PORT12  PORTAbits.RA4
#define PORT13  PORTAbits.RA5
#define PORT14  PORTBbits.RB0
#define PORT15  PORTBbits.RB4
#define PORT16  PORTBbits.RB1


typedef struct {
  unsigned char cfg;
  unsigned char status;
  int addr;
  //unsigned short timer;
} Port;

extern ram Port Ports[16];


void setupIO(void);
void writeOutput(int port, unsigned char val);
unsigned char readInput(int port);

#endif	// __IO_H
