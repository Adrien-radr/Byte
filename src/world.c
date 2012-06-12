#include "world.h"

inline void WorldTile_init( WorldTile *w, u32 x, u32 y ) {
    // init local map
    Map_init( &w->map, lmap_width, lmap_height );

    w->agents = HandleManager_init( 20 );
    w->location = (vec2i){ x, y };
    w->is_active = false;
}

inline void WorldTile_destroy( WorldTile *w ) {
    if( w ) {
        HandleManager_destroy( w->agents );
        Map_destroy( &w->map );
    }
}   


bool World_init( World **w ) {
    *w = byte_alloc( sizeof( World ) );
    check_mem( *w );

    // initialize all world tiles
    for( int y = 0; y < wmap_height; ++y )
        for( int x = 0; x < wmap_width; ++x )
            WorldTile_init( &(*w)->tiles[y*wmap_width + x], x, y );

    // initialize starting active map
    (*w)->active_map_loc = (vec2i){ -1, -1 };
    World_activeMapLocation( (*w), wmap_initX, wmap_initY );

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
        Map_destroy( &w->active_map );

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

inline WorldTile *World_getTilev( World *w, const vec2i *loc ) {
    if( w && loc->x < wmap_width && loc->y < wmap_height ) 
        return &w->tiles[loc->y*wmap_width + loc->x];
    return NULL;
}

void World_activeMapLocation( World *w, u32 x, u32 y ) {
    if( !w || x >= (wmap_width-2) || y >= (wmap_height-2) )
        return;

    // make last active_map's tiles passive (if not initialization)
    if( w->active_map_loc.x >= 0 )
        for( int j= 0;j < 3; ++j )
            for( int i= 0;i < 3; ++i ) {
                WorldTile *t = World_getTile( w, w->active_map_loc.x+i, 
                                                 w->active_map_loc.y+j );
                t->is_active = false;
            }


    w->active_map_loc = (vec2i){ x,y };

    // concat a 3x3 window of world-tiles inside the active map
    Map_init( &w->active_map, 3*lmap_width, 3*lmap_height );

    // copy MapTile data from the 9 WorldTiles to the actiev_map
    for( int j= 0;j < 3; ++j )
        for( int i= 0;i < 3; ++i ) {
            WorldTile *t = World_getTile( w, x+i, y+j );
            t->is_active = true;

            memcpy( w->active_map.tiles + i + j*3,
                    t->map.tiles,
                    sizeof(MapTile) * lmap_size );
        }
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
