#ifndef BYTE_RESOURCE_HPP
#define BYTE_RESOURCE_HPP

#include "common.h"

typedef struct s_ResourceManager ResourceManager;

/// Different types of resource that can be loaded
typedef enum {
    RT_Actor,
    RT_Shader,
    RT_Texture,
    RT_Mesh
} ResourceType;

/// Create and return a new ResourceManager instance
ResourceManager *ResourceManager_new();

/// Destroy the given ResourceManager
void ResourceManager_destroy( ResourceManager *pRM );

/// Load a resource in the given resource manager
/// @param pType : Type of resource to be loaded (ResourceType)
/// @param pFile : file on disk to be loaded
int ResourceManager_load( ResourceManager *pRM, ResourceType pType,  const char *pFile );

#endif // BYTE_RESOURCE
