#include "net.h"

#ifdef BYTE_WIN32
    #include <winsock2.h>
    #pragma comment( lib, "wsock32.lib" )

    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <fcntl.h>
    #include <unistd.h>
#endif

void net_addr_fill( net_addr *a, u8 ip1, u8 ip2, u8 ip3, u8 ip4, u16 port ) {
    if( a ) {
        a->ip[0] = ip1;
        a->ip[1] = ip2;
        a->ip[2] = ip3;
        a->ip[3] = ip4;
        a->port = port;
    } else
        log_err( "Given address receiver not allocated\n" );
}

void net_addr_fill_int( net_addr *a, u32 ip, u16 port ) {
    if( a ) {
        u32_to_bytes( ip, a->ip );
        a->port = port;
    } else
        log_err( "Given address receiver not allocated\n" );
}

bool net_init() {
#ifdef BYTE_WIN32
    WSAData wsa_data;
    return WSAStartup( MAKEWORD( 2,2 ), &wsa_data ) != NO_ERROR;
#else
    return true;
#endif
}

void net_shutdown() {
#ifdef BYTE_WIN32
    WSACleanup();
#endif
}

bool net_open_socket( net_socket *s, u16 port ) {
    check( !*s, "Socket already opened." );

    *s = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    check( *s, "Failed to create socket.\n" );

    struct sockaddr_in addr;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;
    addr.sin_port = htons( port );

    check( !bind( *s, (struct sockaddr*)&addr, sizeof(struct sockaddr_in) ),
            "Failed to bind socket.\n" );

    // set non-blocking IO
#ifdef BYTE_WIN32
    DWORD non_block = 1;
    check( ioctlsocket( *s, FIONBIO, &non_block ),
#else
    int non_block = 1;
    check( !fcntl( *s, F_SETFL, O_NONBLOCK, non_block ),
#endif
            "Failed to set non-blocking socket\n" );

    return true;

error:
    net_close_socket( s );
    return false;
}

void net_close_socket( net_socket *s ) {
    if( *s ) {
    #ifdef BYTE_WIN32
        closesocket( *s );
    #else
        close( *s );
    #endif
        *s = 0;
    }
}

bool net_send_packet( net_socket *s, const net_addr *dest, const void *packet, u32 packet_size ) {
    check( s && *s, "Socket not opened\n" );
    check( packet, "Buffer to send not allocated\n" );
    check( dest, "Packet destination address not allocated\n" );

    u32 addr;
    bytes_to_u32( dest->ip, &addr );

    struct sockaddr_in to;
    to.sin_family = AF_INET;
    to.sin_addr.s_addr = htonl( addr );
    to.sin_port = htons( dest->port );

    int sent_bytes = sendto( *s, (const char*)packet, packet_size, 0, (struct sockaddr*)&to, sizeof(struct sockaddr_in) );

    return sent_bytes == packet_size;

error:
    return false;
}

int  net_receive_packet( net_socket *s, net_addr *sender, void *packet, u32 packet_size ) {
    check( s && *s, "Socket not opened\n" );
    check( packet, "Receiving buffer not allocated\n" );
    check( sender, "Sender address not allocated\n" );

    struct sockaddr_in from;
    socklen_t from_len = sizeof(from);

    int received_bytes = recvfrom( *s, (char*)packet, packet_size, 0, (struct sockaddr*)&from, &from_len );

    if( 0 >= received_bytes ) return 0;

    u32 addr = ntohl( from.sin_addr.s_addr );
    u16 port = ntohs( from.sin_port );

    net_addr_fill_int( sender, addr, port );


    return received_bytes;
    
error :
    return 0;
}

