/*
 * symbolbuffer.h
 *
 *  Created on: Sep 28, 2025
 *      Author: florian
 */
#include "tty_types.h"

#ifndef INC_SYMBOLBUFFER_H_
#define INC_SYMBOLBUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

// --- Basic manipulation ------------------------------------------
uint32_t sbf_len(sbf_t sbf);
sbf_t sbf_appendSym(sbf_t head, symbol_t sym);
sbf_t sbf_concaternate(sbf_t head, sbf_t tail, uint8_t keepTail);

// --- CONVERT -----------------------------------------------------
symbol_t sbf_convertToChar(symbol_t symbol, char* target, char* _newLine, tty_mode_t* current_mode, uint32_t* carriage_pos, uint32_t* last_lf);
sbf_t sbf_charToSymbolBuffer(sbf_t _out, char _c, tty_mode_t* _currentMode);
char* sbf_convertToString(sbf_t _inSbf, char* _outStr, uint16_t outSize, char* _newLine);
sbf_t sbf_convertToSymbolBuffer(char* _inStr, sbf_t _out);

#ifdef __cplusplus
}
#endif

#endif /* INC_SYMBOLBUFFER_H_ */
