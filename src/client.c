#include "client.h"
#include "server.h"
#include "clock.h"

bool cl_init() {
    bool noerr;

    noerr = net_init();
    check( noerr, "Failed to init sockets\n" );

    // create client connection
    net_connection_init( &client.connection, Client, 1992 );

    net_addr sv_addr = { { 127,0,0,1 }, 1991 };

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

    const int size = 256 - 16;
    u8 packet[size];
    int bytes_read;
    
    int recv_packets = 0;

    f32 stat_accum = 0.f, send_accum = 0.f;

    Clock_reset( &clk );
    bool run = true;

    while( run ) {
        f32 now = Clock_getElapsedTime( &clk );
        f32 dt = now - curr_time;
        curr_time = now;

        const f32 send_rate = server.connection.flow == Good ? 30.f : 10.f;

        send_accum += dt;

        while( send_accum > 1.f / send_rate ) {
            u8 pack[size];

            if( recv_packets >= 180 ) {
                printf( "SENDING CLOSE SIGNAL!!!!!!\n" );
                strcpy( (char*)pack, "close" );
                run = false;
            } else
                strcpy( (char*)pack, "FROM CLIENT" );

            net_connection_send( &client.connection, pack, size ); 
            send_accum -= 1.f / send_rate;
        }

        while( true ) {
            bytes_read = net_connection_receive( &client.connection, packet, size );
            if( !bytes_read )
                break;
            else {
                recv_packets++;
            }
        }
        

        if( client.connection.state == ConnectFail )
            break;

        net_connection_update( &client.connection, dt );

        stat_accum += dt;
        while( stat_accum >= 0.25f && client.connection.state == Connected ) {
            connection_t *c = &client.connection;
            printf( "CL : rtt %.1fms, sent %d, ackd %d, lost %d(%.1f%%), sent_bw = %.1fkbps, ackd_bw = %.1fkbps\n",
                    c->rtt * 1000.f, c->sent_packets, c->ackd_packets, c->lost_packets, 
                    c->sent_packets > 0.f ? ((f32)c->lost_packets / (f32)c->sent_packets * 100.f) : 0.f,
                    c->sent_bw, c->ackd_bw );

            stat_accum -= 0.25f;
        }

    }
}
