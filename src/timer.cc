#include <timer.h>

#include <state.h>
#include <statelinker.h>
#include <comparators.h>
#include <number.h>

Interval::Interval(const unsigned long& delta, const void* cb, callback_type_t cbtype) : ConditionalCallback<unsigned long>(CMP_GTR, delta, cb, cbtype) { }

Interval::Interval(const unsigned long& now, const unsigned long& delta, const void* cb, callback_type_t cbtype) : now(now), ConditionalCallback<unsigned long>(CMP_GTR, delta, cb, cbtype) { }

const unsigned long& Interval::childReference(const unsigned long& val) {
  // If the new value is way over the line, modulus and set now
  if(val > (this->reference * INTERVAL_RESET_WINDOW)) this->now = val % this->reference;
  return this->reference;
}

unsigned long Interval::get(void) { return this->now; }

void Interval::set(const unsigned long& val) { this->now = val; this->operator()(val); }

_Timer::_Timer(const unsigned long& start) : start(start), last(start), State<unsigned long>(start) { }

void _Timer::set(const unsigned long& val) {
  const unsigned long last = this->get();
  const unsigned long delta = val > last ? val - last : last - val;
  incrementIntervals(delta);

  // Call super - let getRef handle overflow ;)
  ((State<unsigned long>*)this)->set(val);
}

void _Timer::incrementIntervals(const unsigned long& delta) {
  for(unsigned int i = 0; i < MAX_CONDITIONALS; i++) {
    if(intervals[i] == nullptr) return;

    const unsigned long last = intervals[i]->get();
    intervals[i]->set(last + delta);
  }
}

ConditionalCallback<unsigned long>* _Timer::addEvent(const unsigned long& timestamp, void (* const cb)(bool, unsigned long), unsigned long (*const getReference)(void)) { return addConditional(CMP_GTR, timestamp, cb, getReference); }

ConditionalCallback<unsigned long>* _Timer::addEventFlag(const unsigned long& timestamp, Flag* flag, unsigned long (*const getReference)(void)) { return addConditionalSetFlag(CMP_GTR, timestamp, flag, getReference); }

Interval* _Timer::addInterval(const unsigned long& delta, void (* const cb)(bool, unsigned long), unsigned long (*const getReference)(void)) { 
  unsigned int n = 0;
  while(intervals[n] != nullptr) n++;
  intervals[n] = new Interval(delta, (const void*)cb, CB_COMPVAL);
  return intervals[n];
}

Interval* _Timer::addTwentyFourTimeout(const unsigned long& delay, void (* const cb)(bool, unsigned long), unsigned long (*const getReference)(void)) {
  unsigned int n = 0;
  while(intervals[n] != nullptr) n++;
  intervals[n] = new Interval(TWENTYFOURHRS_MILLIS - delay, TWENTYFOURHRS_MILLIS, (const void*)cb, CB_COMPVAL);
  return intervals[n];
}