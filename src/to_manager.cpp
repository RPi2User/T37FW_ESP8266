#include <Arduino.h>
#include <EEPROM.h>

#include "terminal.h"
#include "to_manager.h"

// rename: Flash-Manager stores: TTY-Object, Terminal-Object & WiFi Settings

static const uint16_t FO_EEPROM_SIZE = sizeof(Teletype) + sizeof(Terminal);

void TO_defaultTTY(Teletype* t) {
    // PIN definitions
	t->TTY_SEND = D5;
	t->TTY_RECV = D0;
	t->TTY_READ_INHIBIT = D1;	// operator LED, driven as NOT-CTS

    // Serial properties
	t->baudrate = 50.0f;
	t->stopbit_cnt = 1.5f;
	t->loopback = LOOPBACK_NORM;

    // printing properties
	t->linewidth = 69;
	t->lettercase = LOWERCASE;
	t->tty_mode = TTY_LETTERS;
	t->tabWidth = 2;
	t->len_autoNewLineIndent = 1;
	t->last_linefeed = 0;
	t->carriage_pos = 0;

    // Firmware check
	t->loadState = TO_VER1_0;
}

void TO_defaultTerminal(Terminal* t) {
	t->SER_RX = D6;
	t->SER_TX = D7;
	t->SER_CTS = D8;
	t->_reserved = 0;
	t->lineTerm = LT_CRLF;
	t->baudrate = 9600;
	t->wordwidth = 8;
	t->parity = 0;
	t->stopbits = 1;
	t->loadState = TO_VER1_0;
}

void TO_storeDefault() {

	Teletype t;
	Terminal term;

	TO_defaultTTY(&t);
	TO_defaultTerminal(&term);

	TO_storeTTY_Object(&t);
	TO_storeTerminal_Object(&term);
}

void TO_storeTTY_Object(const Teletype* t) {
	EEPROM.begin(FO_EEPROM_SIZE);

	EEPROM.put(0, *t);
	EEPROM.commit();
	EEPROM.end();
}

void TO_readTTY_Object(Teletype* t) {
	EEPROM.begin(FO_EEPROM_SIZE);

	EEPROM.get(0, *t);
	EEPROM.end();
}

void TO_storeTerminal_Object(const Terminal* t) {
	EEPROM.begin(FO_EEPROM_SIZE);

	EEPROM.put(sizeof(Teletype), *t);	// offset begin by len(Teletype
	EEPROM.commit();
	EEPROM.end();
}

void TO_readTerminal_Object(Terminal* t) {
	EEPROM.begin(FO_EEPROM_SIZE);

	EEPROM.get(sizeof(Teletype), *t);
	EEPROM.end();
}