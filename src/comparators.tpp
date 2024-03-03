#ifndef FSM_COMPARATORS_H_
#error __FILE__ should only be included AFTER <comparators.h>
// #include <comparators.h>
#endif

#ifdef FSM_COMPARATORS_H_

#ifndef FSM_COMPARATORS_T_
#define FSM_COMPARATORS_T_

// #error COMPARATORS.TPP

#include <debug.h>

// TODO: All T args in callbacks -> const T&
// TODO: Replace all OneShot with Latching (duh)

// WITH KEYS WITH INVERT

// comparators_t cmp, const T& ref, const void* cb, callback_type_t cbtype, bool invert, cb_getref_t getReference

template <typename T> ConditionalCallback<T>::ConditionalCallback(fsm_key_t key, comparators_t cmp, const T& ref, const void* cb, callback_type_t cbtype, bool invert, cb_getref_t getReference) : key(key), comparator(cmp), reference(ref), callback(cb), callbacktype(cbtype), invert(invert), getReference(getReference) {
  #ifdef DEBUG_JSON
    #ifdef DEBUG_USE_BP
      BP_JSON();
    #else
      String m = _F("New Callback ");
      // if(this->key.length() > 0) { m += '\''; m += this->key; m += '\''; } else { m += 'X'; }
      if(this->key != FSM_KEY_NULL) { m += '\''; m += this->key; m += '\''; } else { m += 'X'; }
      m += ' ';
      m += parseComparator(cmp);
      m += ' ';
      m += stateToString<T>(ref);
      if(invert) m += _F(" (INV)");
      if(getReference != nullptr) m += _F(" (DYN)");
      if(this->callback == nullptr) {
        m += _F(", CB None");
      } else {
        m += _F(", CB Args [");
        if(this->callbacktype >= CB_COMP) {
          m += _F("bool");
        }
        if(this->callbacktype >= CB_COMPVAL) {
          m += _F(", val");
        }
        if(this->callbacktype >= CB_COMPVALREF) {
          m += _F(", ref");
        }
        m+= _F("] @0x");
        m += String((unsigned long)this->callback, HEX);
      }
      // DEBUG_JSON(m.c_str(), m.length());
      DEBUG_JSON(m);
    #endif
  #endif
}

template <typename T> ConditionalCallback<T>::ConditionalCallback(fsm_key_t key, comparators_t cmp, const T& ref, cb_none_t cb, bool invert, cb_getref_t getReference) : ConditionalCallback(key, cmp, ref, (const void*)cb, CB_NONE, invert, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(fsm_key_t key, comparators_t cmp, const T& ref, cb_comp_t cb, bool invert, cb_getref_t getReference) : ConditionalCallback(key, cmp, ref, (const void*)cb, CB_COMP, invert, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(fsm_key_t key, comparators_t cmp, const T& ref, cb_compval_t cb, bool invert, cb_getref_t getReference) : ConditionalCallback(key, cmp, ref, (const void*)cb, CB_COMPVAL, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(fsm_key_t key, comparators_t cmp, const T& ref, cb_compvalref_t cb, bool invert, cb_getref_t getReference) : ConditionalCallback(key, cmp, ref, (const void*)cb, invert, CB_COMPVALREF, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(fsm_key_t key, comparators_t cmp, const T& ref, bool invert, cb_getref_t getReference) : ConditionalCallback(key, cmp, ref, nullptr, CB_COMP, invert, getReference) { }

// WITH KEYS WITHOUT INVERT

template <typename T> ConditionalCallback<T>::ConditionalCallback(fsm_key_t key, comparators_t cmp, const T& ref, const void* cb, callback_type_t cbtype, cb_getref_t getReference) : ConditionalCallback<T>::ConditionalCallback(key, cmp, ref, cb, cbtype, false, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(fsm_key_t key, comparators_t cmp, const T& ref, cb_none_t cb, cb_getref_t getReference) : ConditionalCallback(key, cmp, ref, (const void*)cb, CB_NONE, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(fsm_key_t key, comparators_t cmp, const T& ref, cb_comp_t cb, cb_getref_t getReference) : ConditionalCallback(key, cmp, ref, (const void*)cb, CB_COMP, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(fsm_key_t key, comparators_t cmp, const T& ref, cb_compval_t cb, cb_getref_t getReference) : ConditionalCallback(key, cmp, ref, (const void*)cb, CB_COMPVAL, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(fsm_key_t key, comparators_t cmp, const T& ref, cb_compvalref_t cb, cb_getref_t getReference) : ConditionalCallback(key, cmp, ref, (const void*)cb, CB_COMPVALREF, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(fsm_key_t key, comparators_t cmp, const T& ref, cb_getref_t getReference) : ConditionalCallback(key, cmp, ref, nullptr, CB_COMP, getReference) { }

// WITHOUT KEYS WITH INVERT

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, const void* cb, callback_type_t cbtype, bool invert, cb_getref_t getReference) : ConditionalCallback<T>(FSM_KEY_NULL, cmp, ref, cb, cbtype, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, cb_none_t cb, bool invert, cb_getref_t getReference) : ConditionalCallback(cmp, ref, (const void*)cb, CB_NONE, invert, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, cb_comp_t cb, bool invert, cb_getref_t getReference) : ConditionalCallback(cmp, ref, (const void*)cb, CB_COMP, invert, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, cb_compval_t cb, bool invert, cb_getref_t getReference) : ConditionalCallback(cmp, ref, (const void*)cb, CB_COMPVAL, invert, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, cb_compvalref_t cb, bool invert, cb_getref_t getReference) : ConditionalCallback(cmp, ref, (const void*)cb, CB_COMPVALREF, invert, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, bool invert, cb_getref_t getReference) : ConditionalCallback(cmp, ref, nullptr, CB_COMP, invert, getReference) { }

// WITHOUT KEYS WITHOUT INVERT

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, const void* cb, callback_type_t cbtype, cb_getref_t getReference) : ConditionalCallback<T>(String(), cmp, ref, cb, cbtype, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, cb_none_t cb, cb_getref_t getReference) : ConditionalCallback(cmp, ref, (const void*)cb, CB_NONE, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, cb_comp_t cb, cb_getref_t getReference) : ConditionalCallback(cmp, ref, (const void*)cb, CB_COMP, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, cb_compval_t cb, cb_getref_t getReference) : ConditionalCallback(cmp, ref, (const void*)cb, CB_COMPVAL, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, cb_compvalref_t cb, cb_getref_t getReference) : ConditionalCallback(cmp, ref, (const void*)cb, CB_COMPVALREF, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, cb_getref_t getReference) : ConditionalCallback(cmp, ref, nullptr, CB_COMP, getReference) { }

template <typename T> ConditionalCallback<T>& ConditionalCallback<T>::operator=(ConditionalCallback<T> const& other) { return ConditionalCallback<T>(other.comparator, other.reference, other.callback, other.callbacktype, other.getReference); }

template <typename T> void ConditionalCallback<T>::childCallback(bool comp, const T& val, const T& ref) { return; }
template <typename T> const T& ConditionalCallback<T>::childReference(const T& val) { return this->reference; }

template <typename T> void ConditionalCallback<T>::operator()(const T& val) { 
  #ifdef DEBUG_JSON
    #ifdef DEBUG_USE_BP
      BP_JSON();
    #else
      String m = _F("Condition ");
      // if(this->key.length() > 0) { m += '\''; m += this->key; m += '\''; m += ' '; }
      if(this->key != FSM_KEY_NULL) { m += '\''; m += this->key; m += '\''; m += ' '; }
      m += _F("CallOp; ");
      if(this->disabled){
        m += _F("CB Disabled");
        // DEBUG_JSON(m.c_str(), m.length());
        DEBUG_JSON(m);
        // Serial.print(">>>");
        // Serial.println(m);
      }
    #endif
  #endif
  if(this->disabled){
    return;
  }
  
  // Update the reference value
  if (this->getReference != nullptr) {
    this->reference = this->getReference(val);
    #ifdef DEBUG_JSON
      #ifndef DEBUG_USE_BP
        m += _F("DYN = ");
      #endif
    #endif
  } else {
    this->reference = this->childReference(val);
  }
  #ifdef DEBUG_JSON
    #ifdef DEBUG_USE_BP
      BP_JSON();
    #else
      m += _F("Ref = ");
      m += stateToString<T>(this->reference);
      // Serial.print(">>>");
      // Serial.println(m);
      // DEBUG_JSON(m.c_str(), m.length());
      DEBUG_JSON(m);
      // delay(1);
    #endif
  #endif

  this->callOperator(val, this->reference);
}

template <typename T> void ConditionalCallback<T>::operator()(const T& val, const T& ref) { this->callOperator(val, ref); }

template <typename T> void ConditionalCallback<T>::setComparator(comparators_t cmp) {
  #ifdef DEBUG_JSON
    #ifdef DEBUG_USE_BP
      BP_JSON();
    #else
      String m = _F("Comparator Change = ");
      m += parseComparator(cmp);
      // DEBUG_JSON(m.c_str(), m.length());
      DEBUG_JSON(m);
    #endif
  #endif
  this->comparator = cmp;
}

template <typename T> void ConditionalCallback<T>::callOperator(const T& val, const T& ref) {
  bool result = ConditionalCallback<T>::compare(this->comparator, val, ref);

  if(result) this->executeCallback(val, ref);
}

template <typename T> void ConditionalCallback<T>::executeCallback(const T& val, const T& ref) {
  #ifdef DEBUG_JSON
    #ifdef DEBUG_USE_BP
      BP_JSON();
    #else
      String m = _F("CB Execution Args [");
      if(this->callbacktype >= CB_COMP) {
        m += _F("bool");
      }
      if(this->callbacktype >= CB_COMPVAL) {
        m += _F(", val");
      }
      if(this->callbacktype >= CB_COMPVALREF) {
        m += _F(", ref");
      }
      m += ']';
      if(invert) m += _F(" (INV)");
      if(this->callback != nullptr) {
        m += _F(" @0x");
        m += String((unsigned long)this->callback, HEX);
      }
      // Serial.print(">>>");
      // Serial.println(m);
      // delay(1);
      DEBUG_JSON(m);
      // TODO: When the call stack gets too deep all hell breaks loose - @fixed by using a global static buffer
    #endif
  #endif

  bool _ = this->invert ? false : true;

  // #ifdef FSM_DEBUG_SERIAL
  //   DEBUG_DELAY();
  //   FSM_DEBUG_SERIAL.print("Passing ");
  //   FSM_DEBUG_SERIAL.print(_ ? "true" : "false");
  //   FSM_DEBUG_SERIAL.print("\n");
  //   DEBUG_DELAY();
  // #endif

  if(this->callback == nullptr) {
    // Assumes childCallback
    // #ifdef DEBUG_JSON
    //   #ifdef DEBUG_USE_BP
    //     BP_JSON();
    //   #else
    //     // TODO: Fix ALL of these
    //     // DEBUG_JSON(_F("No CB; Child CB"));
    //     Serial.println(_F(">>>No CB; Child CB"));
    //     delay(1);
    //   #endif
    // #endif
    this->childCallback(_, val, ref);
    return;
  };
  
  // Cast void* cb fn pointer to appropriate type and call
  switch(this->callbacktype) {
    case CB_NONE:
      // Called with no args
      ((cb_none_t)this->callback)();
      break;
    case CB_COMP:
      ((cb_comp_t)this->callback)(_);
      break;
    case CB_COMPVAL:
      ((cb_compval_t)this->callback)(_, val);
      break;
    case CB_COMPVALREF:
      ((cb_compvalref_t)this->callback)(_, val, ref);
      break;
  }
  // delay(1);
}

template <typename T> void ConditionalCallback<T>::disable() { this->disabled = true; }
template <typename T> void ConditionalCallback<T>::enable() { this->disabled = false; }
template <typename T> void ConditionalCallback<T>::enable(const T& val) { this->disabled = false; this->executeCallback(val, this->reference); }

#endif

#endif