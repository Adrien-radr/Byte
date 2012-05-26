#include "world.h"

bool World_init( World **w ) {
    *w = byte_alloc( sizeof( World ) );
    check_mem( *w );

    // init local map
    Map_init( &(*w)->local_map );

    (*w)->agents = HandleManager_init( 50 );


    return true;

error:
    World_destroy( *w );
    return false;
}

void World_destroy( World *w ) {
    if( w ) {
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



int World_loadAgent( World *w, const char *file ) {
    int handle = -1;
    
    if( w && file ) {
        // Load a new agent from file
        Agent a;
        bool load = Agent_load( &a, file );

        // Store it in handle manager
        if( load ) 
            handle = HandleManager_addData( w->agents, (void*)&a, true, sizeof(Agent) ); 
    }

    return handle;
}

Agent *World_getAgent( World *w, u32 agent_handle ) {
    if( w && agent_handle < w->agents->mCount )
        return (Agent*)HandleManager_getData( w->agents, agent_handle );
    return NULL;
}

