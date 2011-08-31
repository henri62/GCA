#ifndef __UTILS_H
#define __UTILS_H

unsigned char eeRead(unsigned char addr);
void eeWrite(unsigned char addr, unsigned char data);
unsigned short eeReadShort(unsigned char addr);
void eeWriteShort(unsigned char addr, unsigned short data);
void delay(void);
void lDelay(void);


#endif
