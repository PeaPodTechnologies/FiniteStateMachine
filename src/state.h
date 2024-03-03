#ifndef FSM_STATE_H_
#define FSM_STATE_H_

// Class Forward-Declarations
class Flag;
class Variable;

#include <comparators.h>
#include <number.h>

#define MAX_CONDITIONALS 10

// #ifdef FSM_DEBUG_SERIAL

// template <typename T> void stateDebug(const T& val);

// #endif

template <typename T> class State {
  private:
    T value;
    const T defaultValue;

    bool disabled = false;

    unsigned char numconditionals = 0;
  protected:
    // static String nullKey = String();
    fsm_key_t key;
    
    ConditionalCallback<T>* conditionals[MAX_CONDITIONALS] = { nullptr };
  public:
    explicit State(const T& value);
    State(const T& value, fsm_key_t key);
    ~State();

    fsm_key_t getKey() const { return this->key; }

    T get(void);
    void set(const T& val);

    ConditionalCallback<T>* addConditional(ConditionalCallback<T>* conditional);

    ConditionalCallback<T>* addConditional(comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_none_t cb, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);
    ConditionalCallback<T>* addConditional(fsm_key_t key, comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_none_t cb, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);

    ConditionalCallback<T>* addConditional(comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_comp_t cb, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);
    ConditionalCallback<T>* addConditional(fsm_key_t key, comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_comp_t cb, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);

    ConditionalCallback<T>* addConditional(comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_compval_t cb, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);
    ConditionalCallback<T>* addConditional(fsm_key_t key, comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_compval_t cb, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);

    ConditionalCallback<T>* addConditional(comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_compvalref_t cb, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);
    ConditionalCallback<T>* addConditional(fsm_key_t key, comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_compvalref_t cb, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);

    ConditionalCallback<T>* addConditionalSetFlag(comparators_t cmp, const T& ref, Flag* flag, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);
    ConditionalCallback<T>* addConditionalSetFlag(fsm_key_t key, comparators_t cmp, const T& ref, Flag* flag, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);

    ConditionalCallback<T>* addConditionalSetFlag(comparators_t cmp, const T& ref, Flag* flag, bool invert, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);
    ConditionalCallback<T>* addConditionalSetFlag(fsm_key_t key, comparators_t cmp, const T& ref, Flag* flag, bool invert, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);

    ConditionalCallback<T>* addLatchingConditional(const T& on, const T& off, const void* cb, callback_type_t cbtype);
    ConditionalCallback<T>* addLatchingConditional(const T& on, const T& off, typename ConditionalCallback<T>::cb_compvalref_t cb);
    ConditionalCallback<T>* addLatchingConditional(const T& on, const T& off, typename ConditionalCallback<T>::cb_compval_t cb);
    ConditionalCallback<T>* addLatchingConditional(const T& on, const T& off, typename ConditionalCallback<T>::cb_comp_t cb);

    ConditionalCallback<T>* addLatchingConditional(fsm_key_t key, const T& on, const T& off, const void* cb, callback_type_t cbtype);
    ConditionalCallback<T>* addLatchingConditional(fsm_key_t key, const T& on, const T& off, typename ConditionalCallback<T>::cb_compvalref_t cb);
    ConditionalCallback<T>* addLatchingConditional(fsm_key_t key, const T& on, const T& off, typename ConditionalCallback<T>::cb_compval_t cb);
    ConditionalCallback<T>* addLatchingConditional(fsm_key_t key, const T& on, const T& off, typename ConditionalCallback<T>::cb_comp_t cb);

    ConditionalCallback<T>* addLatchingSetFlag(const T& on, const T& off, Flag* flag);

    ConditionalCallback<T>* addLatchingSetFlag(fsm_key_t key, const T& on, const T& off, Flag* flag);

    // ConditionalCallback<T>* addConditionalSetFlag(comparators_t cmp, T ref, Flag* flag, bool oneshot = false);

    // ConditionalCallback<T>* addConditionalResetFlag(comparators_t cmp, T ref, Flag* flag, T (*const getReference)(void));

    // ConditionalCallback<T>* addConditionalResetFlag(comparators_t cmp, T ref, Flag* flag, bool oneshot = false);

    ConditionalCallback<T>* addLoggerCallback(typename ConditionalCallback<T>::cb_compval_t cb);

    // virtual T get(void) = 0;
    // virtual void set(T val) = 0;

    operator T() const { return value; };

    bool isDisabled();

    // Enable, No Set
    void enable();
    // Enable, Re-Set
    void resume();
    // Enable, Set
    void enable(const T& val);
    // Disable, No-Set
    void disable();
    // Disable, Set
    void disable(const T& val);

    String toString(void);
    static String toString(const T& val);
};

class Variable : public State<Number> {
  private:
  public:
    explicit Variable(const Number& value);
    explicit Variable(const double& value = 0.0f);
    explicit Variable(const float& value);
    explicit Variable(const long& value);
    explicit Variable(const int& value);
    explicit Variable(const unsigned long& value);
    explicit Variable(const unsigned int& value);

    Variable(const Number& value, fsm_key_t key);
    Variable(const double& value, fsm_key_t key);
    Variable(const float& value, fsm_key_t key);
    Variable(const long& value, fsm_key_t key);
    Variable(const int& value, fsm_key_t key);
    Variable(const unsigned long& value, fsm_key_t key);
    Variable(const unsigned int& value, fsm_key_t key);
    // Number get(void) override;
    // void set(Number val) override;
};

class Flag : public State<bool> {
  private:
  public:
    explicit Flag(const bool& value = false);
    Flag(const bool& value, fsm_key_t key);
    explicit Flag(fsm_key_t key);
    // bool get(void) override;
    // void set(bool val) override;
};

#include <statelinker.h>
#include <state.tpp>

#endif