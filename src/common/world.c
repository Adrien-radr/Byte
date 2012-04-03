#include "world.h"

// Local world instance (each client has one. server has one too)
World *world = NULL;

bool World_init() {
    world = byte_alloc( sizeof( World ) );
    check_mem( world );

    // init hm
    world->mActors = HandleManager_init( 50 );

    return true;

error:
    World_destroy();
    return false;
}

void World_destroy() {
    if( world ) {
        HandleManager_destroy( world->mActors );
        DEL_PTR( world );
    }
}   

void World_cpy( World *src ) {
    if( src )
        memcpy( world, src, sizeof(World) );
}

int World_addActor( Actor *actor ) {
    check( world, "Can't add actor to uninitialized world\n" );

    // just copy the given actor in the world handlemanager
    int handle = HandleManager_addData( world->mActors, (void*)actor, true, sizeof(Actor) );
    check( handle >= 0, "Failed to add actor to world handlemanager\n" );

    return handle;

error:
    return -1;
}
