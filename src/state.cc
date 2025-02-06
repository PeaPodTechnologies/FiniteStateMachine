#define FSM_SRC 1

#include <state.h>

#include <statelinker.h>
#include <comparators.h>
#include <number.h>

using namespace FSM;

// template <> String State<bool>::toString(const bool& val) { return (val ? _F("true") : _F("false")); }
// template <> String State<bool>::toString(void) { return State<bool>::toString(this->value); }

// template <> String State<Number>::toString(const Number& val) { return val.toString(); }
// template <> String State<Number>::toString(void) { return State<Number>::toString(this->value); }

// template <> String State<fsm_timestamp_t>::toString(const fsm_timestamp_t& val) { return timestampToString(val); }
// template <> String State<fsm_timestamp_t>::toString(void) { return State<fsm_timestamp_t>::toString(this->value); }

template <> ConditionalCallback<Number>* State<Number>::addLoggerCallback(typename ConditionalCallback<Number>::cb_compval_t cb) { return addConditional(new ConditionalCallback<Number>(this->key, CMP_NEQ, Number(), cb)); }

template <> ConditionalCallback<fsm_timestamp_t>* State<fsm_timestamp_t>::addLoggerCallback(typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb) { return addConditional(new ConditionalCallback<fsm_timestamp_t>(this->key, CMP_NEQ, (fsm_timestamp_t)-1, cb)); }

template <> ConditionalCallback<bool>* State<bool>::addLoggerCallback(typename ConditionalCallback<bool>::cb_compval_t cb) { return addConditional(new ConditionalCallback<bool>(this->key, CMP_NOP, false, cb)); }

template <> ConditionalCallback<Number>* State<Number>::addLatchingSetFlag(fsm_key_t key, const Number& on, const Number& off, Flag* flag) {
  return addConditional(new BangBangFlagSet(key, on, off, flag));
}

template <> ConditionalCallback<Number>* State<Number>::addLatchingSetFlag(const Number& on, const Number& off, Flag* flag) {
  return addLatchingSetFlag(this->key, on, off, flag);
}

template <> ConditionalCallback<Number>* State<Number>::addLatchingConditional(fsm_key_t key, const Number& on, const Number& off, const void* cb, callback_type_t cbtype) {
  return addConditional(new BangBangConditional(key, on, off, cb, cbtype));
}
template <> ConditionalCallback<Number>* State<Number>::addLatchingConditional(const Number& on, const Number& off, const void* cb, callback_type_t cbtype) {
  return addLatchingConditional(this->key, on, off, cb, cbtype);
}

template <> ConditionalCallback<fsm_timestamp_t>* State<fsm_timestamp_t>::addLatchingSetFlag(fsm_key_t key, const fsm_timestamp_t& on, const fsm_timestamp_t& off, Flag* flag) {
  return (ConditionalCallback<fsm_timestamp_t>*)(new BangBangFlagSet(key, Number(on), Number(off), flag));
}

template <> ConditionalCallback<fsm_timestamp_t>* State<fsm_timestamp_t>::addLatchingSetFlag(const fsm_timestamp_t& on, const fsm_timestamp_t& off, Flag* flag) {
  return addLatchingSetFlag(this->key, on, off, flag);
}


template <> ConditionalCallback<fsm_timestamp_t>* State<fsm_timestamp_t>::addLatchingConditional(fsm_key_t key, const fsm_timestamp_t& on, const fsm_timestamp_t& off, const void* cb, callback_type_t cbtype) {
  return (ConditionalCallback<fsm_timestamp_t>*)(new BangBangConditional(key, Number(on), Number(off), cb, cbtype));
}

template <> ConditionalCallback<fsm_timestamp_t>* State<fsm_timestamp_t>::addLatchingConditional(const fsm_timestamp_t& on, const fsm_timestamp_t& off, const void* cb, callback_type_t cbtype) {
  return addLatchingConditional(this->key, on, off, cb, cbtype);
}


// BOOLEAN HYSTERESIS = STEP CHANGE FILTER

template <> ConditionalCallback<bool>* State<bool>::addLatchingConditional(fsm_key_t key, const bool& on, const bool& off, const void* cb, callback_type_t cbtype) {
  return addConditional(new LatchingConditional(key, cb, cbtype, (char)on < (char)off));
}

template <> ConditionalCallback<bool>* State<bool>::addLatchingConditional(const bool& on, const bool& off, const void* cb, callback_type_t cbtype) {
  return addConditional(new LatchingConditional(this->key, cb, cbtype, (char)on < (char)off));
}


template <> ConditionalCallback<bool>* State<bool>::addLatchingSetFlag(fsm_key_t key, const bool& on, const bool& off, Flag* flag) {
  return addConditional(new LatchingFlagSet(key, flag, (char)on < (char)off));
}

template <> ConditionalCallback<bool>* State<bool>::addLatchingSetFlag(const bool& on, const bool& off, Flag* flag) {
  return addLatchingSetFlag(this->key, on, off, flag);
}


Variable::Variable(const Number& value) : State<Number>(value) { }
Variable::Variable(const double& value) : Variable(Number(value)) { }
Variable::Variable(const float& value) : Variable(Number(value)) { }
Variable::Variable(const long& value) : Variable(Number(value)) { }
Variable::Variable(const int& value) : Variable(Number((long)value)) { }
Variable::Variable(const unsigned long& value) : Variable(Number(value)) { }
Variable::Variable(const unsigned int& value) : Variable(Number((unsigned long)value)) { }

Variable::Variable(const Number& value, fsm_key_t key) : State<Number>(value, key) { }
Variable::Variable(const double& value, fsm_key_t key) : Variable(Number(value), key) { }
Variable::Variable(const float& value, fsm_key_t key) : Variable(Number(value), key) { }
Variable::Variable(const long& value, fsm_key_t key) : Variable(Number(value), key) { }
Variable::Variable(const int& value, fsm_key_t key) : Variable(Number((long)value), key) { }
Variable::Variable(const unsigned long& value, fsm_key_t key) : Variable(Number(value), key) { }
Variable::Variable(const unsigned int& value, fsm_key_t key) : Variable(Number((unsigned long)value), key) { }

Flag::Flag(const bool& value) : State<bool>(value) { }
Flag::Flag(fsm_key_t key) : State<bool>(false, key) { }
Flag::Flag(const bool& value, fsm_key_t key) : State<bool>(value, key) {}