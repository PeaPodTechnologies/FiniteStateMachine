#include <pid.h>

using namespace FSM;

void FSM::callback_computePID(bool _, const Number& measured, const Number& setpoint, const CallbackArgs* args) {
  if(args == nullptr || args->type != ARGS_COMPOUND) return;
  const CallbackArgsCompound* compoundArgs = (const CallbackArgsCompound*)args;

  Variable* kp = compoundArgs->getVariable("kp");
  Variable* ki = compoundArgs->getVariable("ki");
  Variable* kd = compoundArgs->getVariable("kd");
  Variable* integral = compoundArgs->getVariable("integral");
  Variable* previousError = compoundArgs->getVariable("previousError");
  Variable* output = compoundArgs->getVariable("output");
  Variable* last_compute_ms = compoundArgs->getVariable("last_compute_ms");

  if(kp == nullptr || ki == nullptr || kd == nullptr || integral == nullptr || previousError == nullptr || output == nullptr || last_compute_ms == nullptr) return;

  Number now = millis();
  Number error = setpoint - measured;

  Number dt = now - last_compute_ms->get();
  Number derivative = Number(0);
  if(dt > Number(0)) {
    Number dt_clamped = dt;
    Number max_dt = Number((unsigned long)FSM_PID_DT_CLAMP_MS);
    if(dt_clamped > max_dt) dt_clamped = max_dt;

    derivative = (error - previousError->get()) / dt_clamped;
    integral->set(integral->get() + error * dt_clamped);
  }
  last_compute_ms->set(now);

  previousError->set(error);

  output->set(kp->get() * error + ki->get() * integral->get() + kd->get() * derivative);
}

FSM::ConditionalCallbackPID::ConditionalCallbackPID(fsm_key_t key, ConditionalCallback<Number>::cb_compval_t cb, const Number& setpoint, const Number& kp, const Number& ki, const Number& kd)
  : ConditionalCallback<Number>(key, CMP_NEQ, setpoint, callback_computePID, false, &pidArgs, nullptr, false),
    kp(kp),
    ki(ki),
    kd(kd),
    setpoint(setpoint),
    integral(0),
    previousError(0),
    output(0),
    last_compute_ms(millis()) {
  pidArgs.addVariable("kp", &this->kp);
  pidArgs.addVariable("ki", &this->ki);
  pidArgs.addVariable("kd", &this->kd);
  pidArgs.addVariable("integral", &this->integral);
  pidArgs.addVariable("previousError", &this->previousError);
  pidArgs.addVariable("output", &this->output);
  pidArgs.addVariable("last_compute_ms", &this->last_compute_ms);

  if(cb != nullptr) this->output.addCallback(cb);
}

FSM::ConditionalCallbackPID::ConditionalCallbackPID(fsm_key_t key, const Number& setpoint, const Number& kp, const Number& ki, const Number& kd)
  : ConditionalCallbackPID(key, nullptr, setpoint, kp, ki, kd) { }

void FSM::ConditionalCallbackPID::setSetpoint(const Number& setpoint) {
  this->setpoint = setpoint;
}

void FSM::ConditionalCallbackPID::setGains(const Number& kp, const Number& ki, const Number& kd) {
  this->kp.set(kp);
  this->ki.set(ki);
  this->kd.set(kd);
}

void FSM::ConditionalCallbackPID::reset() {
  this->integral.set(0);
  this->previousError.set(0);
  this->output.set(0);
  this->last_compute_ms.set(millis());
}

Number FSM::ConditionalCallbackPID::getSetpoint() const {
  return this->setpoint;
}

Number FSM::ConditionalCallbackPID::getOutput() const {
  return this->output.get();
}

Number FSM::ConditionalCallbackPID::getIntegral() const {
  return this->integral.get();
}

Number FSM::ConditionalCallbackPID::getPreviousError() const {
  return this->previousError.get();
}

Number FSM::ConditionalCallbackPID::getLastComputeMs() const {
  return this->last_compute_ms.get();
}

const Number& FSM::ConditionalCallbackPID::childReference(const Number& measured) {
  return setpoint;
}