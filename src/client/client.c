#include "common/clock.h"
#include "client.h"
#include "game.h"

///         TODO
///     Finish Connection procedure
///     Do Disconnection procedure.

bool Client_init( const net_addr *sv_addr ) {
    bool noerr;

    Clock_reset( &client.clock );

    // Init Client log
    InitLog( "Client.log", &client.clock );


    noerr = Net_init();
    check( noerr, "Failed to init sockets\n" );

    // Init client socket
    u16 port = 1992, tmp;
    while( !Net_openSocket( &client.socket, port ) ) {
        check( tmp < 2000, "Failed to open socket\n" );

        tmp = port++;
        log_warn( "Opening socket on port %d failed, will rery on port %d in 3s...\n", tmp, port );
        Clock_sleep( 3.f );
    }

    log_info( "Socket opened on port %d\n", port );

    // create client connection
    Net_connectionInit( &client.c_info, Client );

    // set server address
    memcpy( &client.c_info.address, sv_addr, sizeof(net_addr) );

    return true;
error:
    Client_shutdown();
    return false;
}

void Client_shutdown() {
    Net_connectionShutdown( &client.c_info );
    Net_shutdown();

    CloseLog();
}

void Client_sendGuaranteed( u32 msg_type ) {
    Net_connectionSendGuaranteed( &client.c_info, msg_type );
}

void Client_sendUnguaranteed( u32 msg_type ) {
    Net_connectionSendUnguaranteed( &client.c_info, msg_type );
}

bool ReceivePacket() {
    u32 bytes_read;
    u8 packet[256];
    net_addr from;

    // get packet
    bytes_read = Net_receivePacket( client.socket, &from, packet, 256 );
    if( !bytes_read ) return false;
    if( bytes_read < PACKET_HEADER_SIZE ) return true;

    if( !net_addr_equal( &from, &client.c_info.address ) ) return true;

    // check protocol ID
    u32 id;
    bytes_to_u32( packet, &id );
    if( id != protocol_id )
        return true;

    // add packet to received packets queue
    Net_packetQueueSet( &client.recv_packets, packet, &from );
    Net_packetQueuePush( &client.recv_packets );

    return true;
}

void ConnectStep() {
    u8 *packet = NULL;

    switch( client.c_info.state ) {
    case Disconnected :
        {
            // send first packet, asking for a session ID
            log_info( "Asking Session ID to %d.%d.%d.%d:%d\n", 
                    client.c_info.address.ip[0], 
                    client.c_info.address.ip[1], 
                    client.c_info.address.ip[2], 
                    client.c_info.address.ip[3], 
                    client.c_info.address.port ); 

            Client_sendGuaranteed( CONNECT_ASKID );
            Net_connectionSendNextPacket( &client.c_info, client.socket );

            client.c_info.state = IDDemandSent;
        }
        break;
    case IDDemandSent :
        {
        // receive demand response
        u32 id, type, sequence, ack, ack_bits;

        ReceivePacket();

        while( client.recv_packets.count ) {
            Net_packetQueueGet( &client.recv_packets, &packet, NULL );

            Net_readPacketHeader( packet, NULL, &id, &type, &sequence, &ack, &ack_bits );
            Net_packetQueuePop( &client.recv_packets );

            Net_connectionPacketReceived( &client.c_info, sequence, ack, ack_bits, 256 - PACKET_HEADER_SIZE );

            if( type == CONNECT_SESSIONID ) {
                client.c_info.session_id = id;
                client.c_info.state = IDReceived;
                log_info( "Received Session ID from server : %u\n", id );
                return;
            } else if( type == CONNECT_REFUSE ) {
                log_warn( "Server is full.\n" );
                client.c_info.state = ConnectFail;
            } else 
                log_warn( "Received packet with type=%s, client does not have any session_id yet!\n", PacketTypeStr[type] );
        }
        }
        break;
    case IDReceived :
        // if we already got an ID, try a connection
        Client_sendGuaranteed( CONNECT_TRY );
        Net_connectionSendNextPacket( &client.c_info, client.socket );

        client.c_info.state = ConnectDemandSent;
        log_info( "Sent a connection demand to server...\n" );
        break;
    case ConnectDemandSent :
        {
        // we sent a demand, wait for response
        u32 id, type, sequence, ack, ack_bits;

        ReceivePacket();
        while( client.recv_packets.count ) {
            Net_packetQueueGet( &client.recv_packets, &packet, NULL );

            Net_readPacketHeader( packet, NULL, &id, &type, &sequence, &ack, &ack_bits );
            Net_packetQueuePop( &client.recv_packets );

            Net_connectionPacketReceived( &client.c_info, sequence, ack, ack_bits, 256 - PACKET_HEADER_SIZE );

            if( type == CONNECT_ACCEPT ) {
                client.c_info.state = Connected;
                log_info( "Client is now connected to server.\n" );
                Net_packetQueueInit( &client.c_info.guaranteed );
                return;
            } else if( type == CONNECT_REFUSE ) {
                log_info( "Server refused client connection.\n" );
                client.c_info.state = ConnectFail;
                return;
            } 
        }

        }
        break;
    default :
        return;
    }
}

bool Connect() {
    f32 start_t;

    while( client.c_info.state != Connected && client.c_info.state != ConnectFail ) {
        start_t = Clock_getElapsedTime( &client.clock );

        ConnectStep();
        Net_connectionUpdate( &client.c_info, Clock_getElapsedTime( &client.clock ) - start_t );
        Clock_sleep( 0.2f );
    }

    return client.c_info.state == Connected;
}

void Disconnect() {
    u8 *packet;
    u32 id, msg_type;

    log_info( "Disconnecting...\n" );

    client.c_info.state = Disconnecting;

    // send Connection closing demand
    u8 close_packet[256];
    Net_connectionWritePacketHeader( &client.c_info, close_packet, CONNECT_CLOSE );
    while( !Net_sendPacket( client.socket, &client.c_info.address, close_packet, 256 ) );
    Net_connectionPacketSent( &client.c_info, 256 - PACKET_HEADER_SIZE );

    // We read all incoming packets waiting for a CLOSEOK one, then we send it back to finish disconnection procedure
    while( true ) {
        ReceivePacket();
        // handle packet
        while( client.recv_packets.count ) {
            Net_packetQueueGet( &client.recv_packets, &packet, NULL );

            Net_readPacketHeader( packet, NULL, &id, &msg_type, NULL, NULL, NULL );
            Net_packetQueuePop( &client.recv_packets );

            if( id == client.c_info.session_id ) {
                if( msg_type == CONNECT_CLOSEOK ) {
                    // send disconnection procedure ending msg
                    Net_connectionWritePacketHeader( &client.c_info, close_packet, CONNECT_CLOSEOK );
                    while( !Net_sendPacket( client.socket, &client.c_info.address, close_packet, 256 ) );

                    // reinit connection
                    Net_connectionInit( &client.c_info, Client );
                    return;
                }
            }
            
        }
        Clock_sleep( 0.2f );
    }
}

void HandleReceivedPackets() {
    u8 *packet;

    u32 id, msg_type, sequence, ack, ack_bits;

    while( client.recv_packets.count ) {
        Net_packetQueueGet( &client.recv_packets, &packet, NULL );

        // get header
        Net_readPacketHeader( packet, NULL, &id, &msg_type, &sequence, &ack, &ack_bits );

        // If we got an ID, check if it is ours and switch on msg_type
        if( client.c_info.session_id == id ) {
            Net_connectionPacketReceived( &client.c_info, sequence, ack, ack_bits, 256 - PACKET_HEADER_SIZE );
            switch( msg_type ) {
                case KEEP_ALIVE :
                    break;


                default:
                    break;
            }
        }
        Net_packetQueuePop( &client.recv_packets );
    }
}

void Client_run() {
    f32 start_t = 0.f, end_t = 0.f, frame_t = 0.f;
    f32 stat_accum = 0.f, send_accum = 0.f;
    bool received_smthg;

    // Connection to server
    if( !Connect() ) return;

    while( true ) {
        // sample frame start time
        start_t = Clock_getElapsedTime( &client.clock );


        // SEND GAME PACKETS TO SERVER
        while( send_accum > client.c_info.flow_speed ) {

            Net_connectionSendNextPacket( &client.c_info, client.socket );
            send_accum -= client.c_info.flow_speed;
        }


        while( true ) {
            received_smthg = ReceivePacket();
            if( !received_smthg )
                break;
        }

        // handle received stuff
        HandleReceivedPackets();

        // Run game frame. Disconnect if client want out
        if( !Game_update( frame_t ) ) {
            Disconnect();
            break;
        }

        Net_connectionUpdate( &client.c_info, frame_t );

        if( client.c_info.state == ConnectFail ) 
            break;


        // sample frame end time
        end_t = Clock_getElapsedTime( &client.clock );

        // calculate frame time
        frame_t = end_t - start_t;
        send_accum += frame_t;


        /*

        while( send_accum > 1.f / send_rate ) {
            u8 pack[size];

            if( recv_packets >= 20 ) {
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
        

        */

        /*
        stat_accum += dt;
        while( stat_accum >= 0.25f && client.connection.state == Connected ) {
            connection_t *c = &client.connection;
            //printf( "CL : rtt %.1fms, sent %d, ackd %d, lost %d(%.1f%%), sent_bw = %.1fkbps, ackd_bw = %.1fkbps\n",
            //        c->rtt * 1000.f, c->sent_packets, c->ackd_packets, c->lost_packets, 
            //        c->sent_packets > 0.f ? ((f32)c->lost_packets / (f32)c->sent_packets * 100.f) : 0.f,
            //        c->sent_bw, c->ackd_bw );

            stat_accum -= 0.25f;
        }
        */


    }
}
