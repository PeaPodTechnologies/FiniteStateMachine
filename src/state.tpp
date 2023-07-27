#ifndef FSM_STATE_T_
#define FSM_STATE_T_

#include <number.h>
#include <comparators.h>
#include <state.h>
#include <statelinker.h>

#ifdef DEBUG

template <typename T> void stateDebug(const T& val);

#endif

template <typename T> State<T>::State(const T& value) : value(value), defaultValue(value) { }

template <typename T> State<T>::~State() {
  // for (unsigned char i = 0; i < MAX_CONDITIONALS; i++) {
  //   if(this->conditionals[i] != nullptr) {
  //     delete this->conditionals[i];
  //   }
  // }
}

template <typename T> T State<T>::get(void) {
  return this->value;
}

template <typename T> void State<T>::set(const T& val) {
  #ifdef DEBUG
    DEBUG_DELAY();
    DEBUG.print("State Set: ");
    stateDebug<T>(val);
    DEBUG.print("\n");
    DEBUG.print("----------");
    DEBUG.print("\n");
    DEBUG_DELAY();
  #endif

  for(unsigned int i = 0; i < MAX_CONDITIONALS; i++) {
    if(this->conditionals[i] == nullptr) break;

    this->conditionals[i]->operator()(val);
  }

  #ifdef DEBUG
    DEBUG_DELAY();
    DEBUG.print("----------");
    DEBUG.print("\n");
    DEBUG_DELAY();
  #endif

  this->value = value;
}

template <typename T> ConditionalCallback<T>* State<T>::addConditional(ConditionalCallback<T>* conditional) {
  for (unsigned char i = 0; i < MAX_CONDITIONALS; i++) {
    if(this->conditionals[i] == nullptr) {
      this->conditionals[i] = conditional;
      return this->conditionals[i];
    }
  }
  return nullptr;
}

template <typename T> ConditionalCallback<T>* State<T>::addConditional(comparators_t cmp, const T& ref, void (*const cb)(void), T (*const getReference)(void)) { return addConditional(new ConditionalCallback<T>(cmp, ref, cb, getReference)); }

template <typename T> ConditionalCallback<T>* State<T>::addConditional(comparators_t cmp, const T& ref, void (*const cb)(bool comp), T (*const getReference)(void)) { return addConditional(new ConditionalCallback<T>(cmp, ref, cb, getReference)); }

template <typename T> ConditionalCallback<T>* State<T>::addConditional(comparators_t cmp, const T& ref, void (* const cb)(bool comp, T val), T (*const getReference)(void)) { return addConditional(new ConditionalCallback<T>(cmp, ref, cb, getReference)); }

template <typename T> ConditionalCallback<T>* State<T>::addConditional(comparators_t cmp, const T& ref, void (* const cb)(bool comp, T val, T ref), T (*const getReference)(void)) { return addConditional(new ConditionalCallback<T>(cmp, ref, cb, getReference)); }

template <typename T> ConditionalCallback<T>* State<T>::addConditionalSetFlag(comparators_t cmp, const T& ref, Flag* flag, T (*const getReference)(void)) { return addConditional(new FlagSetCondition<T>(cmp, ref, flag, getReference)); }

template <> ConditionalCallback<Number>* State<Number>::addLoggerCallback(void (* const cb)(bool comp, Number val)) { return addConditional(new ConditionalCallback<Number>(CMP_NEQ, Number(), cb)); }

template <> ConditionalCallback<bool>* State<bool>::addLoggerCallback(void (* const cb)(bool comp, bool val)) { return addConditional(new ConditionalCallback<bool>(CMP_NOP, false, cb)); }

template <> ConditionalCallback<Number>* State<Number>::addLatchingSetFlag(const Number& on, const Number& off, Flag* flag) {
  // if(on < off) {
    // "heater"
    return addConditional(new BangBangFlagSet(on, off, flag));
  //   addConditional(oneshot ? new LatchFlagSet<Number>(CMP_LES, on, flag) : new FlagSetCondition<Number>(CMP_LES, on, flag));
  //   addConditional(oneshot ? new LatchFlagSet<Number>(CMP_GTR, off, flag, true) : new FlagSetCondition<Number>(CMP_GTR, off, flag, true));
  // } else {
    // "cooler"
    // return addConditional(new BangBangFlagSet(off, on, flag));
  //   addConditional(oneshot ? new LatchFlagSet<Number>(CMP_GTR, on, flag) : new FlagSetCondition<Number>(CMP_GTR, on, flag));
  //   addConditional(oneshot ? new LatchFlagSet<Number>(CMP_LES, off, flag, true) : new FlagSetCondition<Number>(CMP_LES, off, flag, true));
  // }
}

template <> ConditionalCallback<Number>* State<Number>::addLatchingConditional(const Number& on, const Number& off, const void* cb, callback_type_t cbtype) {
  // if(on < off) {
    // "heater"
    return addConditional(new BangBangConditional(on, off, cb, cbtype));
    // addConditional(oneshot ? new OneShotConditional<Number>(CMP_LES, on, cb, cbtype) : new ConditionalCallback<Number>(CMP_LES, on, cb, cbtype));
    // addConditional(oneshot ? new OneShotConditional<Number>(CMP_GTR, off, cb, cbtype, true) : new ConditionalCallback<Number>(CMP_GTR, off, cb, cbtype, true));
  // } else {
    // "cooler"
    // return addConditional(new BangBangConditional(off, on, cb, cbtype));
    // addConditional(oneshot ? new OneShotConditional<Number>(CMP_GTR, on, cb, cbtype) : new ConditionalCallback<Number>(CMP_GTR, on, cb, cbtype));
    // addConditional(oneshot ? new OneShotConditional<Number>(CMP_LES, off, cb, cbtype, true) : new ConditionalCallback<Number>(CMP_LES, off, cb, cbtype, true));
  // }
}

// BOOLEAN HYSTERESIS = STEP CHANGE FILTER

template <> ConditionalCallback<bool>* State<bool>::addLatchingConditional(const bool& on, const bool& off, const void* cb, callback_type_t cbtype) {
  return addConditional(new LatchingConditional(cb, cbtype, (char)on < (char)off));
}

template <> ConditionalCallback<bool>* State<bool>::addLatchingSetFlag(const bool& on, const bool& off, Flag* flag) {
  addConditional(new LatchingFlagSet(flag, (char)on < (char)off));
}

// OTHER ARGS

template <typename T> ConditionalCallback<T>* State<T>::addLatchingConditional(const T& on, const T& off, void (* const cb)(bool comp)) { addLatchingConditional(on, off, (const void*)cb, CB_COMP); }

template <typename T> ConditionalCallback<T>* State<T>::addLatchingConditional(const T& on, const T& off, void (* const cb)(bool comp, T val)) { addLatchingConditional(on, off, (const void*)cb, CB_COMPVAL); }

template <typename T> ConditionalCallback<T>* State<T>::addLatchingConditional(const T& on, const T& off, void (* const cb)(bool comp, T val, T ref)) { addLatchingConditional(on, off, (const void*)cb, CB_COMPVALREF); }

// template <typename T> void State<T>::reset(T val) { set(defaultValue); }

Variable::Variable(const Number& value) : State<Number>(value) { }
Variable::Variable(const double& value) : Variable(Number(value)) { }
Variable::Variable(const float& value) : Variable(Number(value)) { }
Variable::Variable(const long& value) : Variable(Number(value)) { }
Variable::Variable(const int& value) : Variable(Number((long)value)) { }
Variable::Variable(const unsigned long& value) : Variable(Number(value)) { }
Variable::Variable(const unsigned int& value) : Variable(Number((unsigned long)value)) { }

Flag::Flag(const bool& value) : State<bool>(value) { }

#ifdef DEBUG

template <> void stateDebug<Number>(const Number& val) {
  if(val.isNaN()) {
    DEBUG.print("NaN");
  } else {
    DEBUG.print(val.operator double());
  }
}

template <> void stateDebug<bool>(const bool& val) {
  DEBUG.print(val ? "true" : "false");
}

#endif

#endif