#ifndef FSM_COMPARATORS_H_
#error __FILE__ should only be included AFTER <comparators.h>
// #include <comparators.h>
#endif

#ifdef FSM_COMPARATORS_H_

#ifndef FSM_COMPARATORS_T_
#define FSM_COMPARATORS_T_

// #error COMPARATORS.TPP

#define BOOLSTR(b) (b ? "true" : "false")

#include <debug.h>

// TODO: All T args in callbacks -> const T&
// TODO: Replace all OneShot with Latching (duh)

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, const void* cb, callback_type_t cbtype, bool invert, cb_getref_t getReference) : comparator(cmp), reference(ref), callback(cb), callbacktype(cbtype), invert(invert), getReference(getReference) {
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

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, cb_none_t cb, bool invert, cb_getref_t getReference) : ConditionalCallback(cmp, ref, (const void*)cb, CB_NONE, invert, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, cb_comp_t cb, bool invert, cb_getref_t getReference) : ConditionalCallback(cmp, ref, (const void*)cb, CB_COMP, invert, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, cb_compval_t cb, bool invert, cb_getref_t getReference) : ConditionalCallback(cmp, ref, (const void*)cb, CB_COMPVAL, invert, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, cb_compvalref_t cb, bool invert, cb_getref_t getReference) : ConditionalCallback(cmp, ref, (const void*)cb, CB_COMPVALREF, invert, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, bool invert, cb_getref_t getReference) : ConditionalCallback(cmp, ref, nullptr, CB_COMP, invert, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, const void* cb, callback_type_t cbtype, cb_getref_t getReference) : comparator(cmp), reference(ref), callback(cb), callbacktype(cbtype), getReference(getReference) {
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

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, cb_none_t cb, cb_getref_t getReference) : ConditionalCallback(cmp, ref, (const void*)cb, CB_NONE, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, cb_comp_t cb, cb_getref_t getReference) : ConditionalCallback(cmp, ref, (const void*)cb, CB_COMP, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, cb_compval_t cb, cb_getref_t getReference) : ConditionalCallback(cmp, ref, (const void*)cb, CB_COMPVAL, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, cb_compvalref_t cb, cb_getref_t getReference) : ConditionalCallback(cmp, ref, (const void*)cb, CB_COMPVALREF, getReference) { }

template <typename T> ConditionalCallback<T>::ConditionalCallback(comparators_t cmp, const T& ref, cb_getref_t getReference) : ConditionalCallback(cmp, ref, nullptr, CB_COMP, getReference) { }

template <typename T> ConditionalCallback<T>& ConditionalCallback<T>::operator=(ConditionalCallback<T> const& other) { return ConditionalCallback<T>(other.comparator, other.reference, other.callback, other.callbacktype, other.getReference); }

template <typename T> void ConditionalCallback<T>::childCallback(bool comp, const T& val, const T& ref) { return; }
template <typename T> const T& ConditionalCallback<T>::childReference(const T& val) { return this->reference; }

template <typename T> void ConditionalCallback<T>::operator()(const T& val) { 
  if(this->disabled) return;
  
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
}

template <typename T> void ConditionalCallback<T>::disable() { this->disabled = true; }
template <typename T> void ConditionalCallback<T>::enable() { this->disabled = false; }
template <typename T> void ConditionalCallback<T>::enable(const T& val) { this->disabled = false; this->executeCallback(val, this->reference); }

#endif

#endif