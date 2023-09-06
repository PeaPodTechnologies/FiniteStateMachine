#include <timer.h>

#include <debug.h>

#include <state.h>
#include <comparators.h>

_Timer Timer = _Timer();

#ifdef DEBUG
// Helper
void debugTimestamp(const fsm_timestamp_t& timestamp);

#endif

Interval::Interval(const fsm_timestamp_t& delta, const void* cb, callback_type_t cbtype, bool invert) : now(delta), delta(delta), ConditionalCallback<fsm_timestamp_t>(CMP_GTR, delta, cb, cbtype, invert) { }

Interval::Interval(const fsm_timestamp_t& delta, const fsm_timestamp_t& phase, const void* cb, callback_type_t cbtype, bool invert) : now(delta), delta(delta), phase(phase % delta), ConditionalCallback<fsm_timestamp_t>(CMP_GTR, delta, cb, cbtype, invert) { }

const fsm_timestamp_t& Interval::childReference(const fsm_timestamp_t& val) {
  // If the new value is way over the line, modulus and set now
  if(val > (this->delta + this->phase)) {
    this->now = val % this->delta; // leaves phase in

    #ifdef DEBUG
      DEBUG_DELAY();
      DEBUG.print("Interval Reset!\n");
      DEBUG_DELAY();
    #endif // DEBUG

    // Trigger!!
    return (refholder = 0);
  }
  return (refholder = (this->delta + this->phase));
}

fsm_timestamp_t Interval::get(void) { return this->now; }

void Interval::set(const fsm_timestamp_t& val) {
  this->now = val;
  #ifdef DEBUG
    DEBUG_DELAY();
    DEBUG.print("Interval Set (");
    DEBUG.print(this->now);
    DEBUG.print("ms)\n");
    DEBUG_DELAY();
  #endif // DEBUG
  this->operator()(this->now); 
}

FlagInterval::FlagInterval(const fsm_timestamp_t& delta, Flag* flag, bool invert) : flag(flag), Interval(delta, nullptr, CB_NONE, invert) { }

FlagInterval::FlagInterval(const fsm_timestamp_t& delta, const fsm_timestamp_t& phase, Flag* flag, bool invert) : flag(flag), Interval(delta, phase, nullptr, CB_NONE, invert) { }

void FlagInterval::childCallback(bool comp, const fsm_timestamp_t& val, const fsm_timestamp_t& ref) {
  #ifdef DEBUG
    DEBUG_DELAY();
    DEBUG.print("Flag Interval Set (");
    DEBUG.print(comp ? "true" : "false");
    DEBUG.print(")\n");
    DEBUG_DELAY();
  #endif
  this->flag->set(comp);
}

Event::Event(const fsm_timestamp_t& timestamp, typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb, bool invert) : ConditionalCallback<fsm_timestamp_t>(CMP_GEQ, timestamp, cb, invert) { }

void Event::callOperator(const fsm_timestamp_t& val, const fsm_timestamp_t& ref) {
  bool result = compare(this->comparator, val, ref);
  if(!this->triggered && result) {
    this->triggered = true;

    #ifdef DEBUG
      DEBUG_DELAY();
      DEBUG.print("Event Triggered!\n");
      DEBUG_DELAY();
    #endif // DEBUG

    this->executeCallback(val, ref);
  } else if(!result) {
    this->triggered = false;
  }
}

FlagEvent::FlagEvent(const fsm_timestamp_t& timestamp, Flag* flag, bool invert) : FlagSetCondition(CMP_GEQ, timestamp, flag, invert) { }

void FlagEvent::callOperator(const fsm_timestamp_t& val, const fsm_timestamp_t& ref) {
  bool result = compare(this->comparator, val, ref);
  if(!this->triggered && result) {
    this->triggered = true;
    this->executeCallback(val, ref);
  } else if(!result) {
    this->triggered = false;
  }
}
void FlagEvent::childCallback(bool comp, const fsm_timestamp_t& val, const fsm_timestamp_t& ref) {
  #ifdef DEBUG
    DEBUG_DELAY();
    DEBUG.print("Flag Event Set (");
    DEBUG.print(comp ? "true" : "false");
    DEBUG.print(")\n");
    DEBUG_DELAY();
  #endif

  this->flag->set(comp);
}

_Timer::_Timer(const fsm_timestamp_t& start) : start(start), last(start), State<fsm_timestamp_t>(start) { 
  // Blank it
  for(int i = 0; i < MAX_INTERVALS; i++) {
    this->intervals[i] = nullptr;
  }
}

void _Timer::set(const fsm_timestamp_t& _val) {
  fsm_timestamp_t val = _val % TWENTYFOURHRS_MILLIS;

  const fsm_timestamp_t last = this->get();

  // Null frame
  const fsm_timestamp_t delta = val > last ? val - last : 0;
  
  #ifdef DEBUG
    DEBUG_DELAY();
    DEBUG.print("Timer Delta: ");
    debugTimestamp(delta);
    DEBUG.print("\n");
    DEBUG_DELAY();
  #endif

  // Call super - let getRef handle overflow ;)
  ((State<fsm_timestamp_t>*)this)->set(val);

  for(unsigned int i = 0; i < MAX_INTERVALS; i++) {
    if(this->intervals[i] == nullptr) return;

    const fsm_timestamp_t last = intervals[i]->get();
    this->intervals[i]->set(last + delta);
  }
}

fsm_timestamp_t _Timer::get(void) { return ((State<fsm_timestamp_t>*)this)->get(); }

ConditionalCallback<fsm_timestamp_t>* _Timer::addEvent(const fsm_timestamp_t& timestamp, typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb, bool invert) { return addConditional(new Event(timestamp, cb, invert)); }

ConditionalCallback<fsm_timestamp_t>* _Timer::addEventFlag(const fsm_timestamp_t& timestamp, Flag* flag, bool invert) { return addConditional(new FlagEvent(timestamp, flag, invert)); }  

Interval* _Timer::addInterval(const fsm_timestamp_t& delta, typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb, bool invert) { 
  return this->addInterval(delta, 0, cb, invert);
}

Interval* _Timer::addInterval(const fsm_timestamp_t& delta, const fsm_timestamp_t& phase, typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb, bool invert) { 
  unsigned int n = 0;
  while(intervals[n] != nullptr) { n++; if(n > MAX_INTERVALS) return nullptr;}

  #ifdef DEBUG
    DEBUG_DELAY();
    DEBUG.print("New Interval (");
    DEBUG.print(delta);
    if(phase > 0) {
      DEBUG.print("+");
      DEBUG.print(phase);
    }
    DEBUG.print("ms)\n");
    DEBUG_DELAY();
  #endif

  intervals[n] = new Interval(delta, phase, (const void*)cb, CB_COMPVAL, invert);
  return intervals[n];
}

Interval* _Timer::addIntervalFlag(const fsm_timestamp_t& delta, const fsm_timestamp_t& phase, Flag* flag, bool invert) { 
  unsigned int n = 0;
  while(intervals[n] != nullptr) { n++; if(n > MAX_INTERVALS) return nullptr;}

  #ifdef DEBUG
    DEBUG_DELAY();
    DEBUG.print("New Interval Flag (");
    DEBUG.print(delta);
    if(phase > 0) {
      DEBUG.print("+");
      DEBUG.print(phase);
    }
    DEBUG.print("ms)\n");
    DEBUG_DELAY();
  #endif

  intervals[n] = new FlagInterval(delta, phase, flag, invert);
  return intervals[n];
}

Interval* _Timer::addIntervalFlag(const fsm_timestamp_t& delta, Flag* flag, bool invert) { 
  return this->addIntervalFlag(delta, 0, flag, invert);
}

Interval* _Timer::addTwentyFourTimeout(const fsm_timestamp_t& phase, typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb, bool invert) {
  unsigned int n = 0;
  while(intervals[n] != nullptr) { n++; if(n > MAX_INTERVALS) return nullptr;}

  #ifdef DEBUG
    DEBUG_DELAY();
    DEBUG.print("New 24Hr Interval Flag");
    if(phase > 0) {
      DEBUG.print("(+");
      DEBUG.print(phase);
      DEBUG.print("ms)\n");
    }
    DEBUG_DELAY();
  #endif

  intervals[n] = new Interval(TWENTYFOURHRS_MILLIS, phase, (const void*)cb, CB_COMPVAL, invert);
  return intervals[n];
}

#ifdef DEBUG

void debugTimestamp(const fsm_timestamp_t& timestamp) {
  unsigned long seconds = timestamp / 1000;
  unsigned long res_ms = timestamp % 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;

  seconds -= minutes * 60;
  minutes -= hours * 60;

  DEBUG_DELAY();
  if(hours > 0) {
    if(hours < 10) DEBUG.print("0");
    DEBUG.print(hours);
    DEBUG.print("h : ");
  }

  if(minutes > 0) {
    if(minutes < 10) DEBUG.print("0");
    DEBUG.print(minutes - hours * 60);
    DEBUG.print("m : ");
  }

  if(seconds < 10) DEBUG.print("0");
  DEBUG.print(seconds);
  DEBUG.print("s");

  if(minutes < 1) {
    // ms
    DEBUG.print(" : ");
    if(res_ms < 100) DEBUG.print("0");
    if(res_ms < 10) DEBUG.print("0");
    DEBUG.print(res_ms);
    DEBUG.print("ms");
  }

  DEBUG.print("\n");
  DEBUG_DELAY();
}

#endif

// ConditionalCallback<fsm_timestamp_t>* _Timer::addLoggerCallback(void (* const cb)(bool comp, fsm_timestamp_t val)) { return addConditional(new ConditionalCallback<fsm_timestamp_t>(CMP_NEQ, -1, cb)); }