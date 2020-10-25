#include <Arduino.h>
#include <Wire.h>
#include <Serial.h>
#include <Arduino_HTS221.h>  // humidity, temperature
#include <Arduino_LPS22HB.h> // barometric pressure
#include <Arduino_LSM9DS1.h> // 9-axis imu

#include "tools.h"

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif
#define SEAT 12

#define LED_RED 9
#define LED_YELLOW 10
#define LED_GREEN 11

int counter = 0;
unsigned long current_time;
bool is_occupied = false;

void setup() {
  hello();

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SEAT, INPUT);

  // sensirion ess
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_YELLOW, HIGH);

  scan_i2c();

  if (!BARO.begin()) {
    Serial.println(NOK("barometric sensor init failure"));
  }

  if (!HTS.begin()) {
    Serial.println(NOK("temperature/humidity sensor init failure"));
  }
}

void loop() {
  // read seat sensor
  is_occupied = digitalRead(SEAT) ? false : true;
  if (is_occupied) {
    digitalWrite(LED_BUILTIN, 1);
  } else {
    digitalWrite(LED_BUILTIN, 0);
  }

  // output some sensor data
  if ((millis()-current_time) > 1000) {
    current_time = millis();
    float temperature = HTS.readTemperature(); // °C
    float humidity = HTS.readHumidity(); // %
    float pressure = BARO.readPressure(); // kPa
    Serial.printf("(%u) temperature: %.1fC, humidity: %.1f%%, pressure: %.1f\n", counter++, temperature, humidity, pressure);
  }
}
