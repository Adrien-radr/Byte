#ifndef BYTE_CLIENT_H
#define BYTE_CLIENT_H

#include "common/net.h"

typedef struct {
    connection_t    connection;
} client_t;
 
client_t client;

bool Client_init();
void Client_shutdown();

void Client_run();

#endif // BYTE_CLIENT
