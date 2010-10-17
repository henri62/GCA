
/* *INDENT-OFF* */
/*Includes*/
#include <inttypes.h>

/*Defines*/

#define	XTAL            F_CPU
#define BAUD19200        19200
#define BAUDDIVIDER19200 (unsigned int) (XTAL / BAUD19200 / 16- 0.5)+1

#define SERIALOUTSIZE    500
#define SERIALINSIZE     1

/*Prototypes*/

void Serial_Init_19200_Baud(void);
void Serial_Enable_Transmit_Interrupts(void);
void Clear_Rec_And_Sent_Buf(void);
void Serial_Buffer_Sent_Fill_Block(uint8_t *s);
void Serial_Buffer_Sent_FillFlash(const char *s);

/* *INDENT-ON* */
