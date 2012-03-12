#ifndef BYTE_SERVER_H
#define BYTE_SERVER_H

#include "net.h"

typedef struct {
    u32         protocol_id;
    u16         max_clients;

    bool        running;

    net_addr    sv_addr;
    net_socket  sv_socket;

    f32         timeout_accum;
        
} server_t;

server_t server;

bool sv_init();
void sv_shutdown();
void sv_run();
void sv_frame( int msec );

bool sv_send( const net_addr *to, const u8 *data, u32 size );
int  sv_receive( net_addr *from, u8 *data, u32 size );



#endif // BYTE_SERVER
