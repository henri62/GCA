
/**
 *******************************************************************************************
 * @file       ln_sw_uart_cfg.h
 * @ingroup    LoconetCfg
 * @defgroup   LoconetCfg LoconetCfg Loconet configuarion.
 * @author     Robert Evers
 * @attention  Header file for Loconet configuration.
 *******************************************************************************************
 */

#ifndef LN_SW_UART_CFG
#define LN_SW_UART_CFG

/*
 *******************************************************************************************
 * Standard include files
 *******************************************************************************************
 */

#include <avr/io.h>

/*
 *******************************************************************************************
 * Macro definitions
 *******************************************************************************************
 */

/* *INDENT-OFF* */

#define LN_RX_PORT               PINB
#define LN_RX_BIT                PB0

#define LN_SB_SIGNAL             SIG_INPUT_CAPTURE1
#define LN_SB_INT_ENABLE_REG     TIMSK
#define LN_SB_INT_ENABLE_BIT     TICIE1
#define LN_SB_INT_STATUS_REG     TIFR
#define LN_SB_INT_STATUS_BIT     ICF1

#define LN_TMR_SIGNAL            SIG_OUTPUT_COMPARE1A
#define LN_TMR_INT_ENABLE_REG    TIMSK
#define LN_TMR_INT_ENABLE_BIT    OCIE1A
#define LN_TMR_INT_STATUS_REG    TIFR
#define LN_TMR_INT_STATUS_BIT    OCF1A
#define LN_TMR_INP_CAPT_REG      ICR1
#define LN_TMR_OUTP_CAPT_REG     OCR1A
#define LN_TMR_COUNT_REG         TCNT1
#define LN_TMR_CONTROL_REG       TCCR1B

#define LN_TMR_PRESCALER         1

#define LN_TX_PORT               PORTB
#define LN_TX_DDR                DDRB
#define LN_TX_BIT                PB1

#define LN_TIMER_TX_RELOAD_ADJUST   106

/*
 *******************************************************************************************
 * Types
 *******************************************************************************************
 */

/*
 *******************************************************************************************
 * Variables
 *******************************************************************************************
 */

/* *INDENT-ON* */

 /* 
  *******************************************************************************************
  * Prototypes
  *******************************************************************************************
  */
#endif   /* LN_SW_UART_CFG */
