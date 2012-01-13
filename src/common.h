#ifndef BYTE_COMMON_H
#define BYTE_COMMON_H

// Version
#define BYTE_MAJOR 0
#define BYTE_MINOR 0
#define BYTE_PATCH 7

// Platform
#if defined(WIN32) || defined(_WIN32)
#   define BYTE_WIN32
#else
#   define BYTE_UNIX
#endif

// Common includes
#include <stdlib.h>
#include <time.h>

// Debug & Log
#include "debug.h"



// ##########################################################################################
//      MISC
    // Types shortcuts
	typedef unsigned char		u8;
	typedef unsigned short		u16;
	typedef unsigned int	    u32;

	typedef float f32;
	typedef double f64;

#   define eol "\n"

    // String types
    typedef char str16[16];
    typedef char str32[32];
    typedef char str64[64];
    typedef char str256[256];
    typedef char str1024[1024];


    // Allocate a pointer 
    inline void* byte_alloc_func( u32 size, const char* file, u32 line ) {
        void* ret = malloc( size );
        MemoryManager_allocation( ret, size, file[4], line );
        return ret;
    }

#ifndef byte_alloc
#   ifdef _DEBUG
#   define byte_alloc( size ) byte_alloc_func( (size), __FILE__, __LINE__ )
#   else
#   define byte_alloc( size ) malloc( (size ) )
#   endif
#endif


    // Free a pointer and set it to NULL (tell memory managment)
#ifndef DEL_PTR
#   ifdef _DEBUG
#   define DEL_PTR(p, size)                         \
        do {                                        \
            if( p ) {                               \
                MemoryManager_deallocation( (p) );  \
                free(p);                            \
                (p) = NULL;                         \
            }                                       \
        } while(0)
#   else
#   define DEL_PTR(p, size)     \
        do {                    \
            if( p ) {           \
                free(p);        \
                (p) = NULL;     \
            }                   \
        } while(0)
#   endif
#endif

    // Format for Date and Time
    extern const char DateFmt[];
    extern const char TimeFmt[];

    /// Returns date/time in given format
    void GetTime( char *t, int t_size, const char *fmt );

    /// Read an entire file in a buffer
    /// @return : pBuffer allocated size (file size + '\0' char)
    u32 ReadFile( char **pBuffer, const char *pFile );

    /// Macro for snprintf
#   define MSG( str, n, M, ... ) snprintf( (str), (n), M, ##__VA_ARGS__) 

// ##########################################################################################


// ##########################################################################################
// Generic array type
// One must choose if the objects in the array hate to be dynamically destroyed or not :
//  - SimpleArray : no memory managment for individual data in array
//  - HeapArray : individual data in array are destroyed with DEL_PTR when the array is destroyed
// 
// Use :
//  SimpleArray( float, Float );
//  
//  int main .. {
//      FloatArray arr;
//      FloatArray_init( &arr, 10 );
//      
//      if( FloatArray_checkSize( &arr ) ) {
//          arr.data[arr.cpt] = 5.f;
//          printf( "%f\n", arr.data[arr.cpt] );
//          ++arr.cpt;
//      }
//      
//      FloatArray_destroy( &arr );
//  }
                /*u32 size = arr->size;                                           \
                arr->size *= 2;                                                 \
                                                                                \
                type tmp[size];                                                 \
                                                                                \
                memcpy( tmp, arr->data, size * sizeof( type ) );                \
                arr->data = realloc( arr->data, arr->size * sizeof( type ) );   \
                check_mem( arr->data );                                         \
                                                                                \
                memcpy( arr->data, tmp, size * sizeof( type ) );                \
                */
#   define Array( type, name )                                                  \
    typedef struct {                                                            \
        type    *data;                                                          \
        u32     cpt;                                                            \
        u32     size;                                                           \
    } name##Array;                                                              \
                                                                                \
    bool name##Array_init( name##Array *arr, u32 size ) {                       \
        arr->data = calloc( size, sizeof( type ) );                             \
        check_mem( arr->data );                                                 \
        arr->cpt  = 0;                                                          \
        arr->size = size;                                                       \
                                                                                \
        return true;                                                            \
    error:                                                                      \
        return false;                                                           \
    }                                                                           \
                                                                                \
    bool name##Array_checkSize( name##Array *arr ) {                            \
        if( arr ) {                                                             \
            if( arr->cpt == arr->size ) {                                       \
                arr->data = realloc( arr->data, (arr->cpt *= 2) * sizeof( type ) ); \
            }                                                                   \
            return true;                                                        \
        }                                                                       \
        return false;                                                           \
    }                                                                           

#   define SimpleArray( type, name )                                            \
    Array( type, name )                                                         \
                                                                                \
    void name##Array_destroy( name##Array *arr ) {                              \
        if( arr )                                                               \
            DEL_PTR( arr->data, sizeof(type) );                                 \
    }                                                                           


#   define HeapArray( type, name, destructionFunc )                             \
    Array( type, name )                                                         \
                                                                                \
    void name##Array_destroy( name##Array *arr ) {                              \
        if( arr ) {                                                             \
            for( int i = 0; i < arr->size; ++i )                                \
                destructionFunc( arr->data[i] );                                \
        }                                                                       \
        DEL_PTR( arr->data, sizeof( type ) );                                   \
    }                                                                           

// ##########################################################################################


// ##########################################################################################
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
// ##########################################################################################


        
#endif // COMMON_H
