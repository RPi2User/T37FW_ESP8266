#include "Arduino.h"
#include "stateMachine.h"
#include "tty.h"
#include "terminal.h"
#include "symbolbuffer.h"

// How many bit-periods a READ state polls for a start-bit before moving on.
// Deliberately small (a few bits, not a whole symbol) so the operator gets
// checked on almost every cycle without ever blocking the automaton for the
// duration of a full character.
#define SM_TTY_POLL_BITS	4
#define SM_SER_POLL_BITS	4

// Backlog of bytes read from the terminal, waiting to be printed on the TTY.
// The TTY (~50 Bd) is far slower than the terminal (9600 Bd), so this queue
// is what actually needs backpressure - CTS follows its fill level, not the
// read/write window timing.
#define SM_PQ_SIZE		STR_MAIN_SIZE
#define SM_PQ_HIGH_WATER	(SM_PQ_SIZE * 3 / 4)

static char sm_print_queue[SM_PQ_SIZE];
static uint16_t sm_pq_head = 0;	// next byte to print
static uint16_t sm_pq_tail = 0;	// next free slot

static SM_State sm_state = SM_READ_TTY;

static uint16_t sm_pq_count(void) {
	return (sm_pq_tail - sm_pq_head + SM_PQ_SIZE) % SM_PQ_SIZE;
}

static uint8_t sm_pq_push(char c) {
	uint16_t next = (sm_pq_tail + 1) % SM_PQ_SIZE;
	if (next == sm_pq_head) return 0;	// full, drop
	sm_print_queue[sm_pq_tail] = c;
	sm_pq_tail = next;
	return 1;
}

static int16_t sm_pq_pop(void) {
	if (sm_pq_head == sm_pq_tail) return -1;	// empty
	char c = sm_print_queue[sm_pq_head];
	sm_pq_head = (sm_pq_head + 1) % SM_PQ_SIZE;
	return (uint8_t)c;
}

// --- READ ----------------------------------------------------------------

static void sm_state_readTTY(void) {
	TTY_ReadKey(SM_TTY_POLL_BITS);			// timeout -> no-op, that's fine
}

static void sm_state_readSer(void) {
	if (sm_pq_count() >= SM_PQ_HIGH_WATER) {
		// Still draining the print backlog - hold the terminal off
		// regardless of whose turn this cycle would otherwise be.
		TERM_setCTS(0);
		return;
	}

	TERM_setCTS(1);
	int16_t c = TERM_read(SM_SER_POLL_BITS);
	if (c >= 0)
		sm_pq_push((char)c);
}

static void sm_state_readAPI(void) {
	// reserved for future use
}

// --- PROCESS ---------------------------------------------------------------

static void sm_state_comp(void) {
	// Placeholder for cross-buffer bookkeeping. Both TTY keystrokes and
	// terminal bytes already land in their respective buffers as part of
	// the READ states; nothing further to reconcile yet.
}

// --- WRITE -----------------------------------------------------------------

static void sm_state_writeAPI(void) {
	// reserved for future use
}

static void sm_state_writeSer(void) {
	// reserved for future use
}

static void sm_state_writeTTY(void) {
	if (sm_pq_count() == 0) return;	// nothing queued - normal case

	// The INHIBIT LED is NOT-CTS and handled by TERM_setCTS() only.

	int16_t c = sm_pq_pop();
	if (c >= 0) {
		uint32_t before = sbf_len();
		sbf_charToSymbolBuffer((char)c, &tty.tty_mode);
		uint32_t after = sbf_len();
		for (uint32_t i = before; i < after; i++)
			TTY_WriteSymbol(sbf_main[i]);
	}

	// Backlog drained: always leave the data stream in LTRS - that is
	// the state the keyboard mechanics are in.
	if (sm_pq_count() == 0 && tty.tty_mode != TTY_LETTERS) {
		TTY_WriteSymbol(ltrs);
		tty.tty_mode = TTY_LETTERS;
	}
}

// ---------------------------------------------------------------------------

void SM_Init(void) {
	sm_pq_head = 0;
	sm_pq_tail = 0;
	sm_state = SM_READ_TTY;
	TERM_setCTS(0);
}

void SM_Run(void) {
	switch (sm_state) {
		case SM_READ_TTY:	sm_state_readTTY();	break;
		case SM_READ_SER:	sm_state_readSer();	break;
		case SM_READ_API:	sm_state_readAPI();	break;
		case SM_COMP:		sm_state_comp();	break;
		case SM_WRITE_API:	sm_state_writeAPI();	break;
		case SM_WRITE_SER:	sm_state_writeSer();	break;
		case SM_WRITE_TTY:	sm_state_writeTTY();	break;
		default: break;
	}

	sm_state = (SM_State)((sm_state + 1) % SM_STATE_COUNT);
}