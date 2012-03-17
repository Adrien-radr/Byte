#include "net.h"

#ifdef BYTE_WIN32
    #include <winsock2.h>
    #pragma comment( lib, "wsock32.lib" )

    #define close closesocket

    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <net/if.h>
    #include <fcntl.h>
    #include <ifaddrs.h>
#endif
 
/// Application Protocol ID. Just the hash of "Byte-Project"
const u32 protocol_id = 1910167069;

const f32 connection_timeout = 10.f;

net_ip local_ips[MAX_IPS];
static int numIP;



  

static void add_local_ip( char *ifname, struct sockaddr *addr, struct sockaddr *mask ) {
    int addrlen;
    sa_family_t family;

    if( !ifname || !addr || !mask )
        return;

    family = addr->sa_family;

    if( numIP < MAX_IPS && family == AF_INET ) {
        addrlen = sizeof(struct sockaddr_in);

        strncpy( local_ips[numIP].ifname, ifname, 16 );
        local_ips[numIP].family = family;
        memcpy( &local_ips[numIP].addr, addr, addrlen );
        memcpy( &local_ips[numIP].mask, mask, addrlen );

        numIP++;
    }
}

static void show_ip() {
    char buf[48];

    for( int i = 0; i < numIP; ++i ) {
        if( getnameinfo((struct sockaddr*)&local_ips[i].addr, sizeof(struct sockaddr_in), buf, 48, NULL, 0, NI_NUMERICHOST ) )
            buf[0] = 0;

        log_info( "IP : %s (%s)\n", buf, local_ips[i].ifname );
    }
}


static void get_local_ips() {
    struct ifaddrs *ifap, *search;

    if( getifaddrs( &ifap ) ) 
        printf( "Unable to get list of net interfaces\n" );
    else {
        numIP = 0;
        for( search = ifap; search; search = search->ifa_next ) {
            if( search->ifa_flags & 0x01 ) 
                add_local_ip( search->ifa_name, search->ifa_addr, search->ifa_netmask );
        }
        freeifaddrs( ifap );
    }

    show_ip();
}


bool net_addr_equal( const net_addr *a, const net_addr *b ) {
    if( !a || !b ) return false;

    u32 ipa, ipb;
    bytes_to_u32( a->ip, &ipa );
    bytes_to_u32( b->ip, &ipb );

    return ipa == ipb && a->port == b->port;
}

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
    bool ret = false;

#ifdef BYTE_WIN32
    WSAData wsa_data;
    ret = WSAStartup( MAKEWORD( 2,2 ), &wsa_data ) != NO_ERROR;
#else
    ret = true;
#endif
 
    get_local_ips();

    return ret;
}

void net_shutdown() {
#ifdef BYTE_WIN32
    WSACleanup();
#endif
}


////////////////////////////////////////////////////////////////////////////
//                      CONNECTION

static void net_connection_clear( connection_t *c ) {
    c->state = Disconnected;
    c->timeout_accum = 0.f;
    memset( c->address.ip, 0, 4 );
    c->address.port = 0;
}

bool net_connection_init( connection_t *c, u16 port ) {
    check( c, "Given connection not allocated\n" );

    c->running = false;
    c->socket = 0;

    net_connection_clear( c );

    bool noerr = net_open_socket( &c->socket, port );
    check( noerr, "Failed to open connection socket\n" );

    c->running = true;

    return true;

error:
    net_connection_shutdown( c );
    return false;
}

void net_connection_shutdown( connection_t *c ) {
    if( c && c->running ) {
        net_connection_clear( c );
        c->running = false;

        if( c->socket )
            net_close_socket( &c->socket );
    }
}

void net_connection_listen( connection_t *c ) {
    if( !c ) return;

    log_info( "Server Listening...\n" );
    c->state = Listening;
}

void net_connection_connect( connection_t *c, net_addr *addr ) {
    if( !c || !addr ) return;

    log_info( "Connecting to %d.%d.%d.%d:%d\n", 
            addr->ip[0], addr->ip[1], addr->ip[2], addr->ip[3], addr->port ); 

    c->state = Connecting;
    memcpy( &c->address, addr, sizeof(net_addr) );
}

void net_connection_update( connection_t *c, f32 dt ) {
    if( !c || !c->running ) return;

    c->timeout_accum += dt;
    if( c->timeout_accum > connection_timeout ) {
        if( c->state == Connecting ) {
            log_info( "Connecting timed out\n" );
            net_connection_clear( c );
            c->state = ConnectFail;
        } else if( c->state == Connected ) {
            log_info( "Connection timed out\n" );
            net_connection_clear( c );
        }
    }
}

bool net_connection_send( connection_t *c, const u8 *packet, u32 size ) {
    if( !c || !packet ) return false;

    u8 msg[size+4];

    check( c->running, "Connection not running\n" );
    
    u32 address;
    bytes_to_u32( c->address.ip, &address );
    check( address != 0, "Invalid destination address\n" );


    // add protocol id to message
    u32_to_bytes( protocol_id, msg );
    
    // add packet data to message
    memcpy( &msg[4], packet, size );

    // send the message
    return net_send_packet( c->socket, &c->address, msg, size + 4 );

error:
    return false;
}

int  net_connection_receive( connection_t *c, u8 *packet, u32 size ) {
    if( !c || !packet ) return -1;

    u8 msg[size+4];
    net_addr sender;

    check( c->running, "Connection not running\n" );


    int bytes_read = net_receive_packet( c->socket, &sender, msg, size + 4 );
    if( bytes_read <= 4 )
        return 0;

    u32 id;
    bytes_to_u32( msg, &id );

    // bad header
    if( id != protocol_id )
        return 0;

    // client connecting to server
    if( c->state == Listening ) {
        log_info( "Server accepting connection from %d.%d.%d.%d:%d\n",
                sender.ip[0], sender.ip[1], sender.ip[2], sender.ip[3], sender.port ); 
        c->state = Connected;
        memcpy( &c->address, &sender, sizeof(net_addr) );
    } 

    if( net_addr_equal( &c->address, &sender ) ) {
        // Client connecting, finalize connection
        if( c->state == Connecting ) {
            log_info( "Client connected with server\n" );
            c->state = Connected;
        }

        // timeout reset
        c->timeout_accum = 0.f;

        // get packet data
        memcpy( packet, &msg[4], bytes_read - 4 );
        return bytes_read - 4;
    }

error:
    return -1;
}


////////////////////////////////////////////////////////////////////////////
//                      PACKET QUEUE

/// Returns true if sequence a is more recent than sequence b
/// Takes into account the fact that buffers cycle after MAX_SEQUENCE
bool sequence_more_recent( u32 a, u32 b ) {
    return ( a > b && ( a - b ) <= MAX_SEQUENCE/2 )
        || ( a < b && ( b - a ) >  MAX_SEQUENCE/2 );
}

void net_packet_queue_init( net_packet_queue *q ) {
    if( q ) {
        q->count = 0;
        q->tail = 0;
        for( int i = 0; i < 256; ++i )
            q->list[i] = &q->arr[i];
    }
}

void net_packet_queue_insert( net_packet_queue *q, net_packet_info *p ) {
    if( !q || !p ) return;

    if( !q->count ) {
        memcpy( &q->arr[0], p, sizeof(net_packet_info) );
        ++q->count;

    } else {
        bool full = false;

        if( 256 == q->count )
            full = true;

        if( sequence_more_recent( q->list[q->tail]->seq, p->seq ) ) {
            if( !full ) {
                memcpy( &q->arr[q->count], p, sizeof(net_packet_info) );
                q->tail = q->count++;
            }
            return;
        }

        u16 i;
        for( i = 0; i < q->count; ++i ) {
            if( sequence_more_recent( p->seq, q->list[i]->seq ) ) {
                // we have space, put new one at end of array and redo linking
                if( !full ) {
                    memcpy( &q->arr[q->count], p, sizeof(net_packet_info) );
                    q->list[q->count] = q->list[q->tail];
                    for( int j = q->count-1; j > i; --j ) {
                        q->list[j] = q->list[j-1];
                    }
                    q->list[i] = &q->arr[q->count];
                    q->tail = q->count++;

                // no more space in array, replace the older packet with the newone
                } else {
                    memcpy( q->list[q->tail], p, sizeof(net_packet_info) );
                    net_packet_info *tmp = q->list[q->tail];
                    for( int j = q->tail; j > i; --j ) {
                        q->list[j] = q->list[j-1];
                    }
                    q->list[i] = tmp;
                }
                return;
            }   
        }
        // error
        printf( "ARR ERROR\n" );
    }
}

bool net_packet_queue_exists( net_packet_queue *q, u32 seq ) {
    if( !q ) return false;

    for( int i = 0; i < q->count; ++i ) 
        if( q->arr[i].seq == seq )
            return true;

    return false;
}
void net_packet_queue_verify( net_packet_queue *q );


////////////////////////////////////////////////////////////////////////////
//                      SOCKETS

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
        close( *s );
        *s = 0;
    }
}

bool net_send_packet( net_socket s, const net_addr *dest, const void *packet, u32 packet_size ) {
    check( s, "Socket not opened\n" );
    check( packet, "Packet to send not allocated\n" );
    check( dest, "Packet destination address not allocated\n" );

    u32 addr;
    bytes_to_u32( dest->ip, &addr );

    struct sockaddr_in to;
    to.sin_family = AF_INET;
    to.sin_addr.s_addr = htonl( addr );
    to.sin_port = htons( dest->port );

    int sent_bytes = sendto( s, (const char*)packet, packet_size, 0, (struct sockaddr*)&to, sizeof(struct sockaddr_in) );

    return sent_bytes == packet_size;

error:
    return false;
}

int  net_receive_packet( net_socket s, net_addr *sender, void *packet, u32 packet_size ) {
    check( s, "Socket not opened\n" );
    check( packet, "Receiving packet not allocated\n" );
    check( sender, "Sender address not allocated\n" );

    struct sockaddr_in from;
    socklen_t from_len = sizeof(from);

    int received_bytes = recvfrom( s, (char*)packet, packet_size, 0, (struct sockaddr*)&from, &from_len );

    if( 0 >= received_bytes ) return 0;

    u32 addr = ntohl( from.sin_addr.s_addr );
    u16 port = ntohs( from.sin_port );

    net_addr_fill_int( sender, addr, port );


    return received_bytes;
    
error :
    return 0;
}

