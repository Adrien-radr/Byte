#ifndef BYTE_CLOCK_H
#define BYTE_CLOCK_H

#include "common.h"

/// Make the current thread sleep
/// @param pTime : sleep time in seconds
void Clock_sleep(f32 pTime);


/// Small pausable Timer
typedef struct s_Clock {
    f64 mLastFrameTime;	///< Global seconds elapsed the previons frame
    f64 mClockTime;		///< Seconds since last clock reset
    f64 mTempTime;		///< Variable used in mClockTime calculation

    bool mPaused;		///< True if the clock is paused
} Clock;


/// Reset the clock to zero
void Clock_reset( Clock *pClock );

/// Pause the clock
void Clock_pause( Clock *pClock );

/// Resume the clock if it has been paused
void Clock_resume( Clock *pClock );

/// Returns time elapsed since last Reset
f32  Clock_getElapsedTime( Clock *pClock );

#endif
