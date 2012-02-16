#include "clock.h"


#if defined(BYTE_WIN32)
#	include <Windows.h>
#else
#   include <sys/time.h>
#   include <unistd.h>
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
        struct timeval Time = {0, 0};
        gettimeofday(&Time, NULL);

        return Time.tv_sec + Time.tv_usec * 0.000001;
    #endif
}

// ==============================  //



void Clock_sleep( f32 pTime ) {
    #if defined(BYTE_WIN32)
       Sleep( (DWORD)( pTime ) );
    #else
       usleep( (u32)( pTime * 1000 ) );
    #endif
}

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
    static f64 temp = 0.0;
    if( !pClock->mPaused ) {
        temp = Byte_GetSystemTime();
        pClock->mClockTime += temp - pClock->mLastFrameTime;
        pClock->mLastFrameTime = temp;
    }
    return (f32)pClock->mClockTime;
}

