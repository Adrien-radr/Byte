#ifndef BYTE_SERVER_H
#define BYTE_SERVER_H

#include "common/net.h"

#define SERVER_PORT 1991

typedef struct {
    u16             max_clients;

    connection_t    connection;

    FILE            *log;
    f32             up_time;
} server_t;

server_t server;

#define sv_log( msg, ... ) \
    do { \
        if( server.log ) { \
            char m[512]; \
            int printed = sprintf( m, "<%.2f> ", server.up_time ); \
            snprintf( &m[printed], 512-printed-1, msg, ##__VA_ARGS__ ); \
            fprintf( server.log, m );\
        } \
    } while(0)


bool Server_init();
void Server_shutdown();
void Server_run();


#endif // BYTE_SERVER
