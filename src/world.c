#include "world.h"

World *World_new() {
    World *w = NULL;
    w = byte_alloc( sizeof( World ) );
    check_mem( w );

    w->mResourceManager = ResourceManager_new();
    check( w->mResourceManager, "Error while creating world's resource manager!\n" );

    return w;

error:
    World_destroy( w );
    return NULL;
}

void World_destroy( World *pWorld ) {
    if( pWorld ) {
        ResourceManager_destroy( pWorld->mResourceManager );
        DEL_PTR( pWorld );
    }
}   

u32  World_loadResource( World *pWorld, ResourceType pType, const char *pFile ) {
    int handle = -1;
    if( pFile && pWorld ) {
          handle = ResourceManager_load( pWorld->mResourceManager, pType, pFile );
          check( handle >= 0, "Error while loading resource in World!\n" );
    }

error:
    return handle;
}

