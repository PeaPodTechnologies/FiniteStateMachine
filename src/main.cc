#ifndef UNIT_TEST

#include <Arduino.h>

#include <debug.h>
#include <chrono.h>

#include <avr/wdt.h>

#define TIMESTAMP_LOG_DELTA_MS 5000 // Default
#define FSM_CYCLE_DELTA_MS     100

// Watering

#define PIN_WATERING        2
#define DURATION_WATERING   2000     // 10 seconds
#define PERIOD_WATERING  10000   // 30 minutes

// Lighting

#define DELAY_START 1000

#define PIN_LIGHTING_LO    5
#define PIN_LIGHTING_HI   6
// #define PIN_LIGHTING_PHOTO  5
#define DURATION_LIGHTING (TWENTYFOURHRS_MILLIS*3/4)
#define PWM_LIGHTING_LO    (unsigned char)(255*0.3)
#define PWM_LIGHTING_HI   (unsigned char)(255*0.6)

// Fun Stuff
#define PIN_DISCO_BUTTON  10
#define DELTA_DISCO  10
#define DISCO_CYCLE_MS 1000

// #define DELAY_PHOTO_FLASH   // 
// #define PERIOD_LIGHTING

void logCycle(bool _, const Number& cycle);
void logTimestamp(bool _, const fsm_timestamp_t& timestamp);
// void logDisco(bool _, const bool& on);

// void timerControlLights(bool on, const fsm_timestamp_t& timestamp);
void controlLights(bool _, const bool& lights);
void controlWatering(bool _, const bool& watering);
void controlDisco(bool _, const bool& __);

void scrollDisco(bool _, const fsm_timestamp_t& __);

Variable* cycle;
Flag* lighting, *watering, *disco;

void setup() {
  // Serial
  Serial.begin(115200);
  while(!Serial);

  // Pin Modes
  pinMode(PIN_LIGHTING_LO, OUTPUT);
  pinMode(PIN_LIGHTING_HI, OUTPUT);
  pinMode(PIN_WATERING, OUTPUT);
  pinMode(PIN_DISCO_BUTTON, INPUT);

  // Construct States
  cycle = new Variable(Number(0, false, false), "Cycle");
  cycle->addLoggerCallback(logCycle);

  // Chronos.addInterval(TIMESTAMP_LOG_DELTA_MS, &logTimestamp);

  lighting = new Flag(false, "Light");
  lighting->addLatchingConditional(true, false, controlLights);

  // Timer Flag Event - Lighting ON (No-Invert)
  Chronos.addEventFlag(DELAY_START, lighting);

  // Timer Flag Event - Lighting OFF (Inverted)
  Chronos.addEventFlag(DELAY_START + DURATION_LIGHTING, lighting, true);

  watering = new Flag(false, "Water");
  watering->addLatchingConditional(true, false, controlWatering);

  // Timer Flag Interval - Watering ON (No-Invert)
  Chronos.addIntervalFlag(PERIOD_WATERING, watering);

  // Timer Flag Interval - Watering OFF (Invert)
  Chronos.addIntervalFlag(PERIOD_WATERING, DURATION_WATERING, watering, true);

  disco = new Flag(false, "Disco");
  disco->addLatchingConditional(true, false, &controlDisco);

  // Disco Scroller Callback - passes interval timestamp
  // discoScroller = s
  Chronos.addInterval(DELTA_DISCO, &scrollDisco);

  // DebugJson::debugSRAM();
  
  #ifdef _AVR_WDT_H_
    wdt_enable(WDTO_4S);
  #endif
}

void loop() {
  #ifdef _AVR_WDT_H_
    wdt_reset();
  #endif

  cycle->set(cycle->get() + Number(1, false, false));

  // Watchdog Timer Kickout
  // if(now > TWENTYFOURHRS_MILLIS) { while(true) { delay(1); } }

  Chronos.set(millis());

  // Other Stuff

  int discoPin = digitalRead(PIN_DISCO_BUTTON);
  disco->set(discoPin == HIGH ? true : false);
}

unsigned long lastCycle = 0;

void logCycle(bool _, const Number& cycle) {
  String m = _F("==== [ Cycle: ");
  m += cycle.toString();
  m += _F(" @ ");
  m += timestampToString(millis());

  unsigned delta = millis() - lastCycle;
  unsigned d = 0;
  if(delta < FSM_CYCLE_DELTA_MS) {
    d = FSM_CYCLE_DELTA_MS - delta;
    delay(d);
  }
  if(d > 0) {
    m += _F(" + ");
    m += d;
    m += _F("ms");
  }
  m += _F(" ] ====");

  DebugJson::debug(m);
}

// void logDisco(bool _, const bool& on) {
//   String m = _F("==== [ Disco: ");
//   m += on ? _F("ENABLED") : _F("DISABLED");
//   m += _F(" ] ====");
//   // DEBUG_JSON(m.c_str(), m.length());
// DEBUG_JSON(m);
// }

void controlLights(bool _, const bool& lights) {
  if(lights) {
    analogWrite(PIN_LIGHTING_HI, PWM_LIGHTING_HI);
    analogWrite(PIN_LIGHTING_LO, PWM_LIGHTING_LO);
  } else {
    analogWrite(PIN_LIGHTING_HI, LOW);
    analogWrite(PIN_LIGHTING_LO, LOW);
  }
  String m = _F("==== [ Lights: ");
  m += lights ? _F("ON") : _F("OFF");
  m += _F(" ] ====");
  DebugJson::debug(DebugJson::EVENT_TELEMETRY, m);
  // Serial.println(m);
}

void controlWatering(bool _, const bool& watering) {
  if(watering) {
    digitalWrite(PIN_WATERING, HIGH);
  } else {
    digitalWrite(PIN_WATERING, LOW);
  }
  String m = _F("==== [ Watering: ");
  m += watering ? _F("ON") : _F("OFF");
  m += _F(" ] ====");
  DebugJson::debug(DebugJson::EVENT_TELEMETRY, m);
  // Serial.println(m);
}

void controlDisco(bool _, const bool& disco) {
  String m = _F("==== [ Disco: ");
  m += disco ? _F("ON") : _F("OFF");
  m += _F(" ] ====");
  DebugJson::debug(DebugJson::EVENT_TELEMETRY, m);
  if(disco) {
    // Disable - don't worry about setting, disco scroll will take it from here
    lighting->disable();
  } else {
    // Enable and retrigger
    lighting->resume();
  }
}

void scrollDisco(bool _, const fsm_timestamp_t& __) {
  fsm_timestamp_t timestamp = Chronos.get();

  uint8_t discoPWM = (uint8_t)((cos(((timestamp % DISCO_CYCLE_MS) / (float)DISCO_CYCLE_MS) * 6.2831853) / 2.25 + 0.5) * 255);

  if(disco->get() && lighting->isDisabled()) {
    analogWrite(PIN_LIGHTING_HI, discoPWM);
    analogWrite(PIN_LIGHTING_LO, 255-discoPWM);
  }
}

#endif