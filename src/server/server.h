#ifndef BYTE_SERVER_H
#define BYTE_SERVER_H

#include "common/net.h"
#include "common/clock.h"

#define SERVER_PORT 1991
#define MAX_CLIENTS 8


struct {
    u32                 client_n;                   ///< Number of clients
    connection          connections[MAX_CLIENTS];

    net_socket          socket;                     ///< Server UDP socket

    net_packet_queue    recv_packets;           ///< Packets received during frame

    FILE                *log;

    Clock               clock;                      ///< Server clock
} server;


bool Server_init();
void Server_shutdown();
void Server_run();

void Server_sendGuaranteed( u32 client_id, u32 type );
void Server_sendUnguaranteed( u32 client_id, u32 type );


#endif // BYTE_SERVER
