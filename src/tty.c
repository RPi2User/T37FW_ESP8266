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

	TO_readTTY_Object(&tty);	// read default config from RAM
	if (tty.loadState != TO_VER1_0) {
		TO_storeDefault();
		_print_default_restored();
	}

	// this loads the contents from flash and sets the variables
	for (uint16_t i = 0 ; i < SBF_MAIN_SIZE; i++){
		sbf_main[i] = SBF_TERMINATOR;
		if (i < STR_MAIN_SIZE) str_main[i] = '\0';
	}

	TTY_WriteSymbol(ltrs);
	
}

// -----------------------------------------------------------------
// Debug function prints a brown fox
void TTY_Fox(void){

	TTY_WriteString(fox, 1);

}

// === WRITE SECTION ===============================================
void TTY_WriteString(char* str, uint8_t keepStr){
    // MAIN WRITE FUNCTION
    // keepStr is used when you don't want to clear a constant
	(void)keepStr;
	sbf_convertToSymbolBuffer(str, sbf_main);
	sbf_convertToString(sbf_main, str_main, STR_MAIN_SIZE, "\r\n");
	TTY_WriteBuffer(sbf_main);
}

void TTY_WriteKey(char key){
	sbf_charToSymbolBuffer(key, &tty.tty_mode);
	TTY_WriteBuffer(sbf_main);
}

sbf_t TTY_WriteBuffer(sbf_t buffer){
	if (buffer == NULL) {
		return NULL;
	}

    // Write all symbols in buffer
	for (uint16_t i = 0; buffer[i] != SBF_TERMINATOR; i++) {
        TTY_WriteSymbol(buffer[i]);
    }

	return buffer;
}

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
char TTY_ReadKey(){

	int sym = readSymbol();
	char _out = -1;
	sbf_convertToChar(sym, &_out, "\r\n",
		&tty.tty_mode, &tty.carriage_pos, &tty.last_linefeed);	// make that one accept the tty struct

	return _out;
}

symbol_t readSymbol() {
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

	// Eval Bits
	if (beg == 0 || end != 0){
		setReadError();
		return -1;
	}

    uint8_t out = 0;

    if (majority(databit[0]) == 0) out += 1;
    if (majority(databit[1]) == 0) out += 2;
    if (majority(databit[2]) == 0) out += 4;
    if (majority(databit[3]) == 0) out += 8;
    if (majority(databit[4]) == 0) out += 16;

    clearReadError();

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

void setLoopback(loopback_mode_t _loopback) {tty.loopback = _loopback;}
void setBaudrate(float baudrate) {tty.baudrate = baudrate;}
void setTermWidth(uint8_t termwidth) {tty.linewidth = termwidth;}
void setStopbits(float stopbit) {tty.stopbit_cnt = stopbit;}
// TODO add Lower/Uppercase "switch"


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
