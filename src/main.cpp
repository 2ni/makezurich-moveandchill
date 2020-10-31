/*
 * use SLEEP_MODE_IDLE when seat in use else deep sleep
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
#include <Arduino_APDS9960.h> // light, proximity

#include "tools.h"
#include "loramodem.h"
#include "keys.h"

#define PIN_FLEXBAND 12
#define PIN_IR 11
#define PIN_LIGHT A0

#define DELAY_SEATING 200 // delay between checking for seat occupancy
#define DELAY_SAMPLE 1000 // delay betwenn getting samples if seat is occupied
unsigned long timekeeper_seating;
unsigned long timekeeper_sample;


unsigned long seating_start_time = 0;
volatile bool is_occupied_flexband = false;
volatile bool is_occupied_ir = false;
bool is_occupied_proximity = false;
bool is_occupied = false;

typedef struct {
  uint8_t temperature;
  uint8_t light;
} Sample;


#define MINIUM_DURATION 1
#define MAX_SAMPLES 4
// #define MAX_SAMPLES 22 // (total(51) - location(4) - duration(2)) / (temp(1) + light(1))
uint16_t sample_count = 0;
Sample samples[MAX_SAMPLES] = {0};

LoRaWANModem modem;
Status join_status;

void ISR_seat_change_flex() {
  is_occupied_flexband = !digitalRead(PIN_FLEXBAND);
}

void ISR_seat_change_ir() {
  is_occupied_ir = !digitalRead(PIN_IR);
}

/*
 * add sample to the buffer
 * if the buffer is full, samples are merged and the new one is added at the last position
 */
void buffer_sample(Sample sample, Sample *samples, uint8_t sample_count) {
  // Serial.printf("adding sample. sample_count=%u, temp=%u light=%u\n", sample_count, sample.temperature, sample.light);

  uint8_t position = sample_count;

  // max buffer sample_count reached -> merge first
  if (sample_count >= MAX_SAMPLES) {
    position = MAX_SAMPLES - 1;
    for (uint8_t i=0; i<(MAX_SAMPLES-1); i++) {
      uint16_t t = (samples[i].temperature + samples[i+1].temperature) / 2;
      samples[i].temperature = t;

      t = (samples[i].light + samples[i+1].light) / 2;
      samples[i].light = t;
    }
  }

  samples[position].temperature = sample.temperature;
  samples[position].light = sample.light;
}

void print_buffer(Sample *samples) {
  for (uint8_t i=0; i<MAX_SAMPLES; i++) {
    Serial.printf("t:%u l:%u | ", samples[i].temperature, samples[i].light);
  }
  Serial.println();
}

void setup() {
  hello();

  digitalWrite(LED_PWR, LOW); // turn off power led

  modem.begin();
  modem.info();

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_IR, INPUT);
  pinMode(PIN_FLEXBAND, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_FLEXBAND), ISR_seat_change_flex, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_IR), ISR_seat_change_ir, CHANGE);

  if (!BARO.begin()) Serial.println(NOK("barometric sensor failure"));
  if (!HTS.begin()) Serial.println(NOK("temperature/humidity sensor failure"));
  if (!APDS.begin()) Serial.println(NOK("light/proximity sensor failure"));

  // modem.cmd_and_result("leave", CMD_LEAVENETWORK);
  join_status = modem.join(appeui, appkey);

  Serial.println("Waiting to be seated...");
}

void loop() {
  if ((millis()-timekeeper_seating) > DELAY_SEATING) {
    timekeeper_seating = millis();
    // pull proximity sensor
    if (APDS.proximityAvailable()) {
      // Serial.printf("proximity: %u\n", APDS.readProximity());
      is_occupied_proximity = APDS.readProximity() > 240 ? false : true;
    }
    // Serial.printf("flex: %u, prox: %u, ir: %u\n", is_occupied_flexband, is_occupied_proximity, is_occupied_ir);
  }

  // someone takes a seat (any sensor reports true)
  if ((is_occupied_flexband || is_occupied_proximity || is_occupied_ir) && !seating_start_time) {
    is_occupied = true;
    seating_start_time = millis();
    digitalWrite(LED_BUILTIN, 1);
    Serial.println(NOK("Seat taken"));
    // low_power();
  }

  // save samples while seat taken
  if (is_occupied && (millis()-timekeeper_sample) > DELAY_SAMPLE) {
    timekeeper_sample = millis();
    float temperature = HTS.readTemperature(); // °C
    float humidity = HTS.readHumidity(); // %
    float pressure = BARO.readPressure(); // kPa
    analogReadResolution(8);
    uint8_t light = analogRead(PIN_LIGHT);
    Serial.printf(" (%u) temperature: %.1fC light: %u humidity: %.1f%% pressure: %.1f\n", sample_count, temperature, light, humidity, pressure);

    // add to buffer
    Sample s = { .temperature=(uint8_t)(temperature + 0.5), .light=light };
    buffer_sample(s, samples, sample_count);
    sample_count++;
  }

  // someone leaves the seat (all sensors report false)
  // TODO ignore short sessions
  // TODO add duration to payload
  if (!is_occupied_flexband && !is_occupied_proximity && !is_occupied_ir && seating_start_time) {
    uint16_t duration = (millis()-seating_start_time)/1000;
    Serial.printf(OK("Seat freed") " \n duration: %u seconds\n", duration);

    if (duration < MINIUM_DURATION) {
      Serial.println(WARN("Duration not long enough. Skipped"));
    } else {
      Serial.print(" "); print_buffer(samples);

      // sent to ttn
      // payload: position(4), duration(2), 0..22 x[temperature(1), light(1)]
      // TODO add position
      if (join_status == OK) {
        uint8_t payload[2*MAX_SAMPLES] = {0};
        uint8_t sample_len = sample_count >= MAX_SAMPLES ? MAX_SAMPLES : sample_count;
        uint8_t payload_len = 2+2*sample_len; // duration(2), per sample 2 measures temp(1), light(1)
        payload[0] = duration & 0x00ff; // lsb
        payload[1] = (uint8_t)(duration >> 8); //msb
        uint8_t p_payload = 2;
        for (uint8_t i=0; i<sample_len; i++) {
          payload[p_payload+2*i] = samples[i].temperature;
          payload[p_payload+2*i+1] = samples[i].light;
        }
        modem.print_arr(payload, payload_len);
        Serial.print("uploading...");
        Status m = modem.send(payload, payload_len);
        Serial.println(m == OK ? OK("done") : NOK("failed"));
      }
    }

    // reset counters
    sample_count = 0;
    is_occupied = false;
    seating_start_time = 0;
    digitalWrite(LED_BUILTIN, 0);
    // low_power();
  }
}
