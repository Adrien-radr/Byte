#ifndef BYTE_WORLD_H
#define BYTE_WORLD_H

#include "common.h"
#include "actor.h"
#include "map.h"
#include "handlemanager.h"

/*
            WORLD
    World is the shared world state between server and clients. No game resources here, only world state storage (actors, buildings, etc..)
*/
typedef struct {
    HandleManager   *mActors;

    // World local map
    Map             local_map;
} World;

/// World instance
extern World *world;


/// Initialize the game world
bool World_init();

/// Copy a world instance in the local one. This is used when server send a copy of the current world to a client, or vice-versa.
void World_cpy( World *src );

/// Destroy the world (pretty scary, heh?)
void World_destroy();

/// Load an actor and add it to the world
/// @return : a handle to the stored actor, or -1 if failed
int World_loadActor( const char *actor_name );

/// Returns the actor data of a given actor handle
Actor *World_getActor( u32 actor_handle );

/// Adds an actor to the world
/// @return : a handle to the stored actor, or -1 if failed
int World_addActor( Actor *actor );



#endif // BYTE_WORLD
