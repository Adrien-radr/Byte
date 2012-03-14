#include "server.h"
#include "clock.h"

bool sv_init() {
    bool noerr;

    server.max_clients = 8;
    server.seq_local = 0;
    server.seq_remote = 0;

    // init sockets 
    noerr = net_init();
    check( noerr, "Failed to init sockets\n" );

    // create server connection
    net_connection_init( &server.connection, SERVER_PORT );
    net_connection_listen( &server.connection );

    log_info( "Server started connection on port %d\n", SERVER_PORT );


    return true;

error:
    sv_shutdown();
    return false;
}

void sv_shutdown() {
    net_connection_shutdown( &server.connection );
    net_shutdown();
}

void sv_run() {
    Clock clk;
    f32 curr_time = 0.f;

    u8 packet[252];
    int bytes_read;

    Clock_reset( &clk );

    while( 1 ) {
        f32 now = Clock_getElapsedTime( &clk );
        f32 dt = now - curr_time;
        curr_time = now;

        bytes_read = net_connection_receive( &server.connection, packet, 252 );

        if( bytes_read ) {
            log_info( "Packet received from client : %s\n", packet );
            
            if( strstr( (char*)packet, "Hello" ) ) {
                strcpy( (char*)packet, "Hello client!" );
                net_connection_send( &server.connection, packet, 252 );
            }
            else if( !strcmp( (char*)packet, "close" ) )
                break;
        }

        net_connection_update( &server.connection, dt );
        
        Clock_sleep( 0.5f );
        printf( "Server Frame\n" );
    }
}

void sv_frame( int msec ) {

}
