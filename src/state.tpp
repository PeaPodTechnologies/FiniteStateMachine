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

template <typename T> State<T>::State(const T& value) : value(value), defaultValue(value) { }

template <typename T> T State<T>::get(void) {
  return this->value;
}

template <typename T> void State<T>::set(const T& val) {
  #ifdef DEBUG
    DEBUG_DELAY();
    DEBUG.print("State Set: ");
    stateDebug<T>(val);
    DEBUG.print("\n");
    DEBUG.print("----------");
    DEBUG.print("\n");
    DEBUG_DELAY();
  #endif

  for(unsigned int i = 0; i < MAX_CONDITIONALS; i++) {
    if(this->conditionals[i] == nullptr) break;

    this->conditionals[i]->operator()(val);
  }

  #ifdef DEBUG
    DEBUG_DELAY();
    DEBUG.print("----------");
    DEBUG.print("\n");
    DEBUG_DELAY();
  #endif

  this->value = val;
}

template <typename T> ConditionalCallback<T>* State<T>::addConditional(ConditionalCallback<T>* conditional) {
  for (unsigned char i = 0; i < MAX_CONDITIONALS; i++) {
    if(this->conditionals[i] == nullptr) {
      this->conditionals[i] = conditional;
      return this->conditionals[i];
    }
  }
  return nullptr;
}

template <typename T> ConditionalCallback<T>* State<T>::addConditional(comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_none_t cb, typename ConditionalCallback<T>::cb_getref_t getReference) { return addConditional(new ConditionalCallback<T>(cmp, ref, cb, getReference)); }

template <typename T> ConditionalCallback<T>* State<T>::addConditional(comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_comp_t cb, typename ConditionalCallback<T>::cb_getref_t getReference) { return addConditional(new ConditionalCallback<T>(cmp, ref, cb, getReference)); }

template <typename T> ConditionalCallback<T>* State<T>::addConditional(comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_compval_t cb, typename ConditionalCallback<T>::cb_getref_t getReference) { return addConditional(new ConditionalCallback<T>(cmp, ref, cb, getReference)); }

template <typename T> ConditionalCallback<T>* State<T>::addConditional(comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_compvalref_t cb, typename ConditionalCallback<T>::cb_getref_t getReference) { return addConditional(new ConditionalCallback<T>(cmp, ref, cb, getReference)); }

template <typename T> ConditionalCallback<T>* State<T>::addConditionalSetFlag(comparators_t cmp, const T& ref, Flag* flag, typename ConditionalCallback<T>::cb_getref_t getReference) { return addConditional(new FlagSetCondition<T>(cmp, ref, flag, getReference)); }

template <typename T> ConditionalCallback<T>* State<T>::addConditionalSetFlag(comparators_t cmp, const T& ref, Flag* flag, bool invert, typename ConditionalCallback<T>::cb_getref_t getReference) { return addConditional(new FlagSetCondition<T>(cmp, ref, flag, invert, getReference)); }

// OTHER ARGS

template <typename T> ConditionalCallback<T>* State<T>::addLatchingConditional(const T& on, const T& off, typename ConditionalCallback<T>::cb_comp_t cb) { return addLatchingConditional(on, off, (const void*)cb, CB_COMP); }

template <typename T> ConditionalCallback<T>* State<T>::addLatchingConditional(const T& on, const T& off, typename ConditionalCallback<T>::cb_compval_t cb) { return addLatchingConditional(on, off, (const void*)cb, CB_COMPVAL); }

template <typename T> ConditionalCallback<T>* State<T>::addLatchingConditional(const T& on, const T& off, typename ConditionalCallback<T>::cb_compvalref_t cb) { return addLatchingConditional(on, off, (const void*)cb, CB_COMPVALREF); }

#endif
#endif
#endif