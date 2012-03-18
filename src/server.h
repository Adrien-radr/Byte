#ifndef BYTE_SERVER_H
#define BYTE_SERVER_H

#include "net.h"

#define SERVER_PORT 1991

typedef struct {
    u16             max_clients;

    connection_t    connection;
} server_t;

server_t server;

bool sv_init();
void sv_shutdown();
void sv_run();
void sv_frame( int msec );


#endif // BYTE_SERVER
