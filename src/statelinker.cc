#include <statelinker.h>

#include <debug.h>
#include <state.h>
#include <comparators.h>
#include <number.h>

BangBangFlagSet::BangBangFlagSet(const Number& lo, const Number& hi, Flag* flag) : low(Number::minimum(lo, hi)), high(Number::maximum(lo, hi)), FlagSetCondition<Number>(CMP_LES, low, flag, (double)hi < (double)lo) {
  #ifdef DEBUG
    DEBUG_DELAY();
    DEBUG.print("BangBang FlagSet [");
    DEBUG.print((double)lo);
    DEBUG.print(", ");
    DEBUG.print((double)hi);
    DEBUG.print("]\n");
    DEBUG_DELAY();
  #endif
}

void BangBangFlagSet::childCallback(bool comp, const Number& val, const Number& ref) {
  this->state = (ref == this->high);

  #ifdef DEBUG
    if(state) {
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
  // comp = this->state ? comp : !comp;
  bool _  = !(this->invert ^ this->state);

  #ifdef DEBUG
    DEBUG_DELAY();
    DEBUG.print("Flag Set (");
    DEBUG.print(_ ? "true" : "false");
    DEBUG.print(")\n");
    DEBUG_DELAY();
  #endif
  this->flag->set(_);
}

const Number& BangBangFlagSet::childReference(const Number& val) {
  #ifdef DEBUG
    DEBUG_DELAY();
    DEBUG.print("BangBang State: ");
    DEBUG.print(BOOLSTR(this->state));
    DEBUG.print("\n");
    DEBUG_DELAY();
  #endif
  if (this->state && val <= this->low) {
    this->setComparator(CMP_LES);
    this->triggered = false;
    this->state = false;
    #ifdef DEBUG
      DEBUG_DELAY();
      DEBUG.print("BangBang State: Expecting Low\n");
      DEBUG_DELAY();
    #endif
  } else if (!this->state && val >= this->high) {
    this->setComparator(CMP_GTR);
    this->triggered = false;
    this->state = true;
    #ifdef DEBUG
      DEBUG_DELAY();
      DEBUG.print("BangBang State: Expecting High\n");
      DEBUG_DELAY();
    #endif
  }
  return this->state ? this->high : this->low;
}

void BangBangFlagSet::callOperator(const Number& val, const Number& ref) {
  bool result = compare(this->comparator, val, ref);
  if(!this->triggered && result) {
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

void LatchingFlagSet::callOperator(const bool& val, const bool& ref) {
  bool result = compare(this->comparator, val, ref);
  if(!this->triggered && result) {
    this->executeCallback(val, ref);
    this->triggered = true;
  } else if(!result) {
    // Reset
    #ifdef DEBUG
      DEBUG_DELAY();
      DEBUG.print("Trigger Reset!\n");
      DEBUG_DELAY();
    #endif
    this->triggered = false;
  }
}

LatchingFlagSet::LatchingFlagSet(Flag* flag, bool invert) : FlagSetCondition<bool>(CMP_EQU, !invert, flag, invert) { }


void LatchingFlagSet::childCallback(bool comp, const bool& val, const bool& ref) {
  this->state = !ref;
  this->triggered = false;

  #ifdef DEBUG
    if(val) {
      DEBUG_DELAY();
      DEBUG.print("Latch (High)\n");
      DEBUG_DELAY();
    } else {
      DEBUG_DELAY();
      DEBUG.print("Latch (Low)\n");
      DEBUG_DELAY();
    }
  #endif

  bool _ = val ^ ref;

  // Cast void* cb fn pointer to appropriate type and call
  #ifdef DEBUG
    DEBUG_DELAY();
    DEBUG.print("Flag Set (");
    DEBUG.print(_ ? "true" : "false");
    DEBUG.print(")\n");
    DEBUG_DELAY();
  #endif
  this->flag->set(_);
}

const bool& LatchingFlagSet::childReference(const bool& val) { return this->state; }