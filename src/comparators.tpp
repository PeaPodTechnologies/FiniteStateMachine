#ifndef FSM_COMPARATORS_T_
#define FSM_COMPARATORS_T_

#include <number.h>

#define BOOLSTR(b) (b ? "true" : "false")

// TODO: All T args in callbacks -> const T&
// TODO: Replace all OneShot with Latching (duh)

// Helper Functions
const char* parseComparator(const comparators_t& cmp, bool result = true);

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, const void* cb, callback_type_t cbtype, bool invert, T (* const getReference)(T val)) : comparator(cmp), reference(ref), callback(cb), callbacktype(cbtype), invert(invert), getReference(getReference) {
  #ifdef DEBUG
    DEBUG_DELAY();
    DEBUG.print("Callback Created: X ");
    DEBUG.print(parseComparator(cmp));
    DEBUG.print(" ");
    DEBUG.print(ref);
    if(invert) DEBUG.print(" (Inverted) ");
    if(getReference != nullptr) DEBUG.print(" (Dynamic Ref) ");
    DEBUG.print("\n");
    DEBUG_DELAY();
  #endif
}

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, void (* const cb)(), bool invert, T (* const getReference)(T val)) : ConditionalCallback(cmp, ref, (const void*)cb, CB_NONE, invert, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, void (* const cb)(bool comp), bool invert, T (* const getReference)(T val)) : ConditionalCallback(cmp, ref, (const void*)cb, CB_COMP, invert, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, void (* const cb)(bool comp, T val), bool invert, T (* const getReference)(T val)) : ConditionalCallback(cmp, ref, (const void*)cb, CB_COMPVAL, invert, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, void (* const cb)(bool comp, T val, T ref), bool invert, T (* const getReference)(T val)) : ConditionalCallback(cmp, ref, (const void*)cb, CB_COMPVALREF, invert, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, bool invert, T (* const getReference)(T val)) : ConditionalCallback(cmp, ref, nullptr, CB_COMP, invert, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, const void* cb, callback_type_t cbtype, T (* const getReference)(T val)) : comparator(cmp), reference(ref), callback(cb), callbacktype(cbtype), getReference(getReference) {
  #ifdef DEBUG
    DEBUG_DELAY();
    DEBUG.print("Callback Created: X ");
    DEBUG.print(parseComparator(cmp));
    DEBUG.print(" ");
    DEBUG.print(ref);
    if(getReference != nullptr) DEBUG.print(" (Dynamic Ref) ");
    DEBUG.print("\n");
    DEBUG_DELAY();
  #endif
}

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, void (* const cb)(), T (* const getReference)(T val)) : ConditionalCallback(cmp, ref, (const void*)cb, CB_NONE, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, void (* const cb)(bool comp), T (* const getReference)(T val)) : ConditionalCallback(cmp, ref, (const void*)cb, CB_COMP, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, void (* const cb)(bool comp, T val), T (* const getReference)(T val)) : ConditionalCallback(cmp, ref, (const void*)cb, CB_COMPVAL, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, void (* const cb)(bool comp, T val, T ref), T (* const getReference)(T val)) : ConditionalCallback(cmp, ref, (const void*)cb, CB_COMPVALREF, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, T (* const getReference)(T val)) : ConditionalCallback(cmp, ref, nullptr, CB_COMP, getReference) { }

template <typename T> ConditionalCallback<T>& ConditionalCallback<T>::operator=(ConditionalCallback<T> const& other) { return ConditionalCallback<T>(other.comparator, other.reference, other.callback, other.callbacktype, other.getReference); }

template <typename T> const T& ConditionalCallback<T>::childReference(const T& val) { return this->reference; }

template <typename T> void ConditionalCallback<T>::operator()(const T& val) { 
  // Update the reference value
  if (this->getReference != nullptr) {
    this->reference = this->getReference(val);
    #ifdef DEBUG
      DEBUG_DELAY();
      DEBUG.print("Got Reference: ");
      DEBUG.print(this->reference);
      DEBUG.print("\n");
      DEBUG_DELAY();
    #endif
  } else {
    this->reference = this->childReference(val);
    #ifdef DEBUG
      DEBUG_DELAY();
      DEBUG.print("Got Child Reference: ");
      DEBUG.print(this->reference);
      DEBUG.print("\n");
      DEBUG_DELAY();
    #endif
  }
  this->callOperator(val, this->reference);
}

template <typename T> void ConditionalCallback<T>::operator()(const T& val, const T& ref) { this->callOperator(val, ref); }

template <typename T> void ConditionalCallback<T>::setComparator(comparators_t cmp) {
  #ifdef DEBUG
    DEBUG_DELAY();
    DEBUG.print("Comparator Change: ");
    DEBUG.print(parseComparator(cmp));
    DEBUG.print("\n");
    DEBUG_DELAY();
  #endif
  this->comparator = cmp;
}

template <typename T> void ConditionalCallback<T>::callOperator(const T& val, const T& ref) {
  bool result = ConditionalCallback<T>::compare(this->comparator, val, ref);

  if(result) this->executeCallback(val, ref);
}

template <typename T> void ConditionalCallback<T>::executeCallback(const T& val, const T& ref) {
  #ifdef DEBUG
    DEBUG_DELAY();
    DEBUG.print(invert ? "Callback! (Inverted)" : "Callback!");
    DEBUG.print("\n");
    DEBUG_DELAY();
  #endif

  bool _ = this->invert ? false : true;

  // #ifdef DEBUG
  //   DEBUG_DELAY();
  //   DEBUG.print("Passing ");
  //   DEBUG.print(_ ? "true" : "false");
  //   DEBUG.print("\n");
  //   DEBUG_DELAY();
  // #endif

  if(this->callback == nullptr) {
    // Assumes childCallback
    #ifdef DEBUG
      DEBUG_DELAY();
      DEBUG.print("No Callback - Calling Child: ");
      DEBUG_DELAY();
    #endif
    this->childCallback(_, val, ref);
    return;
  };
  
  // Cast void* cb fn pointer to appropriate type and call
  switch(this->callbacktype) {
    case CB_NONE:
      // Called with no args
      ((void (*)(void))this->callback)();
      break;
    case CB_COMP:
      ((void (*)(bool))this->callback)(_);
      break;
    case CB_COMPVAL:
      ((void (*)(bool, T))this->callback)(_, val);
      break;
    case CB_COMPVALREF:
      ((void (*)(bool, T, T))this->callback)(_, val, ref);
      break;
  }
}

template <> bool ConditionalCallback<bool>::compare(comparators_t cmp, const bool& val, const bool& ref) {
  bool result = false;
  char ival = (val ? 1 : 0);
  char iref = (ref ? 1 : 0);

  switch(cmp) {
    case CMP_EQU:
      result = (ival == iref);
      break;
    case CMP_NEQ:
      result = !(ival == iref);
      break;
    case CMP_LES:
      result = (ival < iref);
      break;
    case CMP_LEQ:
      result = (ival <= iref);
      break;
    case CMP_GTR:
      result = (ival > iref);
      break;
    case CMP_GEQ:
      result = (ival >= iref);
      break;
    default:
      result = NOP_COMP_BOOL;
      break;
  }
  #ifdef DEBUG
    DEBUG_DELAY();
    DEBUG.print("Flag (");
    DEBUG.print(BOOLSTR(val));
    DEBUG.print(" | ");
    DEBUG.print((int)ival);
    DEBUG.print(") ");
    DEBUG.print(parseComparator(cmp, result));
    DEBUG.print(" Ref (");
    DEBUG.print(BOOLSTR(ref));
    DEBUG.print(" | ");
    DEBUG.print((int)iref);
    DEBUG.print(")\n");
    DEBUG_DELAY();
  #endif
  return result;
}

template <> bool ConditionalCallback<Number>::compare(comparators_t cmp, const Number& val, const Number& ref) {
  if(ref.isNaN()) {
    #ifdef DEBUG
      DEBUG_DELAY();
      DEBUG.print("[Variable Ref NaN - Always True]\n");
      DEBUG_DELAY();
    #endif
    return true;
  }

  bool result = false;
  switch(cmp) {
    case CMP_EQU:
      result = (val == ref);
      break;
    case CMP_NEQ:
      result = (val != ref);
      break;
    case CMP_LES:
      result = (val < ref);
      break;
    case CMP_LEQ:
      result = (val <= ref);
      break;
    case CMP_GTR:
      result = (val > ref);
      break;
    case CMP_GEQ:
      result = (val >= ref);
      break;
    default:
      result = NOP_COMP_NUMBER;
      break;
  }
  #ifdef DEBUG
    DEBUG_DELAY();
    DEBUG.print("Variable (");
    DEBUG.print((double)val);
    DEBUG.print(") ");
    DEBUG.print(parseComparator(cmp, result));
    DEBUG.print(" Ref (");
    DEBUG.print((double)ref);
    DEBUG.print(")\n");
    DEBUG_DELAY();
  #endif
  return result;
}

BangBangConditional::BangBangConditional(const Number& lo, const Number& hi, const void* rising, const void* falling, callback_type_t cbtype) : low(Number::minimum(lo, hi)), high(Number::maximum(lo, hi)), rising(rising), falling(falling), latchcbtype(cbtype), ConditionalCallback<Number>(CMP_LES, low, (double)hi < (double)lo) { 
  #ifdef DEBUG
    DEBUG_DELAY();
    DEBUG.print("BangBang [");
    DEBUG.print((double)lo);
    DEBUG.print(", ");
    DEBUG.print((double)hi);
    DEBUG.print("]\n");
    DEBUG_DELAY();
  #endif
}

BangBangConditional::BangBangConditional(const Number& lo, const Number& hi, void (* const rising)(void), void (* const falling)(void)) : BangBangConditional(lo, hi, (const void*)rising, (const void*)falling, CB_NONE) {  }

BangBangConditional::BangBangConditional(const Number& lo, const Number& hi, void (* const cb)(void)) : BangBangConditional(lo, hi, (const void*)cb, (const void*)cb, CB_NONE) { }

BangBangConditional::BangBangConditional(const Number& lo, const Number& hi, const void* cb, callback_type_t cbtype) : BangBangConditional(lo, hi, (const void*)cb, (const void*)cb, cbtype) { }

void BangBangConditional::childCallback(bool comp, const Number& val, const Number& ref) {
  this->state = (ref == this->high);

  const void* fn = this->state ? this->rising : this->falling;
  #ifdef DEBUG
    if(this->state) {
      DEBUG_DELAY();
      DEBUG.print("BangBang (High)\n");
      DEBUG_DELAY();
    } else {
      DEBUG_DELAY();
      DEBUG.print("BangBang (Low)\n");
      DEBUG_DELAY();
    }
  #endif

  // state ? comp : !comp;
  // T XOR T = T
  // F XOR T = F
  // T XOR F = F
  // F XOR F = T
  // comp = !(comp ^ this->state);

  bool _  = !(this->invert ^ this->state);

  // Cast void* cb fn pointer to appropriate type and call
  if(fn != nullptr){
    switch(this->callbacktype) {
      case CB_NONE:
        // Called with no args
        ((void (*)(void))fn)();
        break;
      case CB_COMP:
        ((void (*)(bool))fn)(_);
        break;
      case CB_COMPVAL:
        ((void (*)(bool, bool))fn)(_, val);
        break;
      case CB_COMPVALREF:
        ((void (*)(bool, bool, bool))fn)(_, val, ref);
        break;
    }
  }
}

const Number& BangBangConditional::childReference(const Number& val) {
  if (this->state && val <= this->low) {
    this->setComparator(CMP_LES);
    this->triggered = false;
    this->state = false;
  } else if (!this->state && val >= this->high) {
    this->setComparator(CMP_GTR);
    this->triggered = false;
    this->state = true;
  }
  return this->state ? this->high : this->low;
}

void BangBangConditional::callOperator(const Number& val, const Number& ref) {
  bool result = compare(this->comparator, val, ref);
  if(!this->triggered && result) {
    #ifdef DEBUG
      DEBUG_DELAY();
      DEBUG.print("BangBang Triggered\n");
      DEBUG_DELAY();
    #endif
    this->triggered = true;
    this->executeCallback(val, ref);
  } else if(!result) {
    // Reset
    #ifdef DEBUG
      DEBUG_DELAY();
      DEBUG.print("BangBang Trigger Reset!\n");
      DEBUG_DELAY();
    #endif
    this->triggered = false;
  }
}

LatchingConditional::LatchingConditional(const void* rising, const void* falling, callback_type_t cbtype, bool invert) : rising(rising), falling(falling), latchcbtype(cbtype), ConditionalCallback<bool>(CMP_EQU, !invert, invert) { }

LatchingConditional::LatchingConditional(void (* const rising)(void), void (* const falling)(void), bool invert) : LatchingConditional((const void*)rising, (const void*)falling, CB_NONE, invert) { }

LatchingConditional::LatchingConditional(const void* cb, callback_type_t cbtype, bool invert) : LatchingConditional(cb, cb, cbtype, invert) { }

LatchingConditional::LatchingConditional(void (* const cb)(void), bool invert) : LatchingConditional((const void*)cb, CB_NONE, invert) { }

void LatchingConditional::callOperator(const bool& val, const bool& ref) {
  bool result = compare(this->comparator, val, ref);
  #ifdef DEBUG
    if(!result && this->triggered) {
      DEBUG_DELAY();
      DEBUG.print("Trigger Reset!\n");
      DEBUG_DELAY();
    }
  #endif
  if(!this->triggered && result) {
    this->triggered = true;
    this->executeCallback(val, ref);
  } else if(!result) {
    // Reset
    this->triggered = false;
  }
}

void LatchingConditional::childCallback(bool comp, const bool& val, const bool& ref) {
  this->state = !ref;
  this->triggered = false;

  const void* fn = val ? this->rising : this->falling;
  #ifdef DEBUG
    if(val) {
      DEBUG_DELAY();
      DEBUG.print("Latch Set (true)\n");
      DEBUG_DELAY();
    } else {
      DEBUG_DELAY();
      DEBUG.print("Latch Reset (false)\n");
      DEBUG_DELAY();
    }
  #endif

  // Cast void* cb fn pointer to appropriate type and call
  if(fn != nullptr){
    switch(this->callbacktype) {
      case CB_NONE:
        // Called with no args
        ((void (*)(void))fn)();
        break;
      case CB_COMP:
        ((void (*)(bool))fn)(comp);
        break;
      case CB_COMPVAL:
        ((void (*)(bool, bool))fn)(comp, val);
        break;
      case CB_COMPVALREF:
        ((void (*)(bool, bool, bool))fn)(comp, val, ref);
        break;
    }
  }
}

const char* parseComparator(const comparators_t& cmp, bool result) {
  switch(cmp) {
    case CMP_EQU:
      return result ? "==" : "!==";
    case CMP_NEQ:
      return result ? "!=" : "!!=";
    case CMP_LES:
      return result ? "<" : "!<";
    case CMP_LEQ:
      return result ? "<=" : "!<=";
    case CMP_GTR:
      return result ? ">" : "!>";
    case CMP_GEQ:
      return result ? ">=" : "!>=";
    default:
      return result ? "??" : "!??";
  }
}

#endif