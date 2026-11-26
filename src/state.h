#ifndef FSM_STATE_H_
#define FSM_STATE_H_

// Class Forward-Declarations
namespace FSM {
  class Flag;
  class Variable;
};

#include <comparators.h>
#include <hashtable.h>
#include <number.h>

#define MAX_CONDITIONALS 10

// #ifdef FSM_DEBUG_SERIAL

// template <typename T> void stateDebug(const T& val);

// #endif

namespace FSM {
  typedef enum {
    ARGS_NONE = 0,
    ARGS_VOID_PTR,
    ARGS_COMPOUND,
    ARGS_BOOL,
    ARGS_CHAR,
    ARGS_SHORT,
    ARGS_INT,
    ARGS_LONG,
    ARGS_UCHAR,
    ARGS_USHORT,
    ARGS_UINT,
    ARGS_ULONG,
    ARGS_FLOAT,
    ARGS_DOUBLE,
  } args_type_t;

  typedef struct {
    const bool* flags;
    unsigned int flagCount;
    const Number* variables;
    unsigned int variableCount;
  } callback_args_compound_values_t;

  class CallbackArgs {
    private:
      bool owned = false;

      void clearOwnedValue();
      void copyFrom(const CallbackArgs& other);
      static const void* cloneValue(const void* source, args_type_t type);

    public:
      const void* value = nullptr;
      args_type_t type = ARGS_NONE;

      CallbackArgs();
      CallbackArgs(const void* value, args_type_t type = ARGS_VOID_PTR);

      CallbackArgs(const bool& value);
      CallbackArgs(const char& value);
      CallbackArgs(const short& value);
      CallbackArgs(const int& value);
      CallbackArgs(const long& value);
      CallbackArgs(const unsigned char& value);
      CallbackArgs(const unsigned short& value);
      CallbackArgs(const unsigned int& value);
      CallbackArgs(const unsigned long& value);
      CallbackArgs(const float& value);
      CallbackArgs(const double& value);

      CallbackArgs(const CallbackArgs& other);
      CallbackArgs& operator=(const CallbackArgs& other);

      virtual const void* getPointer(args_type_t* typeOut = nullptr) const;
      virtual bool get(bool& out) const;
      virtual bool get(char& out) const;
      virtual bool get(short& out) const;
      virtual bool get(int& out) const;
      virtual bool get(long& out) const;
      virtual bool get(unsigned char& out) const;
      virtual bool get(unsigned short& out) const;
      virtual bool get(unsigned int& out) const;
      virtual bool get(unsigned long& out) const;
      virtual bool get(float& out) const;
      virtual bool get(double& out) const;
      virtual bool get(callback_args_compound_values_t& out) const;

      virtual ~CallbackArgs();
  };

  template <typename T> class State {
    private:
      T value;
      const T defaultValue;

      bool disabled = false;

      unsigned char numconditionals = 0;
    protected:
      detail::OwnedKey key;
      
      ConditionalCallback<T>* conditionals[MAX_CONDITIONALS] = { nullptr };
    public:
      explicit State(const T& value);
      State(const T& value, fsm_key_t key);
      ~State();

      fsm_key_t getKey() const { return this->key; }

      virtual T get(void) const;
      virtual void set(const T& val);

      void removeConditional(ConditionalCallback<T>* conditional);

      ConditionalCallback<T>* addConditional(ConditionalCallback<T>* conditional);

      ConditionalCallback<T>* addConditional(comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_none_t cb, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);
      ConditionalCallback<T>* addConditional(fsm_key_t key, comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_none_t cb, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);

      ConditionalCallback<T>* addConditional(comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_comp_t cb, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);
      ConditionalCallback<T>* addConditional(fsm_key_t key, comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_comp_t cb, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);

      ConditionalCallback<T>* addConditional(comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_compval_t cb, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);
      ConditionalCallback<T>* addConditional(fsm_key_t key, comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_compval_t cb, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);

      ConditionalCallback<T>* addConditional(comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_compvalref_t cb, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);
      ConditionalCallback<T>* addConditional(fsm_key_t key, comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_compvalref_t cb, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);
      ConditionalCallback<T>* addConditional(comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_compvalrefargs_t cb, const CallbackArgs* callbackArgs, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);
      ConditionalCallback<T>* addConditional(fsm_key_t key, comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_compvalrefargs_t cb, const CallbackArgs* callbackArgs, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);
      ConditionalCallback<T>* addConditional(comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_compvalrefargs_t cb, const CallbackArgs& callbackArgs, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);
      ConditionalCallback<T>* addConditional(fsm_key_t key, comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_compvalrefargs_t cb, const CallbackArgs& callbackArgs, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);
      ConditionalCallback<T>* addConditionalWithFlag(comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_compvalrefargs_t cb, const Flag& callbackFlag, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);
      ConditionalCallback<T>* addConditionalWithFlag(fsm_key_t key, comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_compvalrefargs_t cb, const Flag& callbackFlag, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);
      ConditionalCallback<T>* addConditionalWithVariable(comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_compvalrefargs_t cb, const Variable& callbackVariable, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);
      ConditionalCallback<T>* addConditionalWithVariable(fsm_key_t key, comparators_t cmp, const T& ref, typename ConditionalCallback<T>::cb_compvalrefargs_t cb, const Variable& callbackVariable, typename ConditionalCallback<T>::cb_getref_t getReference = nullptr);

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

      ConditionalCallback<T>* addCallback(typename ConditionalCallback<T>::cb_compval_t cb);

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

  class CallbackArgsVariable : public CallbackArgs {
    private:
      const Variable* source;

    public:
      CallbackArgsVariable(const Variable& source);

      bool get(char& out) const override;
      bool get(short& out) const override;
      bool get(int& out) const override;
      bool get(long& out) const override;
      bool get(unsigned char& out) const override;
      bool get(unsigned short& out) const override;
      bool get(unsigned int& out) const override;
      bool get(unsigned long& out) const override;
      bool get(float& out) const override;
      bool get(double& out) const override;
  };

  class CallbacksArgsFlag : public CallbackArgs {
    private:
      const Flag* source;

    public:
      CallbacksArgsFlag(const Flag& source);

      bool get(bool& out) const override;
  };

  class CallbackArgsCompound : public CallbackArgs {
    private:
      mutable bool* flagValues = nullptr;
      mutable Number* variableValues = nullptr;
      mutable unsigned int flagValuesCount = 0;
      mutable unsigned int variableValuesCount = 0;

    public:
      HashTable<Variable> variables;
      HashTable<Flag> flags;

      CallbackArgsCompound();
      ~CallbackArgsCompound() override;

      bool get(callback_args_compound_values_t& out) const override;

      void addVariable(const char* key, Variable* variable, bool overwrite = true);
      void addFlag(const char* key, Flag* flag, bool overwrite = true);

      Variable* getVariable(const char* key) const;
      Flag* getFlag(const char* key) const;

      CallbackArgsCompound(const CallbackArgsCompound&) = delete;
      CallbackArgsCompound& operator=(const CallbackArgsCompound&) = delete;
      CallbackArgsCompound(CallbackArgsCompound&& other) = delete;
      CallbackArgsCompound& operator=(CallbackArgsCompound&& other) = delete;
  };
};

#include <statelinker.h>
#include <state.tpp>

#endif