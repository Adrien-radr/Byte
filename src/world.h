#ifndef BYTE_WORLD_HPP
#define BYTE_WORLD_HPP

#include "common.h"
#include "handlemanager.h"

typedef struct {
    HandleManager   mActors;
} World;

/// Create and returns a new world instance
World *World_new();

/// Destroy the given world (pretty scary, heh?)
void World_destroy( World *pWorld );

#endif // BYTE_WORLD
