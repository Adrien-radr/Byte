#include "common.h"
#include "keys.h"
#include "color.h"
#include "clock.h"
#include "vector.h"

int main() {
    printf( "Hello, Byte World!!\n" );

    char date[50];
    char time[16];
    GetTime( date, 50, DateFmt );
    GetTime( time, 16, TimeFmt );

    strncat( date, " - ", 3 );
    strncat( date, time, 16 );

    printf( "%s\n", date );


    vec2 a = { .x = 2.f, .y = 3.f }, b = { .x= 5.f, .y = -6.f };

    vec2 c = vec2_add( &a, &b );
    log_info( "a + b = { %f, %f }", c.x, c.y );

    c = vec2_sub( &a, &b );
    log_info( "a - b = { %f, %f }", c.x, c.y );

    vec2_normalize( &c );
    log_info( "c normalized = { %f, %f }", c.x, c.y );

    enum Key kA = K_A;
    printf( "%d\n", kA );

    Color col1 = White;
    Color col2 = White;

    check( Color_cmp( &col1, &col2 ), "col1 & col2 not equal" );




    return 0;

error:
    return -1;
}
