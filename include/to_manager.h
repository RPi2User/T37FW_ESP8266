#include <stdbool.h>
#include "tty_types.h"

#ifndef TO_MANAGER_H
#define TO_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

void TO_storeDefault(Teletype* t);
void TO_storeTTY_Object(const Teletype* t);
void TO_readTTY_Object(Teletype* t);

#ifdef __cplusplus
}
#endif

#endif // TO_MANAGER_H