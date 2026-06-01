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
    uint8_t BOOTY;

    LineTerm_T lineTerm;
    uint32_t baudrate;
    uint8_t wordwidth;
    uint8_t parity;     // 0 = none, 1 = odd, 2 = even
    uint8_t stopbits;   // 1 or 2
    E_InitState loadState;
} Terminal;

void TERM_Init(void);

void TERM_write(uint8_t c);
int8_t TERM_read(void);

void TERM_sendHome();

void TERM_sendLineTerm();

#ifdef __cplusplus
}
#endif

#endif /* INC_TERMINAL_H_ */
