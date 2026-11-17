/*
 * booty.h
 *
 * BOOTY - boot prompt and (later) the mini-shell that edits the EEPROM
 * objects directly from the teletype keyboard.
 */
#include "tty_types.h"

#ifndef INC_BOOTY_H_
#define INC_BOOTY_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 Boot sequence, runs once between TTY_Init()/TERM_Init() and SM_Init():

    TTY:      CR FIGS ? LTRS SPACE
    <operator has BOOTY_TIMEOUT_MS to press any key>

    key:      FIGS BELL LTRS CR LF   -> BOOTY shell (stub for now)
    no key:   objects are read from EEPROM, CR LF

 Either way the objects are loaded from EEPROM afterwards and the data
 stream is left in LTRS, which is what the keyboard mechanics default to.
 */
void BOOTY_Boot(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_BOOTY_H_ */