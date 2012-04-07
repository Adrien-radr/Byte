#include "server.h"
#include "common/clock.h"

bool Server_init() {
    server.client_n = 0;

    Clock_reset( &server.clock );

    // Init Server Log
    InitLog( "Server.log", &server.clock );


    // init sockets 
    if ( !Net_init() ) {
        log_err( "Failed to init sockets\n" );
        return false;
    }

    // Init server socket
    if( !Net_openSocket( &server.socket, SERVER_PORT ) ) {
        log_err( "Failed to open Server socket\n" );
        Net_shutdown();
        return false;
    }

    // Init all connection slots
    for( int i = 0; i < MAX_CLIENTS; ++i )
        Net_connectionInit( &server.connections[i], Server );

    // Listen on first slot
    Net_connectionListen( &server.connections[0] ); 

    log_info( "Connection slot 0 listening...\n" );

    return true;
}

void Server_shutdown() {
    // close all connections
    for( int i = 0; i < MAX_CLIENTS; ++i )
        Net_connectionShutdown( &server.connections[i] );

    Net_shutdown();

    CloseLog();
}

void Server_sendGuaranteed( u32 client, u32 msg_type ) {
    Net_connectionSendGuaranteed( &server.connections[client], msg_type );
}

void Server_sendUnguaranteed( u32 client, u32 msg_type ) {
    Net_connectionSendUnguaranteed( &server.connections[client], msg_type );
}


void ClientConnect( const net_addr *addr ) {
    // search for first available connection slot that is listening
    int i;
    bool found = false;

    for( i = 0; i < MAX_CLIENTS; ++i ) 
        if( server.connections[i].state == Listening )  {
            found = true;
            break; 
        }
 
    // if slot open, set new connection session ID and address
    if( found ) {
        server.connections[i].session_id = Net_generateSessionID( (int)(1000 * Clock_getElapsedTime( &server.clock )) );
        server.connections[i].state = IDSent;
        memcpy( &server.connections[i].address, addr, sizeof(net_addr) );
        Server_sendGuaranteed( i, CONNECT_SESSIONID );

        server.client_n++;

        // Listen on a new slot
        if( server.client_n < MAX_CLIENTS ) {
            Net_connectionListen( &server.connections[i+1] );
            log_info( "Connection slot %d listening...\n", i+1 );
        }
    } else {
        log_warn( "Max clients limit reached!\n" );
        // direcly send to him
        u8 packet[256];
        u32_to_bytes( protocol_id, packet );
        u32_to_bytes( 0, packet + 4 );
        u32_to_bytes( CONNECT_REFUSE, packet + 8 );
        while( !Net_sendPacket( server.socket, addr, packet, 256 ) );
    }

    // debug
    printf( "Clients : \n" );
    for( int i = 0; i < MAX_CLIENTS; ++i )
        printf( "%d ", server.connections[i].session_id );
    printf( "\n" );
}

void ClientDisconnected( u32 client ) {
    // if multiple client were connected, redo their order in array to avoid empty slots in between used one
    if( 1 < server.client_n )
        for( u32 i = client + 1; i < server.client_n; ++i ) {
            memcpy( &server.connections[i-1], &server.connections[i], sizeof(connection) );
        }

    // stop listening where we were, Listen on the new last slot
    Net_connectionInit( &server.connections[server.client_n], Server );
    Net_connectionInit( &server.connections[server.client_n-1], Server );
    Net_connectionListen( &server.connections[server.client_n-1] );
    log_info( "Connection slot %d listening...\n", server.client_n-1 );

    server.client_n--;

    // debug
    printf( "Clients : \n" );
    for( int i = 0; i < MAX_CLIENTS; ++i )
        printf( "%d ", server.connections[i].session_id );
    printf( "\n" );
}

void HandleReceivedInputs() {
    u8 *packet;
    net_addr from;

    u32 client_id, msg_type, sequence, ack, ack_bits;

    while( server.recv_packets.count ) {
        Net_packetQueueGet( &server.recv_packets, &packet, &from );

        // get packet header
        Net_readPacketHeader( packet, NULL, &client_id, &msg_type, &sequence, &ack, &ack_bits );


        // if no client ID and a ASKID msg received, somebody is trying to connect
        if( !client_id && msg_type == CONNECT_ASKID ) {
            log_info( "received CONNECT_ASKID\n" );
            ClientConnect( &from );
        }

        // else, update connection info of corresponding client, and handle inputs for server-side game logic
        if( client_id ) {
            // find client
            u32 i;
            bool found = false;
            for( i = 0; i < MAX_CLIENTS; ++i ) 
                if( server.connections[i].session_id == client_id ) {
                    found = true;
                    break;
                }

            if( found ) {
                Net_connectionPacketReceived( &server.connections[i], sequence, ack, ack_bits, 256 - PACKET_HEADER_SIZE );

                // HANDLE INPUTS FOR LOGIC
                switch( msg_type ) {
                case KEEP_ALIVE :
                    break;


                case CONNECT_TRY :
                    log_info( "received CONNECT_TRY\n" );
                    if( server.connections[i].state == IDSent ) {
                        // Known client try to connect, accept it
                        server.connections[i].state = Connected;
                        Server_sendGuaranteed( i, CONNECT_ACCEPT );
                    }
                    break;
                case CONNECT_CLOSE :
                    log_info( "received CONNECT_CLOSE\n" );
                    server.connections[i].state = Disconnecting;
                    Server_sendGuaranteed( i, CONNECT_CLOSEOK );
                    break;
                case CONNECT_CLOSEOK :
                    log_info( "received CONNECT_CLOSEOK\n" );
                    if( server.connections[i].state == Disconnecting )
                        ClientDisconnected( i );
                    break;
                default:
                    break;
                }
            }
        }
        Net_packetQueuePop( &server.recv_packets );
    }
}

bool ReceivePacket() {
    u32 bytes_read;
    u8 packet[256];
    net_addr from;

    // get packet
    bytes_read = Net_receivePacket( server.socket, &from, packet, 256 );
    if( !bytes_read ) return false;
    if( bytes_read < PACKET_HEADER_SIZE ) return true;

    // check protocol ID
    u32 id;
    bytes_to_u32( packet, &id );
    if( id != protocol_id )
        return true;

    // add packet to received packets queue
    Net_packetQueueSet( &server.recv_packets, packet, &from );
    Net_packetQueuePush( &server.recv_packets );

    return true;
}

void Server_run() {
    f32 start_t = 0.f, end_t = 0.f, frame_t = 0.f;
    f32 stat_accum = 0.f, send_accum[MAX_CLIENTS] = {0.f};
    bool received_smthg;




    // Server Frame
    while( true ) {
        // sample frame start time
        start_t = Clock_getElapsedTime( &server.clock );

        // read client inputs from received packets
        while( true ) {
            received_smthg = ReceivePacket();
            if( !received_smthg )
                break;
        }

        // handle received packets
        HandleReceivedInputs();



        // SEND GAME INFO TO CLIENTS
        for( int i = 0; i < MAX_CLIENTS; ++i ) 
            if( server.connections[i].state != Disconnected ) {
                send_accum[i] += frame_t;
                while( send_accum[i] > server.connections[i].flow_speed ) {
                    send_accum[i] -= server.connections[i].flow_speed;
                    Net_connectionSendNextPacket( &server.connections[i], server.socket );
                }

                // update connection info
                Net_connectionUpdate( &server.connections[i], frame_t );
            }


        Clock_sleep( Clock_getElapsedTime( &client.clock ) - start_t );

        // sample frame end time
        end_t = Clock_getElapsedTime( &server.clock );

        // calculate frame time
        frame_t = end_t - start_t;


        
        

        /*
        stat_accum += dt;
        while( stat_accum >= 0.25f && server.connection.state == Connected ) {
            connection_t *c = &server.connection;
            //printf( "SV : rtt %.1fms, sent %d, ackd %d, lost %d(%.1f%%), sent_bw = %.1fkbps, ackd_bw = %.1fkbps\n\n",
            //        c->rtt * 1000.f, c->sent_packets, c->ackd_packets, c->lost_packets, 
            //        c->sent_packets > 0.f ? ((f32)c->lost_packets / (f32)c->sent_packets * 100.f) : 0.f,
            //        c->sent_bw, c->ackd_bw );

            stat_accum -= 0.25f;
        }
        */



    }
}
