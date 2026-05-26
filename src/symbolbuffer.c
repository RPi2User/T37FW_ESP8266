#include <ctype.h>
#include <string.h>
#include "symbolbuffer.h"

uint32_t sbf_len(){
	uint32_t out = 0;
	while(sbf_main[out] != SBF_TERMINATOR) out++;
	return out;
}

void sbf_appendSym(symbol_t sym){
	uint32_t len = sbf_len(sbf_main);

	if (len >= (SBF_MAIN_SIZE - 1))
		len = 0;	// roll over to first position

	sbf_main[len] = sym;
	sbf_main[len + 1] = SBF_TERMINATOR;
}

/*	Convert: symbol to char
 * 	This conversion needs a lot of contextual data. Hence why the
 * 	function head is that cluttered.
 *
 * 	This function returns:
 *  - 2 when line-terminator should be appended
 *  - 1 when the char is successfully converted
 *  - 0 when it sym is a "dead key" like a ltrs or figs
 *  - -1 when a conversion error occurred
 *
 * char* target is a string with two chars.
 * it either contains the desired char on target[0] OR the specified Line termination (which can be up to 2 chars long)
 * hence the return code contains the length of *target
 */
int8_t sbf_convertToChar(symbol_t symbol, char* target, char* _newLine, tty_mode_t* current_mode, uint32_t* carriage_pos, uint32_t* last_lf){

	if (symbol == -1) {
		return -1;
	}
	if (symbol == lf) *last_lf = 0;
	else *last_lf = 1;

	// Handle common symbols
	switch(symbol){
		case cr: *carriage_pos = 0; *target = 0x0D; return 1;
		case space: (*carriage_pos)++; *target = ' '; return 1;
		case ltrs: *current_mode = TTY_LETTERS; return 0;
		case figs: *current_mode = TTY_FIGURES; return 0;
	}

	/*
	 * There are three valid cases for a new-line Insertion.
	 *  1. "CR LF"
	 *  2. "LF CR"
	 *  3. multiple "LF" (only works on carriage == 0)
	 */

	// case 1 "lfcr"
	if (*last_lf == 0 && symbol == cr){
		*carriage_pos = 0;
		*last_lf = 0;
		target = _newLine;		// 'return' new-Line Pointer
		return 2;
	}

	// case 2 "crlf" AND case 3 "crlflflflf..."
	if (*carriage_pos == 0 && symbol == lf){
		*carriage_pos = 0;
		*last_lf = 0;
		target = _newLine;		// 'return' new-Line Pointer
		return 2;
	}

	if (symbol == lf) return 0;

	if (*current_mode == TTY_LETTERS){
		*target = ltrs_to_char[symbol];
		(*carriage_pos)++;
		return 1;
	}
	else {
		*target = figs_to_char[symbol];
		(*carriage_pos)++;
		return 1;
	}

	return -1;	// If no-one returns there is an unpredictable error
}

void sbf_charToSymbolBuffer(char _c, tty_mode_t* _currentMode){
	// handle special chars
	if (_c <= 0x20){
		if (_c == 0x09){	// tab? -> append TAB_WIDTH-many spaces
			for (uint8_t t = 0; t < 4; t++)
				sbf_appendSym(space);
		}
		if(_c == 0x0A) sbf_appendSym(lf);	// \n? -> append LineFeed
		if(_c == 0x0C){	// FormFeed? -> Append 10 Lines
			sbf_appendSym(cr);
			for (uint8_t f = 0; f < 10; f++)
				sbf_appendSym(lf);
		}
		if(_c == 0x0D) sbf_appendSym(cr);	// \r? -> CarriageReturn
		if(_c == 0x20) sbf_appendSym(space);	// space
	}
	// Handle all !Letters and !special chars -> Handle all FIGS
	if ((_c > 0x20 && _c < 'A') || _c == 0x07 || _c > 'z'){
		if (*_currentMode != TTY_FIGURES){
			sbf_appendSym(figs);
			*_currentMode = TTY_FIGURES;
		}
		sbf_appendSym(char_to_symFIGS[(uint8_t)_c]);
	}
	// Handle all Letters
	if (_c >= 'A' && _c <= 'z'){
		if (*_currentMode != TTY_LETTERS){
			sbf_appendSym(ltrs);
			*_currentMode = TTY_LETTERS;
		}
		sbf_appendSym(char_to_symLTRS[(uint8_t)_c]);
	}
}

// --- AI Stuff following -------

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

void sbf_convertToSymbolBuffer(char* _inStr, sbf_t _out){
	tty_mode_t mode = TTY_LETTERS;

	sbf_main[0] = SBF_TERMINATOR;

	for (size_t i = 0; _inStr[i] != '\0'; i++) {
		sbf_charToSymbolBuffer(_inStr[i], &mode);
	}
}
