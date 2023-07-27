#ifndef FSM_STATE_H_
#define FSM_STATE_H_

#include <number.h>
#include <comparators.h>

#define MAX_CONDITIONALS 10

// Class Pre-Declarations (for Set-Conditionals)
class Flag;
class FlipFlop;

template <typename T> class State {
  private:
    T value;
    const T defaultValue;

    unsigned char numconditionals = 0;
  protected:
    ConditionalCallback<T>* conditionals[MAX_CONDITIONALS] = { nullptr };
  public:
    State(const T& value);
    ~State();

    T get(void);
    void set(const T& val);

    ConditionalCallback<T>* addConditional(ConditionalCallback<T>* conditional);

    ConditionalCallback<T>* addConditional(comparators_t cmp, const T& ref, void (*cb)(void), T (*getReference)(void));

    ConditionalCallback<T>* addConditional(comparators_t cmp, const T& ref, void (*cb)(bool comp), T (*getReference)(void));

    ConditionalCallback<T>* addConditional(comparators_t cmp, const T& ref, void (*cb)(bool comp, T val), T (*getReference)(void));

    ConditionalCallback<T>* addConditional(comparators_t cmp, const T& ref, void (*cb)(bool comp, T val, T ref), T (*getReference)(void));

    ConditionalCallback<T>* addConditionalSetFlag(comparators_t cmp, const T& ref, Flag* flag, T (*const getReference)(void));

    ConditionalCallback<T>* addLatchingConditional(const T& on, const T& off, const void* cb, callback_type_t cbtype);
    ConditionalCallback<T>* addLatchingConditional(const T& on, const T& off, void (* const cb)(bool comp, T val, T ref));
    ConditionalCallback<T>* addLatchingConditional(const T& on, const T& off, void (* const cb)(bool comp, T val));
    ConditionalCallback<T>* addLatchingConditional(const T& on, const T& off, void (* const cb)(bool comp));

    ConditionalCallback<T>* addLatchingSetFlag(const T& on, const T& off, Flag* flag);

    // ConditionalCallback<T>* addConditionalSetFlag(comparators_t cmp, T ref, Flag* flag, bool oneshot = false);

    // ConditionalCallback<T>* addConditionalResetFlag(comparators_t cmp, T ref, Flag* flag, T (*const getReference)(void));

    // ConditionalCallback<T>* addConditionalResetFlag(comparators_t cmp, T ref, Flag* flag, bool oneshot = false);

    ConditionalCallback<T>* addLoggerCallback(void (*cb)(bool comp, T val));

    // virtual T get(void) = 0;
    // virtual void set(T val) = 0;

    operator T() const { return value; };
};

class Variable : public State<Number> {
  private:
  public:
    Variable(const Number& value);
    Variable(const double& value);
    Variable(const float& value);
    Variable(const long& value);
    Variable(const int& value);
    Variable(const unsigned long& value);
    Variable(const unsigned int& value);
    // Number get(void) override;
    // void set(Number val) override;
};

class Flag : public State<bool> {
  private:
  public:
    Flag(const bool& value = false);
    // bool get(void) override;
    // void set(bool val) override;
};

#include <state.tpp>

#endif