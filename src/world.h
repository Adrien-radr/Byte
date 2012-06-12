#ifndef BYTE_WORLD_H
#define BYTE_WORLD_H

#include "common/common.h"
#include "common/handlemanager.h"

#include "agent.h"
#include "map.h"
#include "event.h"

/// A tile is a squared region of the whole world.
/// It can contain agents, static objects and lights, and is represented 
/// visually by a SceneTile (scene.h)
typedef struct {
    HandleManager   *agents;
    Map             map;

    vec2i           location;   ///< Absolute location of world tile
    bool            is_active;  ///< True if this tile is currently in active_map
} WorldTile;


void WorldTile_init( WorldTile *t, u32 x, u32 y );
void WorldTile_destroy( WorldTile *t );

/// World contains info about every agents and static objects of the game. Also contains the world map and the current local map
typedef struct {
    HandleManager   *agents;            ///< World agents, Actual Agent data stored here. Referenced in WorldTiles.

    WorldTile       tiles[wmap_size];   ///< World tiles

    vec2i           active_map_loc;     ///< Location of the topleft tile of active_map
    Map             active_map;         ///< Currently active map (3x3 centered on PL)
} World;


/// Initialize the game world
bool World_init( World **w );

/// Destroy the world (pretty scary, heh?)
void World_destroy( World *w );

/// World receive an event to be processed
void World_receiveEvent( World *w, const Event *event );

/// Returns the WorldTile present at (i,j) coordinates (if present)
WorldTile *World_getTile( World *w, u32 x, u32 y );
WorldTile *World_getTilev( World *w, const vec2i *loc );

/// Sets the active map location ((x,y) is the top-left corner of the 3x3 window
void World_activeMapLocation( World *w, u32 x, u32 y );

/// Load an agent in the World Agents array
/// Add its reference to the world tile located at the given position
/// @return : a handle to the stored agent, or -1 if failed
int World_loadAgent( World *w, const char *file, u32 x, u32 y );

/// Returns the agent data of a given handle, in the given world tile
Agent *World_getAgent( World *w, u32 agent_handle, u32 x, u32 y );

// Returns global agent ptr (index in world->agents)
Agent *World_getGlobalAgent( World *w, u32 agent_handle );



#endif // BYTE_WORLD
