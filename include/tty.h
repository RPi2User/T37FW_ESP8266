#include "tty_types.h"

#ifndef TTY_H
#define TTY_H

#ifdef __cplusplus
extern "C" {
#endif


extern Teletype tty;

// Compiled-in defaults only, no EEPROM access. Enough to print the boot
// prompt; the stored object is applied later by TTY_LoadConfig().
void TTY_Init(void);

// Reads the Teletype object from EEPROM (restores and stores defaults if
// it is missing or from another version). Runtime state never comes from
// flash: mode is LTRS, carriage position 0.
void TTY_LoadConfig(void);

void TTY_Fox(void);
void TTY_Loopback(void);
void TTY_WriteSymbol(symbol_t _sym);

// Writes a SBF_TERMINATOR-terminated symbol buffer as-is. The caller is
// responsible for leaving the stream in LTRS.
void TTY_WriteSbf(const symbol_t* _sbf);

// max_wait_bits: how many bit-periods to poll for a start-bit before giving
// up for this cycle. A timeout is the normal case (operator typed nothing)
// and simply returns without side effects.
void TTY_ReadKey(uint8_t max_wait_bits);
symbol_t readSymbol(uint8_t max_wait_bits);

#ifdef __cplusplus
}
#endif

#endif // TTY_H