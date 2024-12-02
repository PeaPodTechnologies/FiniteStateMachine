#ifndef UNIT_TEST

#include <Arduino.h>

#include <debug.h>
#include <chrono.h>

#include <avr/wdt.h>

#define TIMESTAMP_LOG_DELTA_MS 5000 // Default
#define FSM_CYCLE_DELTA_MS     100
#define FSM_FPS_FRAME_COUNT    1000

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

#define PIN_LIGHTING_CAMERA   7
#define PWM_LIGHTING_CAMERA   (unsigned char)(255*0.4)

// Fun Stuff
#define PIN_DISCO_BUTTON  10
#define DELTA_DISCO  10
#define DISCO_CYCLE_MS 1000

using namespace FiniteStateMachine;

// #define DELAY_PHOTO_FLASH   // 
// #define PERIOD_LIGHTING

void logCycle(bool _, const Number& cycle);
void logTimestamp(bool _, const fsm_timestamp_t& timestamp);
// void logDisco(bool _, const bool& on);

// void timerControlLights(bool on, const fsm_timestamp_t& timestamp);
void controlLights(bool _, const bool& lights);
void controlCamera(bool _, const bool& camera);
void controlWatering(bool _, const bool& watering);
void controlDisco(bool _, const bool& __);

void scrollDisco(bool _, const fsm_timestamp_t& __);

void commandHandler(JsonObject commands);
void configHandler(JsonObject config);

// DebugJson::debugjson_cb_commands_t handleCommand = &commandHandler;
// DebugJson::debugjson_cb_config_t handleConfig = &configHandler;

Variable* cycle;
Flag* lighting, *watering, *disco, *camera;

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

  camera = new Flag(false, "Camera");
  camera->addLatchingConditional(true, false, &controlCamera);

  // DebugJson::debugSRAM();
  
  #ifdef _AVR_WDT_H_
    wdt_enable(WDTO_4S);
  #endif
}

unsigned long lastLoop = 0;
unsigned long fpsSum = 0;
unsigned long fpsCount = 0;
unsigned long lastFixedUpdate = 0;

void loop() {
  #ifdef _AVR_WDT_H_
    wdt_reset();
  #endif

  cycle->set(cycle->get() + Number(1, false, false));

  DebugJson::update(commandHandler, configHandler);

  unsigned long now = millis();
  if(now - lastFixedUpdate > DEBUG_JSON_REFRESH_MICROS) {
    DebugJson::fixedUpdate(now);
    lastFixedUpdate = now;
  }

  fpsSum += 1000.0 / (now - lastLoop);
  lastLoop = now;
  fpsCount++;
  if(fpsCount > FSM_FPS_FRAME_COUNT) {
    unsigned long fps = fpsSum / fpsCount;
    String fpsm = _F("==== [ FPS: ");
    fpsm += fps;
    fpsm += _F(" ] ====");
    DebugJson::debug(DebugJson::EVENT_TELEMETRY, fpsm);

    fpsSum = 0;
    fpsCount = 0;
  }

  // Other Stuff

  int discoPin = digitalRead(PIN_DISCO_BUTTON);
  disco->set(discoPin == HIGH ? true : false);
}

// unsigned long lastCycle = 0;

void logCycle(bool _, const Number& cycle) {
  String m = _F("==== [ Cycle: ");
  m += cycle.toString();
  m += _F(" @ ");

  unsigned long now = millis();
  m += timestampToString(now);

  // unsigned delta = now - lastCycle;
  // unsigned d = 0;
  // if(delta < FSM_CYCLE_DELTA_MS) {
  //   d = FSM_CYCLE_DELTA_MS - delta;
  //   delay(d);
  // }
  // if(d > 0) {
  //   m += _F(" + ");
  //   m += d;
  //   m += _F("ms");
  // }
  m += _F(" ] ====");

  // lastCycle = now;
  DebugJson::debug(m);

  Chronos.set(now);
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

void controlCamera(bool _, const bool& camera) {
  String m = _F("==== [ Camera: ");
  m += camera ? _F("ON") : _F("OFF");
  m += _F(" ] ====");
  DebugJson::debug(DebugJson::EVENT_TELEMETRY, m);
  if(camera) {
    lighting->disable(); // Detach conditionals, don't change
    analogWrite(PIN_LIGHTING_HI, LOW);
    analogWrite(PIN_LIGHTING_LO, LOW);
    analogWrite(PIN_LIGHTING_CAMERA, PWM_LIGHTING_CAMERA);
  } else {
    analogWrite(PIN_LIGHTING_CAMERA, LOW);
    lighting->resume(); // Attach conditionals and call
  }
}

Flag* flags[] = { lighting, watering };

void commandHandler(JsonObject commands) {
  for(JsonPair kv : commands) {
    String k = kv.key().c_str();
    if(k == "reset" && !kv.value().isNull()) {
      if(kv.value().is<bool>() && kv.value().as<bool>()) {
        DebugJson::debug(DebugJson::EVENT_TELEMETRY, _F("==== [ RESET: FLAGS ] ===="), 26);

        delay(1000);

        for(Flag* f : flags) {
          f->reset();

          delay(1000);
        }

        DebugJson::debug(DebugJson::EVENT_TELEMETRY, _F("==== [ RESET: `JMP 0x00` ] ===="), 31);


        // delay(3000);
        // asm volatile ("  jmp 0");
      }
    } else if(k == "flags" && !kv.value().isNull()) {
      for(Flag* f : flags) {
        DebugJson::debug(DebugJson::EVENT_TELEMETRY, f->toString());
      }
    }
  }
}

void configHandler(JsonObject config) {
  for(JsonPair kv : config) {
    String k = kv.key().c_str();
    for(Flag* f : flags) {
      if(k == f->getKey() && kv.value().is<bool>()) f->set(kv.value().as<bool>());
    }
  }
}

#endif

// {"type":"event","timestamp":732085,"data":{"sram":493},"msg":"==== [ Watering: OFF ] ===="}
// {"type":"heartbeat","timestamp":737562}
// {"type":"event","timestamp":740094,"data":{"sram":494},"msg":"==== [ Watering: ON ] ===="}
// {"type":"event","timestamp":742052,"data":{"sram":493},"msg":"==== [ Watering: OFF ] ===="}
// {"type":"heartbeat","timestamp":747697}
// {"type":"event","timestamp":750064,"data":{"sram":494},"msg":"==== [ Watering: ON ] ===="}
// {"type":"event","timestamp":752097,"data":{"sram":493},"msg":"==== [ Watering: OFF ] ===="}
// {"type":"heartbeat","timestamp":757823}
// {"type":"event","timestamp":760050,"data":{"sram":494},"msg":"==== [ Watering: ON ] ===="}
// {"type":"event","timestamp":762081,"data":{"sram":493},"msg":"==== [ Watering: OFF ] ===="}
// {"type":"heartbeat","timestamp":767961}
// {"type":"event","timestamp":770087,"data":{"sram":494},"msg":"==== [ Watering: ON ] ===="}
// {"type":"event","timestamp":772063,"data":{"sram":493},"msg":"==== [ Watering: OFF ] ===="}
// {"type":"heartbeat","timestamp":777083}
// {"type":"event","timestamp":780069,"data":{"sram":494},"msg":"==== [ Watering: ON ] ===="}
// {"type":"event","timestamp":782048,"data":{"sram":493},"msg":"==== [ Watering: OFF ] ===="}
// {"type":"heartbeat","timestamp":787221}
// {"type":"event","timestamp":790099,"data":{"sram":494},"msg":"==== [ Watering: ON ] ===="}
// {"type":"event","timestamp":792086,"data":{"sram":493},"msg":"==== [ Watering: OFF ] ===="}
// {"type":"heartbeat","timestamp":797361}
// ---- Sent utf8 encoded message: "{}\n" ----
// {}
// null
// {"type":"error","timestamp":799385,"data":{"sram":519},"msg":"DJ: Invalid JSON (ENOENT 'type')"}
// {"type":"event","timestamp":800032,"data":{"sram":494},"msg":"==== [ Watering: ON ] ===="}
// {"type":"event","timestamp":802064,"data":{"sram":493},"msg":"==== [ Watering: OFF ] ===="}
// {"type":"heartbeat","timestamp":807494}
// {"type":"event","timestamp":810075,"data":{"sram":494},"msg":"==== [ Watering: ON ] ===="}
// {"type":"event","timestamp":812053,"data":{"sram":493},"msg":"==== [ Watering: OFF ] ===="}
// {"type":"heartbeat","timestamp":817630}
// ---- Sent utf8 encoded message: "{\"type\":\"command\",\"data\":{\"reset\":true}}\n" ----
// {"type":"command","data":{"reset":true}}
// command
// {"type":"error","timestamp":818442,"data":{"sram":337},"msg":"DJ: No Callback 'Commands'"}
// {"type":"event","timestamp":820071,"data":{"sram":305},"msg":"==== [ Watering: ON ] ===="}
// {"type":"event","timestamp":822038,"data":{"sram":305},"msg":"==== [ Watering: OFF ] ===="}
// {"type":"heartbeat","timestamp":827788}
// {"type":"event","timestamp":830067,"data":{"sram":305},"msg":"==== [ Watering: ON ] ===="}
// {"type":"event","timestamp":832046,"data":{"sram":305},"msg":"==== [ Watering: OFF ] ===="}
// {"type":"heartbeat","timestamp":837933}
// {"type":"event","timestamp":840066,"data":{"sram":305},"msg":"==== [ Watering: ON ] ===="}
// {"type":"event","timestamp":842045,"data":{"sram":305},"msg":"==== [ Watering: OFF ] ===="}
// {"type":"heartbeat","timestamp":847082}
// {"type":"event","timestamp":850062,"data":{"sram":305},"msg":"==== [ Watering: ON ] ===="}
// {"type":"event","timestamp":852094,"data":{"sram":305},"msg":"==== [ Watering: OFF ] ===="}
// {"type":"heartbeat","timestamp":857222}
// {"type":"event","timestamp":860064,"data":{"sram":305},"msg":"==== [ Watering: ON ] ===="}
// {"type":"event","timestamp":862093,"data":{"sram":305},"msg":"==== [ Watering: OFF ] ===="}
// {"type":"heartbeat","timestamp":867385}
// {"type":"event","timestamp":870064,"data":{"sram":305},"msg":"==== [ Watering: ON ] ===="}
// {"type":"event","timestamp":872095,"data":{"sram":305},"msg":"==== [ Watering: OFF ] ===="}
// {"type":"heartbeat","timestamp":877527}
// {"type":"event","timestamp":880066,"data":{"sram":305},"msg":"==== [ Watering: ON ] ===="}
// {"type":"event","timestamp":882101,"data":{"sram":305},"msg":"==== [ Watering: OFF ] ===="}
// {"type":"heartbeat","timestamp":887688}
// {"type":"event","timestamp":890067,"data":{"sram":305},"msg":"==== [ Watering: ON ] ===="}
// {"type":"event","timestamp":892048,"data":{"sram":305},"msg":"==== [ Watering: OFF ] ===="}
// {"type":"heartbeat","timestamp":897828}
// {"type":"event","timestamp":900060,"data":{"sram":305},"msg":"==== [ Watering: ON ] ===="}
// {"type":"event","timestamp":902095,"data":{"sram":305},"msg":"==== [ Watering: OFF ] ===="}
// {"type":"heartbeat","timestamp":907991}
// {"type":"event","timestamp":910065,"data":{"sram":305},"msg":"==== [ Watering: ON ] ===="}
// ---- Closed the serial port /dev/tty.usbserial-AB0LQ96V ----
// ---- Opened the serial port /dev/tty.usbserial-AB0LQ96V ----
// {"type":"event","timestamp":101,"data":{"sram":493},"msg":"==== [ Watering: ON ] ===="}
// {"type":"event","timestamp":1018,"data":{"sram":489},"msg":"==== [ Lights: ON ] ===="}
// {"type":"event","timestamp":2038,"data":{"sram":485},"msg":"==== [ Watering: OFF ] ===="}
// {"type":"heartbeat","timestamp":7073}
// {"type":"event","timestamp":10037,"data":{"sram":484},"msg":"==== [ Watering: ON ] ===="}
// {"type":"event","timestamp":12064,"data":{"sram":483},"msg":"==== [ Watering: OFF ] ===="}
// {"type":"heartbeat","timestamp":17190}
// {"type":"event","timestamp":20067,"data":{"sram":484},"msg":"==== [ Watering: ON ] ===="}
// {"type":"event","timestamp":22097,"data":{"sram":483},"msg":"==== [ Watering: OFF ] ===="}
// {"type":"heartbeat","timestamp":27307}
// {"type":"event","timestamp":30097,"data":{"sram":484},"msg":"==== [ Watering: ON ] ===="}
// {"type":"event","timestamp":32060,"data":{"sram":483},"msg":"==== [ Watering: OFF ] ===="}
// ---- Sent utf8 encoded message: "{\"type\":\"command\",\"data\":{\"reset\":true}}\n" ----
// {"type":"command","data":{"reset":true}}
// command
// {"type":"event","timestamp":33241,"data":{"sram":272},"msg":"==== [ RESET ] ===="}
// {"type":"event","timestamp":102,"data":{"sram":488},"msg":"==== [ Watering: ON ] ===="}
// {"type":"event","timestamp":1020,"data":{"sram":488},"msg":"==== [ Lights: ON ] ===="}
// {"type":"event","timestamp":2038,"data":{"sram":485},"msg":"==== [ Watering: OFF ] ===="}
// {"type":"heartbeat","timestamp":7076}
// {"type":"event","timestamp":10035,"data":{"sram":484},"msg":"==== [ Watering: ON ] ===="}
// ---- Sent utf8 encoded message: "{\"type\":\"command\",\"data\":{\"flags\":true}}\n" ----
// {"type":"command","data":{"flags":true}}
// command
// {"type":"event","timestamp":11330,"data":{"sram":283},"msg":": 0"}
// {"type":"event","timestamp":11337,"data":{"sram":281},"msg":": 192"}
// {"type":"event","timestamp":11339,"data":{"sram":281},"msg":": 192"}
// {"type":"event","timestamp":12062,"data":{"sram":304},"msg":"==== [ Watering: OFF ] ===="}
// {"type":"heartbeat","timestamp":17218}