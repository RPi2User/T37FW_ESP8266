#include <Arduino.h>
#include "tty.h"

void setup() {
   pinMode(LED_BUILTIN, OUTPUT);
   pinMode(D5, OUTPUT);
   pinMode(D0, INPUT);
   TTY_Fox();
   setLoopback(1);
   // put your setup code here, to run once:
}

void loop() {
   TTY_ReadKey();

   delay(10);
}
