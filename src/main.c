#include "common.h"
#include "keys.h"
#include "color.h"
#include "clock.h"
#include "vector.h"
#include "context.h"
#include "event.h"


void Clean() {
    Context_destroy();
    EventManager_destroy();

    CloseLog();
}

void listener( const Event* pEvent ) {
    if( pEvent->mType == E_CharPressed )
        printf( "%c\n", pEvent->mChar );
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
    EventManager_init();



    while( !IsKeyUp( K_Escape ) ) {
        EventManager_update();
        Context_update();

        Context_swap();
    }

    Clean();

    return 0;

error :
    Clean();
    return -1;
}

