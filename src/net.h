#ifndef BYTE_NET_H
#define BYTE_NET_H

#include "common.h"

typedef struct {
    u8  ip[4];
    u16 port;
} net_addr;

typedef int net_socket;



void net_addr_fill( net_addr *a, u8 ip1, u8 ip2, u8 ip3, u8 ip4, u16 port );
void net_addr_fill_int( net_addr *a, u32 ip, u16 port );



bool net_init();
void net_shutdown();


bool net_open_socket( net_socket *s, u16 port );
void net_close_socket( net_socket *s );

bool net_send_packet( net_socket *s, const net_addr *dest, const void *packet, u32 packet_size );
int  net_receive_packet( net_socket *s, net_addr *sender, void *packet, u32 packet_size );


#endif // BYTE_NET
