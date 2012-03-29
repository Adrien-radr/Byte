#ifndef BYTE_WORLD_HPP
#define BYTE_WORLD_HPP

//#include "resource.h"
#include "common.h"

/// Initialize the game world
bool World_init();

/// Destroy the world (pretty scary, heh?)
void World_destroy();

/// Adds a resource to the World Resource Manager
//void World_addResource( const char *pName, u32 pResource );

/// Returns a resource handle from its filename
//int  World_getResource( const char *pFile );

/// Load all resources found in resource directories
/// @return : True of all resources were loaded correctly.
//bool World_loadAllResources();


#endif // BYTE_WORLD
