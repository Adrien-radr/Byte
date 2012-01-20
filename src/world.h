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
int  World_loadResource( World *pWorld, ResourceType pType, const char *pFile );

/// Returns a resource handle from its filename
int  World_getResource( World *pWorld, const char *pFile );

/// Load all resources found in resource directories
void World_loadAllResources( World *pWorld );


#endif // BYTE_WORLD
