/*
 * stateMachine.h
 *
 *  Created on: Jul 12, 2026
 *      Author: florian
 */
#include "tty_types.h"

#ifndef INC_STATEMACHINE_H_
#define INC_STATEMACHINE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 This state machine executes each step in the following order, slowest
 interface first on the read side and fastest first on the write side:

    1. Listen
        1.1 READ_TTY  (~50 Bd, operator keystrokes)
        1.2 READ_SER  (9600 Bd, host terminal)
        1.3 READ_API  (reserved for future use)
    2. Process
        2.1 COMP      - moves data between the read/write side buffers
    3. Print
        3.1 WRITE_API (reserved for future use)
        3.2 WRITE_SER (reserved for future use)
        3.3 WRITE_TTY (~50 Bd, prints the queued backlog)

 SM_Run() advances exactly one state per call and is meant to be polled
 from the main loop. A timeout in any READ state is the normal case
 (nothing was sent) and simply falls through to the next state.
 */
typedef enum {
	SM_READ_TTY = 0,
	SM_READ_SER,
	SM_READ_API,

	SM_COMP,

	SM_WRITE_API,
	SM_WRITE_SER,
	SM_WRITE_TTY,

	SM_STATE_COUNT
} SM_State;

void SM_Init(void);
void SM_Run(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_STATEMACHINE_H_ */
