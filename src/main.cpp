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
#include "loramodem.h"

#define SEAT 12

unsigned long current_time;
unsigned long seating_start_time = 0;
volatile bool is_occupied = false;

LoRaWANModem modem;

void ISR_seat_change() {
  is_occupied = !digitalRead(SEAT);
}

void setup() {
  hello();

  digitalWrite(LED_PWR, LOW); // turn off power led

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SEAT, INPUT);
  attachInterrupt(digitalPinToInterrupt(SEAT), ISR_seat_change, CHANGE);

  if (!BARO.begin()) {
    Serial.println(NOK("barometric sensor init failure"));
  }

  if (!HTS.begin()) {
    Serial.println(NOK("temperature/humidity sensor init failure"));
  }
  Serial.println("Waiting to be seated...");

  const uint8_t appeui[8] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x03, 0x71, 0x46 };
  const uint8_t appkey[16] = { 0x0D, 0x48, 0x3C, 0xF3, 0xAF, 0x9D, 0xE1, 0x4D, 0x86, 0x7F, 0xD0, 0x62, 0x18, 0xD3, 0xB6, 0xEE };
  uint8_t payload[3] = { 0x12, 0x13, 0x14 };
  modem.begin();

  /*
  modem.cmd_and_result("set app eui", CMD_SETJOINEUI, appeui, 8);
  modem.cmd_and_result("set app key", CMD_SETNWKKEY, appkey, 16);
  modem.info();
  modem.write(CMD_SETJOINEUI, appeui, 8);
  modem.write(CMD_SETNWKKEY, appkey, 16);
  Serial.println("done.");
  return;
  */

  modem.cmd_and_result("leave", CMD_LEAVENETWORK);

  modem.join(appeui, appkey);
  current_time = millis();
  while (modem.is_joining()) {
    if ((millis()-current_time) > 1000) {
      current_time = millis();
      Serial.println("waiting ...");
    }
  }

  /*
  bool joining = true;
  while (joining) {
    if ((millis()-current_time) > 1000) {
      uint8_t response_len;
      uint8_t response[5] = {0};
      Status s = modem.command(CMD_GETEVENT, response, &response_len);
      joining = response[0] != EVT_JOINED || s == OK ? true : false;
      current_time = millis();
      Serial.printf("waiting...status: 0x%02x, response: 0x%02x\n", (uint8_t)s, response[0]);
      modem.print_arr(response, response_len);
    }
  }
  */

  modem.send(payload, 3);

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
