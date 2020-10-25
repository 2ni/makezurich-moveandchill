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
unsigned long seating_start_time = 0;
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
  Serial.println("Waiting to be seated...");
}

void loop() {
  is_occupied = digitalRead(SEAT) ? false : true;

  // someone takes a seat
  if (is_occupied && !seating_start_time) {
    seating_start_time = millis();
    digitalWrite(LED_BUILTIN, 1);
    Serial.println("Seat taken");
  }

  // someone leaves the seat
  if (!is_occupied && seating_start_time) {
    digitalWrite(LED_BUILTIN, 0);
    Serial.printf("Seat freed\n duration: %lu seconds\n", (millis()-seating_start_time)/1000);
    seating_start_time = 0;
    float temperature = HTS.readTemperature(); // °C
    float humidity = HTS.readHumidity(); // %
    float pressure = BARO.readPressure(); // kPa
    Serial.printf(" temperature: %.1fC\n humidity: %.1f%%\n pressure: %.1f\n", temperature, humidity, pressure);
  }
}
