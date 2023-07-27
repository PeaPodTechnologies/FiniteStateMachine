#include <Arduino.h>
#include <unity.h>

#include <state.h>

#define REF_TEMPERATURE_MAX 25

Variable temperature = Variable(23.4);

Variable timestamp = Variable((unsigned long)millis());

Variable delta = Variable((unsigned long)0);

Flag fan = Flag(false);

void turnOnFan(void) {
  fan.set(true);
}

void turnOffFan(void) {

}

void setDelta(bool comp, unsigned long current, unsigned long last) {
  current
}

void resetDelta(bool comp, unsigned long current, unsigned long last) {

}

void setup() {
  delay(2000);

  UNITY_BEGIN();
  RUN_TEST(test_led_builtin_pin_number);

  // Add a conditional callback to Temperature
  // Every time value is set, a condition is checked
  // The condition is a comparison between the value and a reference value
  // If the condition evaluates to true, the callback is called
  // We set one-shot to true, so the condition is only checked when it hasn't evaluated to true before
  temperature.addConditional(CMP_GTR, REF_TEMPERATURE_MAX, &turnOnFan, true);

  // Dynamic Ref: Ref is updated from millis() every time, and
  // Not One-Shot: comparison is performed every time
  // When timestamp is greater than 
  timestamp.addConditional(CMP_GTR, (unsigned long)0, &setDelta, &timestamp.get);
  temperature.addConditional(CMP_LEQ, (unsigned long)0, &resetDelta, &timestamp.get);

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {

  timestamp.set

  

  // Read temperature sensor
  temperature.set(23.5);
  temperature.set(24.5);
  temperature.set(25.5);
}