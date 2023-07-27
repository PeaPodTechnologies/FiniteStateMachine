#ifndef FSM_STATELINKER_H_
#define FSM_STATELINKER_H_

#include <state.h>
#include <comparators.h>
#include <number.h>

template <typename T> class FlagSetCondition : public ConditionalCallback<T> {
  protected:
    Flag* flag;

    void setFlag(bool flag);

    void childCallback(bool comp, const T& val, const T& ref) override;
    
  public:
    FlagSetCondition(comparators_t cmp, const T& ref, Flag* flag, T (* const getReference)(T val) = nullptr);
    FlagSetCondition(comparators_t cmp, const T& ref, Flag* flag, bool invert, T (* const getReference)(T val) = nullptr);
};

class BangBangFlagSet : public FlagSetCondition<Number> {
  private:
    bool state = false;

    // Has the callback been triggered yet?
    bool triggered = false;

    const Number low;
    const Number high;
  protected:
    void callOperator(const Number& val, const Number& ref) override;
    void childCallback(bool comp, const Number& val, const Number& ref) override;
    const Number& childReference(const Number& val) override;
  public:
    BangBangFlagSet(const Number& lo, const Number& hi, Flag* flag);
};

// Mostly useless
class LatchingFlagSet : public FlagSetCondition<bool> {
  private:
    bool state = true;

    // Has the callback been triggered yet?
    bool triggered = false;
  protected:
    void callOperator(const bool& val, const bool& ref) override;
    void childCallback(bool comp, const bool& val, const bool& ref) override;
    const bool& childReference(const bool& val) override { return this->state; };
  public:
    LatchingFlagSet(Flag* flag, bool invert = false);
};

// template <typename T> class OneShotFlagReset : public FlagSetCondition<T> {
//   private:
//     bool triggered = false;

//     void callOperator(T val, T ref);
//   public:
//     OneShotFlagSet(comparators_t cmp, T ref, Flag* flag);

//     void operator()(T val);
//     void operator()(T val, T ref);
// };

#include <statelinker.tpp>

#endif