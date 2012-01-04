#include <stdarg.h>

#include "common.h"


// Definitions of debug.h
FILE *Log;
bool LogOpened = false;
const char LogFile[] = "Byte.log";

int InitLog() {
    Log = fopen( LogFile, "w" );

    if( !Log ) {
        log_err( "Error while opening log \"%s\"", LogFile );
        return 0;
    }

    LogOpened = true;

    str16 time;
    str64 date;
    GetTime( date, 64, DateFmt );
    GetTime( time, 16, TimeFmt );
    strncat( date, " - ", 3 ); 
    strncat( date, time, 16 );

    WriteLog( "Byte-Project Log v%d.%d.%d\n", BYTE_MAJOR, BYTE_MINOR, BYTE_PATCH );
    WriteLog( "%s\n", date );
    WriteLog( "==========================\n" );

    return 1;
}

extern inline void CloseLog();



// Definitions of common.h
const char DateFmt[] = "%a %d %b %Y";
const char TimeFmt[] = "%H:%M:%S";
 
extern inline int Sign( const f32 a, const f32 threshold );
extern inline f32 Abs( const f32 a );
extern inline bool Eq( f32 a, f32 b, f32 e );
extern inline f32 RandomValue( f32 a, f32 b ); 
extern inline f32 Deg2Rad( const f32 a );
extern inline f32 Rad2Deg( const f32 a );


bool ReadFile( char **pBuffer, const char *pFile ) {
    check( pFile, "In ReadFile : given file name is uninitialized!\n" );

    // if buffer exists, destroy it
    if( *pBuffer )
        DEL_PTR( *pBuffer );

    FILE *file;
    u32 file_size = 0;

    file = fopen( pFile, "r" );

    check( file, "Couldn't open file \"%s\".\n", pFile );

    // Get file size in bytes
    fseek( file, 0L, SEEK_END );
    file_size = ftell( file );
    fseek( file, 0L, SEEK_SET );

    // allocate string and copy file contents in it
    *pBuffer = malloc( file_size + 1 );
    fread( *pBuffer, 1, file_size, file );

    fclose( file );

    return 1;

error:
    fclose( file );
    return 0;
}

void GetTime( char *t, int t_size,  const char *fmt ) {
    if( !t || !fmt ) return;

    time_t ti = time( NULL );
    struct tm* lt = localtime( &ti );
    strftime( t, t_size, fmt, lt );
}

