#include "client.h"
#include "server.h"
#include "clock.h"

bool cl_init() {
    bool noerr;

    client.seq_local = 0;
    client.seq_remote = 0;

    noerr = net_init();
    check( noerr, "Failed to init sockets\n" );

    // create client connection
    net_connection_init( &client.connection, 1992 );

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
    u8 packet[252];
    int bytes_read;

    Clock_reset( &clk );

    while( 1 ) {
        f32 now = Clock_getElapsedTime( &clk );
        f32 dt = now - curr_time;
        curr_time = now;
        
        net_connection_update( &client.connection, dt );

        Clock_sleep( 0.5f );

        strcpy( (char*)packet, "Hello server!" );
        bool sent = net_connection_send( &client.connection, packet, 252 );

        if( sent ) {
            Clock_sleep( 1.f );
            while( 1 ) {
                bytes_read = net_connection_receive( &client.connection, packet, 252);

                if( bytes_read ) {
                    log_info( "Packet received from server : %s\n", packet );

                    strcpy( (char*)packet, "close" );
                    net_connection_send( &client.connection, packet, 252 );
                    break;
                }
                Clock_sleep( 0.2f );
            }
        }


        if( client.connection.state == ConnectFail ||
                client.connection.state == Connected )
            break;
    }
}
