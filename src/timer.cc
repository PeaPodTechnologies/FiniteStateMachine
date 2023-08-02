#define FSM_SRC 1

#include <timer.h>

#include <state.h>
#include <comparators.h>

Interval::Interval(const fsm_timestamp_t& delta, const void* cb, callback_type_t cbtype, bool invert) : ConditionalCallback<fsm_timestamp_t>(CMP_GTR, delta, cb, cbtype, invert) { }

Interval::Interval(const fsm_timestamp_t& delta, const fsm_timestamp_t& phase, const void* cb, callback_type_t cbtype, bool invert) : phase(phase), ConditionalCallback<fsm_timestamp_t>(CMP_GTR, delta, cb, cbtype, invert) { }

const fsm_timestamp_t& Interval::childReference(const fsm_timestamp_t& val) {
  // If the new value is way over the line, modulus and set now
  if(val > (this->reference * INTERVAL_RESET_WINDOW)) this->now = val % this->reference;
  return this->reference;
}

fsm_timestamp_t Interval::get(void) { return this->now; }

void Interval::set(const fsm_timestamp_t& val) { this->now = (val - this->phase); this->operator()(this->now); }

FlagInterval::FlagInterval(const fsm_timestamp_t& delta, Flag* flag, bool invert) : flag(flag), Interval(delta, nullptr, CB_NONE, invert) { }

FlagInterval::FlagInterval(const fsm_timestamp_t& delta, const fsm_timestamp_t& phase, Flag* flag, bool invert) : flag(flag), Interval(delta, phase, nullptr, CB_NONE, invert) { }

void FlagInterval::childCallback(bool comp, const fsm_timestamp_t& val, const fsm_timestamp_t& ref) {
  this->flag->set(comp);
}

Event::Event(const fsm_timestamp_t& timestamp, typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb, bool invert) : ConditionalCallback<fsm_timestamp_t>(CMP_GEQ, timestamp, cb, invert) { }

void Event::callOperator(const fsm_timestamp_t& val, const fsm_timestamp_t& ref) {
  bool result = compare(this->comparator, val, ref);
  if(!this->triggered && result) {
    this->triggered = true;
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
  this->flag->set(comp);
}

_Timer::_Timer(const fsm_timestamp_t& start) : start(start), last(start), State<fsm_timestamp_t>(start) { }

void _Timer::set(const fsm_timestamp_t& val) {
  const fsm_timestamp_t last = this->get();
  const fsm_timestamp_t delta = val > last ? val - last : last - val;
  incrementIntervals(delta);

  // Call super - let getRef handle overflow ;)
  ((State<fsm_timestamp_t>*)this)->set(val);
}

void _Timer::incrementIntervals(const fsm_timestamp_t& delta) {
  for(unsigned int i = 0; i < MAX_CONDITIONALS; i++) {
    if(intervals[i] == nullptr) return;

    const fsm_timestamp_t last = intervals[i]->get();
    intervals[i]->set(last + delta);
  }
}

ConditionalCallback<fsm_timestamp_t>* _Timer::addEvent(const fsm_timestamp_t& timestamp, typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb, bool invert) { return addConditional(new Event(timestamp, cb, invert)); }

ConditionalCallback<fsm_timestamp_t>* _Timer::addEventFlag(const fsm_timestamp_t& timestamp, Flag* flag, bool invert) { return addConditional(new FlagEvent(timestamp, flag, invert)); }  

Interval* _Timer::addInterval(const fsm_timestamp_t& delta, typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb, bool invert) { 
  return this->addInterval(delta, 0, cb, invert);
}

Interval* _Timer::addInterval(const fsm_timestamp_t& delta, const fsm_timestamp_t& phase, typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb, bool invert) { 
  unsigned int n = 0;
  while(intervals[n] != nullptr) n++;
  intervals[n] = new Interval(delta, phase, (const void*)cb, CB_COMPVAL, invert);
  return intervals[n];
}

Interval* _Timer::addIntervalFlag(const fsm_timestamp_t& delta, const fsm_timestamp_t& phase, Flag* flag, bool invert) { 
  unsigned int n = 0;
  while(intervals[n] != nullptr) n++;
  intervals[n] = new FlagInterval(delta, phase, flag, invert);
  return intervals[n];
}

Interval* _Timer::addIntervalFlag(const fsm_timestamp_t& delta, Flag* flag, bool invert) { 
  return this->addIntervalFlag(delta, 0, flag, invert);
}

Interval* _Timer::addTwentyFourTimeout(const fsm_timestamp_t& delay, typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb, bool invert) {
  unsigned int n = 0;
  while(intervals[n] != nullptr) n++;
  intervals[n] = new Interval(TWENTYFOURHRS_MILLIS - delay, TWENTYFOURHRS_MILLIS, (const void*)cb, CB_COMPVAL, invert);
  return intervals[n];
}

// ConditionalCallback<fsm_timestamp_t>* _Timer::addLoggerCallback(void (* const cb)(bool comp, fsm_timestamp_t val)) { return addConditional(new ConditionalCallback<fsm_timestamp_t>(CMP_NEQ, -1, cb)); }