#include <chrono.h>

#include <debug.h>

#include <state.h>
#include <comparators.h>

const char* key_chronograph = {"Chronograph"};
Chronograph Chronos = Chronograph();

IntervalCallback::IntervalCallback(const fsm_timestamp_t& delta, const void* cb, callback_type_t cbtype, bool invert) : now(delta), delta(delta), ConditionalCallback<fsm_timestamp_t>(CMP_GTR, delta, cb, cbtype, invert) { }

IntervalCallback::IntervalCallback(const fsm_timestamp_t& delta, const fsm_timestamp_t& phase, const void* cb, callback_type_t cbtype, bool invert) : now(delta), delta(delta), phase(phase % delta), ConditionalCallback<fsm_timestamp_t>(CMP_GTR, delta, cb, cbtype, invert) { }

IntervalCallback::IntervalCallback(fsm_key_t key, const fsm_timestamp_t& delta, const void* cb, callback_type_t cbtype, bool invert) : now(delta), delta(delta), ConditionalCallback<fsm_timestamp_t>(key, CMP_GTR, delta, cb, cbtype, invert) { }

IntervalCallback::IntervalCallback(fsm_key_t key, const fsm_timestamp_t& delta, const fsm_timestamp_t& phase, const void* cb, callback_type_t cbtype, bool invert) : now(delta), delta(delta), phase(phase % delta), ConditionalCallback<fsm_timestamp_t>(key, CMP_GTR, delta, cb, cbtype, invert) { }

const fsm_timestamp_t& IntervalCallback::childReference(const fsm_timestamp_t& val) {
  // If the new value is way over the line, modulus and set now
  if(val > (this->delta + this->phase)) {
    this->now = val % this->delta; // leaves phase in

    #ifdef DEBUG_JSON
      #ifdef DEBUG_USE_BP
        BP_JSON();
      #else
        String m = _F("Chrono: Interval ");
        // if(this->key.length() > 0) { m += '\''; m += this->key; m += "' "; }
        if(this->key != FSM_KEY_NULL) { m += '\''; m += this->key; m += "' "; }
        m += _F("Reset");
        // DEBUG_JSON(m.c_str(), m.length());
        DEBUG_JSON(m);
      #endif
    #endif

    // Trigger!!
    return (refholder = 0);
  }
  return (refholder = (this->delta + this->phase));
}

fsm_timestamp_t IntervalCallback::get(void) { return this->now; }

void IntervalCallback::set(const fsm_timestamp_t& val) {
  this->now = val;
  #ifdef DEBUG_JSON
    #ifdef DEBUG_USE_BP
      BP_JSON();
    #else
      String m = _F("Chrono: Interval ");
      // if(this->key.length() > 0) { m += '\''; m += this->key; m += "' "; }
      if(this->key != FSM_KEY_NULL) { m += '\''; m += this->key; m += "' "; }
      m += _F("Set (");
      m += timestampToString(val);
      m += ')';
      // DEBUG_JSON(m.c_str(), m.length());
      DEBUG_JSON(m);
    #endif
  #endif // DEBUG
  this->operator()(val);
}

FlagInterval::FlagInterval(const fsm_timestamp_t& delta, Flag* flag, bool invert) : flag(flag), IntervalCallback(delta, nullptr, CB_COMP, invert) { }

FlagInterval::FlagInterval(const fsm_timestamp_t& delta, const fsm_timestamp_t& phase, Flag* flag, bool invert) : flag(flag), IntervalCallback(delta, phase, nullptr, CB_COMP, invert) { }

FlagInterval::FlagInterval(fsm_key_t key, const fsm_timestamp_t& delta, Flag* flag, bool invert) : flag(flag), IntervalCallback(key, delta, nullptr, CB_COMP, invert) { }

FlagInterval::FlagInterval(fsm_key_t key, const fsm_timestamp_t& delta, const fsm_timestamp_t& phase, Flag* flag, bool invert) : flag(flag), IntervalCallback(key, delta, phase, nullptr, CB_COMP, invert) { }

void FlagInterval::childCallback(bool comp, const fsm_timestamp_t& val, const fsm_timestamp_t& ref) {
  // #ifdef DEBUG_JSON
  //   #ifdef DEBUG_USE_BP
  //     BP_JSON();
  //   #else
  //     String m = _F("Chrono: FlagInterval ");
  //     if(this->key != nullptr) { m += '\''; m += key; m += "' "; }
  //     m += _F("Set (");
  //     m += Flag::toString(comp);
  //     m += ')';
  //     // DEBUG_JSON(m.c_str(), m.length());
  //   #endif
  // #endif // DEBUG
  this->flag->set(comp);
}

ChronoEvent::ChronoEvent(const fsm_timestamp_t& timestamp, typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb, bool invert) : ConditionalCallback<fsm_timestamp_t>(CMP_GEQ, timestamp, cb, invert) { }

ChronoEvent::ChronoEvent(fsm_key_t key, const fsm_timestamp_t& timestamp, typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb, bool invert) : ConditionalCallback<fsm_timestamp_t>(key, CMP_GEQ, timestamp, cb, invert) { }

void ChronoEvent::callOperator(const fsm_timestamp_t& val, const fsm_timestamp_t& ref) {
  bool result = compare(this->comparator, val, ref);
  if(!this->triggered && result) {
    this->triggered = true;

    #ifdef DEBUG_JSON
      #ifdef DEBUG_USE_BP
        BP_JSON();
      #else
        String m = _F("Chrono: Event ");
        // if(this->key.length() > 0) { m += '\''; m += this->key; m += "' "; }
        if(this->key != FSM_KEY_NULL) { m += '\''; m += this->key; m += "' "; }
        
        m += _F("Triggered");
        // DEBUG_JSON(m.c_str(), m.length());
        DEBUG_JSON(m);
      #endif
    #endif // DEBUG

    this->executeCallback(val, ref);
  } else if(!result) {
    this->triggered = false;
  }
}

FlagEvent::FlagEvent(const fsm_timestamp_t& timestamp, Flag* flag, bool invert) : FlagSetCondition(CMP_GEQ, timestamp, flag, invert) { }
FlagEvent::FlagEvent(fsm_key_t key, const fsm_timestamp_t& timestamp, Flag* flag, bool invert) : FlagSetCondition(key, CMP_GEQ, timestamp, flag, invert) { }

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
  // #ifdef DEBUG_JSON
  //   #ifdef DEBUG_USE_BP
  //     BP_JSON();
  //   #else
  //     String m = _F("Chrono: FlagEvent ");
  //     if(this->key.length() > 0) { m += '\''; m += this->key; m += "' "; }
  //     m += _F("Set (");
  //     m += Flag::toString(comp);
  //     m += ')';
  //     // DEBUG_JSON(m.c_str(), m.length());
  //   #endif
  // #endif

  this->flag->set(comp);
}

Chronograph::Chronograph(const fsm_timestamp_t& start) : start(start), last(start), State<fsm_timestamp_t>(start, key_chronograph) { 
  // Blank it
  for(int i = 0; i < MAX_INTERVALS; i++) {
    this->intervals[i] = nullptr;
  }
}

void Chronograph::set(const fsm_timestamp_t& _val) {
  fsm_timestamp_t val = _val % TWENTYFOURHRS_MILLIS;

  const fsm_timestamp_t last = this->get();

  // Null frame
  const fsm_timestamp_t delta = val > last ? val - last : 0;
  
  #ifdef DEBUG_JSON
    #ifdef DEBUG_USE_BP
      BP_JSON();
    #else
      String m = _F("Chrono: Set Time (");

      m += timestampToString(val);

      m += ')';

      // DEBUG_JSON(m.c_str(), m.length());
      DEBUG_JSON(m);
    #endif
  #endif

  // Call super - let getRef handle overflow ;)
  ((State<fsm_timestamp_t>*)this)->set(val);

  for(unsigned int i = 0; i < MAX_INTERVALS; i++) {
    if(this->intervals[i] == nullptr) return;

    const fsm_timestamp_t last = intervals[i]->get();
    this->intervals[i]->set(last + delta);
  }
}

fsm_timestamp_t Chronograph::get(void) { return ((State<fsm_timestamp_t>*)this)->get(); }

ConditionalCallback<fsm_timestamp_t>* Chronograph::addEvent(const fsm_timestamp_t& timestamp, typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb, bool invert) { return addConditional(new ChronoEvent(timestamp, cb, invert)); }
ConditionalCallback<fsm_timestamp_t>* Chronograph::addEvent(fsm_key_t key, const fsm_timestamp_t& timestamp, typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb, bool invert) { return addConditional(new ChronoEvent(key, timestamp, cb, invert)); }

ConditionalCallback<fsm_timestamp_t>* Chronograph::addEventFlag(const fsm_timestamp_t& timestamp, Flag* flag, bool invert) { 
  return addEventFlag(flag->getKey(), timestamp, flag, invert); }  
ConditionalCallback<fsm_timestamp_t>* Chronograph::addEventFlag(fsm_key_t key, const fsm_timestamp_t& timestamp, Flag* flag, bool invert) { 
  #ifdef DEBUG_JSON
    #ifdef DEBUG_USE_BP
      BP_JSON();
    #else
      String m = _F("Chrono: Add FlagEvent ");
      // if(key.length() > 0) { m += '\''; m += key; m += "' "; }
      if(key != FSM_KEY_NULL) { m += '\''; m += key; m += "' "; }
      m += '(';
      m += timestampToString(timestamp);
      if(invert) m += ", INV";
      // if(flag->getKey().length() > 0) {
      if(flag->getKey() != FSM_KEY_NULL) {
        m += ", Flag '";
        m += flag->getKey();
        m += "')";
      } else {
        m += ')';
      }
      // DEBUG_JSON(m.c_str(), m.length());
      DEBUG_JSON(m);
    #endif
  #endif
  return addConditional(new FlagEvent(key, timestamp, flag, invert)); 
}  

IntervalCallback* Chronograph::addInterval(const fsm_timestamp_t& delta, typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb, bool invert) { 
  return this->addInterval(delta, 0, cb, invert);
}
IntervalCallback* Chronograph::addInterval(fsm_key_t key, const fsm_timestamp_t& delta, typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb, bool invert) { 
  return this->addInterval(key, delta, 0, cb, invert);
}

IntervalCallback* Chronograph::addInterval(const fsm_timestamp_t& delta, const fsm_timestamp_t& phase, typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb, bool invert) { 
  return this->addInterval(FSM_KEY_NULL, delta, phase, cb, invert);
}

IntervalCallback* Chronograph::addInterval(fsm_key_t key, const fsm_timestamp_t& delta, const fsm_timestamp_t& phase, typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb, bool invert) { 
  unsigned int n = 0;
  while(intervals[n] != nullptr) { n++; if(n > MAX_INTERVALS) { Serial.println("~"); return nullptr;}}

  intervals[n] = new IntervalCallback(key, delta, phase, (const void*)cb, CB_COMPVAL, invert);

  #ifdef DEBUG_JSON
    #ifdef DEBUG_USE_BP
      BP_JSON();
    #else
      String m = _F("Chrono: Add Interval ");
      // if(key.length() > 0) { m += '\''; m += key; m += "' "; }
      if(key != FSM_KEY_NULL) { m += '\''; m += key; m += "' "; }
      m += "(";
      m += delta;
      if(phase > 0) {
        m += '+';
        m += phase;
      }
      m += "ms)";
      // DEBUG_JSON(m.c_str(), m.length());
      DEBUG_JSON(m);
    #endif
  #endif

  return intervals[n];
}

IntervalCallback* Chronograph::addIntervalFlag(const fsm_timestamp_t& delta, const fsm_timestamp_t& phase, Flag* flag, bool invert) { return this->addIntervalFlag(flag->getKey(), delta, phase, flag, invert); }

IntervalCallback* Chronograph::addIntervalFlag(fsm_key_t key, const fsm_timestamp_t& delta, const fsm_timestamp_t& phase, Flag* flag, bool invert) { 
  unsigned int n = 0;
  while(intervals[n] != nullptr) { n++; if(n > MAX_INTERVALS) return nullptr;}

  intervals[n] = new FlagInterval(key, delta, phase, flag, invert);
  if(intervals[n] == nullptr) {Serial.println('~'); return nullptr;}

  #ifdef DEBUG_JSON
    #ifdef DEBUG_USE_BP
      BP_JSON();
    #else
      String m = _F("Chrono: Add FlagInterval ");
      // if(key.length() > 0) { m += '\''; m += key; m += "' "; }
      if(key != FSM_KEY_NULL) { m += '\''; m += key; m += "' "; }
      m += '(';
      m += timestampToString(delta);
      if(phase > 0) {
        m += " + ";
        m += timestampToString(phase);
      }
      if(invert) m += ", INV";
      // if(flag->getKey().length() > 0) {
      if(flag->getKey() != FSM_KEY_NULL) {
        m += ", Flag '";
        m += flag->getKey();
        m += "')";
      } else {
        m += ')';
      }
      // DEBUG_JSON(m.c_str(), m.length());
      DEBUG_JSON(m);
    #endif
  #endif

  return intervals[n];
}

IntervalCallback* Chronograph::addIntervalFlag(const fsm_timestamp_t& delta, Flag* flag, bool invert) { 
  return this->addIntervalFlag(delta, 0, flag, invert);
}

IntervalCallback* Chronograph::addIntervalFlag(fsm_key_t key, const fsm_timestamp_t& delta, Flag* flag, bool invert) { 
  return this->addIntervalFlag(key, delta, 0, flag, invert);
}

IntervalCallback* Chronograph::addTwentyFourTimeout(const fsm_timestamp_t& phase, typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb, bool invert) { return addTwentyFourTimeout(FSM_KEY_NULL, phase, cb, invert); }

IntervalCallback* Chronograph::addTwentyFourTimeout(fsm_key_t key, const fsm_timestamp_t& phase, typename ConditionalCallback<fsm_timestamp_t>::cb_compval_t cb, bool invert) {
  unsigned int n = 0;
  while(intervals[n] != nullptr) { n++; if(n > MAX_INTERVALS) return nullptr;}

  intervals[n] = new IntervalCallback(key, TWENTYFOURHRS_MILLIS, phase, (const void*)cb, CB_COMPVAL, invert);
  
  #ifdef DEBUG_JSON
    #ifdef DEBUG_USE_BP
      BP_JSON();
    #else
      String m = _F("Chrono: Add 24HR FlagInterval ");
      // if(key.length() > 0) { m += '\''; m += key; m += "' "; }
      if(key != FSM_KEY_NULL) { m += '\''; m += key; m += "' "; }
      if(phase > 0) {
        m += "(+";
        m += phase;
        m += "ms)";
      }
      // DEBUG_JSON(m.c_str(), m.length());
      DEBUG_JSON(m);
    #endif
  #endif

  return intervals[n];
}

// ConditionalCallback<fsm_timestamp_t>* _Timer::addLoggerCallback(void (* const cb)(bool comp, fsm_timestamp_t val)) { return addConditional(new ConditionalCallback<fsm_timestamp_t>(CMP_NEQ, -1, cb)); }