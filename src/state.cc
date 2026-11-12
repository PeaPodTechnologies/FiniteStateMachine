#define FSM_SRC 1

#include <state.h>

#include <statelinker.h>
#include <comparators.h>
#include <hashtable.h>
#include <number.h>
#include <stdlib.h>

using namespace FSM;

namespace {
  template <typename T> const void* allocateOwnedValue(const T& value) {
    T* pointer = (T*)malloc(sizeof(T));
    if(pointer == nullptr) return nullptr;
    *pointer = value;
    return pointer;
  }
}

CallbackArgs::CallbackArgs() : owned(false), value(nullptr), type(ARGS_NONE) { }

CallbackArgs::CallbackArgs(const void* value, args_type_t type) : owned(false), value(value), type(type) { }

CallbackArgs::CallbackArgs(const bool& value) : owned(false), value(allocateOwnedValue(value)), type(ARGS_BOOL) { this->owned = (this->value != nullptr); }
CallbackArgs::CallbackArgs(const char& value) : owned(false), value(allocateOwnedValue(value)), type(ARGS_CHAR) { this->owned = (this->value != nullptr); }
CallbackArgs::CallbackArgs(const short& value) : owned(false), value(allocateOwnedValue(value)), type(ARGS_SHORT) { this->owned = (this->value != nullptr); }
CallbackArgs::CallbackArgs(const int& value) : owned(false), value(allocateOwnedValue(value)), type(ARGS_INT) { this->owned = (this->value != nullptr); }
CallbackArgs::CallbackArgs(const long& value) : owned(false), value(allocateOwnedValue(value)), type(ARGS_LONG) { this->owned = (this->value != nullptr); }
CallbackArgs::CallbackArgs(const unsigned char& value) : owned(false), value(allocateOwnedValue(value)), type(ARGS_UCHAR) { this->owned = (this->value != nullptr); }
CallbackArgs::CallbackArgs(const unsigned short& value) : owned(false), value(allocateOwnedValue(value)), type(ARGS_USHORT) { this->owned = (this->value != nullptr); }
CallbackArgs::CallbackArgs(const unsigned int& value) : owned(false), value(allocateOwnedValue(value)), type(ARGS_UINT) { this->owned = (this->value != nullptr); }
CallbackArgs::CallbackArgs(const unsigned long& value) : owned(false), value(allocateOwnedValue(value)), type(ARGS_ULONG) { this->owned = (this->value != nullptr); }
CallbackArgs::CallbackArgs(const float& value) : owned(false), value(allocateOwnedValue(value)), type(ARGS_FLOAT) { this->owned = (this->value != nullptr); }
CallbackArgs::CallbackArgs(const double& value) : owned(false), value(allocateOwnedValue(value)), type(ARGS_DOUBLE) { this->owned = (this->value != nullptr); }

CallbackArgs::CallbackArgs(const CallbackArgs& other) : owned(false), value(nullptr), type(ARGS_NONE) {
  copyFrom(other);
}

CallbackArgs& CallbackArgs::operator=(const CallbackArgs& other) {
  if(this == &other) return *this;

  clearOwnedValue();
  copyFrom(other);
  return *this;
}

const void* CallbackArgs::getPointer(args_type_t* typeOut) const {
  if(typeOut != nullptr) (*typeOut) = this->type;
  return this->value;
}

bool CallbackArgs::get(bool& out) const {
  if(this->type != ARGS_BOOL || this->value == nullptr) return false;
  out = *(const bool*)this->value;
  return true;
}

bool CallbackArgs::get(char& out) const {
  if(this->type != ARGS_CHAR || this->value == nullptr) return false;
  out = *(const char*)this->value;
  return true;
}

bool CallbackArgs::get(short& out) const {
  if(this->type != ARGS_SHORT || this->value == nullptr) return false;
  out = *(const short*)this->value;
  return true;
}

bool CallbackArgs::get(int& out) const {
  if(this->type != ARGS_INT || this->value == nullptr) return false;
  out = *(const int*)this->value;
  return true;
}

bool CallbackArgs::get(long& out) const {
  if(this->type != ARGS_LONG || this->value == nullptr) return false;
  out = *(const long*)this->value;
  return true;
}

bool CallbackArgs::get(unsigned char& out) const {
  if(this->type != ARGS_UCHAR || this->value == nullptr) return false;
  out = *(const unsigned char*)this->value;
  return true;
}

bool CallbackArgs::get(unsigned short& out) const {
  if(this->type != ARGS_USHORT || this->value == nullptr) return false;
  out = *(const unsigned short*)this->value;
  return true;
}

bool CallbackArgs::get(unsigned int& out) const {
  if(this->type != ARGS_UINT || this->value == nullptr) return false;
  out = *(const unsigned int*)this->value;
  return true;
}

bool CallbackArgs::get(unsigned long& out) const {
  if(this->type != ARGS_ULONG || this->value == nullptr) return false;
  out = *(const unsigned long*)this->value;
  return true;
}

bool CallbackArgs::get(float& out) const {
  if(this->type != ARGS_FLOAT || this->value == nullptr) return false;
  out = *(const float*)this->value;
  return true;
}

bool CallbackArgs::get(double& out) const {
  if(this->type != ARGS_DOUBLE || this->value == nullptr) return false;
  out = *(const double*)this->value;
  return true;
}

bool CallbackArgs::get(callback_args_compound_values_t& out) const {
  (void)out;
  return false;
}

CallbackArgs::~CallbackArgs() {
  clearOwnedValue();
}

void CallbackArgs::clearOwnedValue() {
  if(this->owned && this->value != nullptr) free((void*)this->value);
  this->value = nullptr;
  this->type = ARGS_NONE;
  this->owned = false;
}

void CallbackArgs::copyFrom(const CallbackArgs& other) {
  this->type = other.type;

  if(other.owned) {
    this->value = cloneValue(other.value, other.type);
    this->owned = (this->value != nullptr);
  } else {
    this->value = other.value;
    this->owned = false;
  }
}

const void* CallbackArgs::cloneValue(const void* source, args_type_t type) {
  if(source == nullptr) return nullptr;

  switch(type) {
    case ARGS_BOOL: return allocateOwnedValue(*(const bool*)source);
    case ARGS_CHAR: return allocateOwnedValue(*(const char*)source);
    case ARGS_SHORT: return allocateOwnedValue(*(const short*)source);
    case ARGS_INT: return allocateOwnedValue(*(const int*)source);
    case ARGS_LONG: return allocateOwnedValue(*(const long*)source);
    case ARGS_UCHAR: return allocateOwnedValue(*(const unsigned char*)source);
    case ARGS_USHORT: return allocateOwnedValue(*(const unsigned short*)source);
    case ARGS_UINT: return allocateOwnedValue(*(const unsigned int*)source);
    case ARGS_ULONG: return allocateOwnedValue(*(const unsigned long*)source);
    case ARGS_FLOAT: return allocateOwnedValue(*(const float*)source);
    case ARGS_DOUBLE: return allocateOwnedValue(*(const double*)source);
    default: return nullptr;
  }
}

// template <> String State<bool>::toString(const bool& val) { return (val ? _F("true") : _F("false")); }
// template <> String State<bool>::toString(void) { return State<bool>::toString(this->value); }

// template <> String State<Number>::toString(const Number& val) { return val.toString(); }
// template <> String State<Number>::toString(void) { return State<Number>::toString(this->value); }

// template <> String State<fsm_timestamp_t>::toString(const fsm_timestamp_t& val) { return timestampToString(val); }
// template <> String State<fsm_timestamp_t>::toString(void) { return State<fsm_timestamp_t>::toString(this->value); }

template <> ConditionalCallback<Number>* State<Number>::addCallback(typename ConditionalCallback<Number>::cb_compval_t cb) { return addConditional(new ConditionalCallback<Number>(this->key, CMP_NEQ, Number(), cb)); }

template <> ConditionalCallback<fsm_timestamp_t>* State<fsm_timestamp_t>::addCallback(typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb) { return addConditional(new ConditionalCallback<fsm_timestamp_t>(this->key, CMP_NEQ, (fsm_timestamp_t)-1, cb)); }

template <> ConditionalCallback<bool>* State<bool>::addCallback(typename ConditionalCallback<bool>::cb_compval_t cb) { return addConditional(new ConditionalCallback<bool>(this->key, CMP_NOP, false, cb)); }

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

CallbackArgsVariable::CallbackArgsVariable(const Variable& source) : CallbackArgs((const void*)&source, ARGS_VOID_PTR), source(&source) { }

bool CallbackArgsVariable::get(char& out) const {
  if(this->source == nullptr) return false;
  out = (char)(double)this->source->get();
  return true;
}

bool CallbackArgsVariable::get(short& out) const {
  if(this->source == nullptr) return false;
  out = (short)(double)this->source->get();
  return true;
}

bool CallbackArgsVariable::get(int& out) const {
  if(this->source == nullptr) return false;
  out = (int)(double)this->source->get();
  return true;
}

bool CallbackArgsVariable::get(long& out) const {
  if(this->source == nullptr) return false;
  out = (long)(double)this->source->get();
  return true;
}

bool CallbackArgsVariable::get(unsigned char& out) const {
  if(this->source == nullptr) return false;
  out = (unsigned char)(double)this->source->get();
  return true;
}

bool CallbackArgsVariable::get(unsigned short& out) const {
  if(this->source == nullptr) return false;
  out = (unsigned short)(double)this->source->get();
  return true;
}

bool CallbackArgsVariable::get(unsigned int& out) const {
  if(this->source == nullptr) return false;
  out = (unsigned int)(double)this->source->get();
  return true;
}

bool CallbackArgsVariable::get(unsigned long& out) const {
  if(this->source == nullptr) return false;
  out = (unsigned long)(double)this->source->get();
  return true;
}

bool CallbackArgsVariable::get(float& out) const {
  if(this->source == nullptr) return false;
  out = (float)(double)this->source->get();
  return true;
}

bool CallbackArgsVariable::get(double& out) const {
  if(this->source == nullptr) return false;
  out = (double)this->source->get();
  return true;
}

CallbacksArgsFlag::CallbacksArgsFlag(const Flag& source) : CallbackArgs((const void*)&source, ARGS_VOID_PTR), source(&source) { }

bool CallbacksArgsFlag::get(bool& out) const {
  if(this->source == nullptr) return false;
  out = this->source->get();
  return true;
}

CallbackArgsCompound::CallbackArgsCompound() : CallbackArgs(nullptr, ARGS_COMPOUND), variables(false), flags(false) { }

CallbackArgsCompound::~CallbackArgsCompound() {
  if(this->variableValues != nullptr) {
    delete[] this->variableValues;
    this->variableValues = nullptr;
  }
  if(this->flagValues != nullptr) {
    delete[] this->flagValues;
    this->flagValues = nullptr;
  }
  this->flagValuesCount = 0;
  this->variableValuesCount = 0;
}

bool CallbackArgsCompound::get(callback_args_compound_values_t& out) const {
  const unsigned int currentFlagCount = this->flags.size();
  const unsigned int currentVariableCount = this->variables.size();

  if(currentFlagCount != this->flagValuesCount) {
    if(this->flagValues != nullptr) delete[] this->flagValues;
    this->flagValues = (currentFlagCount > 0) ? new bool[currentFlagCount] : nullptr;
    this->flagValuesCount = currentFlagCount;
  }

  if(currentVariableCount != this->variableValuesCount) {
    if(this->variableValues != nullptr) delete[] this->variableValues;
    this->variableValues = (currentVariableCount > 0) ? new Number[currentVariableCount] : nullptr;
    this->variableValuesCount = currentVariableCount;
  }

  if(currentFlagCount > 0 && this->flagValues == nullptr) return false;
  if(currentVariableCount > 0 && this->variableValues == nullptr) return false;

  for(unsigned int i = 0; i < currentFlagCount; i++) {
    Flag* flag = this->flags.getByFirstOccurrence((uint8_t)i);
    if(flag == nullptr) continue;
    this->flagValues[i] = flag->get();
  }

  for(unsigned int i = 0; i < currentVariableCount; i++) {
    Variable* variable = this->variables.getByFirstOccurrence((uint8_t)i);
    if(variable == nullptr) continue;
    this->variableValues[i] = variable->get();
  }

  out.flags = this->flagValues;
  out.flagCount = currentFlagCount;
  out.variables = this->variableValues;
  out.variableCount = currentVariableCount;
  return true;
}

void CallbackArgsCompound::addVariable(const char* key, Variable* variable, bool overwrite) {
  if(key == nullptr || variable == nullptr) return;
  this->variables.set(key, variable, overwrite);
}

void CallbackArgsCompound::addFlag(const char* key, Flag* flag, bool overwrite) {
  if(key == nullptr || flag == nullptr) return;
  this->flags.set(key, flag, overwrite);
}

Variable* CallbackArgsCompound::getVariable(const char* key) const {
  if(key == nullptr) return nullptr;
  return this->variables[key];
}

Flag* CallbackArgsCompound::getFlag(const char* key) const {
  if(key == nullptr) return nullptr;
  return this->flags[key];
}