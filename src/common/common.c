#include <stdarg.h>

#include "common.h"
#include "vector.h"

// Definition of common array types
SimpleArrayDef( u32, u32 )
SimpleArrayDef( vec2, vec2 )
SimpleArrayDef( vec2i, vec2i )

// Definitions of common.h
const char DateFmt[] = "%a %d %b %Y";
const char TimeFmt[] = "%H:%M:%S";


inline int Sign( const f32 a, const f32 threshold ) {
    if( a > threshold )
        return 1;
    else if ( a < - threshold )
        return -1;
    return 0;
}


inline f32 Abs( const f32 a ) {
    return ( a >= 0.f ) ? a : -a;
}


inline bool Eq( f32 a, f32 b, f32 e ) {
    return Abs( a - b ) < e;
}


inline f32 RandomValue( f32 a, f32 b ) {
    f32 range = Abs( a - b );
    return ( (f32)rand() / RAND_MAX ) * range + ( ( a < b ) ? a : b );
}

inline vec2 RandomVec2( f32 a, f32 b ) {
    f32 range = Abs( a - b );
    return (vec2){ ( (f32)rand() / RAND_MAX) * range + ((a<b)?a:b),
                   ( (f32)rand() / RAND_MAX) * range + ((a<b)?a:b) };
}

inline vec2i RandomVec2i( int a, int b ) {
    int range = Abs( a - b );
    return (vec2i){ rand() % range + ((a<b)?a:b),
                    rand() % range + ((a<b)?a:b) };
}

inline f32 Deg2Rad( const f32 a ) {
    return a * ( M_PI / 180.f );
}


inline f32 Rad2Deg( const f32 a ) {
    return a * ( 180.f / M_PI );
}


inline int Clamp( int x, const int min, const int max ) {
    return x > max ? max : ( x < min ? min : x );
}


inline f32 Clampf( f32 x, const f32 min, const f32 max ) {
    return x > max ? max : ( x < min ? min : x );
}

inline int PointOnLinef( const vec2 *p, const vec2 *A, const vec2 *B ) {
    f32 calc = (B->x - A->x) * (p->y - A->y) - (B->y - A->y) * (p->x - A->x);

    return calc > 0.f ? 1 : (calc < 0.f ? -1 : 0);
}

inline int PointOnLinei( const vec2i *p, const vec2i *A, const vec2i *B ) {
    int calc = (B->x - A->x) * (p->y - A->y) - (B->y - A->y) * (p->x - A->x);

    return calc > 0 ? 1 : (calc < 0 ? -1 : 0);
}


u32 Byte_ReadFile( char **pBuffer, const char *pFile ) {
    check( pFile, "In ReadFile : given file name is uninitialized!\n" );

    // if buffer exists, destroy it
    if( *pBuffer )
        DEL_PTR( *pBuffer );

    FILE *file = NULL;
    u32 file_size = 0;

    file = fopen( pFile, "r" );

    check( file, "Couldn't open file \"%s\".\n", pFile );

    // Get file size in bytes
    fseek( file, 0L, SEEK_END );
    file_size = ftell( file );
    fseek( file, 0L, SEEK_SET );

    // allocate string and copy file contents in it
    *pBuffer = byte_alloc( file_size + 1 );
    fread( *pBuffer, 1, file_size, file );

    fclose( file );

    return (file_size + 1);

error:
    if( file ) fclose( file );
    return 0;
}

bool CheckExtension( const char *pFile, const char *pExtension ) {
    if( pFile && pExtension ) {
        int pos;
        int file_len = strlen( pFile );
        for( pos = file_len - 1; pos >= 0; --pos )
            if( '.' == pFile[pos] )
                break;

        if( pos ) {
            const char *f = pFile + pos + 1;
            return (0 == (strstr( f, pExtension )  - f) );
        }

    }
    return false;
}

void GetTime( char *t, int t_size,  const char *fmt ) {
    if( !t || !fmt ) return;

    time_t ti = time( NULL );
    struct tm* lt = localtime( &ti );
    strftime( t, t_size, fmt, lt );
}

u32 Byte_GetHash( const char *pStr ) {
    unsigned long hash = 5381;
    int c;

    while ((c = *pStr++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

void u32_to_bytes( const u32 i, u8 b[4] ) {
    b[0] = ((i & 0xFF000000) >> 24);
    b[1] = ((i & 0x00FF0000) >> 16);
    b[2] = ((i & 0x0000FF00) >> 8);
    b[3] =  (i & 0x000000FF);
}

void bytes_to_u32( const u8 b[4], u32 *i ) {
    *i = (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
}

