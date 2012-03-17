// lcd_test.c

#if defined(HI_TECH_C)
	#include <pic18.h>
#else
	#include <p18cxxx.h>
#endif

#include "test.h"


#define	BIT0	0x01
#define	BIT1	0x02
#define	BIT2	0x04
#define	BIT3	0x08
#define	BIT4	0x10
#define	BIT5	0x20
#define	BIT6	0x40
#define	BIT7	0x80

#define DELAY_TIME 			300
#define DT 					150
#define	DELAY_TIME_LONG		2000
#define	BIT2	0x04


// simple delay ~mS
void Delay_mS(unsigned long a) { 
	 a *= 170;
	while (--a!=0); 
}

void InitLed(void) {
	
	// led as output
	TRISC &= ~BIT2; 

	// led off -> high
	PORTC |= BIT2; 
}

// Blink if OK :-)
void LedBlinkOK(void) {
	
	PORTC &= ~BIT2; 
	Delay_mS(200);
	PORTC |= BIT2; 
	Delay_mS(200);
	PORTC &= ~BIT2; 
	Delay_mS(200);
	PORTC |= BIT2; 
	Delay_mS(200);
	Delay_mS(1500);

}

// Blink if ERROR :-)
void LedBlinkError(unsigned char n) {

	// unsigned char i;

	// for(i=0; i<n; i++) {	

	while(n--) {

		PORTC &= ~BIT2; 
		Delay_mS(1000);
		PORTC |= BIT2; 
		Delay_mS(1000);

	}
}

volatile unsigned char mask_port_a 	= 0x2F;
volatile unsigned char mask_port_b 	= 0x02&(~BIT1); // pull up BIT1;
volatile unsigned char mask_port_c 	= 0xC3;


volatile unsigned char temp_porta 	= 0x0;
volatile unsigned char temp_portb 	= 0x0;
volatile unsigned char temp_portc 	= 0x0;

/*
unsigned char mask_port_a 	= 0x0F;
unsigned char mask_port_b 	= 0x02&(~BIT1); // pull up BIT2;
unsigned char mask_port_c 	= 0xC3;

unsigned char temp_porta 	= 0x0;
unsigned char temp_portb 	= 0x0;
unsigned char temp_portc 	= 0x0;
*/

unsigned char	err_vcc_a	= 0;
unsigned char	err_vcc_b	= 0;
unsigned char	err_vcc_c	= 0;

unsigned char	err_gnd_a	= 0;
unsigned char	err_gnd_b	= 0;
unsigned char	err_gnd_c	= 0;

unsigned char	err_pin_a	= 10;
unsigned char	err_pin_b	= 10;
unsigned char	err_pin_c	= 10;

unsigned int 	i		= 0;

// simple delay ~ 1 mS
void Delay(unsigned long a) { 
	 a *= 370;
	while (--a!=0); 
}




void Stop(void) {

	// Led Init
	InitLed();


	if(err_vcc_a) {
		LedBlinkError(1);
	}

	if(err_vcc_b) {
		LedBlinkError(2);
	}	

	if(err_vcc_c) {
		LedBlinkError(3);
	}

	if(err_gnd_a) {
		LedBlinkError(4);
	}

	if(err_gnd_b) {
		LedBlinkError(5);
	}

	if(err_gnd_c) {
		LedBlinkError(6);
	}


	if(err_pin_a!=10) {
		
		LedBlinkError(7+err_pin_a);		

		// Usart1WriteText("\n\rPin error -> ");			
		// Usart1WriteChar(err_pin_a+48);
		// Usart1WriteText(" at PortA\0");			
	}

	if(err_pin_b!=10) {

		LedBlinkError(15+err_pin_b);				

		// Usart1WriteText("\n\rPin error -> ");			
		// Usart1WriteChar(err_pin_b+48);
		// Usart1WriteText(" at PortB\0");				
	}

	if(err_pin_c!=10) {

		LedBlinkError(23+err_pin_c);				

		// Usart1WriteText("\n\rPin error -> ");			
		// Usart1WriteChar(err_pin_c+48);
		// Usart1WriteText(" at PortC\0");				
	}



	// all as input
	ADCON1 	= 0x0F;
	TRISA 	= 0xFF;
	TRISB 	= 0xFF;
	TRISC 	= 0xFF;

	while(1);

}



void TestExt(void) {


	// TEST EXTENSION PORT

	// Check for GND ======================================
	// all as input
	ADCON1 	= 0x0F;
	TRISA 	= 0xFF;
	TRISC 	= 0xFF;
	TRISB 	= 0xFF;

	//pull up - output
	TRISB &= ~BIT1;
	//pull up - high
	PORTB |= BIT1;

	Delay(10);

	temp_porta 	= PORTA;
	temp_porta 	|= (~mask_port_a); 
	if((temp_porta) != (0xFF)) {
	// if((PORTA|(~mask_port_a)) != 0xFF) {
		err_gnd_a = 1;
		Stop();
	} 

	temp_portb 	= PORTB;
	temp_portb 	|=(~mask_port_b); 
	if((temp_portb) != (0xFF)) {
	// if((PORTB|(mask_port_b)) != 0xFF) {
		err_gnd_b = 1;
		Stop();
	} 

	temp_portc 	= PORTC;
	temp_portc 	|=(~mask_port_c); 
	if((temp_portc) != (0xFF)) {
		err_gnd_c = 1;
		Stop();
	} 

	// Check for VCC =============================

	// all as input
	ADCON1 	= 0x0F;
	TRISA 	= 0xFF;
	TRISC 	= 0xFF;
	TRISB 	= 0xFF;

	//pull up - output
	TRISB &= ~BIT1;
	//pull up - low
	PORTB &= ~BIT1;

	Delay(10);

	temp_porta 	= PORTA;
	temp_porta 	&= mask_port_a; 
	if((temp_porta) != (0x0)) {
		err_vcc_a = 1;
		Stop();
	} 
 
	temp_portb 	= PORTB;
	temp_portb 	&= mask_port_b; 
	if((temp_portb) != (0x0)) {
		err_vcc_b = 1;
		Stop();
	} 

	temp_portc 	= PORTC;
	temp_portc 	&= mask_port_c; 
	if((temp_portc) != (0x0)) {
		err_vcc_c = 1;
		Stop();
	} 


	// Running zero =========================================
	// all as input
	ADCON1 	= 0x0F;
	TRISA 	= 0xFF;
	TRISC 	= 0xFF;
	TRISB 	= 0xFF;


	// PortA
	//pull up - output
	TRISB &= ~BIT1;
	//pull up - high
	PORTB |= BIT1;
	
	Delay(10);

	for(i=0; i<8; i++) {
	
		// this port is not tested
		if(!((mask_port_a)&(1<<i))) continue;
		
		// TRISA = ~((1<<i)|BIT2);
		TRISA = ~(1<<i);
		PORTA = ~(1<<i);

		Delay(10);

		// check for other zero at PortA
		temp_porta 	= PORTA;
		temp_porta 	|= (~mask_port_a); 
		if((temp_porta) != (0xFF&(~(1<<i)))) {
			err_pin_a = i;
			Stop();
		} 

		// check for other zero at PortB	 
		temp_portb 	= PORTB;
		temp_portb 	|=(~mask_port_b); 
		if((temp_portb) != (0xFF)) {
			err_pin_b = i;
			Stop();
		} 
	
		// check for other zero at PortC
		temp_portc 	= PORTC;
		temp_portc 	|=(~mask_port_c); 
		if((temp_portc) != (0xFF)) {
			err_pin_c = i;
			Stop();
		} 

	}


	// all as input
	ADCON1 	= 0x0F;
	TRISA 	= 0xFF;
	TRISC 	= 0xFF;
	TRISB 	= 0xFF;

	// PortB
	//pull up - output
	TRISB &= ~BIT1;
	//pull up - high
	PORTB |= BIT1;

	Delay(10);

	for(i=0; i<8; i++) {
	
		// this port is not tested
		if(!((mask_port_b)&(1<<i))) continue;
	
	
		TRISB = ~((1<<i)|BIT1);
		PORTB = ~(1<<i);
		
		Delay(10);

		// check for other zero at PortA
		temp_porta 	= PORTA;
		temp_porta 	|= (~mask_port_a); 
		if((temp_porta) != (0xFF)) {
			err_pin_a = i;
			Stop();
		} 

		// check for other zero at PortB	 
		temp_portb 	= PORTB;
		temp_portb 	|=(~mask_port_b); 
		if((temp_portb) != (0xFF&(~(1<<i)))) {
			err_pin_b = i;
			Stop();
		} 
	
		// check for other zero at PortC
		temp_portc 	= PORTC;
		temp_portc 	|=(~mask_port_c); 
		if((temp_portc) != (0xFF)) {
			err_pin_c = i;
			Stop();
		} 
	}


	// all as input
	ADCON1 	= 0x0F;
	TRISA 	= 0xFF;
	TRISC 	= 0xFF;
	TRISB 	= 0xFF;


	// PortC
	//pull up - output
	TRISB &= ~BIT1;
	//pull up - high
	PORTB |= BIT1;

	Delay(10);

	for(i=0; i<8; i++) {
	
		// this port is not tested
		if(!((mask_port_c)&(1<<i))) continue;
	
		TRISC = ~(1<<i);
		PORTC = ~(1<<i);
		
		Delay(10);
	
		// check for other zero at PortA
		temp_porta 	= PORTA;
		temp_porta 	|= (~mask_port_a); 
		if((temp_porta) != (0xFF)) {
			err_pin_a = i;
			Stop();
		} 

		// check for other zero at PortB	 
		temp_portb 	= PORTB;
		temp_portb 	|=(~mask_port_b); 
		if((temp_portb) != (0xFF)) {
			err_pin_b = i;	
			Stop();
		} 
	
		// check for other zero at PortC
		temp_portc 	= PORTC;
		temp_portc 	|=(~mask_port_c); 
		if((temp_portc) != (0xFF&(~(1<<i)))) {
			err_pin_c = i;
			Stop();
		} 
 	
	}


	// Led Init
	InitLed();	

	// TEST EXTENSION OK :-)

	while(1) {
		LedBlinkOK();
	}

	// END TEST EXTENSION PORT

}




