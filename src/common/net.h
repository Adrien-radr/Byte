#ifndef BYTE_NET_H
#define BYTE_NET_H

#include "common.h"
#include <arpa/inet.h>

/// GAME NET PACKAGES TYPES
typedef enum {
    CONNECT_ASKID,
    CONNECT_SESSIONID,
    CONNECT_TRY,
    CONNECT_ACCEPT,
    CONNECT_REFUSE,
    CONNECT_CLOSE,
    CONNECT_CLOSEOK,

    KEEP_ALIVE,

    PT_ARRAYEND
} PacketType;

extern str32 PacketTypeStr[PT_ARRAYEND];

// Default packet size (including header
#define PACKET_SIZE 165

// Packet header size in bytes
// [protocolID, sessionID, msgType, sequence, ack, ack bits]
#define PACKET_HEADER_SIZE 24

// Max sequence number, after that, go to 0 (for u32 here)
#define MAX_SEQUENCE 0xFFFFFFFF

// Max expected RTT(round trip time)
#define MAX_RTT 1.f

// RTT Threshold, after that, pass flow to Bad Mode
#define RTT_THRESHOLD 250.f

// Connection Flow Bad and Good packets per second
#define FLOW_BAD   5.f
#define FLOW_GOOD 10.f

/// Application Protocol ID
extern const u32 protocol_id;

typedef int net_socket;

typedef struct {
    u8  ip[4];
    u16 port;
} net_addr;


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
    u32                         tail;
    u32                         count;
} net_packet_info_queue;

/// Circular buffer for packets
typedef struct {
    struct {
        u8          data[PACKET_SIZE];
        u32         stack_pos;

        net_addr    addr;           
        u32         sequence;
    }                           packets[256];

    u32                         start,
                                count;
} net_packet_queue;

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

/// Informations concerning a connection to an address
///  Ex: A client has only one connection_info
///      The server has MAX_CLIENT connection_info to manage them separtly
typedef struct {
    bool                    running;
    net_addr                address;        ///< Address of destination
    u32                     session_id;     ///< Session ID. 

    net_packet_queue        guaranteed,     ///< Ordered and acknowledged packets to send
                            unguaranteed;   ///< other packets to send

    net_packet_info_queue   sent_queue,
                            recv_queue,
                            pending_acks,
                            ackd_queue;

    f32                     timeout_accum;

    u32                     seq_local,
                            seq_remote;

    u32                     sent_packets,
                            recv_packets,
                            lost_packets,
                            ackd_packets;

    f32                     sent_bw,
                            ackd_bw,
                            rtt;        

    // Flow Control
    enum {
        Good,
        Bad
    }                       flow;           ///< Flow control mode
    f32                     flow_speed;     ///< Speed depending on flow mode

    f32                     penalty_time,   ///< Penalty time (in sec)
                            good_cond_time, ///< Time spent in good mode
                            penalty_accum;  ///< Penaly reduction accumulator

    enum {
        Disconnected,
        Disconnecting,

        // client-side connection
        IDDemandSent,
        IDReceived,
        ConnectDemandSent,

        // server-side connection
        Listening,
        IDSent,
        TryAccepted,
        
        // when connection is done :
        ConnectFail,
        Connected
    }                       state;          ///< Connection state

    connection_mode         mode;           ///< Connection mode
} connection;


bool Net_connectionInit( connection *c, connection_mode mode );
void Net_connectionShutdown( connection *c );
void Net_connectionUpdate( connection *c, f32 dt );
void Net_connectionWritePacketHeader( connection *c, u8 *packet, u32 msg_type );

void Net_connectionSendGuaranteed( connection *c, u32 msg_type );
void Net_connectionSendUnguaranteed( connection *c, u32 msg_type );

void Net_connectionSendNextPacket( connection *c, net_socket socket );

void Net_connectionPacketSent( connection *c );
void Net_connectionPacketReceived( connection *c, u32 sequence, u32 ack, u32 ack_bits );

// Server functions
    void Net_connectionListen( connection *c );

void Net_readPacketHeader( const u8 *packet, u32 *protocolID, u32 *sessionID, u32 *msg_type, u32 *sequence, u32 *ack, u32 *ack_bits );

void Net_packetQueueInit( net_packet_queue *q );
void Net_packetQueuePush( net_packet_queue *q );
void Net_packetQueuePop( net_packet_queue *q );
bool Net_packetQueueIsFull( const net_packet_queue *q );
void Net_packetQueueGet( net_packet_queue *q, u8 **data_ptr, net_addr *addr );
void Net_packetQueueSet( net_packet_queue *q, const u8 *data, const net_addr *addr );
void Net_packetQueueStackInt( net_packet_queue *q, int data );


void net_addr_cpy( net_addr *dst, const net_addr *src );
bool net_addr_equal( const net_addr *a, const net_addr *b );
void net_addr_fill( net_addr *a, u8 ip1, u8 ip2, u8 ip3, u8 ip4, u16 port );
void net_addr_fill_int( net_addr *a, u32 ip, u16 port );



bool Net_init();
void Net_shutdown();

u32  Net_generateSessionID( int seed );

bool Net_openSocket( net_socket *s, u16 port );
void Net_closeSocket( net_socket *s );

bool Net_sendPacket( net_socket s, const net_addr *dest, const void *packet, u32 packet_size );
int  Net_receivePacket( net_socket s, net_addr *sender, void *packet, u32 packet_size );


#endif // BYTE_NET
