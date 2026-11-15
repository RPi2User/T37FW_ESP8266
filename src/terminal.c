#include <Arduino.h>

#include "terminal.h"
#include "to_manager.h"

static Terminal term;
static uint8_t term_cts_ready = 0;	// mirrors the last state requested via TERM_setCTS()

const char TERM_LINETERM[4] = {'\r', '\n', '\r', '\n'};

static uint32_t TERM_bitDelay(void) {
	return (uint32_t)(1000000.0f / term.baudrate);
}

void TERM_Init(void) {
	TO_readTerminal_Object(&term);
	if (term.loadState != TO_VER1_0) {
		term.SER_RX = D6;
		term.SER_TX = D7;
		term.SER_CTS = D8;
		term.BOOTY = D2;
		term.lineTerm = LT_CRLF;
		term.baudrate = 9600.0f;
		term.wordwidth = 8;
		term.parity = 0;
		term.stopbits = 1;
		term.loadState = TO_VER1_0;
		TO_storeTerminal_Object(&term);
	}

	digitalWrite(term.SER_TX, HIGH);
	TERM_setCTS(0);	// not ready until the automaton says otherwise
}

void TERM_setCTS(uint8_t ready) {
	term_cts_ready = ready ? 1 : 0;
	digitalWrite(term.SER_CTS, term_cts_ready ? HIGH : LOW);
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