#include <Arduino.h>
#include <unity.h>

#include <number.h>

#define NUM_NUMBERS 4

Number numbers[NUM_NUMBERS];

void test_number_create(void) {
  numbers[0] = Number(13);
  numbers[1] = Number(6.2);
  numbers[2] = Number(-4);
  numbers[3] = Number(-52.61);

  // 1. Value
  TEST_ASSERT_EQUAL_FLOAT(13.0, ((double)numbers[0]));
  TEST_ASSERT_EQUAL_FLOAT(6.2, ((double)numbers[1]));
  TEST_ASSERT_EQUAL_FLOAT(-4, ((double)numbers[2]));
  TEST_ASSERT_EQUAL_FLOAT(-52.61, ((double)numbers[3]));

  // 2. String Representation
  String s;

  s = numbers[0].toString();
  TEST_ASSERT_EQUAL_STRING("13", s.c_str());

  s = numbers[1].toString();
  TEST_ASSERT_EQUAL_STRING("6.2", s.c_str());
  
  s = numbers[2].toString();
  TEST_ASSERT_EQUAL_STRING("-4", s.c_str());
  
  s = numbers[3].toString();
  TEST_ASSERT_EQUAL_STRING("-52.61", s.c_str());
}

void test_number_comparisons(void) {
  // 3. Comparisons
  TEST_ASSERT_TRUE(numbers[0] > numbers[1]);
  TEST_ASSERT_TRUE(numbers[1] < numbers[0]);

  TEST_ASSERT_TRUE(numbers[1] > numbers[2]);
  TEST_ASSERT_TRUE(numbers[2] < numbers[1]);

  TEST_ASSERT_TRUE(numbers[2] > numbers[3]);
  TEST_ASSERT_TRUE(numbers[3] < numbers[2]);
}

void setup() {
  delay(2000);

  // Using four Numbers (two floats, a signed integer, and an unsigned integer) test all Number operations for consistency of floating-point & signed "rules"

  UNITY_BEGIN();
  RUN_TEST(test_number_create);
  RUN_TEST(test_number_comparisons);
  // RUN_TEST(test_number_operations);

  UNITY_END();
}

// uint8_t i = 0;

void loop() {
  // if (i < 3) {
  //   RUN_TEST(test_led_state_high);
  //   delay(500);
  //   RUN_TEST(test_led_state_low);
  //   delay(500);
  //   i++;
  // } else {
  //   UNITY_END();
  // }
}