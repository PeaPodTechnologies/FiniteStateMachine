#include <statelinker.h>

#include <debug.h>
#include <state.h>
#include <comparators.h>
#include <number.h>

BangBangFlagSet::BangBangFlagSet(const Number& lo, const Number& hi, Flag* flag) : low(Number::minimum(lo, hi)), high(Number::maximum(lo, hi)), FlagSetCondition<Number>(CMP_LES, low, flag, (double)hi < (double)lo) {
  // #ifdef DEBUG_JSON
  //   #ifdef DEBUG_USE_BP
  //     BP_JSON();
  //   #else
  //     FSM_DEBUG_SERIAL.print(_F("New BangBang FlagSet ["));
  //     FSM_DEBUG_SERIAL.print((double)lo);
  //     FSM_DEBUG_SERIAL.print(", ");
  //     FSM_DEBUG_SERIAL.print((double)hi);
  //     FSM_DEBUG_SERIAL.print("]\n");
  //   #endif
  // #endif
}

BangBangFlagSet::BangBangFlagSet(fsm_key_t key, const Number& lo, const Number& hi, Flag* flag) : low(Number::minimum(lo, hi)), high(Number::maximum(lo, hi)), FlagSetCondition<Number>(key, CMP_LES, low, flag, (double)hi < (double)lo) {
  // #ifdef DEBUG_JSON
  //   #ifdef DEBUG_USE_BP
  //     BP_JSON();
  //   #else
  //     FSM_DEBUG_SERIAL.print(_F("New BangBang FlagSet ["));
  //     FSM_DEBUG_SERIAL.print((double)lo);
  //     FSM_DEBUG_SERIAL.print(", ");
  //     FSM_DEBUG_SERIAL.print((double)hi);
  //     FSM_DEBUG_SERIAL.print("]\n");
  //   #endif
  // #endif
}

void BangBangFlagSet::childCallback(bool comp, const Number& val, const Number& ref) {
  this->state = (ref == this->high);

  // #ifdef DEBUG_JSON
  //   #ifdef DEBUG_USE_BP
  //     BP_JSON();
  //   #else
  //   String m = _F("BangBang FlagSet ")
  //   m += this->state ? 1 : 0

  //     FSM_DEBUG_SERIAL.print(_F("BangBang (High)\n"));
  //     DEBUG_DELAY();
  //   } else {
  //     DEBUG_DELAY();
  //     FSM_DEBUG_SERIAL.print(_F("BangBang (Low)\n"));
  //     DEBUG_DELAY();
  //   }
  // #endif

  // state ? comp : !comp;
  // T XOR T = T
  // F XOR T = F
  // T XOR F = F
  // F XOR F = T
  // comp = !(comp ^ this->state);
  // comp = this->state ? comp : !comp;
  bool _  = !(this->invert ^ this->state);

  // #ifdef FSM_DEBUG_SERIAL
  //   DEBUG_DELAY();
  //   FSM_DEBUG_SERIAL.print(_F("Flag Set ("));
  //   FSM_DEBUG_SERIAL.print(_ ? "true" : "false");
  //   FSM_DEBUG_SERIAL.print(")\n");
  //   DEBUG_DELAY();
  // #endif
  this->flag->set(_);
}

const Number& BangBangFlagSet::childReference(const Number& val) {
  #ifdef DEBUG_JSON
    #ifdef DEBUG_USE_BP
      BP_JSON();
    #else
      String m = _F("Flag BangBang State: ");
      m += this->state ? "Hi" : "Lo";
    #endif
  #endif
  if (this->state && val <= this->low) {
    this->setComparator(CMP_LES);
    this->triggered = false;
    this->state = false;
    #ifdef DEBUG_JSON
      #ifndef DEBUG_USE_BP
        m += _F(", Expecting Low");
      #endif
    #endif
  } else if (!this->state && val >= this->high) {
    this->setComparator(CMP_GTR);
    this->triggered = false;
    this->state = true;
    #ifdef DEBUG_JSON
      #ifndef DEBUG_USE_BP
        m += _F(", Expecting High");
      #endif
    #endif
  }
  #ifdef DEBUG_JSON
    #ifndef DEBUG_USE_BP
      // DEBUG_JSON(m.c_str(), m.length());
      DEBUG_JSON(m);
    #endif
  #endif

  return this->state ? this->high : this->low;
}

void BangBangFlagSet::callOperator(const Number& val, const Number& ref) {
  bool result = compare(this->comparator, val, ref);
  if(!this->triggered && result) {
    #ifdef DEBUG_JSON
      #ifdef DEBUG_USE_BP
        BP_JSON();
      #else
        DEBUG_JSON(_F("Flag BangBang Triggered"), 23);
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
        DEBUG_JSON(_F("Flag BangBang Trigger Reset"), 27);
      #endif
    #endif
    this->triggered = false;
  }
}

void LatchingFlagSet::callOperator(const bool& val, const bool& ref) {
  bool result = compare(this->comparator, val, ref);
  if(!this->triggered && result) {
    #ifdef DEBUG_JSON
      #ifdef DEBUG_USE_BP
        BP_JSON();
      #else
        DEBUG_JSON(_F("Flag Latch Triggered"), 20);
      #endif
    #endif
    this->executeCallback(val, ref);
    this->triggered = true;
  } else if(!result) {
    // Reset
    #ifdef DEBUG_JSON
      #ifdef DEBUG_USE_BP
        BP_JSON();
      #else
        DEBUG_JSON(_F("Flag Latch Trigger Reset"), 24);
      #endif
    #endif
    this->triggered = false;
  }
}

LatchingFlagSet::LatchingFlagSet(Flag* flag, bool invert) : FlagSetCondition<bool>(CMP_EQU, !invert, flag, invert) { }

LatchingFlagSet::LatchingFlagSet(fsm_key_t key, Flag* flag, bool invert) : FlagSetCondition<bool>(key, CMP_EQU, !invert, flag, invert) { }


void LatchingFlagSet::childCallback(bool comp, const bool& val, const bool& ref) {
  this->state = !ref;
  this->triggered = false;

  #ifdef DEBUG_JSON
    #ifdef DEBUG_USE_BP
      BP_JSON();
    #else
      String m = _F("Flag Latch ");
      m += val ? _F("(High), ") : _F("(Low), ");
    #endif
  #endif

  bool _ = val ^ ref;

  // Cast void* cb fn pointer to appropriate type and call
  #ifdef DEBUG_JSON
    #ifdef DEBUG_USE_BP
      BP_JSON();
    #else
      m += _F("Flag Set (");
      m += stateToString(_);
      m += ')';
      // DEBUG_JSON(m.c_str(), m.length());
      DEBUG_JSON(m);
    #endif
  #endif
  
  this->flag->set(_);
}

const bool& LatchingFlagSet::childReference(const bool& val) { return this->state; }