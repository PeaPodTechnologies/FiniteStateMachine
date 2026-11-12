#ifndef FSM_CONTROL_H_
#define FSM_CONTROL_H_

#include <state.h>

#ifndef FSM_PID_DT_CLAMP_MS
#define FSM_PID_DT_CLAMP_MS 1000UL
#endif

namespace FSM {

  void callback_computePID(bool _, const Number& measured, const Number& setpoint, const CallbackArgs* args);

  class ConditionalCallbackPID: public ConditionalCallback<Number> {
    public:
      ConditionalCallbackPID(fsm_key_t key, cb_compval_t cb, const Number& setpoint, const Number& kp, const Number& ki, const Number& kd);
      ConditionalCallbackPID(fsm_key_t key, const Number& setpoint, const Number& kp, const Number& ki, const Number& kd);

      void setSetpoint(const Number& setpoint);
      void setGains(const Number& kp, const Number& ki, const Number& kd);
      void reset();

      Number getSetpoint() const;
      Number getOutput() const;
      Number getIntegral() const;
      Number getPreviousError() const;
      Number getLastComputeMs() const;

      const Number& childReference(const Number& measured) override;

      ConditionalCallbackPID(const ConditionalCallbackPID&) = delete;
      ConditionalCallbackPID& operator=(const ConditionalCallbackPID&) = delete;
      ConditionalCallbackPID(ConditionalCallbackPID&&) = delete;
      ConditionalCallbackPID& operator=(ConditionalCallbackPID&&) = delete;

    private:
      Number setpoint;
      CallbackArgsCompound pidArgs;
      Variable kp, ki, kd;
      Variable integral;
      Variable previousError;
      Variable output;
      Variable last_compute_ms;
  };
} // namespace FSM

#endif // FSM_CONTROL_H_