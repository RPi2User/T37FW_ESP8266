#include "tty_types.h"

#ifndef TTY_H
#define TTY_H

#ifdef __cplusplus
extern "C" {
#endif


extern Teletype tty;

void TTY_Init(void);
void TTY_Fox(void);
void TTY_Loopback(void);
void TTY_WriteSymbol(symbol_t _sym);

// max_wait_bits: how many bit-periods to poll for a start-bit before giving
// up for this cycle. A timeout is the normal case (operator typed nothing)
// and simply returns without side effects.
void TTY_ReadKey(uint8_t max_wait_bits);
symbol_t readSymbol(uint8_t max_wait_bits);

#ifdef __cplusplus
}
#endif

#endif // TTY_H
