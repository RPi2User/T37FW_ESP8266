#include "tty_types.h"

#ifndef TTY_H
#define TTY_H

#ifdef __cplusplus
extern "C" {
#endif

extern char str_main[STR_MAIN_SIZE];
extern symbol_t sbf_main[SBF_MAIN_SIZE];
extern sbf_t currentLine;

void TTY_Init(void);

void TTY_Fox(void);

void TTY_WriteKey(char key);
void TTY_WriteString(char* str, uint8_t keepStr);
sbf_t TTY_WriteBuffer(sbf_t buffer);

void TTY_WriteSymbol(symbol_t _sym);

char TTY_ReadKey();
symbol_t readSymbol();


void TTY_raiseMemoryError(void);

void setLoopback(uint8_t _loopback);
void setBaudrate(float baudrate);
void setTermWidth(uint8_t termwidth);
void setStopbits(float stopbit);

#ifdef __cplusplus
}
#endif

#endif // TTY_H
