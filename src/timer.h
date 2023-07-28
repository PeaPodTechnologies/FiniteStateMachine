#ifndef FSM_TIMER_H_
#define FSM_TIMER_H_

#include <state.h>
#include <statelinker.h>
#include <comparators.h>
#include <number.h>

#define INTERVAL_RESET_WINDOW 1.25
#define TWENTYFOURHRS_MILLIS (24*60*60*1000)

class Interval : private ConditionalCallback<unsigned long> {
  private:
    // Time since last reset (ms)
    unsigned long now = 0;
  protected:
    const unsigned long& childReference(const unsigned long& val) override;
  public:
    Interval(const unsigned long& delta, const void* cb, callback_type_t cbtype);

    Interval(const unsigned long& now, const unsigned long& delta, const void* cb, callback_type_t cbtype);

    unsigned long get(void);
    void set(const unsigned long& val);
};

/**
 * TIMER
 * 
 * Keeps track of relative time, in milliseconds from program start.
*/

class _Timer : private State<unsigned long> {
  private:
    const unsigned long start;
    const unsigned long last;

    Interval* intervals[MAX_CONDITIONALS] = { };

    void incrementIntervals(const unsigned long& delta);
  public:
    _Timer(const unsigned long& start = 1000);

    ConditionalCallback<unsigned long>* addEvent(const unsigned long& timestamp, void (* const cb)(bool, unsigned long), unsigned long (*const getReference)(void) = nullptr);
    ConditionalCallback<unsigned long>* addEventFlag(const unsigned long& timestamp, Flag* flag, unsigned long (*const getReference)(void) = nullptr);
    Interval* addInterval(const unsigned long& delta, void (* const cb)(bool, unsigned long), unsigned long (*const getReference)(void) = nullptr);
    
    Interval* addTwentyFourTimeout(const unsigned long& delay, void (* const cb)(bool, unsigned long), unsigned long (*const getReference)(void) = nullptr);

    // EXPOSE SOME PUBLIC BASE CLASS MEMBERS

    void set(const unsigned long& val);

    ConditionalCallback<unsigned long>* addLoggerCallback(void (*cb)(bool, unsigned long));
};

static _Timer Timer;

#endif