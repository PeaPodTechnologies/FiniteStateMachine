#ifndef FSM_STATE_H_
#error __FILE__ should only be included AFTER <state.h>
// #include <state.h>
#endif
#ifndef FSM_STATELINKER_H_
#error __FILE__ should only be included AFTER <statelinker.h>
// #include <statelinker.h>
#endif

#ifdef FSM_STATE_H_
#ifdef FSM_STATELINKER_H_

#ifndef FSM_STATE_T_
#define FSM_STATE_T_

// #error STATE.TPP

#include <debug.h>

template <typename T> String FiniteStateMachine::State<T>::toString(const T& val) { return String(parseType<T>()) + " = " + stateToString<T>(val); }
template <typename T> String FiniteStateMachine::State<T>::toString(void) { return (this->key != nullptr ? this->key + String(": ") : String()) + FiniteStateMachine::State<T>::toString(this->value); }

template <typename T> FiniteStateMachine::State<T>::State(const T& value) : value(value), defaultValue(value), key(FSM_KEY_NULL) {
  #ifdef DEBUG_JSON
    #ifdef DEBUG_USE_BP
      BP_JSON();
    #else
      String m = _F("New State = ");
      m += FiniteStateMachine::State<T>::toString(value);
      // DEBUG_JSON(m.c_str(), m.length());
      DEBUG_JSON(m);
    #endif
  #endif
}
template <typename T> FiniteStateMachine::State<T>::State(const T& value, fsm_key_t key) : value(value), defaultValue(value), key(key) {
  #ifdef DEBUG_JSON
    #ifdef DEBUG_USE_BP
      BP_JSON();
    #else
      String m = _F("New State '");
      m += key;
      m += "' = ";
      m += FiniteStateMachine::State<T>::toString(value);
      // DEBUG_JSON(m.c_str(), m.length());
      DEBUG_JSON(m);
    #endif
  #endif
}

template <typename T> FiniteStateMachine::State<T>::~State() {
  for(unsigned int i = 0; i < MAX_CONDITIONALS; i++) {
    if(this->conditionals[i] != nullptr) {
      delete this->conditionals[i];
    }
  }
}

template <typename T> T FiniteStateMachine::State<T>::get(void) {
  return this->value;
}

template <typename T> void FiniteStateMachine::State<T>::reset(void) {
  this->set(this->defaultValue);
}

template <typename T> void FiniteStateMachine::State<T>::set(const T& val) {
  #ifdef DEBUG_JSON
    #ifdef DEBUG_USE_BP
      BP_JSON();
    #else
      String m = _F("State ");
      // if(this->key.length() > 0) { m += '\''; m += this->key; m += '\''; m += ' '; }
      if(this->key != FSM_KEY_NULL) { m += '\''; m += this->key; m += '\''; m += ' '; }
      m += _F("Set: ");
      m += stateToString(val);
      m += ", ";
      m += this->numconditionals;
      m += _F(" Conditionals");
      if(this->disabled) m += _F(", Disabled");
      // DEBUG_JSON(m.c_str(), m.length());
      DEBUG_JSON(m);
      // 
      // Serial.print(">>>");
      // Serial.println(m);

      // delay(1);
    #endif
  #endif

  if(!this->disabled) {
    // Call Conditionals - If comparison (etc.), sets execute == true
    for(unsigned int i = 0; i < MAX_CONDITIONALS; i++) {
      if(this->conditionals[i] == nullptr) break;
      delay(1); // Comment out to break the code
      this->conditionals[i]->operator()(val);

      delay(1); // this too

      this->conditionals[i]->executeCallback(this->conditionals[i]->getExecutionVal(), this->conditionals[i]->getExecutionRef());
    }
  }

  // #ifdef DEBUG_JSON
  //   #ifdef DEBUG_USE_BP
  //     BP_JSON();
  //   #else
  //     m = _F("End of State ");
  //     if(this->key.length() > 0) { m += '\''; m += this->key; m += '\''; m += ' '; }
  //     m += _F("Conditionals, Caching ");
  //     m += State<T>::toString(val);
  //     // DEBUG_JSON(m.c_str(), m.length());
  //   #endif
  // #endif

  this->value = val;
}

template <typename T> FiniteStateMachine::ConditionalCallback<T>* FiniteStateMachine::State<T>::addConditional(ConditionalCallback<T>* conditional) {
  for (unsigned char i = 0; i < MAX_CONDITIONALS; i++) {
    if(this->conditionals[i] == nullptr) {
      this->conditionals[i] = conditional;
      this->numconditionals++;
      return this->conditionals[i];
    }
  }
  return nullptr;
}

template <typename T> FiniteStateMachine::ConditionalCallback<T>* FiniteStateMachine::State<T>::addConditional(comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_none_t cb, typename ConditionalCallback<T>::cb_getref_t getReference) { return addConditional(this->key, cmp, ref, cb, getReference); }

template <typename T> FiniteStateMachine::ConditionalCallback<T>* FiniteStateMachine::State<T>::addConditional(fsm_key_t key, comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_none_t cb, typename ConditionalCallback<T>::cb_getref_t getReference) { return addConditional(new FiniteStateMachine::ConditionalCallback<T>(key, cmp, ref, cb, getReference)); }

template <typename T> FiniteStateMachine::ConditionalCallback<T>* FiniteStateMachine::State<T>::addConditional(fsm_key_t key, comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_comp_t cb, typename ConditionalCallback<T>::cb_getref_t getReference) { return addConditional(new FiniteStateMachine::ConditionalCallback<T>(key, cmp, ref, cb, getReference)); }

template <typename T> FiniteStateMachine::ConditionalCallback<T>* FiniteStateMachine::State<T>::addConditional(comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_comp_t cb, typename ConditionalCallback<T>::cb_getref_t getReference) { return addConditional(this->key, cmp, ref, cb, getReference); }

template <typename T> FiniteStateMachine::ConditionalCallback<T>* FiniteStateMachine::State<T>::addConditional(fsm_key_t key, comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_compval_t cb, typename ConditionalCallback<T>::cb_getref_t getReference) { return addConditional(new FiniteStateMachine::ConditionalCallback<T>(key, cmp, ref, cb, getReference)); }
template <typename T> FiniteStateMachine::ConditionalCallback<T>* FiniteStateMachine::State<T>::addConditional(comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_compval_t cb, typename ConditionalCallback<T>::cb_getref_t getReference) { return addConditional(this->key, cmp, ref, cb, getReference); }

template <typename T> FiniteStateMachine::ConditionalCallback<T>* FiniteStateMachine::State<T>::addConditional(fsm_key_t key, comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_compvalref_t cb, typename ConditionalCallback<T>::cb_getref_t getReference) { return addConditional(new FiniteStateMachine::ConditionalCallback<T>(key, cmp, ref, cb, getReference)); }
template <typename T> FiniteStateMachine::ConditionalCallback<T>* FiniteStateMachine::State<T>::addConditional(comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_compvalref_t cb, typename ConditionalCallback<T>::cb_getref_t getReference) { return addConditional(this->key, cmp, ref, cb, getReference); }

template <typename T> FiniteStateMachine::ConditionalCallback<T>* FiniteStateMachine::State<T>::addConditionalSetFlag(comparators_t cmp, const T& ref, Flag* flag, typename ConditionalCallback<T>::cb_getref_t getReference) { return addConditionalSetFlag(this->key, cmp, ref, flag, getReference); }
template <typename T> FiniteStateMachine::ConditionalCallback<T>* FiniteStateMachine::State<T>::addConditionalSetFlag(fsm_key_t key, comparators_t cmp, const T& ref, Flag* flag, typename ConditionalCallback<T>::cb_getref_t getReference) { return addConditional(new FlagSetCondition<T>(cmp, ref, flag, getReference)); }

template <typename T> FiniteStateMachine::ConditionalCallback<T>* FiniteStateMachine::State<T>::addConditionalSetFlag(comparators_t cmp, const T& ref, Flag* flag, bool invert, typename ConditionalCallback<T>::cb_getref_t getReference) { return addConditionalSetFlag(this->key, cmp, ref, flag, invert, getReference); }
template <typename T> FiniteStateMachine::ConditionalCallback<T>* FiniteStateMachine::State<T>::addConditionalSetFlag(fsm_key_t key, comparators_t cmp, const T& ref, Flag* flag, bool invert, typename ConditionalCallback<T>::cb_getref_t getReference) { return addConditional(new FlagSetCondition<T>(key, cmp, ref, flag, invert, getReference)); }

// OTHER ARGS

template <typename T> FiniteStateMachine::ConditionalCallback<T>* FiniteStateMachine::State<T>::addLatchingConditional(const T& on, const T& off, typename ConditionalCallback<T>::cb_comp_t cb) { return addLatchingConditional(on, off, (const void*)cb, CB_COMP); }

template <typename T> FiniteStateMachine::ConditionalCallback<T>* FiniteStateMachine::State<T>::addLatchingConditional(const T& on, const T& off, typename ConditionalCallback<T>::cb_compval_t cb) { return addLatchingConditional(on, off, (const void*)cb, CB_COMPVAL); }

template <typename T> FiniteStateMachine::ConditionalCallback<T>* FiniteStateMachine::State<T>::addLatchingConditional(const T& on, const T& off, typename ConditionalCallback<T>::cb_compvalref_t cb) { return addLatchingConditional(on, off, (const void*)cb, CB_COMPVALREF); }

template <typename T> FiniteStateMachine::ConditionalCallback<T>* FiniteStateMachine::State<T>::addLatchingConditional(fsm_key_t key, const T& on, const T& off, typename ConditionalCallback<T>::cb_comp_t cb) { return addLatchingConditional(key, on, off, (const void*)cb, CB_COMP); }

template <typename T> FiniteStateMachine::ConditionalCallback<T>* FiniteStateMachine::State<T>::addLatchingConditional(fsm_key_t key, const T& on, const T& off, typename ConditionalCallback<T>::cb_compval_t cb) { return addLatchingConditional(key, on, off, (const void*)cb, CB_COMPVAL); }

template <typename T> FiniteStateMachine::ConditionalCallback<T>* FiniteStateMachine::State<T>::addLatchingConditional(fsm_key_t key, const T& on, const T& off, typename ConditionalCallback<T>::cb_compvalref_t cb) { return addLatchingConditional(key, on, off, (const void*)cb, CB_COMPVALREF); }

template <typename T> bool FiniteStateMachine::State<T>::isDisabled() { return this->disabled; }
template <typename T> void FiniteStateMachine::State<T>::enable() { this->disabled = false; }
template <typename T> void FiniteStateMachine::State<T>::enable(const T& val) { this->disabled = false; this->set(val); }
template <typename T> void FiniteStateMachine::State<T>::disable() { this->disabled = true; }
template <typename T> void FiniteStateMachine::State<T>::disable(const T& val) { this->set(val); this->disabled = true; }
template <typename T> void FiniteStateMachine::State<T>::resume() { this->disabled = false; this->set(this->value); }

#endif
#endif
#endif