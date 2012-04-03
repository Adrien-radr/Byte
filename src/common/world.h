#ifndef BYTE_WORLD_HPP
#define BYTE_WORLD_HPP

#include "common.h"
#include "actor.h"
#include "handlemanager.h"

/*
            WORLD
    World is the shared world state between server and clients. No game resources here, only world state storage (actors, buildings, etc..)
*/
typedef struct {
    HandleManager   *mActors;
} World;

/// Initialize the game world
bool World_init();

/// Copy a world instance in the local one. This is used when server send a copy of the current world to a client, or vice-versa.
void World_cpy( World *src );

/// Destroy the world (pretty scary, heh?)
void World_destroy();

/// Adds an actor to the world, and load its assets
/// @return : a handle to the stored actor, or -1 if failed
int World_addActor( Actor *actor );



#endif // BYTE_WORLD
