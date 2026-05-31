#include <stdio.h>
#include <stdlib.h>
#include "Arduino.h"
#include "tty.h"
#include "symbolbuffer.h"

Teletype tty;

// shared buffers
char str_main[STR_MAIN_SIZE];
symbol_t sbf_main[SBF_MAIN_SIZE];
const symbol_t SBF_TERMINATOR = -1;

// System variables
uint8_t READ_TIMEOUT = 100;	// Timeout of 1000ms

char* fox = "\r\nthe quick brown fox jumps over the lazy dog";

const symbol_t SBF_FOX[] = {
	cr, lf, ltrs, t, h, e, space, q, u, i, c, k, space,
	b, r, o, w, n, space, f, o, x, space, j, u, m, p, s, space,
	o, v, e, r, space, t, h, e, space, l, a, z, y, space,
	d, o, g, -1
};

const symbol_t SBF_DEFAULTS_LOADED[] = {
	cr, lf, figs, bell, ltrs, 
	d, e, f, a, u, l, t, space, s, e, t, t, i, n, g, s, 
	space, l, o, a, d, e, d, figs, period, cr, lf, ltrs,
	-1
};

const symbol_t SBF_MEM_ERROR[] = {
	cr, lf, ltrs, m, e, m, o, r, y, space, e, r, r, o, r, figs,
	comma, ltrs, space, r, e, s, e, t, t, i, n, g, space,
	c, p, u, figs, period, bell, bell, bell, bell, bell, -1
};

// --- private function definitions --------------------------------

uint8_t majority(Databit d);

void _print_defaults_restored();

void TTY_DelayMS(uint32_t ms);
void TTY_Delay(float cycles);
void TTY_Startbit();
void TTY_Stopbit();

void setTTY(uint8_t state);
symbol_t readTTY();

// -----------------------------------------------------------------
void TTY_Init(void){

	tty.TTY_SEND = D5;
	tty.TTY_RECV = D0;
	tty.TTY_READ_INHIBIT = D1;

    // Serial properties
	tty.baudrate = 50.0f;
	tty.stopbit_cnt = 1.5f;
	tty.loopback = LOOPBACK_NONE;

    // printing properties
	tty.linewidth = 69;
	tty.lettercase = LOWERCASE;
	tty.tty_mode = TTY_LETTERS;
	tty.tabWidth = 2;
	tty.last_linefeed = 0;
	tty.carriage_pos = 0;

    // Firmware check
	tty.loadState = TO_VER1_0;

	TTY_WriteSymbol(ltrs);
	TTY_Fox();
	TTY_SlowLoopback();
	return;
	// this loads the contents from flash and sets the variables
	TO_readTTY_Object(&tty);	// read default config from RAM
	if (tty.loadState != TO_VER1_0) {
		TO_storeDefault();
		_print_default_restored();
	}



}

// -----------------------------------------------------------------
// Debug functions
void TTY_Fox(void){
	for (uint8_t i = 0; SBF_FOX[i] != SBF_TERMINATOR; i++)
		TTY_WriteSymbol(SBF_FOX[i]);
}

void TTY_SlowLoopback(void){
	while(1)
		TTY_WriteSymbol(readSymbol());
}

// -----------------------------------------------------------------

void TTY_WriteSymbol(symbol_t _sym){
	if (_sym == -1) return;

	TTY_Startbit();

    for (uint8_t i = 0; i < 5; i++) {	// send data, bit by bit
        uint8_t bit = ((_sym >> i) & 0x01) ^ 1;
        setTTY(bit);
        TTY_Delay(1);
    }

	TTY_Stopbit();
}
// =================================================================

/*	==== READ OPERATIONS ===========================================
 *
 * This section contains all functions regarding reading Data from
 * the Teletype. I tried to keep this section clean but some stuff
 * is quite necessary for a stable "API" like infrastructure.
 *
 *	- TTY_ReadKey() is the main function, this reads a single Symbol
 *				    and converts it into a char
 */
void TTY_ReadKey(){
	// This one processes all symbols that can be processed without contextual data

	symbol_t sym = readSymbol();
	sbf_appendSym(sym);

	uint8_t editor_enabled = 0;

	if (sym == null || sym == SBF_TERMINATOR) 
		return; // discard illegal symbols


	if (sym == lf && editor_enabled != 0)
		editor_enabled = 0;	// close single line editor mode

	if (sym == space && editor_enabled != 0){
		// advace carriage by one known symbol
	}

	if (sym == ltrs){
		if (tty.tty_mode != TTY_LETTERS){
			tty.tty_mode = TTY_LETTERS;
			sbf_appendSym(sym);
		}
		return;	// else discard duplicate symbol
	}

	if (sym == figs){
		if (tty.tty_mode != TTY_FIGURES){
			tty.tty_mode = TTY_FIGURES;
			sbf_appendSym(sym);
		}
		return;
	}

	if (sym == cr){

		if (tty.carriage_pos != 0 && editor_enabled == 1)
			TTY_moveToPrev();	// third time cr -> move to prev

		if (tty.carriage_pos != 0){
			tty.carriage_pos = 0;
			sbf_appendSym(sym);
			return;
		}
		else {
			editor_enabled = 1;
			// first cr: do cr, second: enable edit mode (advance with spaces) third: goto sbf_main[-2]
			TERM_sendHome();
			TTY_moveToPrev();	// move carriage to the last typed symbol "xyCz-1" so C can overtype "z"
			return;
		}
	}

	sbf_advance(sym, &tty);
}

symbol_t readSymbol() {
	// TODO: Calc offsets from baudrate
	// wait for Symbol-Trigger
	while(1){
		if (readTTY() == 0) TTY_DelayMS(2);
		else break;
	}
	// read start-bit
	// pattern: 20ms startbit, 5x20ms Databit, 1.n stopbits
	// STARTBIT is 20ms HIGH
	TTY_DelayMS(10);	// Wait 10ms
	uint8_t beg = readTTY();
	TTY_DelayMS(15);	// Wait 5 + 10ms
	// NOTE: the additional delay (above) is the SAMPLING OFFSET!

	Databit databit[5];
	for (uint8_t i = 0; i < 5; i++){
		databit[i].s1 = readTTY();
		TTY_DelayMS(5); // 5ms Delay
		databit[i].s2 = readTTY();
		TTY_DelayMS(5); // 5ms Delay
		databit[i].s3 = readTTY();
		TTY_DelayMS(10); // 10ms delay
	}

	TTY_DelayMS(5);

	uint8_t end = readTTY();

	if ((beg + end) != 1)
		return SBF_TERMINATOR;
    
	symbol_t out = 0;

    if (majority(databit[0]) == 0) out += 1;
    if (majority(databit[1]) == 0) out += 2;
    if (majority(databit[2]) == 0) out += 4;
    if (majority(databit[3]) == 0) out += 8;
    if (majority(databit[4]) == 0) out += 16;

	if (tty.loopback == LOOPBACK_SLOW)
		TTY_WriteSymbol(out);

    return out;
}

// === System Functions ============================================
void setTTY(uint8_t state){
	digitalWrite(tty.TTY_SEND, 
		state ? HIGH : LOW);
}

int8_t readTTY(){
	int8_t out = -1;
	out =  digitalRead(tty.TTY_RECV) ?	// invert high/low for this chip
         HIGH : LOW;
	if (tty.loopback == LOOPBACK_NORM) setTTY(out);
	return out;
}

// === Private Functions ===========================================
uint8_t majority(Databit d) {
    return (d.s1 + d.s2 + d.s3) >= 2 ? 1 : 0;
}

void _print_defaults_restored(){
	for (uint8_t i = 0; SBF_DEFAULTS_LOADED[i] != SBF_TERMINATOR; i++)
		TTY_WriteSymbol(SBF_DEFAULTS_LOADED[i]);
}

// --- Timing ------------------------------------------------------
void TTY_DelayMS(uint32_t ms){
	unsigned long start = millis();
	while ((millis() - start) < ms) {
		yield();
	}
}

void TTY_Delay(float cycles){
	uint32_t delay_ms = (cycles * ( 1000 / tty.baudrate));
	TTY_DelayMS(delay_ms);
}

void TTY_Startbit(){
	setTTY(1);
	TTY_Delay(1.0);
}

void TTY_Stopbit(){
	setTTY(0);
	TTY_Delay(tty.stopbit_cnt);
}
