#ifndef BYTE_COMMON_H
#define BYTE_COMMON_H

// Version
#define BYTE_MAJOR 0
#define BYTE_MINOR 0
#define BYTE_PATCH 3

// Platform
#if defined(WIN32) || defined(_WIN32)
#   define BYTE_WIN32
#else
#   define BYTE_UNIX
#endif

// Types shortcuts
	typedef unsigned char		u8;
	typedef unsigned short		u16;
	typedef unsigned int	    u32;

	typedef float f32;
	typedef double f64;

#   define eol "\n"

// Common includes
#include <stdlib.h>
#include <time.h>

// Debug & Log
#include "debug.h"

// Function shortcuts
#ifndef DEL_PTR
#   define DEL_PTR(p)       \
        do {                \
            if( p ) {        \
                free(p); \
                (p) = NULL; \
            }               \
        } while(0)
#endif


//      MATH
//#include "vector.h"
#include <math.h>

#   define M_EPS  1.0e-5
#ifndef M_PI
#   define M_PI 3.14159265358979323846
#endif


    /// Returns the sign of a float
    inline int Sign( const f32 a, const f32 threshold ) {
        if( a > threshold )
            return 1;
        else if ( a < - threshold )
            return -1;
        return 0;
    }

    /// Return absolute value of float number
    inline f32 Abs( const f32 a ) {
        return ( a >= 0.f ) ? a : -a;
    }

    /// Test equality of two floating pt numbers    
    inline bool Eq( f32 a, f32 b, f32 e ) {
        return Abs( a - b ) < e;
    }

    /// Returns a random floating pt number between to vals
    inline f32 RandomValue( f32 a, f32 b ) {
        f32 range = Abs( a - b );
        return ( (f32)rand() / RAND_MAX ) * range + ( ( a < b ) ? a : b );
    }

    /// Return the given radian value in degrees
    inline f32 Deg2Rad( const f32 a ) {
        return a * ( M_PI / 180.f );
    }

    /// Returns the given degree value in radians
    inline f32 Rad2Deg( const f32 a ) {
        return a * ( 180.f / M_PI );
    }


//      MISC
    // String types
    typedef char str16[16];
    typedef char str64[64];
    typedef char str256[256];
    typedef char str1024[1024];

    // Format for Date and Time
    extern const char DateFmt[];
    extern const char TimeFmt[];

    /// Returns date/time in given format
    void GetTime( char *t, int t_size, const char *fmt );

    /// Read an entire file in a buffer
    bool ReadFile( char **pBuffer, const char *pFile );

    /// Macro for snprintf
#   define MSG( str, n, M, ... ) snprintf( (str), (n), M, ##__VA_ARGS__) 

        
#endif // COMMON_H
