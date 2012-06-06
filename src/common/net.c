#include "net.h"

#ifdef BYTE_WIN32
<<<<<<< HEAD
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <ws2tcpip.h>
=======
    #include <ws2tcpip.h>
>>>>>>> da3d2e1b9ccf48c55a305e5c4e4141723efbf8ff

    #define close closesocket

    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netdb.h>
    #include <net/if.h>
    #include <fcntl.h>
    #include <ifaddrs.h>
#endif

/// Application Protocol ID. Just the hash of "Byte-Project"
const u32 protocol_id = 1910167069;

const f32 connectiontimeout =  120.f;

net_ip local_ips[MAX_IPS];
static int numIP;

str32 PacketTypeStr[PT_ARRAYEND] = {
    "CONNECT_ASKID",
    "CONNECT_SESSIONID",
    "CONNECT_TRY",
    "CONNECT_ACCEPT",
    "CONNECT_REFUSE",
    "CONNECT_CLOSE",
    "CONNECT_CLOSEOK",
    "KEEP_ALIVE"
};




#ifndef BYTE_WIN32
static void add_local_ip( char *ifname, struct sockaddr *addr, struct sockaddr *mask ) {
    int addrlen;
    u16 family;

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
#endif

static void show_ip() {
    char buf[48];

    for( int i = 0; i < numIP; ++i ) {
#ifdef BYTE_WIN32
        log_info( "IP : %s\n", inet_ntoa( local_ips[i].addr ) );
#else
        if( getnameinfo((struct sockaddr*)&local_ips[i].addr, sizeof(struct sockaddr_in), buf, 48, NULL, 0, NI_NUMERICHOST ) )
            buf[0] = 0;

        log_info( "IP : %s (%s)\n", buf, local_ips[i].ifname );
#endif
    }
}


static void get_local_ips() {
#ifdef BYTE_WIN32
    char ac[80];
    if( gethostname( ac, sizeof(ac) ) == SOCKET_ERROR ) {
        log_err( "Unable to get list of net interfaces(gethostname)\n" );
        return;
    }

    struct hostent *phe = gethostbyname(ac);
    if( phe == NULL ) {
        log_err( "Unable to get host list of net interfaces(gethostbyname)\n" );
        return;
    }

    for ( int i = 0; phe->h_addr_list[i] != 0; ++i )
<<<<<<< HEAD
    for ( int i = 0; phe->h_addr_list[i] != 0; ++i )
=======
>>>>>>> da3d2e1b9ccf48c55a305e5c4e4141723efbf8ff
        memcpy( &local_ips[i].addr, phe->h_addr_list[i], sizeof(struct in_addr) );

#else
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

#endif
    show_ip();
}


void net_addr_cpy( net_addr *dst, const net_addr *src ) {
    if( src )
        net_addr_fill( dst, src->ip[0], src->ip[1], src->ip[2], src->ip[3], src->port );
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

bool Net_init() {
    bool ret = false;

#ifdef BYTE_WIN32
    WSADATA wsa_data;
    ret = ( WSAStartup( MAKEWORD( 2,2 ), &wsa_data ) == 0 );
#else
    ret = true;
#endif

    get_local_ips();

    return ret;
}

void Net_shutdown() {
#ifdef BYTE_WIN32
    WSACleanup();
#endif
}

u32  Net_generateSessionID( int seed ) {
    srand( seed );
    u8 random_str[4];
    u32_to_bytes( rand(), random_str );

    return Byte_GetHash( (char*)random_str );
}


////////////////////////////////////////////////////////////////////////////
//                      PACKET QUEUE
void Net_packetQueueInit( net_packet_queue *q ) {
    q->start = 0;
    q->count = 0;

    // Set the stack position to be after the header size
    for( int i = 0; i < 256; ++i )
        q->packets[i].stack_pos = PACKET_HEADER_SIZE;
}

void Net_packetQueuePush( net_packet_queue *q ) {
    // if buffer full, overwrite last one
    if( q->count == 256 )
        q->start = ( q->start + 1 ) % 256;
    else
        q->count++;
}

void Net_packetQueuePop( net_packet_queue *q ) {
    if( q->count ) {
        // reposition the stack index
        q->packets[q->start].stack_pos = PACKET_HEADER_SIZE;

        // Advance in circular buffer
        q->start = ( q->start + 1 ) % 256;
        q->count--;
    }
}

bool Net_packetQueueIsFull( const net_packet_queue *q ) {
    return q->count == 256;
}

void Net_packetQueueGet( net_packet_queue *q, u8 **data_ptr, net_addr *addr ) {
    if( q ) {
        if( data_ptr ) *data_ptr = q->packets[q->start].data;
        if( addr     ) net_addr_cpy( addr, &q->packets[q->start].addr );
    }
}
void Net_packetQueueSet( net_packet_queue *q, const u8 *data, const net_addr *addr ) {
    if( q ) {
        int end = ( q->start + q->count ) % 256;
        memcpy( &q->packets[end].data, data, PACKET_SIZE );
        net_addr_cpy( &q->packets[end].addr, addr );

        // get sequence number of packet
        bytes_to_u32( data + 12, &q->packets[end].sequence );
    }
}

void Net_packetQueueStackInt( net_packet_queue *q, int data ) {
    int end = ( q->start + q->count ) % 256;
    u32_to_bytes( data, &q->packets[end].data[q->packets[end].stack_pos] );
    q->packets[end].stack_pos += 4;
}


////////////////////////////////////////////////////////////////////////////
//                      PACKET INFO QUEUE

/// Returns true if sequence a is more recent than sequence b
/// Takes into account the fact that buffers cycle after MAX_SEQUENCE
static bool sequence_more_recent( u32 a, u32 b ) {
    return ( a > b && ( a - b ) <= MAX_SEQUENCE/2 )
        || ( a < b && ( b - a ) >  MAX_SEQUENCE/2 );
}

void net_packet_info_queue_init( net_packet_info_queue *q );
void net_packet_info_queue_insert( net_packet_info_queue *q, net_packet_info *p );
void net_packet_info_queue_remove( net_packet_info_queue *q, u32 index );
bool net_packet_info_queue_exists( net_packet_info_queue *q, u32 seq );
void net_packet_info_queue_verify( net_packet_info_queue *q );
void net_packet_info_queue_print( net_packet_info_queue *q );
bool net_packet_info_queue_empty( net_packet_info_queue *q );
void net_packet_info_queue_popback( net_packet_info_queue *q );

void net_packet_info_queue_update( net_packet_info_queue *q, f32 dt );

void net_packet_info_queue_init( net_packet_info_queue *q ) {
    if( q ) {
        q->count = 0;
        q->tail = 0;
        for( u32 i = 0; i < 256; ++i )
            q->list[i] = &q->arr[i];
    }
}

void net_packet_info_queue_insert( net_packet_info_queue *q, net_packet_info *p ) {
    if( !q || !p ) return;

    if( net_packet_info_queue_exists( q, p->seq ) ) return;

    if( !q->count ) {
        memcpy( &q->arr[0], p, sizeof(net_packet_info) );
        ++q->count;

    } else {
        bool full = false;

        if( 256 == q->count )
            full = true;

        if( sequence_more_recent( q->list[q->tail]->seq, p->seq ) ) {
            if( !full ) {
                memcpy( q->list[q->count], p, sizeof(net_packet_info) );
                q->tail = q->count++;
            }
            // dont handle the case where arr is full
            return;
        }

        for( u32 i = 0; i < q->count; ++i ) {
            if( sequence_more_recent( p->seq, q->list[i]->seq ) ) {
                // we have space, put new one at first available spot and redo linking
                if( !full ) {
                    memcpy( q->list[q->count], p, sizeof(net_packet_info) );
                    net_packet_info *tmp = &(*q->list[q->count]);
                    q->list[q->count] = q->list[q->tail];

                    for( int j = q->count-1; j > i; --j )
                        q->list[j] = q->list[j-1];

                    q->list[i] = tmp;
                    q->tail = q->count++;

                // no more space in array, replace the older packet with the newone
                } else {
                    memcpy( q->list[q->tail], p, sizeof(net_packet_info) );
                    net_packet_info *tmp = q->list[q->tail];

                    for( u32 j = q->tail; j > i; --j )
                        q->list[j] = q->list[j-1];

                    q->list[i] = tmp;
                }
                return;
            }
        }
        // error
        net_packet_info_queue_print( q );
        printf( "new seq=%d\n", p->seq );
        printf( "ARR ERROR\n" );
        exit( 0 );
    }
}

void net_packet_info_queue_remove( net_packet_info_queue *q, u32 index ) {
    if( !q || index >= q->count ) return;

    if( 1 == q->count ) {
        q->list[0] = &q->arr[0];
        q->list[0]->seq = 0;
        q->count--;
    } else {
        net_packet_info *tmp = q->list[index];

        for( u32 i = index; i < q->tail; ++i ) {
            q->list[i] = q->list[i+1];
        }

        q->list[q->tail] = tmp;
        q->list[q->tail]->seq = 0;
        q->count--;
        q->tail--;
    }
}

bool net_packet_info_queue_exists( net_packet_info_queue *q, u32 seq ) {
    if( !q ) return false;

    for( u32 i = 0; i < q->count; ++i )
        if( q->list[i]->seq == seq )
            return true;

    return false;
}

void net_packet_info_queue_verify( net_packet_info_queue *q );

void net_packet_info_queue_print( net_packet_info_queue *q ) {
    if( !q ) return;

    for( u32 i = 0; i < q->count; ++i )
        printf( " + %d\n", q->list[i]->seq );
}

bool net_packet_info_queue_empty( net_packet_info_queue *q ) {
    return q && !q->count;
}

void net_packet_info_queue_update( net_packet_info_queue *q, f32 dt ) {
    if( !q ) return;

    for( u32 i = 0; i < q->count; ++i )
        q->list[i]->time += dt;
}

void net_packet_info_queue_popback( net_packet_info_queue *q ) {
    if( !q ) return;

    if( 1 < q->count )
        q->tail--;

    q->count--;
}

////////////////////////////////////////////////////////////////////////////
//                      CONNECTION
static void Net_connectionClear( connection *c ) {
    c->state = Disconnected;
    c->timeout_accum = 0.f;
    memset( c->address.ip, 0, 4 );
    c->address.port = 0;
}

bool Net_connectionInit( connection *c, connection_mode mode ) {
    check( c, "Given connection not allocated\n" );

    c->running = false;
    c->mode = mode;

    c->session_id = 0;

    c->seq_local = 0;
    c->seq_remote = 0;

    Net_packetQueueInit( &c->guaranteed );
    Net_packetQueueInit( &c->unguaranteed );

    net_packet_info_queue_init( &c->sent_queue );
    net_packet_info_queue_init( &c->recv_queue );
    net_packet_info_queue_init( &c->pending_acks );
    net_packet_info_queue_init( &c->ackd_queue );

    c->sent_packets = 0;
    c->recv_packets = 0;
    c->lost_packets = 0;
    c->ackd_packets = 0;

    c->sent_bw = 0.f;
    c->ackd_bw = 0.f;
    c->rtt= 0.f;

    c->flow = Bad;
    c->flow_speed = 1.f / FLOW_BAD;
    c->penalty_time = 4.f;
    c->good_cond_time = 0.f;
    c->penalty_accum = 0.f;

    Net_connectionClear( c );

    c->running = true;

    return true;

error:
    Net_connectionShutdown( c );
    return false;
}

void Net_connectionShutdown( connection *c ) {
    if( c && c->running ) {
        Net_connectionClear( c );
        c->running = false;
    }
}

void Net_connectionListen( connection *c ) {
    if( !c || c->mode == Client ) return;

    c->state = Listening;
}

static void nc_update_queues( connection *c, f32 dt ) {
    // update queues packet time
    net_packet_info_queue_update( &c->sent_queue, dt );
    net_packet_info_queue_update( &c->recv_queue, dt );
    net_packet_info_queue_update( &c->ackd_queue, dt );
    net_packet_info_queue_update( &c->pending_acks, dt );


    // remove outdated packets from all queues
    while( !net_packet_info_queue_empty( &c->sent_queue ) && c->sent_queue.list[c->sent_queue.tail]->time > (MAX_RTT + M_EPS) )
        net_packet_info_queue_popback( &c->sent_queue );

    if( !net_packet_info_queue_empty( &c->recv_queue ) ) {
        const u32 last_seq = c->recv_queue.list[0]->seq;
        const u32 min_seq = last_seq >= 34 ? (last_seq - 34) : (MAX_SEQUENCE - (34 - last_seq));

        while( !net_packet_info_queue_empty( &c->recv_queue ) && !sequence_more_recent( c->recv_queue.list[c->recv_queue.tail]->seq, min_seq ) )
            net_packet_info_queue_popback( &c->recv_queue );
    }

    while( !net_packet_info_queue_empty( &c->ackd_queue ) && c->ackd_queue.list[c->ackd_queue.tail]->time > (MAX_RTT * 2 - M_EPS) )
        net_packet_info_queue_popback( &c->ackd_queue );

    while( !net_packet_info_queue_empty( &c->pending_acks ) && c->pending_acks.list[c->pending_acks.tail]->time > (MAX_RTT + M_EPS) ) {
        net_packet_info_queue_popback( &c->pending_acks );
        c->lost_packets++;
    }
}

static void nc_update_flow( connection *c, f32 dt ) {
    const f32 rtt = c->rtt * 1000.f;

    if( c->flow == Good ) {
        // if bad condition, go to Bad
        if( rtt > RTT_THRESHOLD ) {
            log_info( "Connection going to Bad Mode\n" );
            c->flow = Bad;
            c->flow_speed = 1.f / FLOW_BAD;

            // Increase penalty time if oscillating between Good & Bad to often
            if( c->good_cond_time < 10.f && c->penalty_time < 60.f ) {
                c->penalty_time *= 2.f;
                if( c->penalty_time > 60.f )
                    c->penalty_time = 60.f;
                log_info( "Penalty time increased to %f\n", c->penalty_time );
            }

            c->good_cond_time = 0.f;
            c->penalty_accum = 0.f;
            return;
        }

        // else, update flow variables
        c->good_cond_time += dt;
        c->penalty_accum += dt;

        // decrease penalty if in good mode for some time
        if( c->penalty_accum > 10.f && c->penalty_time > 1.f ) {
            c->penalty_accum = 0.f;
            c->penalty_time *= 0.5f;
            if( c->penalty_time < 1.f )
                c->penalty_time = 1.f;
            log_info( "Penalty time decreased to %f\n", c->penalty_time );
        }
    }

    if( c->flow == Bad ) {
        if( rtt <= RTT_THRESHOLD )
            c->good_cond_time += dt;
        else
            c->good_cond_time = 0.f;

        // if good condition for some time, go to Good
        if( c->good_cond_time > c->penalty_time ) {
            log_info( "Connection going to Good Mode\n" );
            c->good_cond_time = 0.f;
            c->penalty_accum = 0.f;
            c->flow = Good;
            c->flow_speed = 1.f / FLOW_GOOD;
        }
    }
}

void Net_connectionUpdate( connection *c, f32 dt ) {
    if( !c || !c->running ) return;

    // update reliability system and flow control
    nc_update_queues( c, dt );
    if( c->state == Connected )
        nc_update_flow( c, dt );

    // Update connection stats
    int sent_bps = 0;
    for( u32 i = 0; i < c->sent_queue.count; ++i )
        sent_bps += c->sent_queue.list[i]->size;

    int ackd_pps = 0, ackd_bps = 0;
    for( u32 i = 0; i < c->ackd_queue.count; ++i )
        if( c->ackd_queue.list[i]->time >= MAX_RTT ) {
            ackd_pps++;
            ackd_bps += c->ackd_queue.list[i]->size;
        }

    sent_bps /= MAX_RTT;
    ackd_bps /= MAX_RTT;
    c->sent_bw = sent_bps * ( 8 / 1000.f );
    c->ackd_bw = ackd_bps * ( 8 / 1000.f );


    // check connection timeout
    c->timeout_accum += dt;
    if( c->timeout_accum > connectiontimeout ) {
        if( c->state == IDDemandSent || c->state == IDReceived ||
                c->state == ConnectDemandSent ) {
            log_info( "Connecting timed out\n" );
            Net_connectionClear( c );
            c->state = ConnectFail;
        } else if( c->state == Connected ) {
            log_info( "Connection timed out\n" );
            Net_connectionClear( c );
            c->state = ConnectFail;
        }
        return;
    }
}


void Net_readPacketHeader( const u8 *packet, u32 *protocolID, u32 *sessionID, u32 *msg_type, u32 *sequence, u32 *ack, u32 *ack_bits ) {
    if( !packet ) return;

    if( protocolID ) bytes_to_u32( packet, protocolID );
    if( sessionID  ) bytes_to_u32( packet + 4, sessionID );
    if( msg_type   ) bytes_to_u32( packet + 8, msg_type );
    if( sequence   ) bytes_to_u32( packet + 12, sequence );
    if( ack        ) bytes_to_u32( packet + 16, ack );
    if( ack_bits   ) bytes_to_u32( packet + 20, ack_bits );
}

static int bit_index_for_seq( u32 seq, u32 ack ) {
    if( seq > ack )
        return ack + ( MAX_SEQUENCE - seq );
    else
        return ack - 1 - seq;
}

static u32 Net_getAckbits( connection *c ) {
    if( !c ) return 0;

    u32 ack_bits = 0;
    const u32 ack = c->seq_remote;

    for( u32 i = c->recv_queue.tail; i >= 0; --i ) {
        if( c->recv_queue.list[i]->seq == ack || sequence_more_recent( c->recv_queue.list[i]->seq, ack ) )
            break;

        int bit_index = bit_index_for_seq( c->recv_queue.list[i]->seq, ack );

        if( bit_index <= 31 )
            ack_bits |= 1 << bit_index;
    }
    return ack_bits;
}

static void Net_processAckbits( connection *c, u32 ack, u32 ack_bits ) {
    if( !c ) return;
    if( net_packet_info_queue_empty( &c->pending_acks ) ) return;

    int i = c->pending_acks.tail;
    while( i >= 0 ) {
        bool acked = false;

        if( c->pending_acks.list[i]->seq == ack )
            acked = true;
        else if( !sequence_more_recent( c->pending_acks.list[i]->seq, ack ) ) {
            int bit_index = bit_index_for_seq( c->pending_acks.list[i]->seq, ack );
            if( bit_index <= 31 )
                acked = ( ack_bits >> bit_index ) & 1;
        }

        if( acked ) {
            c->rtt += ( c->pending_acks.list[i]->time - c->rtt ) * 0.1f;

            // check the sequence of the first packet in guaranteed queue. If acked, Pop it
            if( c->guaranteed.packets[c->guaranteed.start].sequence == c->pending_acks.list[i]->seq )
                Net_packetQueuePop( &c->guaranteed );


            net_packet_info_queue_insert( &c->ackd_queue, c->pending_acks.list[i] );
            c->ackd_packets++;
            net_packet_info_queue_remove( &c->pending_acks, i );
        }

        --i;
    }
}

void Net_connectionWritePacketHeader( connection *c, u8 *packet, u32 msg_type ) {
    if( !c || !packet ) return;
    if( !c->running ) return;

    u32_to_bytes( protocol_id,          packet );
    u32_to_bytes( c->session_id,        packet + 4 );
    u32_to_bytes( msg_type,             packet + 8 );
    u32_to_bytes( c->seq_local,         packet + 12 );
    u32_to_bytes( c->seq_remote,        packet + 16 );
    u32_to_bytes( Net_getAckbits( c ),  packet + 20 );
}

void Net_connectionSendGuaranteed( connection *c, u32 msg_type ) {
    if( !c ) return;

    u8 packet[PACKET_SIZE];
    Net_connectionWritePacketHeader( c, packet, msg_type );
    Net_packetQueueSet( &c->guaranteed, packet, &c->address );
    Net_packetQueuePush( &c->guaranteed );
}

void Net_connectionSendUnguaranteed( connection *c, u32 msg_type ) {
    if( !c ) return;

    u8 packet[PACKET_SIZE];
    Net_connectionWritePacketHeader( c, packet, msg_type );
    Net_packetQueueSet( &c->unguaranteed, packet, &c->address );
    Net_packetQueuePush( &c->unguaranteed );
}

void Net_connectionSendNextPacket( connection *c, net_socket socket ) {
    // send guaranteed and unguaranteed packets equally frequently
    static bool type = false;
    static int  cpt  = 0;
    const int guaranteed_ratio   = 1,
              unguaranteed_ratio = 1;

    bool unguaranteed = false;
    u8 *packet = NULL;

    if( type ) {
        // try to pop a guaranteed packet
        if( c->guaranteed.count ) {
            Net_packetQueueGet( &c->guaranteed, &packet, NULL );
            cpt++;
        }

        // else, no guaranteed packet available, try to get a unguaranteed one
        else if( c->unguaranteed.count ) {
            Net_packetQueueGet( &c->unguaranteed, &packet, NULL );
            unguaranteed = true;
        }


        if( cpt == guaranteed_ratio ) {
            cpt = 0;
            type = !type;
        }

    } else {
        // try to pop an unguaranteed packet
        if( c->unguaranteed.count ) {
            Net_packetQueueGet( &c->unguaranteed, &packet, NULL );
            unguaranteed = true;
            cpt++;
        }

        // else, no unguaranteed packet available, try to get a guaranteed one
        else if( c->guaranteed.count )
            Net_packetQueueGet( &c->guaranteed, &packet, NULL );


        if( cpt == unguaranteed_ratio ) {
            cpt = 0;
            type = !type;
        }
    }

    // if we got something, send it to client
    if( packet ) {
        u32 type, seq;
        bytes_to_u32( packet + 8, &type );
        bytes_to_u32( packet + 12, &seq );
        log_info( "sending %s(seq=%d)\n", PacketTypeStr[type], seq );
        Net_sendPacket( socket, &c->address, packet, PACKET_SIZE );
        Net_connectionPacketSent( c );

        if( unguaranteed )
            Net_packetQueuePop( &c->unguaranteed );

    // else, send a keepalive
    } else if( c->state == Connected ) {
        //log_info( "sending KEEP_ALIVE\n" );
        u8 keep_alive[PACKET_SIZE];
        Net_connectionWritePacketHeader( c, keep_alive, KEEP_ALIVE );
        Net_sendPacket( socket, &c->address, keep_alive, PACKET_SIZE );
        Net_connectionPacketSent( c );
    }
}

void Net_connectionPacketSent( connection *c ) {
    if( !c ) return;

    if( net_packet_info_queue_exists( &c->sent_queue, c->seq_local ) ) {
        log_info( "Local sequence %d exists\n", c->seq_local );
        net_packet_info_queue_print( &c->sent_queue );
        return;
    }

    if( net_packet_info_queue_exists( &c->pending_acks, c->seq_local ) )
        return;

    // add a new sent packet info
    net_packet_info pi;
    pi.seq = c->seq_local;
    pi.time = 0.f;
    pi.size = PACKET_SIZE - PACKET_HEADER_SIZE;

    net_packet_info_queue_insert( &c->sent_queue, &pi );
    net_packet_info_queue_insert( &c->pending_acks, &pi );


    c->sent_packets++;
    c->seq_local++;
    if( c->seq_local > MAX_SEQUENCE )
        c->seq_local = 0;
}

void Net_connectionPacketReceived( connection *c, u32 sequence, u32 ack, u32 ack_bits ) {
    if( !c || !c->running ) return;

    // update connection
    c->recv_packets++;

    if( net_packet_info_queue_exists( &c->recv_queue, sequence ) )
        return;

    // add new received packet info
    net_packet_info pi;
    pi.seq = sequence;
    pi.time = 0.f;
    pi.size = PACKET_SIZE - PACKET_HEADER_SIZE;

    net_packet_info_queue_insert( &c->recv_queue, &pi );


    // update remote sequence if necessary
    if( sequence_more_recent( sequence, c->seq_remote ) )
        c->seq_remote = sequence;


    Net_processAckbits( c, ack, ack_bits );

    // timeout reset
    c->timeout_accum = 0.f;
}


////////////////////////////////////////////////////////////////////////////
//                      SOCKETS

bool Net_openSocket( net_socket *s, u16 port ) {
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
    check( !ioctlsocket( *s, FIONBIO, &non_block ),
#else
    int non_block = 1;
    check( !fcntl( *s, F_SETFL, O_NONBLOCK, non_block ),
#endif
            "Failed to set non-blocking socket\n" );

    return true;

error:
    Net_closeSocket( s );
    return false;
}

void Net_closeSocket( net_socket *s ) {
    if( *s ) {
        close( *s );
        *s = 0;
    }
}

bool Net_sendPacket( net_socket s, const net_addr *dest, const void *packet, u32 packet_size ) {
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

int  Net_receivePacket( net_socket s, net_addr *sender, void *packet, u32 packet_size ) {
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

