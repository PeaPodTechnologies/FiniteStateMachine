#ifndef I2CIP_DEBUG_H_
#define I2CIP_DEBUG_H_

#include <Arduino.h>
// CAN BE INCLUDED IN ALL SOURCE FILES

#define DEBUG_DELAY() {delay(30);}

// CROSS-LIBRARY DEBUG COMPATIBILITY
#ifdef DEBUG_SERIAL
#define FSM_DEBUG_SERIAL DEBUG_SERIAL
#endif

#ifdef DEBUG
#if DEBUG == true
#ifndef FSM_DEBUG_SERIAL
#define FSM_DEBUG_SERIAL Serial
#endif
#endif
#endif

#endif