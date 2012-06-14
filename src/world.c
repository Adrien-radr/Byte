#include "world.h"

inline void WorldTile_init( WorldTile *w, u32 x, u32 y ) {
    w->agents = HandleManager_init( 20 );
    w->location = (vec2i){ x, y };
    w->is_active = false;

    // init local map
    vec2i glob_loc = Map_worldToGlobal( &w->location );
    Map_init( &w->map, &glob_loc, &(vec2i){ lmap_width, lmap_height } );
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

void World_activeMapLocation( World *w, int x, int y ) {
    // Clamp our 3x3 window
    x = Clamp( x, 0, wmap_width-3 );
    y = Clamp( y, 0, wmap_height-3 );


    // Do this only when map is already initialized (by default, loc.x is -1)
    if( w->active_map_loc.x >= 0 ) {
        // make last active_map's tiles passive
        for( int j= 0;j < 3; ++j )
            for( int i= 0;i < 3; ++i ) {
                WorldTile *t = World_getTile( w, w->active_map_loc.x+i, 
                                                 w->active_map_loc.y+j );
                t->is_active = false;
            }
    }


    w->active_map_loc = (vec2i){ x,y };
    vec2i global_loc = Map_worldToGlobal( &w->active_map_loc );

    // concat a 3x3 window of world-tiles inside the active map
    Map_init( &w->active_map, &global_loc, &(vec2i){ 3*lmap_width, 3*lmap_height } );

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
            if( global_handle >= 0 ) {
                handle = HandleManager_addHandle( t->agents, global_handle );

                // sets its world_id
                Agent *a = (Agent*)HandleManager_getData( w->agents, global_handle );
                a->world_id = handle;
            }
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

void World_movedAgent( World *w, u32 agent_handle, const vec2i *from, const vec2i *to ) {
    WorldTile *from_wt = World_getTile( w, from->x, from->y );
    WorldTile *to_wt = World_getTile( w, to->x, to->y );

    if( to_wt && from_wt ) {
        int glob_handle = HandleManager_getHandle( from_wt->agents, agent_handle );

        if( glob_handle >= 0 ) {
            // remove from old tile
            HandleManager_remove( from_wt->agents, agent_handle );
            
            // add to new tile
            int handle = HandleManager_addHandle( to_wt->agents, glob_handle );

            // sets its world_id
            Agent *a = (Agent*)HandleManager_getData( w->agents, glob_handle );
            a->world_id = handle;
        }
    }
}
