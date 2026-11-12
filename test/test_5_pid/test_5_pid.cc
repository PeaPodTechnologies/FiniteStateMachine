#include <Arduino.h>
#include <unity.h>

#include <pid.h>

using namespace FSM;

static unsigned int outputCallbackCount = 0;
static Number lastOutput(0);

static void onPidOutput(bool _, const Number& value) {
  outputCallbackCount++;
  lastOutput = value;
}

void test_pid_updates_internal_state_via_conditional_operator(void) {
  ConditionalCallbackPID pid("pid", Number(10), Number(2), Number(0.5), Number(0));
  pid(Number(8)); // error = 2

  TEST_ASSERT_DOUBLE_WITHIN(0.0001, 2.0, (double)pid.getIntegral());
  TEST_ASSERT_DOUBLE_WITHIN(0.0001, 2.0, (double)pid.getPreviousError());
  TEST_ASSERT_DOUBLE_WITHIN(0.0001, 5.0, (double)pid.getOutput());
}

void test_pid_accumulates_integral_over_multiple_updates(void) {
  ConditionalCallbackPID pid("pid", Number(10), Number(2), Number(0.5), Number(0));
  pid(Number(8)); // error = 2
  pid(Number(9)); // error = 1

  TEST_ASSERT_DOUBLE_WITHIN(0.0001, 3.0, (double)pid.getIntegral());
  TEST_ASSERT_DOUBLE_WITHIN(0.0001, 1.0, (double)pid.getPreviousError());
  TEST_ASSERT_DOUBLE_WITHIN(0.0001, 3.5, (double)pid.getOutput());
}

void test_pid_output_callback_fires_with_output_updates(void) {
  outputCallbackCount = 0;
  lastOutput = Number(0);

  ConditionalCallbackPID pid("pid", &onPidOutput, Number(10), Number(2), Number(0.5), Number(0));
  pid(Number(8));

  TEST_ASSERT_EQUAL_UINT(1, outputCallbackCount);
  TEST_ASSERT_DOUBLE_WITHIN(0.0001, 5.0, (double)lastOutput);
}

void test_pid_setters_and_reset_manage_state(void) {
  ConditionalCallbackPID pid("pid", Number(10), Number(1), Number(0), Number(0));

  pid(Number(8));
  TEST_ASSERT_DOUBLE_WITHIN(0.0001, 2.0, (double)pid.getOutput());

  pid.setGains(Number(2), Number(1), Number(0));
  pid.setSetpoint(Number(12));
  pid(Number(10)); // error = 2, integral was 2 so output = 2*2 + 1*4 = 8
  TEST_ASSERT_DOUBLE_WITHIN(0.0001, 8.0, (double)pid.getOutput());
  TEST_ASSERT_DOUBLE_WITHIN(0.0001, 12.0, (double)pid.getSetpoint());

  pid.reset();
  TEST_ASSERT_DOUBLE_WITHIN(0.0001, 0.0, (double)pid.getIntegral());
  TEST_ASSERT_DOUBLE_WITHIN(0.0001, 0.0, (double)pid.getPreviousError());
  TEST_ASSERT_DOUBLE_WITHIN(0.0001, 0.0, (double)pid.getOutput());
}

void test_pid_does_not_compute_when_measured_equals_setpoint(void) {
  ConditionalCallbackPID pid("pid", Number(12), Number(2), Number(1), Number(0));
  pid(Number(12));

  TEST_ASSERT_DOUBLE_WITHIN(0.0001, 0.0, (double)pid.getIntegral());
  TEST_ASSERT_DOUBLE_WITHIN(0.0001, 0.0, (double)pid.getPreviousError());
  TEST_ASSERT_DOUBLE_WITHIN(0.0001, 0.0, (double)pid.getOutput());
  TEST_ASSERT_DOUBLE_WITHIN(0.0001, 12.0, (double)pid.getSetpoint());
}

void setup() {
  delay(2000);
  UNITY_BEGIN();
  RUN_TEST(test_pid_updates_internal_state_via_conditional_operator);
  RUN_TEST(test_pid_accumulates_integral_over_multiple_updates);
  RUN_TEST(test_pid_output_callback_fires_with_output_updates);
  RUN_TEST(test_pid_setters_and_reset_manage_state);
  RUN_TEST(test_pid_does_not_compute_when_measured_equals_setpoint);
  UNITY_END();
}

void loop() { }
