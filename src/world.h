#ifndef BYTE_WORLD_H
#define BYTE_WORLD_H

#include "common/common.h"
#include "common/handlemanager.h"

#include "agent.h"
#include "map.h"
#include "event.h"

/// Tile of the world, a tile is a squared region of the whole world.
/// It can contain agents, static objects and lights, and is represented 
/// visually by a SceneTile (scene.h)
typedef struct {
    HandleManager   *agents;
    Map             map;
} WorldTile;


void WorldTile_init( WorldTile *t );
void WorldTile_destroy( WorldTile *t );

/// World contains info about every agents and static objects of the game. Also contains the world map and the current local map
typedef struct {
    HandleManager   *agents;        ///< World agents, Actual Agent data stored here. Referenced in WorldTiles.
    //Map             local_map;      ///< World local map

    WorldTile       tiles[wmap_size];  ///< World tiles
} World;


/// Initialize the game world
bool World_init( World **w );

/// Destroy the world (pretty scary, heh?)
void World_destroy( World *w );

/// World receive an event to be processed
void World_receiveEvent( World *w, const Event *event );

/// Returns the WorldTile present at (i,j) coordinates (if present)
WorldTile *World_getTile( World *w, u32 x, u32 y );

/// Load an agent in the World Agents array
/// Add its reference to the world tile located at the given position
/// @return : a handle to the stored agent, or -1 if failed
int World_loadAgent( World *w, const char *file, u32 x, u32 y );

/// Returns the agent data of a given handle, in the given world tile
Agent *World_getAgent( World *w, u32 agent_handle, u32 x, u32 y );

// Returns global agent ptr (index in world->agents)
Agent *World_getGlobalAgent( World *w, u32 agent_handle );



#endif // BYTE_WORLD
