/*
 * terminal.h
 *
 *  Created on: Sep 28, 2025
 *      Author: florian
 */
#include "tty_types.h"

#ifndef INC_TERMINAL_H_
#define INC_TERMINAL_H_

#ifdef __cplusplus
extern "C" {
#endif

extern const char TERM_LINETERM[4];

typedef enum {
    LT_CRLF = 0,
    LT_LF = 1,
    LT_CR = 2,
    LT_NL = 3 
} LineTerm_T;


typedef struct {
    uint8_t SER_RX;
    uint8_t SER_TX;
    uint8_t SER_CTS;
    uint8_t _reserved;  // was the BOOTY pin (obsolete); keeps the EEPROM layout

    LineTerm_T lineTerm;
    uint32_t baudrate;
    uint8_t wordwidth;
    uint8_t parity;     // 0 = none, 1 = odd, 2 = even
    uint8_t stopbits;   // 1 or 2
    E_InitState loadState;
} Terminal;

// Compiled-in defaults only, no EEPROM access. Call after TTY_Init():
// TERM_setCTS() also drives the operator's INHIBIT LED on the TTY side.
void TERM_Init(void);

// Reads the Terminal object from EEPROM (stores defaults if it is missing
// or from another version).
void TERM_LoadConfig(void);

void TERM_write(uint8_t c);

// max_wait_bits: how many bit-periods to poll for a start-bit before giving
// up for this cycle. Returns -1 on timeout (normal case: nothing sent).
int16_t TERM_read(uint8_t max_wait_bits);

// Single point of control for the CTS line. ready=1 tells the terminal it
// may send; ready=0 tells it to hold off (e.g. we're still draining the
// print queue). TERM_write() temporarily pulls CTS low while transmitting
// and restores whatever TERM_setCTS() last requested.
//
// The operator's INHIBIT LED is NOT-CTS and is driven from here as well,
// nowhere else.
void TERM_setCTS(uint8_t ready);

void TERM_sendHome();

void TERM_sendLineTerm();

#ifdef __cplusplus
}
#endif

#endif /* INC_TERMINAL_H_ */