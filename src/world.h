#ifndef BYTE_WORLD_HPP
#define BYTE_WORLD_HPP

#include "common.h"
#include "handlemanager.h"
#include "resource.h"

typedef struct {
    HandleManager   mActors;

    ResourceManager *mResourceManager;  
} World;

/// Create and returns a new world instance
World *World_new();

/// Destroy the given world (pretty scary, heh?)
void World_destroy( World *pWorld );

/// Load a resource from the given file name
/// @return : the handle to the loaded resource. -1 if error ;
u32  World_loadResource( World *pWorld, ResourceType pType, const char *pFile );

#endif // BYTE_WORLD
