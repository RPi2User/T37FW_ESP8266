#include <Arduino.h>
#include <EEPROM.h>

#include "to_manager.h"

void TO_storeDefault(Teletype* t) {
	if (t == NULL) {
		return;
	}

    // PIN definitions
	t->TTY_SEND = D5;
	t->TTY_RECV = D0;
	t->TTY_READ_INHIBIT = A0;

    // Serial properties
	t->baudrate = 45.45f;
	t->stopbit_cnt = 1.5f;

    // printing properties
	t->linewidth = 69;
	t->lettercase = LOWERCASE;
	t->tabWidth = 2;

    // Firmware check
	t->loadState = TO_VER1_0;
}

void TO_storeTTY_Object(const Teletype* t) {
	EEPROM.begin(sizeof(Teletype));

	EEPROM.put(0, *t);
	EEPROM.commit();
	EEPROM.end();
}

void TO_readTTY_Object(Teletype* t) {
	EEPROM.begin(sizeof(Teletype));

	EEPROM.get(0, *t);
	EEPROM.end();
}