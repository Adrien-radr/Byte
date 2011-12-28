#include "common.h"
#include "keys.h"
#include "color.h"
#include "clock.h"
#include "vector.h"
#include "context.h"


void Clean() {
    Context_destroy();


    CloseLog();
}

int main() {
    InitLog();


    printf( "Hello, Byte World!!\n" );

    str64 date;
    str16 time;
    GetTime( date, 64, DateFmt );
    GetTime( time, 16, TimeFmt );

    strncat( date, " - ", 3 );
    strncat( date, time, 16 );

    printf( "%s\n", date );


    char title[20];
    MSG( title, 20, "Byte-Project v%d.%d.%d", BYTE_MAJOR, BYTE_MINOR, BYTE_PATCH );

    Context_init( 800, 600, false, title, 0 );

    Clock_sleep( 1000 );

    Clean();

    return 0;

error :
    Clean();
    return -1;
}

