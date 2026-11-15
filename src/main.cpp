#include <Arduino.h>
#include <SoftwareSerial.h>
#include "tty.h"
#include "terminal.h"
#include "stateMachine.h"

#define TTY_RECV D0  //orange
#define TTY_SEND D5  // 
#define SER_RX D6
#define SER_TX D7
#define SER_CTS D8
#define TTY_INHIBIT D1
#define BOOTY D2

//SoftwareSerial softSerial(D6, D5, 1); // RX, TX


void setup() {
   pinMode(LED_BUILTIN, OUTPUT);
   pinMode(TTY_SEND, OUTPUT);
   pinMode(TTY_INHIBIT, OUTPUT);
   pinMode(SER_TX, OUTPUT);
   pinMode(SER_CTS, OUTPUT);

   pinMode(TTY_RECV, INPUT);
   pinMode(SER_RX, INPUT);
   pinMode(BOOTY, INPUT);

   TTY_Init();
   TERM_Init();
   SM_Init();

   //softSerial.begin(9600);
   //delay(100);
   //softSerial.write('A');

   //TTY_Fox();
   //setLoopback(1);
   // put your setup code here, to run once:
}

void loop() {
   SM_Run();
}
