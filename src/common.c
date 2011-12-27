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
    return 1;
}

void CloseLog() {
    fclose( Log );
}



// Definitions of common.h
const char DateFmt[] = "%a %d %b %Y";
const char TimeFmt[] = "%H:%M:%S";

extern inline int Sign( const f32 a, const f32 threshold );
extern inline f32 Abs( const f32 a );
extern inline bool Eq( f32 a, f32 b, f32 e );
extern inline f32 RandomValue( f32 a, f32 b ); 
extern inline f32 Deg2Rad( const f32 a );
extern inline f32 Rad2Deg( const f32 a );


void GetTime( char *t, int t_size,  const char *fmt ) {
    if( !t || !fmt ) return;

    time_t ti = time( NULL );
    struct tm* lt = localtime( &ti );
    strftime( t, t_size, fmt, lt );
}

