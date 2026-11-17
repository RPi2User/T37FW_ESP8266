#include "Arduino.h"
#include "booty.h"
#include "tty.h"
#include "terminal.h"

#define BOOTY_TIMEOUT_MS	5000
#define BOOTY_POLL_BITS		4	// start-bit poll window per readSymbol() call

static const symbol_t SBF_BOOT_PROMPT[] = { cr, figs, question, ltrs, space, -1 };
static const symbol_t SBF_BOOTY_ACK[]   = { figs, bell, ltrs, cr, lf, -1 };
static const symbol_t SBF_READY[]       = { cr, lf, -1 };

// Returns 1 as soon as the operator sends any complete symbol, 0 after
// BOOTY_TIMEOUT_MS without one. The symbol itself is discarded.
static uint8_t booty_keyPressed(void) {
	uint32_t start = millis();
	while ((millis() - start) < BOOTY_TIMEOUT_MS) {
		if (readSymbol(BOOTY_POLL_BITS) != SBF_TERMINATOR)
			return 1;
	}
	return 0;
}

// The interactive shell goes here. For the first build it only
// acknowledges that it was entered.
static void booty_shell(void) {
	TTY_WriteSbf(SBF_BOOTY_ACK);
}

void BOOTY_Boot(void) {
	TTY_WriteSbf(SBF_BOOT_PROMPT);

	uint8_t enter_booty = booty_keyPressed();

	if (enter_booty)
		booty_shell();

	// Objects are read only now, so whatever BOOTY changed is picked up.
	TTY_LoadConfig();
	TERM_LoadConfig();

	if (!enter_booty)
		TTY_WriteSbf(SBF_READY);	// terminal is RDY
}