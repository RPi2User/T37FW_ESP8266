#include <Arduino.h>

#include "terminal.h"
#include "tty.h"
#include "to_manager.h"

static Terminal term;
static uint8_t term_cts_ready = 0;	// mirrors the last state requested via TERM_setCTS()

const char TERM_LINETERM[4] = {'\r', '\n', '\r', '\n'};

static uint32_t TERM_bitDelay(void) {
	return (uint32_t)(1000000.0f / term.baudrate);
}

static void TERM_applyPins(void) {
	pinMode(term.SER_TX, OUTPUT);
	pinMode(term.SER_CTS, OUTPUT);
	pinMode(term.SER_RX, INPUT);

	digitalWrite(term.SER_TX, HIGH);
	TERM_setCTS(0);	// not ready until the automaton says otherwise
}

void TERM_Init(void) {
	// Defaults only - the EEPROM is not touched before the boot prompt
	// has timed out (see BOOTY_Boot).
	TO_defaultTerminal(&term);
	TERM_applyPins();
}

void TERM_LoadConfig(void) {
	TO_readTerminal_Object(&term);
	if (term.loadState != TO_VER1_0) {
		TO_defaultTerminal(&term);
		TO_storeTerminal_Object(&term);
	}
	TERM_applyPins();
}

void TERM_setCTS(uint8_t ready) {
	term_cts_ready = ready ? 1 : 0;
	digitalWrite(term.SER_CTS, term_cts_ready ? HIGH : LOW);
	digitalWrite(tty.TTY_READ_INHIBIT, term_cts_ready ? LOW : HIGH);	// INHIBIT = NOT-CTS
}

void TERM_write(uint8_t c) {
	uint32_t d = TERM_bitDelay();
	uint8_t was_ready = term_cts_ready;

	TERM_setCTS(0);	// we're driving TX ourselves, terminal must hold off
	digitalWrite(term.SER_TX, LOW);
	delayMicroseconds(d);

	for (uint8_t i = 0; i < 8; i++) {
		digitalWrite(term.SER_TX, (c >> i) & 1);
		delayMicroseconds(d);
	}

	digitalWrite(term.SER_TX, HIGH);
	delayMicroseconds(d * term.stopbits);
	TERM_setCTS(was_ready);	// restore whatever the automaton had requested
}

int16_t TERM_read(uint8_t max_wait_bits) {
	uint32_t d = TERM_bitDelay();

	for (uint8_t waited = 0; digitalRead(term.SER_RX); waited++){
		if (waited >= max_wait_bits) return -1;	// nothing sent - normal case
		delayMicroseconds(d);
	}

	delayMicroseconds(d + (d / 2));

	uint8_t c = 0;
	for (uint8_t i = 0; i < 8; i++) {
		c |= (digitalRead(term.SER_RX) ? 1 : 0) << i;
		delayMicroseconds(d);
	}

	delayMicroseconds(d);
	return (int16_t)c;
}

void TERM_sendHome() {
	TERM_sendLineTerm();
}

void TERM_sendLineTerm() {
    
	if (term.lineTerm == LT_LF || term.lineTerm == LT_NL) {
		TERM_write('\n');
		return;
	}

	TERM_write('\r');
	if (term.lineTerm == LT_CRLF) {
		TERM_write('\n');
	}
}