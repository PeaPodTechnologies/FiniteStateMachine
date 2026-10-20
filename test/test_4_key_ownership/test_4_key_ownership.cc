#include <Arduino.h>
#include <unity.h>

#include <state.h>
#include <string.h>

using namespace FSM;

static void onCondition(bool) { }

void test_keys_copy_mutable_source(void) {
  char source[] = "temperature";
  State<bool> state(false, source);
  ConditionalCallback<bool> conditional(source, CMP_EQU, true, &onCondition);

  TEST_ASSERT_NOT_EQUAL(source, state.getKey());
  TEST_ASSERT_NOT_EQUAL(source, conditional.getKey());
  memset(source, 'x', sizeof(source) - 1);

  TEST_ASSERT_EQUAL_STRING("temperature", state.getKey());
  TEST_ASSERT_EQUAL_STRING("temperature", conditional.getKey());
}

void test_keys_outlive_source(void) {
  State<bool>* state;
  ConditionalCallback<bool>* conditional;
  {
    String source("temporary");
    state = new State<bool>(false, source.c_str());
    conditional = new ConditionalCallback<bool>(source.c_str(), CMP_EQU, true, &onCondition);
  }

  TEST_ASSERT_EQUAL_STRING("temporary", state->getKey());
  TEST_ASSERT_EQUAL_STRING("temporary", conditional->getKey());
  delete state;
  delete conditional;
}

void test_null_and_empty_keys_remain_distinct(void) {
  State<bool> unkeyed(false);
  State<bool> nullState(false, FSM_KEY_NULL);
  State<bool> emptyState(false, "");
  ConditionalCallback<bool> unkeyedConditional(CMP_EQU, true, &onCondition);
  ConditionalCallback<bool> nullConditional(FSM_KEY_NULL, CMP_EQU, true, &onCondition);
  ConditionalCallback<bool> emptyConditional("", CMP_EQU, true, &onCondition);

  TEST_ASSERT_NULL(unkeyed.getKey());
  TEST_ASSERT_NULL(nullState.getKey());
  TEST_ASSERT_NOT_NULL(emptyState.getKey());
  TEST_ASSERT_EQUAL_STRING("", emptyState.getKey());
  TEST_ASSERT_NULL(unkeyedConditional.getKey());
  TEST_ASSERT_NULL(nullConditional.getKey());
  TEST_ASSERT_NOT_NULL(emptyConditional.getKey());
  TEST_ASSERT_EQUAL_STRING("", emptyConditional.getKey());
}

void test_long_keys_are_not_truncated(void) {
  char source[257];
  memset(source, 'k', sizeof(source) - 1);
  source[sizeof(source) - 1] = '\0';
  State<bool> state(false, source);
  ConditionalCallback<bool> conditional(source, CMP_EQU, true, &onCondition);

  TEST_ASSERT_EQUAL_STRING(source, state.getKey());
  TEST_ASSERT_EQUAL_STRING(source, conditional.getKey());
  TEST_ASSERT_EQUAL_UINT(256, strlen(state.getKey()));
  TEST_ASSERT_EQUAL_UINT(256, strlen(conditional.getKey()));
}

void test_copied_objects_own_their_keys(void) {
  State<bool>* stateCopy;
  ConditionalCallback<bool>* conditionalCopy;
  {
    State<bool> state(false, "copied");
    ConditionalCallback<bool> conditional("copied", CMP_EQU, true, &onCondition);
    stateCopy = new State<bool>(state);
    conditionalCopy = new ConditionalCallback<bool>(conditional);

    TEST_ASSERT_NOT_EQUAL(state.getKey(), stateCopy->getKey());
    TEST_ASSERT_NOT_EQUAL(conditional.getKey(), conditionalCopy->getKey());
  }

  TEST_ASSERT_EQUAL_STRING("copied", stateCopy->getKey());
  TEST_ASSERT_EQUAL_STRING("copied", conditionalCopy->getKey());
  delete stateCopy;
  delete conditionalCopy;
}

void test_conditionals_copy_state_key(void) {
  State<bool> state(false, "state");
  ConditionalCallback<bool>* conditional = state.addConditional(CMP_EQU, true, &onCondition);

  TEST_ASSERT_NOT_NULL(conditional);
  TEST_ASSERT_EQUAL_STRING("state", conditional->getKey());
  TEST_ASSERT_NOT_EQUAL(state.getKey(), conditional->getKey());
}

void setup() {
  delay(2000);
  UNITY_BEGIN();
  RUN_TEST(test_keys_copy_mutable_source);
  RUN_TEST(test_keys_outlive_source);
  RUN_TEST(test_null_and_empty_keys_remain_distinct);
  RUN_TEST(test_long_keys_are_not_truncated);
  RUN_TEST(test_copied_objects_own_their_keys);
  RUN_TEST(test_conditionals_copy_state_key);
  UNITY_END();
}

void loop() { }
