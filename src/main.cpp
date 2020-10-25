#include "Arduino.h"
#include "Wire.h"
#include "Serial.h"

#include "tools.h"

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

#define LED_RT 9

int counter = 0;

void setup() {
  hello();

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_RT, OUTPUT);

  scan_i2c();
}

void loop() {
  Serial.printf("Loop: %u\n", counter++);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(LED_RT, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(LED_RT, LOW);
  delay(1000);
}
