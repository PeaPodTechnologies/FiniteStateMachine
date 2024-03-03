#include <comparators.h>

#include <debug.h>
#include <number.h>

// String FSM_KEY_NULL = String();
fsm_key_t FSM_KEY_NULL = nullptr;

template <> ConditionalCallback<bool>::~ConditionalCallback() { }
template <> ConditionalCallback<Number>::~ConditionalCallback() { }
template <> ConditionalCallback<fsm_timestamp_t>::~ConditionalCallback() { }

template <> String stateToString<bool>(const bool& val) { return val ? _F("true") : _F("false"); }
template <> String stateToString<Number>(const Number& val) { return val.toString(); }
template <> String stateToString<fsm_timestamp_t>(const fsm_timestamp_t& val) { return timestampToString(val); }

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
  #ifdef DEBUG_JSON
    #ifdef DEBUG_USE_BP
      BP_JSON();
    #else
      String m = _F("Flag (");
      m += stateToString(val);
      m += " | ";
      m += (int)ival;
      m += ") ";
      m += parseComparator(cmp, result);
      m += _F(" Ref (");
      m += stateToString(ref);
      m += " | ";
      m += (int)iref;
      m += ')';
      DEBUG_JSON(m);
      // Serial.print(">>>");
      // Serial.println(m);
    #endif
  #endif
  return result;
}

template <> bool ConditionalCallback<Number>::compare(comparators_t cmp, const Number& val, const Number& ref) {
  if(ref.isNaN()) {
    // #ifdef FSM_DEBUG_SERIAL
    //   DEBUG_DELAY();
    //   FSM_DEBUG_SERIAL.print(_F("[Variable Ref NaN - Always True]\n"));
    //   DEBUG_DELAY();
    // #endif
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
  #ifdef DEBUG_JSON
    #ifdef DEBUG_USE_BP
      BP_JSON();
    #else
      String m = _F("Variable (");
      m += val.toString();
      m += ") ";
      m += parseComparator(cmp, result);
      m += _F(" Ref (");
      m += ref.toString();
      m += ')';
      DEBUG_JSON(m);
      // Serial.print(">>>");
      // Serial.println(m);
    #endif
  #endif
  return result;
}

template <> bool ConditionalCallback<fsm_timestamp_t>::compare(comparators_t cmp, const fsm_timestamp_t& val, const fsm_timestamp_t& ref) {
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
  #ifdef DEBUG_JSON
    #ifdef DEBUG_USE_BP
      BP_JSON();
    #else
      String m = _F("Timestamp (");
      m += timestampToString(val);
      m += ") ";
      m += parseComparator(cmp, result);
      m += _F(" Ref (");
      m += timestampToString(ref);
      m += ')';
      DEBUG_JSON(m);
      // Serial.print(">>>");
      // Serial.println(m);
    #endif
  #endif
  return result;
}

BangBangConditional::BangBangConditional(const Number& lo, const Number& hi, const void* rising, const void* falling, callback_type_t cbtype) : low(Number::minimum(lo, hi)), high(Number::maximum(lo, hi)), rising(rising), falling(falling), latchcbtype(cbtype), ConditionalCallback<Number>(CMP_LES, low, (double)hi < (double)lo) {
  #ifdef DEBUG_JSON
    #ifdef DEBUG_USE_BP
      BP_JSON();
    #else
      String m = _F("New BangBang [");
      m += (unsigned)lo;
      double l = lo;
      m += (unsigned)(((unsigned)(l * 100) - (unsigned)lo) / 100);
      m += ", ";
      m += (unsigned)hi;
      double h = hi;
      m += (unsigned)(((unsigned)(h * 100) - (unsigned)hi) / 100);
      m += ']';
      // DEBUG_JSON(m.c_str(), m.length());
      DEBUG_JSON(m);
    #endif
  #endif
}

BangBangConditional::BangBangConditional(const Number& lo, const Number& hi, cb_none_t rising, cb_none_t falling) : BangBangConditional(lo, hi, (const void*)rising, (const void*)falling, CB_NONE) {  }

BangBangConditional::BangBangConditional(const Number& lo, const Number& hi, cb_none_t cb) : BangBangConditional(lo, hi, (const void*)cb, (const void*)cb, CB_NONE) { }

BangBangConditional::BangBangConditional(const Number& lo, const Number& hi, const void* cb, callback_type_t cbtype) : BangBangConditional(lo, hi, (const void*)cb, (const void*)cb, cbtype) { }

// wITH KEYS

BangBangConditional::BangBangConditional(fsm_key_t key, const Number& lo, const Number& hi, const void* rising, const void* falling, callback_type_t cbtype) : low(Number::minimum(lo, hi)), high(Number::maximum(lo, hi)), rising(rising), falling(falling), latchcbtype(cbtype), ConditionalCallback<Number>(key, CMP_LES, low, (double)hi < (double)lo) {
  #ifdef DEBUG_JSON
    #ifdef DEBUG_USE_BP
      BP_JSON();
    #else
      String m = _F("New BangBang [");
      m += (unsigned)lo;
      double l = lo;
      m += (unsigned)(((unsigned)(l * 100) - (unsigned)lo) / 100);
      m += ", ";
      m += (unsigned)hi;
      double h = hi;
      m += (unsigned)(((unsigned)(h * 100) - (unsigned)hi) / 100);
      m += ']';
      // DEBUG_JSON(m.c_str(), m.length());
      DEBUG_JSON(m);
    #endif
  #endif
}

BangBangConditional::BangBangConditional(fsm_key_t key, const Number& lo, const Number& hi, cb_none_t rising, cb_none_t falling) : BangBangConditional(key, lo, hi, (const void*)rising, (const void*)falling, CB_NONE) {  }

BangBangConditional::BangBangConditional(fsm_key_t key, const Number& lo, const Number& hi, cb_none_t cb) : BangBangConditional(key, lo, hi, (const void*)cb, (const void*)cb, CB_NONE) { }

BangBangConditional::BangBangConditional(fsm_key_t key, const Number& lo, const Number& hi, const void* cb, callback_type_t cbtype) : BangBangConditional(key, lo, hi, (const void*)cb, (const void*)cb, cbtype) { }

void BangBangConditional::childCallback(bool comp, const Number& val, const Number& ref) {
  this->state = (ref == this->high);

  const void* fn = this->state ? this->rising : this->falling;
  #ifdef DEBUG_JSON
    #ifdef DEBUG_USE_BP
      BP_JSON();
    #else
      String m = _F("BangBang ");
      m += (this->state ? _F("High") : _F("Low"));
      // DEBUG_JSON(m.c_str(), m.length());
      DEBUG_JSON(m);
    #endif
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
        ((cb_none_t)fn)();
        break;
      case CB_COMP:
        ((cb_comp_t)fn)(_);
        break;
      case CB_COMPVAL:
        ((cb_compval_t)fn)(_, val);
        break;
      case CB_COMPVALREF:
        ((cb_compvalref_t)fn)(_, val, ref);
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
    #ifdef DEBUG_JSON
      #ifdef DEBUG_USE_BP
        BP_JSON();
      #else
        DEBUG_JSON(_F("BangBang Triggered"), 18);
      #endif
    #endif
    this->triggered = true;
    this->executeCallback(val, ref);
  } else if(!result) {
    // Reset
    #ifdef DEBUG_JSON
      #ifdef DEBUG_USE_BP
        BP_JSON();
      #else
        DEBUG_JSON(_F("BangBang Trigger Reset"), 22);
      #endif
    #endif
    this->triggered = false;
  }
}

LatchingConditional::LatchingConditional(const void* rising, const void* falling, callback_type_t cbtype, bool invert) : rising(rising), falling(falling), latchcbtype(cbtype), ConditionalCallback<bool>(CMP_EQU, !invert, nullptr, cbtype, invert) { } // old args: (CMP_EQU, !invert, invert)

// LatchingConditional::LatchingConditional(void const (* const rising)(void), void const (* const falling)(void), bool invert) : LatchingConditional((const void*)rising, (const void*)falling, CB_NONE, invert) { }

LatchingConditional::LatchingConditional(const void* cb, callback_type_t cbtype, bool invert) : LatchingConditional(cb, cb, cbtype, invert) { }

// LatchingConditional::LatchingConditional(void const (* const cb)(void), bool invert) : LatchingConditional((const void*)cb, CB_NONE, invert) { }

LatchingConditional::LatchingConditional(cb_comp_t rising, cb_comp_t falling, bool invert) : LatchingConditional((const void*)rising, (const void*)falling, CB_COMP, invert) { }
LatchingConditional::LatchingConditional(cb_comp_t cb, bool invert) : LatchingConditional((const void*)cb, CB_COMP, invert) { }

// WITH KEYs

LatchingConditional::LatchingConditional(fsm_key_t key, const void* rising, const void* falling, callback_type_t cbtype, bool invert) : rising(rising), falling(falling), latchcbtype(cbtype), ConditionalCallback<bool>(key, CMP_EQU, !invert, nullptr, cbtype, invert) { } // NOTE: Passing cbtype upward in this manner is of utmost importance

// LatchingConditional::LatchingConditional(fsm_key_t key, void const (* const rising)(void), void (* const falling)(void), bool invert) : LatchingConditional(key, (const void*)rising, (const void*)falling, CB_NONE, invert) { }

LatchingConditional::LatchingConditional(fsm_key_t key, const void* cb, callback_type_t cbtype, bool invert) : LatchingConditional(key, cb, cb, cbtype, invert) { }

// LatchingConditional::LatchingConditional(fsm_key_t key, void const (* const cb)(void), bool invert) : LatchingConditional(key, (const void*)cb, CB_NONE, invert) { }

LatchingConditional::LatchingConditional(fsm_key_t key, cb_comp_t rising, cb_comp_t falling, bool invert) : LatchingConditional(key, (const void*)rising, (const void*)falling, CB_COMP, invert) { }

LatchingConditional::LatchingConditional(fsm_key_t key, cb_comp_t cb, bool invert) : LatchingConditional(key, (const void*)cb, CB_COMP, invert) { }

void LatchingConditional::callOperator(const bool& val, const bool& ref) {
  bool result = compare(this->comparator, val, ref);
  if(!this->triggered && result) {
    #ifdef DEBUG_JSON
      #ifdef DEBUG_USE_BP
        BP_JSON();
      #else
        DEBUG_JSON(_F("Latch Triggered"), 15);
      #endif
    #endif
    this->triggered = true;
    this->executeCallback(val, ref);
  } else if(!result) {
    // Reset
    this->triggered = false;
    #ifdef DEBUG_JSON
      #ifdef DEBUG_USE_BP
        BP_JSON();
      #else
        DEBUG_JSON(_F("Latch Trigger Reset"), 19);
      #endif
    #endif
  }
}

void LatchingConditional::childCallback(bool comp, const bool& val, const bool& ref) {
  this->state = !ref;
  this->triggered = false;

  const void* fn = val ? this->rising : this->falling;
  #ifdef DEBUG_JSON
    #ifdef DEBUG_USE_BP
      BP_JSON();
    #else
      if(val) {
        DEBUG_JSON(_F("Latch Set 1"), 11);
      } else {
        DEBUG_JSON(_F("Latch Reset 0"), 13);
      }
    #endif
  #endif

  // Cast void* cb fn pointer to appropriate type and call
  if(fn != nullptr){
    switch(this->callbacktype) {
      case CB_NONE:
        // Called with no args
        ((cb_none_t)fn)();
        break;
      case CB_COMP:
        ((cb_comp_t)fn)(comp);
        break;
      case CB_COMPVAL:
        ((cb_compval_t)fn)(comp, val);
        break;
      case CB_COMPVALREF:
        ((cb_compvalref_t)fn)(comp, val, ref);
        break;
    }
  }
}

const bool& LatchingConditional::childReference(const bool& val) { return this->state; }

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