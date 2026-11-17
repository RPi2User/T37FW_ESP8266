#include <Arduino.h>
#include "tty.h"
#include "terminal.h"
#include "booty.h"
#include "stateMachine.h"

// Pin assignment lives in the Teletype/Terminal objects (defaults in
// to_manager.cpp); each module sets up its own pins.
//
//   TTY_RECV    D0  (orange)
//   TTY_INHIBIT D1  (operator LED, NOT-CTS)
//   TTY_SEND    D5
//   SER_RX      D6
//   SER_TX      D7
//   SER_CTS     D8

void setup() {
   pinMode(LED_BUILTIN, OUTPUT);

   TTY_Init();    // defaults, line idle
   TERM_Init();   // defaults, CTS low (must come after TTY_Init)
   BOOTY_Boot();  // prompt, 5s window, EEPROM objects, RDY
   SM_Init();
}

void loop() {
   SM_Run();
}