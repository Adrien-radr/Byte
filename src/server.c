#include "server.h"
#include "clock.h"

bool sv_init() {
    bool noerr;

    server.max_clients = 8;

    // init sockets 
    noerr = net_init();
    check( noerr, "Failed to init sockets\n" );

    // create server connection
    net_connection_init( &server.connection, Server, SERVER_PORT );
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

    const int size = 256 - 12;
    u8 packet[size];
    int bytes_read;

    Clock_reset( &clk );

    while( 1 ) {
        f32 now = Clock_getElapsedTime( &clk );
        f32 dt = now - curr_time;
        curr_time = now;

        bytes_read = net_connection_receive( &server.connection, packet, size );

        if( bytes_read ) {
            log_info( "Packet received from client : %s\n", packet );
            
            if( strstr( (char*)packet, "Hello" ) ) {
                strcpy( (char*)packet, "Hello client!" );
                net_connection_send( &server.connection, packet, size );
            }
            else if( !strcmp( (char*)packet, "close" ) )
                break;
        }

        net_connection_update( &server.connection, dt );
        
        Clock_sleep( 0.5f );
    }
}

void sv_frame( int msec ) {

}
