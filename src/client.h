#ifndef BYTE_CLIENT_H
#define BYTE_CLIENT_H

#include "net.h"

typedef struct {
    connection_t    connection;

    //net_msg     msg_queue[128];     // received msg queue
    u32             seq_local;          // local sequence number
    u32             seq_remote;         // remote sequence number
} client_t;
 
client_t client;

bool cl_init();
void cl_shutdown();

void cl_run();

#endif // BYTE_CLIENT
