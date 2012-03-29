#include "server.h"
#include "common/clock.h"

bool Server_init() {
    server.max_clients = 8;
    server.up_time = 0.f;

    server.log = fopen( "Server.log", "w" );

    if( !server.log ) {
        printf( "Failed to create server log\n" );
        return false;
    }

    // init sockets 
    if ( !net_init() ) {
        sv_log( "Failed to init sockets\n" );
        return false;
    }

    // create server connection
    net_connection_init( &server.connection, Server, SERVER_PORT );
    net_connection_listen( &server.connection ); 

    sv_log( "Server started connection on port %d\n", SERVER_PORT );



    return true;
}

void Server_shutdown() {
    fclose( server.log );
    net_connection_shutdown( &server.connection );
    net_shutdown();
}

void Server_run() {
    Clock clk;
    f32 curr_time = 0.f;
    f32 stat_accum = 0.f, send_accum = 0.f;

    const int size = 256 - 16;
    u8 packet[size];
    int bytes_read;

    Clock_reset( &clk );

    while( 1 ) {
        f32 now = Clock_getElapsedTime( &clk );
        f32 dt = now - curr_time;
        curr_time = now;

        send_accum += dt;

        server.up_time += dt;

        const f32 send_rate = server.connection.flow == Good ? 30.f : 10.f;

        while( send_accum > 1.f / send_rate ) {
            u8 pack[size];
            strcpy( (char*)pack, "FROM SERVER" );
            net_connection_send( &server.connection, pack, size ); 
            send_accum -= 1.f / send_rate;
        }

        while( true ) {
            bytes_read = net_connection_receive( &server.connection, packet, size );
            if( !bytes_read )
                break;
            else  {
                if( !strcmp( (char*)packet, "close" ) ) {
                    sv_log( "Server back to Listening...\n" );
                    server.connection.state = Listening;
                    return;
                }
            }
        }

        net_connection_update( &server.connection, dt );
        
        

        stat_accum += dt;
        while( stat_accum >= 0.25f && server.connection.state == Connected ) {
            connection_t *c = &server.connection;
            //printf( "SV : rtt %.1fms, sent %d, ackd %d, lost %d(%.1f%%), sent_bw = %.1fkbps, ackd_bw = %.1fkbps\n\n",
            //        c->rtt * 1000.f, c->sent_packets, c->ackd_packets, c->lost_packets, 
            //        c->sent_packets > 0.f ? ((f32)c->lost_packets / (f32)c->sent_packets * 100.f) : 0.f,
            //        c->sent_bw, c->ackd_bw );

            stat_accum -= 0.25f;
        }

    }
}
