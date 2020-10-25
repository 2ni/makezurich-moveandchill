#include "tools.h"
#include "Arduino.h"
#include "Serial.h"
#include "Wire.h"

void blink(byte amount) {
  for (uint8_t i=0; i<amount; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    if (i<amount) delay(50);
  }
}

void hello() {
  while (!Serial);
  blink(3);
  Serial.printf("\n\033[1;38;5;226;48;5;18m Hello from 0x%08lX%08lX \033[0m\n", NRF_FICR->DEVICEID[0], NRF_FICR->DEVICEID[1]);
}

void scan_i2c() {
  Serial.println("Scanning i2c bus...");
  Wire.begin();
  for (byte addr=1; addr<127; addr++) {
    Wire.beginTransmission(addr);
    if (!Wire.endTransmission()) {
      Serial.printf(" 0x%02x\n", addr);
    }
  }
  Serial.println(OK("Done."));
}
