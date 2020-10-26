/*
 * use SLEEP_MODE_IDLE when seat in use else deep sleep
 * wake up on SEAT change attachInterrupt(digitalPinToInterrupt(pin), ISR, mode)
 * mode: CHANGE, RISING, FALLING
 *
 * IMU.end();
 * BLE.end();
 * digitalWrite(PIN_ENABLE_SENSORS_3V3, LOW);
 * digitalWrite(PIN_ENABLE_I2C_PULLUP, LOW);
 *
 *
 *  proximity:
 *  if (!APDS.begin()) {
 *    Serial.println("Error initializing APDS9960 sensor!");
 *  }
 *  if (APDS.proximityAvailable()) {
 *    // read the proximity
 *    // - 0   => close
 *    // - 255 => far
 *    // - -1  => error
 *    int proximity = APDS.readProximity();
 *  }
 */

#include <Arduino.h>
#include <Wire.h>
#include <Serial.h>
#include <Arduino_HTS221.h>  // humidity, temperature
#include <Arduino_LPS22HB.h> // barometric pressure
#include <Arduino_LSM9DS1.h> // 9-axis imu
#include <Arduino_APDS9960.h> // light

#include "tools.h"

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif
#define SEAT 12

#define LED_RED 9
#define LED_YELLOW 10
#define LED_GREEN 11

unsigned long current_time;
unsigned long seating_start_time = 0;
volatile bool is_occupied = false;

void ISR_seat_change() {
  is_occupied = !digitalRead(SEAT);
}

void setup() {
  hello();

  digitalWrite(LED_PWR, LOW); // turn off power led

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SEAT, INPUT);
  attachInterrupt(digitalPinToInterrupt(SEAT), ISR_seat_change, CHANGE);

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
  /*
  if ((millis()-current_time) > 1000) {
    Serial.printf("is_occupied: %u\n", is_occupied);
    current_time = millis();
  }
  */

  // someone takes a seat
  if (is_occupied && !seating_start_time) {
    seating_start_time = millis();
    digitalWrite(LED_BUILTIN, 1);
    Serial.println(NOK("Seat taken"));
    low_power();

  }

  // someone leaves the seat
  if (!is_occupied && seating_start_time) {
    digitalWrite(LED_BUILTIN, 0);
    Serial.printf(OK("Seat freed") "\n duration: %lu seconds\n", (millis()-seating_start_time)/1000);
    seating_start_time = 0;
    float temperature = HTS.readTemperature(); // °C
    float humidity = HTS.readHumidity(); // %
    float pressure = BARO.readPressure(); // kPa
    Serial.printf(" temperature: %.1fC\n humidity: %.1f%%\n pressure: %.1f\n", temperature, humidity, pressure);
    low_power();
  }
}
