#include "Arduino.h"
#include "Wire.h"

#include "tools.h"

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

#define LED_RT 9

int counter = 0;

void setup() {
  while (!Serial);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_RT, OUTPUT);
  blink(3);
  Serial.println("Hello.");
  Wire.begin();

  byte error;
  for (byte addr=1; addr<127; addr++) {
    Wire.beginTransmission(addr);
    error = Wire.endTransmission();
    if (!error) {
      Serial.printf("0x%02x\n", addr);
    }
  }
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
