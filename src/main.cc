#ifndef UNIT_TEST

#include <Arduino.h>
// #define DEBUG Serial

#include <state.h>
#include <timer.h>

// #include <avr/wdt.h>

// Watering

#define PIN_WATERING        2
#define DURATION_WATERING   10000     // 10 seconds
#define PERIOD_WATERING  1800000   // 30 minutes

// Lighting

#define DELAY_START 1000

#define PIN_LIGHTING_LO    4
#define PIN_LIGHTING_HI   5
// #define PIN_LIGHTING_PHOTO  5
#define DURATION_LIGHTING  1000  // 16 hours
#define PERIOD_LIGHTING     (DELAY_LIGHTING_ON + DELAY_LIGHTING_OFF)
#define PWM_LIGHTING_LO    (unsigned char)(255*0.3)
#define PWM_LIGHTING_HI   (unsigned char)(255*0.6)

// #define DELAY_PHOTO_FLASH   // 
// #define PERIOD_LIGHTING

void logTimestamp(bool _, const fsm_timestamp_t& timestamp);
void logHiLights(bool _, const bool& on);
void logLoLights(bool _, const bool& on);

void timerControlLights(bool on, const fsm_timestamp_t& timestamp);

Flag lighting = Flag();
Flag watering = Flag();

void setup() {
  Serial.begin(115200);
  while(!Serial);

  pinMode(PIN_LIGHTING_LO, OUTPUT);
  pinMode(PIN_LIGHTING_HI, OUTPUT);
  pinMode(PIN_WATERING, OUTPUT);

  Timer.addEventFlag(DELAY_START, &lighting);
  Timer.addEventFlag(DELAY_START + DURATION_LIGHTING, &lighting, true);

  Timer.addIntervalFlag(PERIOD_WATERING, &watering);
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

  delay(1000);
}

void logTimestamp(bool _, const fsm_timestamp_t& timestamp) {
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

void timerControlLights(bool on, const fsm_timestamp_t& timestamp) {
  if(on) {
    analogWrite(PIN_LIGHTING_HI, PWM_LIGHTING_HI);
    analogWrite(PIN_LIGHTING_LO, PWM_LIGHTING_LO);
  } else {
    analogWrite(PIN_LIGHTING_HI, LOW);
    analogWrite(PIN_LIGHTING_LO, LOW);
  }
}

#endif