#include "world.h"

inline void WorldTile_init( WorldTile *w ) {
    // init local map
    Map_init( &w->map );

    w->agents = HandleManager_init( 20 );
}

inline void WorldTile_destroy( WorldTile *w ) {
    if( w ) 
        HandleManager_destroy( w->agents );
}   


bool World_init( World **w ) {
    *w = byte_alloc( sizeof( World ) );
    check_mem( *w );

    // initialize all world tiles
    for( int i = 0; i < wmap_size; ++i )
        WorldTile_init( &(*w)->tiles[i] );

    (*w)->agents = HandleManager_init( 100 );

    return true;

error:
    World_destroy( *w );
    return false;
}

inline void World_destroy( World *w ) {
    if( w ) {
        for( int i = 0; i < wmap_size; ++i )
            WorldTile_destroy( &w->tiles[i] );

        HandleManager_destroy( w->agents );

        DEL_PTR( w );
    }
}   

void World_receiveEvent( World *w, const Event *event ) {
    switch( event->type ) {
    default :
        break;
    }
}

inline WorldTile *World_getTile( World *w, u32 x, u32 y ) {
    if( w && x < wmap_width && y < wmap_height ) 
        return &w->tiles[y*wmap_width + x];
    return NULL;
}


int World_loadAgent( World *w, const char *file, u32 x, u32 y ) {
    WorldTile *t = World_getTile( w, x, y );
    int handle = -1;
    
    if( w && file && t ) {
        // Load a new agent from file
        Agent a;
        bool load = Agent_load( &a, file );

        // Store it in handle manager of corresponding WorldTile
        if( load ) {
            int global_handle = HandleManager_addData( w->agents, (void*)&a, true, sizeof(Agent) ); 
            if( global_handle >= 0 )
                handle = HandleManager_addHandle( t->agents, global_handle );
        }
    }

    return handle;
}

inline Agent *World_getAgent( World *w, u32 agent_handle, u32 x, u32 y ) {
    WorldTile * t = World_getTile( w, x, y );

    if( w && t && agent_handle < t->agents->mCount ) {
        // get global agent handle from WorldTile Handlemanager
        u32 global_agent_handle = HandleManager_getHandle( t->agents, agent_handle );

        // return Agent pointer from World Handlemanager
        return (Agent*)HandleManager_getData( w->agents, global_agent_handle );
    }
    return NULL;
}

inline Agent *World_getGlobalAgent( World *w, u32 agent_handle ) {
    if( w && agent_handle < w->agents->mCount ) 
        return (Agent*)HandleManager_getData( w->agents, agent_handle );
    return NULL;
}
