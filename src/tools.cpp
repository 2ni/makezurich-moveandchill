#include "tools.h"
#include "Arduino.h"

void blink(byte amount) {
  for (uint8_t i=0; i<amount; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    if (i<amount) delay(50);
  }
}
