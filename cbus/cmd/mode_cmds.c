//
// CANCMD Programmer/Command Station (C) 2009 SPROG DCC
//	web:	http://www.sprog-dcc.co.uk
//	e-mail:	sprog@sprog-dcc.co.uk
//

#include "project.h"

#pragma code APP

//
// cmd_rmode()
//
// Read mode word and current limit from EEPROM
//
void cmd_rmode(void) {
  mode_word.byte = ee_read(EE_MW);
  imax = ee_read(EE_IMAX);
}

//
// cmd_wmode()
//
// Write mode word and current limit to EEPROM
//
void cmd_wmode(void) {
  ee_write(EE_IMAX, imax);
  ee_write(EE_MW, mode_word.byte);
  ee_write(EE_MAGIC, MAGIC);
}

