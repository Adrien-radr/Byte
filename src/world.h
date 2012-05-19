#ifndef BYTE_WORLD_H
#define BYTE_WORLD_H

#include "common/common.h"
#include "common/handlemanager.h"

#include "agent.h"
#include "map.h"

/// World contains info about every agents and static objects of the game. Also contains the world map and the current local map
typedef struct {
    HandleManager   *agents;        ///< World agents

    Map             local_map;      ///< World local map
} World;


/// Initialize the game world
bool World_init( World **w );

/// Destroy the world (pretty scary, heh?)
void World_destroy( World *w );

/// Send an event to the world to be processed.
void World_sendEvent( World *w );

/// Load an agent and add it to the world
/// @return : a handle to the stored agent, or -1 if failed
int World_loadAgent( World *w, const char *file );

/// Returns the agent data of a given handle
Agent *World_getAgent( World *w, u32 agent_handle );

/// Adds an actor to the world
/// @return : a handle to the stored actor, or -1 if failed
//int World_addActor( Actor *actor );



#endif // BYTE_WORLD
