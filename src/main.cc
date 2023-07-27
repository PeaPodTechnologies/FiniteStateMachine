#ifndef UNIT_TEST

#include <Arduino.h>
// #define DEBUG Serial

#include <state.h>

#define PIN_BUTTON_DEC 2
#define PIN_BUTTON_INC 3
#define PIN_FAN 4
#define PIN_HEATER 5

void logTemperature(bool _, Number num);
void logHeater(bool _, bool heat);
void logFan(bool _, bool fan);

void controlFan(bool _, bool fan, bool __);
void controlHeater(bool _, bool heat, bool __);

Variable temperature(0.0);

Flag fan(false);
Flag heater(false);

void setup(void) {
  Serial.begin(115200);

  // Serial.print("NaN == ");
  // Serial.println((double)NAN);

  Serial.println("===== [ SETUP ] =====");

  pinMode(PIN_BUTTON_INC, INPUT);
  pinMode(PIN_BUTTON_DEC, INPUT);
  pinMode(PIN_FAN, OUTPUT);
  pinMode(PIN_HEATER, OUTPUT);

  // Loggers
  temperature.addLoggerCallback(&logTemperature);
  // heater.addLoggerCallback(&logHeater);
  // fan.addLoggerCallback(&logFan);

  // Add oneshot "trigger" conditionals: ONCE each time when temp crosses >25 and <18 (both rising and falling edges)
  // temperature.addConditionalSetFlag(CMP_GTR, 25.0, &button_fan, true);
  // temperature.addConditionalSetFlag(CMP_LES, 18.0, &button_heater, true);

  Serial.println("== [ CBs: Temperature ] ==");
  temperature.addLatchingSetFlag(25.0, 18.0, &fan);
  temperature.addLatchingSetFlag(19.0, 22.0, &heater);

  // Serial.println("== [ CBs: Fan ] ==");
  fan.addLatchingConditional(true, false, &controlFan);

  // Serial.println("== [ CBs: Heater ] ==");
  heater.addLatchingConditional(true, false, &controlHeater);
}

long i = 0;

double t = 22.3;

void loop(void) {
  i++;
  Serial.print("===== [ CYCLE ");
  Serial.print(i);
  Serial.println(" ] =====");

  // double t = (random(150, 300) / 10.0);
  // Serial.print("== [ Temperature Set: ");
  // Serial.print(t);
  // Serial.println("] ==");
  if (digitalRead(PIN_BUTTON_DEC) == HIGH) {
    t--;
  } else if (digitalRead(PIN_BUTTON_INC) == HIGH) {
    t++;
  }
  temperature.set(t);

  delay(1000);
}

void controlFan(bool _, bool fan, bool __) {
  digitalWrite(PIN_FAN, fan ? HIGH : LOW);
  logFan(_, fan);
}

void controlHeater(bool _, bool heat, bool __) {
  digitalWrite(PIN_HEATER, heat ? HIGH : LOW);
  logHeater(_, heat);
}

void logTemperature(bool _, Number num) {
  DEBUG_DELAY();
  Serial.print("== [ Temperature: ");
  Serial.print(num);
  Serial.println("degC ] ==");
  DEBUG_DELAY();
}

void logHeater(bool _, bool heat) {
  const char* buf = ((char)heat == 1 ? "== [ Heat: ON ] ==" : "== [ Heat: OFF ] ==");
  DEBUG_DELAY();
  Serial.println(buf);
  DEBUG_DELAY();
}

void logFan(bool _, bool fan) {
  const char* buf = ((char)fan == 1 ? "== [ Fan: ON ] ==" : "== [ Fan: OFF ] ==");
  DEBUG_DELAY();
  Serial.println(buf);
  DEBUG_DELAY();
}

#endif