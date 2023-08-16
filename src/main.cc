#ifndef UNIT_TEST

#include <Arduino.h>

#include <debug.h>
#include <timer.h>

// #include <avr/wdt.h>

#define TIMESTAMP_LOG_DELTA_MS 5000 // Default

// Watering

#define PIN_WATERING        2
#define DURATION_WATERING   10000     // 10 seconds
#define PERIOD_WATERING  1800000   // 30 minutes

// Lighting

#define DELAY_START 1000

#define PIN_LIGHTING_LO    3
#define PIN_LIGHTING_HI   4
// #define PIN_LIGHTING_PHOTO  5
#define DURATION_LIGHTING (TWENTYFOURHRS_MILLIS*3/4)
#define PWM_LIGHTING_LO    (unsigned char)(255*0.3)
#define PWM_LIGHTING_HI   (unsigned char)(255*0.6)

// #define DELAY_PHOTO_FLASH   // 
// #define PERIOD_LIGHTING

void logTimestamp(bool _, const fsm_timestamp_t& timestamp);
void logLights(bool _, const bool& on);
void logWatering(bool _, const bool& on);

// void timerControlLights(bool on, const fsm_timestamp_t& timestamp);
void controlLights(bool _, const bool& lights);
void controlWatering(bool _, const bool& watering);

Flag lighting = Flag();
Flag watering = Flag();

void setup() {
  // Serial
  Serial.begin(BAUDRATE);
  while(!Serial);

  // Pin Modes
  pinMode(PIN_LIGHTING_LO, OUTPUT);
  pinMode(PIN_LIGHTING_HI, OUTPUT);
  pinMode(PIN_WATERING, OUTPUT);

  lighting.addLoggerCallback(&logLights);
  watering.addLoggerCallback(&logWatering);

  lighting.addLatchingConditional(true, false, &controlLights);
  watering.addLatchingConditional(true, false, &controlWatering);

  // Timestamp output
  Timer.addInterval(TIMESTAMP_LOG_DELTA_MS, &logTimestamp);

  // Timer Flag Event - Lighting ON (No-Invert)
  Timer.addEventFlag(DELAY_START, &lighting);

  // Timer Flag Event - Lighting OFF (Inverted)
  Timer.addEventFlag(DELAY_START + DURATION_LIGHTING, &lighting, true);

  // Timer Flag Interval - Watering ON (No-Invert)
  Timer.addIntervalFlag(PERIOD_WATERING, &watering);

  // Timer Flag Interval - Watering OFF (Invert)
  Timer.addIntervalFlag(PERIOD_WATERING, DURATION_WATERING, &watering, true);

  #ifdef _AVR_WDT_H_
    wdt_enable(WDTO_4S);
  #endif
}

void loop() {
  #ifdef _AVR_WDT_H_
    wdt_reset();
  #endif

  fsm_timestamp_t now = millis();

  // Watchdog Timer Kickout
  if(now > TWENTYFOURHRS_MILLIS) { while(true) { delay(1); } }

  Timer.set(now);

  // logTimestamp(true, Timer.get());

  delay(100);
}

void logTimestamp(bool _, const fsm_timestamp_t& __) {
  fsm_timestamp_t timestamp = Timer.get();

  DEBUG_DELAY();
  Serial.print("==== [ Time Elapsed: ");
  unsigned long seconds = timestamp / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;

  seconds -= minutes * 60;
  minutes -= hours * 60;

  if(hours < 10) Serial.print("0");
  Serial.print(hours);
  Serial.print("h : ");
  if(minutes < 10) Serial.print("0");
  Serial.print(minutes - hours * 60);
  Serial.print("m : ");
  if(seconds < 10) Serial.print("0");
  Serial.print(seconds);
  Serial.println("s ] ====");
  DEBUG_DELAY();
}

void logLights(bool _, const bool& on) {
  const char* buf = (on ? "== [ Lights: ON ] ==" : "== [ Lights: OFF ] ==");
  DEBUG_DELAY();
  Serial.println(buf);
  DEBUG_DELAY();
}

void logWatering(bool _, const bool& on) {
  const char* buf = (on ? "== [ Watering: ON ] ==" : "== [ Watering: OFF ] ==");
  DEBUG_DELAY();
  Serial.println(buf);
  DEBUG_DELAY();
}

void timerControlLights(bool on, const fsm_timestamp_t& timestamp) {
  if(on) {
    analogWrite(PIN_LIGHTING_HI, PWM_LIGHTING_HI);
    analogWrite(PIN_LIGHTING_LO, PWM_LIGHTING_LO);
  } else {
    analogWrite(PIN_LIGHTING_HI, LOW);
    analogWrite(PIN_LIGHTING_LO, LOW);
  }
}

void controlLights(bool _, const bool& lights) {
  if(lights) {
    analogWrite(PIN_LIGHTING_HI, PWM_LIGHTING_HI);
    analogWrite(PIN_LIGHTING_LO, PWM_LIGHTING_LO);
  } else {
    analogWrite(PIN_LIGHTING_HI, LOW);
    analogWrite(PIN_LIGHTING_LO, LOW);
  }
}

void controlWatering(bool _, const bool& watering) {
  if(watering) {
    digitalWrite(PIN_WATERING, HIGH);
  } else {
    digitalWrite(PIN_WATERING, LOW);
  }
}

#endif