#include "tty_types.h"

#ifndef TTY_H
#define TTY_H

#ifdef __cplusplus
extern "C" {
#endif


void TTY_Init(void);
void TTY_Fox(void);
void TTY_SlowLoopback(void);
void TTY_WriteSymbol(symbol_t _sym);
void TTY_ReadKey();
symbol_t readSymbol();

#ifdef __cplusplus
}
#endif

#endif // TTY_H
