#include "client.h"
#include "server.h"
#include "clock.h"

bool cl_init() {
    bool noerr;

    noerr = net_init();
    check( noerr, "Failed to init sockets\n" );

    // create client connection
    net_connection_init( &client.connection, Client, 1992 );

    net_addr sv_addr = { { 192,168,1,2 }, 1991 };

    net_connection_connect( &client.connection, &sv_addr );

    return true;
error:
    cl_shutdown();
    return false;
}

void cl_shutdown() {
    net_connection_shutdown( &client.connection );
    net_shutdown();
}

void cl_run() {
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
        
        net_connection_update( &client.connection, dt );

        Clock_sleep( 0.5f );

        strcpy( (char*)packet, "Hello server!" );
        bool sent = net_connection_send( &client.connection, packet, size );

        if( sent ) {
            Clock_sleep( 1.f );
            while( 1 ) {
                bytes_read = net_connection_receive( &client.connection, packet, size);

                if( bytes_read ) {
                    log_info( "Packet received from server : %s\n", packet );

                    strcpy( (char*)packet, "close" );
                    net_connection_send( &client.connection, packet, size );
                    return;
                }
                Clock_sleep( 0.2f );
            }
        }


        if( client.connection.state == ConnectFail ||
                client.connection.state == Connected )
            break;
    }
}
