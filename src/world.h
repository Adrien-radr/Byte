#ifndef BYTE_WORLD_HPP
#define BYTE_WORLD_HPP

#include "common.h"


/// Initialize the game world
bool World_init();

/// Destroy the world (pretty scary, heh?)
void World_destroy();

/// Returns a resource handle from its filename
int  World_getResource( const char *pFile );

/// Load all resources found in resource directories
/// @return : True of all resources were loaded correctly.
bool World_loadAllResources();


#endif // BYTE_WORLD
