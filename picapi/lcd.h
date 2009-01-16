#ifndef PICAPI_LCD_H
#define PICAPI_LCD_H

#include "types.h"

struct output {
  volatile byte* port;
  byte pin;
};

struct lcdInf {
  struct output data;
  struct output rs;
  struct output enable;
};


void lcdClear(struct lcdInf* lcd);
void lcdCursor(struct lcdInf* lcd, bool enable);
void lcdHome(struct lcdInf* lcd, byte line);
void lcdStr(struct lcdInf* lcd, byte line, char* str);

#endif
