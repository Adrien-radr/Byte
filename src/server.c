#include "server.h"
#include "clock.h"

bool sv_init() {
    bool noerr;

    server.protocol_id = Byte_GetHash( "Byte-Project" ); 
    server.max_clients = 8;
    server.running = false;
    server.timeout_accum = 0.f;

    net_addr_fill( &server.sv_addr, 127, 0, 0, 1, 1991 );
    server.sv_socket = 0;

    // init sockets (windows only)
    noerr = net_init();
    check( noerr, "Failed to init sockets\n" );

    // create server socket
    noerr = net_open_socket( &server.sv_socket, server.sv_addr.port );
    check( noerr, "Failed to open server socket\n" );

    log_info( "Server started connection on port %d\n", server.sv_addr.port );

    server.running = true;


    return true;

error:
    sv_shutdown();
    return false;
}

void sv_shutdown() {
    net_close_socket( &server.sv_socket );
    net_shutdown();
}

void sv_run() {
    char packet[252];
    log_info( "Server listening...\n" );

    while( 1 ) {
        net_addr cl;
        int bytes_read = sv_receive( &cl, (u8*)packet, 252 );
        if( bytes_read ) {
            log_info( "Packet received from client : %s\n", packet );
            
            if( !strcmp( packet, "Hello" ) ) {
                MSG( packet, 252, "Hello, %d,%d,%d,%d:%d", cl.ip[0], cl.ip[1], cl.ip[2],
                                                      cl.ip[3], cl.port );  
                //log_info( "SV : %s\n", packet );
                sv_send( &cl, (u8*)packet, 252 );
                

            } else if( !strcmp( packet, "close" ) )
                break;
        }

        Clock_sleep( 0.5f );
        printf( "Server Frame\n" );
    }
}

void sv_frame( int msec ) {

}

bool sv_send( const net_addr *to, const u8 *data, u32 size ) {
    u8 packet[size+4];

    check( server.running, "Server not running\n" );
    check( to && data, "to_addr or data is not allocated\n" );

    u32_to_bytes( server.protocol_id, packet );
    memcpy( &packet[4], data, size );
    return net_send_packet( &server.sv_socket, to, (const void*)packet, size + 4 );

error:
    return false;
}

int  sv_receive( net_addr *from, u8 *data, u32 size ) {
    u8 packet[size+4];

    check( server.running, "Server not running\n" );
    check( from && data, "from_addr or data not allocated\n" );

    int bytes_read = net_receive_packet( &server.sv_socket, from, packet, size + 4 );
    if( bytes_read <= 4 )
        return 0;

    u32 id;
    bytes_to_u32( packet, &id );

    if( id != server.protocol_id )
        return 0;

    memcpy( data, &packet[4], size );

    if( !strcmp( (const char*)data, "Hello" ) ) {
        log_info( "Server accepts connection from client %d.%d.%d.%d:%d\n",
                    from->ip[0], from->ip[1], from->ip[2], from->ip[3], from->port );
    }


    return size;

error:
    return 0;
}
