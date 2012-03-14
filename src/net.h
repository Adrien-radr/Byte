#ifndef BYTE_NET_H
#define BYTE_NET_H

#include "common.h"
#include <arpa/inet.h>

// Max sequence number, after that, go to 0
#define MAX_SEQUENCE 0xFFFFFFFF

typedef struct {
    u32     seq;    ///< sequence number of packet
    u32     size;   ///< data size in bytes
    f32     time;   ///< time offset since packet was sent or received
} net_msg_info;

typedef struct {
    u8  ip[4];
    u16 port;
} net_addr;

typedef struct {
    str16                       ifname;
    sa_family_t                 family;
    struct sockaddr_storage     addr;
    struct sockaddr_storage     mask;
} local_addr;

#define MAX_IPS 8
extern local_addr local_ips[MAX_IPS];

typedef int net_socket;

/// Application Protocol ID
extern const u32 protocol_id;

typedef struct {
    bool                running;
    net_socket          socket;
    net_addr            address;
    f32                 timeout_accum;



    enum {
        Disconnected,
        Connected, 
        Listening,
        Connecting,
        ConnectFail
    }                   state;      ///< Connection state
} connection_t;

bool net_connection_init( connection_t *c, u16 port );
void net_connection_shutdown( connection_t *c );

void net_connection_listen( connection_t *c );
void net_connection_connect( connection_t *c, net_addr *addr );
void net_connection_update( connection_t *c, f32 dt );

bool net_connection_send( connection_t *c, const u8 *packet, u32 size );
int  net_connection_receive( connection_t *c, u8 *packet, u32 size );


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
