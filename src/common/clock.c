#include "clock.h"

#ifdef BYTE_WIN32
#	include <Windows.h>
#endif

// ==============================  //

f64 Byte_GetSystemTime(){
    #if defined(BYTE_WIN32)
        static LARGE_INTEGER Frequency;
        static BOOL          UseHighPerformanceTimer;
        UseHighPerformanceTimer = QueryPerformanceFrequency(&Frequency);

        if (UseHighPerformanceTimer)
        {
            // High performance counter available : use it
            LARGE_INTEGER CurrentTime;
            QueryPerformanceCounter(&CurrentTime);

            return (f64)(CurrentTime.QuadPart) / Frequency.QuadPart;
        }
        else
        {
            // High performance counter not available : use GetTickCount (less accurate)
            return GetTickCount() * 0.001;
        }
    #else
        struct timespec time;
        clock_gettime( CLOCK_MONOTONIC, &time );
        return (f64)time.tv_sec + (f64)time.tv_nsec / 1000000000L;// / 1000000.0;
    #endif
}


void Clock_sleep( f32 pTime ) {
    #if defined(BYTE_WIN32)
       Sleep( (DWORD)( pTime ) );
    #else
       usleep( (u32)( pTime * 1000000.f ) );
    #endif
}

// ==============================  //



void Clock_reset( Clock *pClock ) {
    pClock->mLastFrameTime = Byte_GetSystemTime();
    pClock->mClockTime = 0.0;
    pClock->mPaused = false;
}

void Clock_pause( Clock *pClock ) {
    pClock->mPaused = true;
}

void Clock_resume( Clock *pClock ) {
    pClock->mPaused = false;
    pClock->mLastFrameTime = Byte_GetSystemTime();
}

f32 Clock_getElapsedTime( Clock *pClock ) {
    static f64 now = 0.0;
    if( !pClock->mPaused ) {
        now = Byte_GetSystemTime();
        pClock->mClockTime += now - pClock->mLastFrameTime;
        pClock->mLastFrameTime = now;
    }
    return (f32)pClock->mClockTime;
}

