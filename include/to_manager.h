#include <stdbool.h>
#include "tty_types.h"
#include "terminal.h"

#ifndef TO_MANAGER_H
#define TO_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

// Compiled-in defaults - the single source for them. No EEPROM access.
void TO_defaultTTY(Teletype* t);
void TO_defaultTerminal(Terminal* t);

// Writes both default objects to EEPROM.
void TO_storeDefault();

void TO_storeTTY_Object(const Teletype* t);
void TO_readTTY_Object(Teletype* t);
void TO_storeTerminal_Object(const Terminal* t);
void TO_readTerminal_Object(Terminal* t);

#ifdef __cplusplus
}
#endif

#endif // TO_MANAGER_H