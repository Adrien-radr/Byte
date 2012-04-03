#ifndef BYTE_CLIENT_H
#define BYTE_CLIENT_H

#include "common/net.h"
#include "common/clock.h"

typedef struct {
    connection          c_info;         ///< Connection info
    net_socket          socket;

    net_packet_queue    recv_packets;

    Clock               clock;
} client_t;
 
client_t client;

bool Client_init( const net_addr *sv_addr );
void Client_shutdown();
void Client_run();

void Client_sendGuaranteed( u32 msg_type );
void Client_sendUnguaranteed( u32 msg_type );

#endif // BYTE_CLIENT
