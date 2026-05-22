#include <ctype.h>
#include <string.h>
#include "symbolbuffer.h"

void TTY_raiseMemoryError(void);

uint32_t sbf_len(sbf_t sbf){
	uint32_t len = 0;
	if (sbf == NULL) return 0;
	while (sbf[len] != SBF_TERMINATOR) {
		len++;
	}
	return len;
}

sbf_t sbf_appendSym(sbf_t head, symbol_t sym){
	if (head == NULL) {
		return NULL;
	}

	uint32_t len = sbf_len(head);
	if (len >= (SBF_MAIN_SIZE - 1)) {
		return head;
	}

	head[len] = sym;
	head[len + 1] = SBF_TERMINATOR;
	return head;
}

sbf_t sbf_concaternate(sbf_t head, sbf_t tail, uint8_t keepTail){
	if (head == NULL) {
		return NULL;
	}

	uint32_t head_len = sbf_len(head);
	uint32_t tail_len = sbf_len(tail);
	uint32_t free_slots = (SBF_MAIN_SIZE - 1) - head_len;
	if (tail_len > free_slots) {
		tail_len = free_slots;
	}
	memcpy(&head[head_len], tail, tail_len * sizeof(symbol_t));
	head[head_len + tail_len] = SBF_TERMINATOR;
	if (keepTail == 0) {
	}
	return head;
}

symbol_t sbf_convertToChar(symbol_t symbol, char* target, char* _newLine, tty_mode_t* current_mode, uint32_t* carriage_pos, uint32_t* last_lf){
	(void)_newLine;
	(void)carriage_pos;
	(void)last_lf;

	if (target == NULL) {
		return symbol;
	}

	tty_mode_t mode = (current_mode != NULL) ? *current_mode : TTY_LETTERS;

	if (symbol == cr) {
		*target = '\r';
		return symbol;
	}

	if (symbol == lf) {
		*target = '\n';
		return symbol;
	}

	if (mode == TTY_LETTERS && symbol >= 0 && symbol < 32) {
		*target = ltrs_to_char[(uint8_t)symbol];
		return symbol;
	}

	if (mode == TTY_FIGURES && symbol >= 0 && symbol < 32) {
		*target = figs_to_char[(uint8_t)symbol];
		return symbol;
	}

	*target = (char)symbol;
	return symbol;
}

sbf_t sbf_charToSymbolBuffer(sbf_t _out, char _c, tty_mode_t* _currentMode){
	symbol_t sym = SBF_TERMINATOR;
	unsigned char uc = (unsigned char)_c;

	if (_currentMode == NULL) {
		return _out;
	}

	if (_c == '\r') {
		sym = cr;
	} else if (_c == '\n') {
		sym = lf;
	} else if (*_currentMode == TTY_FIGURES) {
		sym = char_to_symFIGS[uc];
	} else {
		sym = char_to_symLTRS[(unsigned char)toupper(uc)];
		if (sym == 0) {
			sym = char_to_symCommon[uc];
		}
	}

	if (sym == 0) {
		sym = char_to_symCommon[uc];
	}

	return sbf_appendSym(_out, sym);
}

char* sbf_convertToString(sbf_t _inSbf, char* _outStr, uint16_t outSize, char* _newLine){
	if (_outStr == NULL || outSize == 0) {
		return NULL;
	}

	uint32_t len = sbf_len(_inSbf);
	if (len >= outSize) {
		len = outSize - 1;
	}
	for (uint32_t i = 0; i < len; i++) {
		char c = 0;
		sbf_convertToChar(_inSbf[i], &c, _newLine, NULL, NULL, NULL);
		_outStr[i] = c;
	}
	_outStr[len] = '\0';
	return _outStr;
}

sbf_t sbf_convertToSymbolBuffer(char* _inStr, sbf_t _out){
	tty_mode_t mode = TTY_LETTERS;
	if (_out == NULL) {
		return NULL;
	}
	_out[0] = SBF_TERMINATOR;
	if (_inStr == NULL) {
		return _out;
	}
	for (size_t i = 0; _inStr[i] != '\0'; i++) {
		_out = sbf_charToSymbolBuffer(_out, _inStr[i], &mode);
	}
	return _out;
}
