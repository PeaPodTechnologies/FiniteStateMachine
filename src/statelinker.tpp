#ifndef FSM_STATELINKER_H_
#error __FILE__ should only be included AFTER <statelinker.h>
// #include <statelinker.h>
#endif

#ifndef FSM_STATE_H_
#error __FILE__ should only be included AFTER <state.h>
// #include <state.h>
#endif

#ifdef FSM_STATELINKER_H_
#ifdef FSM_STATE_H_

#ifndef FSM_STATELINKER_T_
#define FSM_STATELINKER_T_

// #error STATELINKER.TPP

typedef void (*const flagSetCB_t)(bool);

template <typename T> FlagSetCondition<T>::FlagSetCondition(comparators_t cmp, const T& ref, Flag* flag, typename ConditionalCallback<T>::cb_getref_t getReference) : flag(flag), ConditionalCallback<T>(cmp, ref, getReference) { }

template <typename T> FlagSetCondition<T>::FlagSetCondition(comparators_t cmp, const T& ref, Flag* flag, bool invert, typename ConditionalCallback<T>::cb_getref_t getReference) : flag(flag), ConditionalCallback<T>(cmp, ref, invert, getReference) { }

template <typename T> void FlagSetCondition<T>::childCallback(bool comp, const T& val, const T& ref) {
  #ifdef DEBUG
    DEBUG_DELAY();
    DEBUG.print("Flag Set (");
    DEBUG.print(comp ? "true" : "false");
    DEBUG.print(")\n");
    DEBUG_DELAY();
  #endif
  this->flag->set(comp);
}

#endif
#endif
#endif