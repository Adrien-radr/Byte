#include "common.h"
#include "keys.h"
#include "color.h"
#include "clock.h"

int main() {
    printf( "Hello, Byte World!!\n" );

    char date[50];
    char time[16];
    GetTime( date, 50, DateFmt );
    GetTime( time, 16, TimeFmt );

    strncat( date, " - ", 3 );
    strncat( date, time, 16 );

    printf( "%s\n", date );




    enum Key kA = K_A;
    printf( "%d\n", kA );

    Color col1 = White;
    Color col2 = White;

    check( Color_cmp( &col1, &col2 ), "col1 & col2 not equal" );


    Clock c;
    Clock_reset( &c );
    Clock_sleep( 2000 );

    f32 t = Clock_getElapsedTime( &c );
    log_info( "Time : %f", t );


    return 0;

error:
    return -1;
}
