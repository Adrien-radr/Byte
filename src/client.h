#ifndef BYTE_CLIENT_H
#define BYTE_CLIENT_H

#include "net.h"

typedef struct {
    connection_t    connection;
} client_t;
 
client_t client;

bool cl_init();
void cl_shutdown();

void cl_run();

#endif // BYTE_CLIENT
