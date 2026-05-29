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

const char LINETERM[4] = {'\r', '\n', '\r', 0x15};

typedef enum {
    LT_CRLF = 0,
    LT_LF = 1,
    LT_CR = 2,
    LT_NL = 3 
} LineTerm_T;


typedef struct {
    LineTerm_T lineTerm;
    uint32_t baudrate;
    uint8_t wordwidth;
    uint8_t parity;     // 0 = none, 1 = odd, 2 = even
    uint8_t stopbits;   // 1 or 2
} Terminal;

#ifdef __cplusplus
}
#endif

#endif /* INC_TERMINAL_H_ */
