#include "world.h"

World *World_new() {
    World *w = NULL;
    w = byte_alloc( sizeof( World ) );
    check_mem( w );

error:
    return w;
}

void World_destroy( World *pWorld ) {
    if( pWorld ) {
        DEL_PTR( pWorld );
    }
}   
