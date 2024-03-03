#include <Arduino.h>

// Uncomment to enable debug
#define DEBUG 1

// CROSS-LIBRARY DEBUG COMPATIBILITY
#ifdef DEBUG
#if DEBUG == true
#ifndef DEBUG_SERIAL
#define DEBUG_SERIAL Serial
#endif
#endif
#endif

#ifdef DEBUG_SERIAL
// Just once
// #define FSM_DEBUG_SERIAL DEBUG_SERIAL

#include <DebugJson.h>

#ifndef DEBUG_DELAY
#define DEBUG_DELAY() { DEBUG_SERIAL.flush(); delay(10); }
#endif
#endif

// #define DEBUG_DISABLE_FSTRINGS 1
#ifdef DEBUG_DISABLE_FSTRINGS
#if DEBUG_DISABLE_FSTRINGS == 1
#define _F(x) x
#endif
#endif
#define _F(x) F(x)

// #define DEBUG_USE_BP 1