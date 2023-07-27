#ifndef FSM_COMPARATORS_H_
#define FSM_COMPARATORS_H_

#include <number.h>

#define DEBUG_DELAY() {delay(100);}

#define NOP_COMP_BOOL true
#define NOP_COMP_NUMBER false

typedef enum {
  CMP_NOP = 0,
  CMP_EQU,
  CMP_NEQ,
  CMP_LES,
  CMP_LEQ,
  CMP_GTR,
  CMP_GEQ,
} comparators_t;

typedef enum {
  CB_NONE = 0,   // void (* const)(void) - NO ARGS
  CB_COMP,       // void (* const)(bool) - ADD: CONDITION RESULT
  CB_COMPVAL,    // void (* const)(bool, T) - ADD: PASSED VALUE
  CB_COMPVALREF, // void (* const)(bool, T, T) - ADD: REFERENCE VALUE
} callback_type_t;

// template <typename A> struct conditional_callbacks_t {
//   typedef ;
//   typedef ;
//   typedef );
//   typedef void (* const cb_ref_t)(bool comp, A val, A ref);
//   typedef A (* const cb_getref_t)(void);
// };

// TODO: Compound conditionals?

// CONSTRUCTION: 



// USE: CONDITION - COMPARISON OF VALUE <TYPE T> TO REFERENCE
// IF TRUE: EXECUTION

/**
 * @param T type of the two values to be compared
*/
template <typename T> class ConditionalCallback {
  public:
    /**
     * @param cmp COMPARATOR
     * @param ref REFERENCE VALUE <TYPE T>
     * @param cb CALLBACK FUNCTION POINTER (CAST TO CONST VOID*)
     * @param cbtype CALLBACK TYPE - dictates argument set, cb pointer recast
     * REFERENCE UPDATOR (OPTIONAL)
     **/
    ConditionalCallback(comparators_t cmp, const T& ref, const void* cb, callback_type_t cbtype, T (* const getReference)(T val) = nullptr);
    ConditionalCallback(comparators_t cmp, const T& ref, void (* const cb)(void), T (* const getReference)(T val) = nullptr);
    ConditionalCallback(comparators_t cmp, const T& ref, void (* const cb)(bool comp), T (* const getReference)(T val) = nullptr);
    ConditionalCallback(comparators_t cmp, const T& ref, void (* const cb)(bool comp, T val), T (* const getReference)(T val) = nullptr);
    ConditionalCallback(comparators_t cmp, const T& ref, void (* const cb)(bool comp, T val, T ref), T (* const getReference)(T val) = nullptr);

    ConditionalCallback(comparators_t cmp, const T& ref, const void* cb, callback_type_t cbtype, bool invert, T (* const getReference)(T val) = nullptr);
    ConditionalCallback(comparators_t cmp, const T& ref, void (* const cb)(void), bool invert, T (* const getReference)(T val) = nullptr);
    ConditionalCallback(comparators_t cmp, const T& ref, void (* const cb)(bool comp), bool invert, T (* const getReference)(T val) = nullptr);
    ConditionalCallback(comparators_t cmp, const T& ref, void (* const cb)(bool comp, T val), bool invert, T (* const getReference)(T val) = nullptr);
    ConditionalCallback(comparators_t cmp, const T& ref, void (* const cb)(bool comp, T val, T ref), bool invert, T (* const getReference)(T val) = nullptr);

    ConditionalCallback<T>& operator=(ConditionalCallback<T> const& other);

    void operator()(const T& val);
    void operator()(const T& val, const T& ref);

  protected:
    // NO CALLBACK - ASSUMES childCallback is implemented
    ConditionalCallback(comparators_t cmp, const T& ref, T (* const getReference)(T val) = nullptr);
    ConditionalCallback(comparators_t cmp, const T& ref, bool invert, T (* const getReference)(T val) = nullptr);

    // FALLBACKS FOR FUNCPOINTERS
    virtual void childCallback(bool comp, const T& val, const T& ref) { }; // NOP
    virtual const T& childReference(const T& val); // NOP

    // PROPERTIES

    void setComparator(comparators_t cmp);

    comparators_t comparator;
    
    static bool compare(comparators_t cmp, const T& val, const T& ref);

    virtual void callOperator(const T& val, const T& ref);

    T reference;

    const bool invert = false;

    const void* callback;

    const callback_type_t callbacktype;

    T (* const getReference)(T val) = nullptr;

    void executeCallback(const T& val, const T& ref);
};

/**
 * Only performs comparison if the result has never been true (until reset)
 * Overrides call operator() - if not `triggered`, calls the base class (ConditionalCallback<T>) operator()
*/
class BangBangConditional : public ConditionalCallback<Number> {
  private:
    bool state = false;

    // Has the callback been triggered yet?
    bool triggered = false;

    const Number low;
    const Number high;

    const void* rising;
    const void* falling;

    const callback_type_t latchcbtype;

  protected:
    void childCallback(bool comp, const Number& val, const Number& ref) override;
    const Number& childReference(const Number& val) override;
    void callOperator(const Number& val, const Number& ref) override;
  public:
    /**
     * When val < `lo`, callback(true)
     * When val > `hi`, callback(false)
     * `invert` inverts argument value
     * Args are parsed with min/max so that hi > lo
    */
    BangBangConditional(const Number& lo, const Number& hi, void (* const rising)(void), void (* const falling)(void));
    BangBangConditional(const Number& lo, const Number& hi, const void* rising, const void* falling, callback_type_t cbtype);
    BangBangConditional(const Number& lo, const Number& hi, void (* const cb)(void));
    BangBangConditional(const Number& lo, const Number& hi, const void* cb, callback_type_t cbtype);
};

/**
 * Only performs comparison if the result has never been true (until reset)
 * Overrides call operator() - if not `triggered`, calls the base class (ConditionalCallback<T>) operator()
*/
class LatchingConditional : public ConditionalCallback<bool> {
  private:
    bool state = true;

    // Has the callback been triggered yet?
    bool triggered = false;

    const void* rising;
    const void* falling;

    const callback_type_t latchcbtype;

  protected:
    void childCallback(bool comp, const bool& val, const bool& ref) override;
    const bool& childReference(const bool& val) override { return this->state; };
    void callOperator(const bool& val, const bool& ref) override;
  public:
    LatchingConditional(void (* const rising)(void), void (* const falling)(void), bool invert = false);
    LatchingConditional(const void* rising, const void* falling, callback_type_t cbtype, bool invert = false);
    LatchingConditional(void (* const cb)(void), bool invert = false);
    LatchingConditional(const void* cb, callback_type_t cbtype, bool invert = false);
};

#include <comparators.tpp>

#endif