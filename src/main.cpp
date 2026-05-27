#include <Arduino.h>
#include <SoftwareSerial.h>
#include "tty.h"

SoftwareSerial softSerial(D6, D5, 1); // RX, TX

void setup() {
   pinMode(LED_BUILTIN, OUTPUT);
   //pinMode(A0, INPUT);  // -> RX
   //pinMode(D5, OUTPUT);

   softSerial.begin(9600);
   delay(100);
   softSerial.write('A');

   //TTY_Fox();
   //setLoopback(1);
   // put your setup code here, to run once:
}

void loop() { 
   if (softSerial.available()) {
      digitalWrite(LED_BUILTIN, LOW);
      int c = softSerial.read();
      softSerial.write(c);
   }
   else {
      delay(10);
      digitalWrite(LED_BUILTIN, LOW);
      delay(100);
      digitalWrite(LED_BUILTIN, HIGH);
   }
}
