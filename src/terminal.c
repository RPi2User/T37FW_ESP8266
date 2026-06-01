#include <Arduino.h>

#include "terminal.h"
#include "to_manager.h"

static Terminal term;

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

	pinMode(term.SER_RX, INPUT);
	pinMode(term.SER_TX, OUTPUT);
	pinMode(term.SER_CTS, OUTPUT);
	pinMode(term.BOOTY, INPUT);
	digitalWrite(term.SER_TX, HIGH);
	digitalWrite(term.SER_CTS, HIGH);
}

void TERM_write(uint8_t c) {
	uint32_t d = TERM_bitDelay();

	digitalWrite(term.SER_CTS, LOW);
	digitalWrite(term.SER_TX, LOW);
	delayMicroseconds(d);

	for (uint8_t i = 0; i < 8; i++) {
		digitalWrite(term.SER_TX, (c >> i) & 1);
		delayMicroseconds(d);
	}

	digitalWrite(term.SER_TX, HIGH);
	delayMicroseconds(d * term.stopbits);
	digitalWrite(term.SER_CTS, HIGH);
}

int8_t TERM_read(void) {
	uint32_t d = TERM_bitDelay();
	while (digitalRead(term.SER_RX)) {}

	delayMicroseconds(d + (d / 2));

	uint8_t c = 0;
	for (uint8_t i = 0; i < 8; i++) {
		c |= (digitalRead(term.SER_RX) ? 1 : 0) << i;
		delayMicroseconds(d);
	}

	delayMicroseconds(d);
	return (int8_t)c;
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