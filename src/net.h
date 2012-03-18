#ifndef BYTE_NET_H
#define BYTE_NET_H

#include "common.h"
#include <arpa/inet.h>

// Max sequence number, after that, go to 0 (for u32 here)
#define MAX_SEQUENCE 0xFFFFFFFF

/// Application Protocol ID
extern const u32 protocol_id;

typedef int net_socket;


/// Information about a net packet
typedef struct {
    u32             seq;    ///< sequence number of packet
    u32             size;   ///< data size in bytes
    f32             time;   ///< time offset since packet was sent or received
} net_packet_info;

/// Fixed-size list for net_packet_info
typedef struct {
    net_packet_info             arr[256];   
    net_packet_info             *list[256];
    u16                         tail;
    u16                         count;
} net_packet_queue;



typedef struct {
    u8  ip[4];
    u16 port;
} net_addr;

typedef struct {
    str16                       ifname;
    sa_family_t                 family;
    struct sockaddr_storage     addr;
    struct sockaddr_storage     mask;
} net_ip;

#define MAX_IPS 8
extern net_ip local_ips[MAX_IPS];


typedef enum {
    Server,
    Client
} connection_mode;

typedef struct {
    bool                running;
    net_socket          socket;
    net_addr            address;
    f32                 timeout_accum;

    u32                 seq_local,
                        seq_remote;

    u32                 sent_packets,
                        recv_packets,
                        lost_packets,
                        ackd_packets;

    f32                 sent_bw,
                        ackd_bw,
                        rtt;        

    net_packet_queue    sent_queue,
                        recv_queue,
                        pending_acks,
                        ackd_queue;
    enum {
        Disconnected,
        Connected, 
        Listening,
        Connecting,
        ConnectFail
    }                   state;      ///< Connection state
    connection_mode     mode;       ///< Connection mode
} connection_t;

bool net_connection_init( connection_t *c, connection_mode mode, u16 port );
void net_connection_shutdown( connection_t *c );

void net_connection_listen( connection_t *c );
void net_connection_connect( connection_t *c, net_addr *addr );
void net_connection_update( connection_t *c, f32 dt );

bool net_connection_send( connection_t *c, const u8 *packet, u32 size );
int  net_connection_receive( connection_t *c, u8 *packet, u32 size );

void net_packet_queue_init( net_packet_queue *q );

/// Inserts a new packet into the queue, in sorted order (more recent first)
void net_packet_queue_insert( net_packet_queue *q, net_packet_info *p );
bool net_packet_queue_exists( net_packet_queue *q, u32 seq );
void net_packet_queue_verify( net_packet_queue *q );
void net_packet_queue_print( net_packet_queue *q );
bool net_packet_queue_empty( net_packet_queue *q );

bool net_addr_equal( const net_addr *a, const net_addr *b );
void net_addr_fill( net_addr *a, u8 ip1, u8 ip2, u8 ip3, u8 ip4, u16 port );
void net_addr_fill_int( net_addr *a, u32 ip, u16 port );



bool net_init();
void net_shutdown();


bool net_open_socket( net_socket *s, u16 port );
void net_close_socket( net_socket *s );

bool net_send_packet( net_socket s, const net_addr *dest, const void *packet, u32 packet_size );
int  net_receive_packet( net_socket s, net_addr *sender, void *packet, u32 packet_size );


#endif // BYTE_NET
