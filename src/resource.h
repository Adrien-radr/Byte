#ifndef BYTE_RESOURCE_HPP
#define BYTE_RESOURCE_HPP

#include "common.h"


// Directories where the resource manager look for resources
extern const char ShaderDirectory[];
extern const char MeshDirectory[];
extern const char TextureDirectory[];
extern const char FontDirectory[];

/// ResourceManager struct declaration
typedef struct s_ResourceManager ResourceManager;

/// Different types of resource that can be loaded
typedef enum {
    RT_Actor,
    RT_Shader,
    RT_Texture,
    RT_Mesh,
    RT_Font
} ResourceType;

/// Create and return a new ResourceManager instance
ResourceManager *ResourceManager_new();

/// Destroy the given ResourceManager
void ResourceManager_destroy( ResourceManager *pRM );

/// Add a resource in the given resource manager
void ResourceManager_add( ResourceManager *pRM, const char *pName, u32 pResource );

/// Load a resource in the given resource manager
/// @param pType : Type of resource to be loaded (ResourceType)
/// @param pFile : file on disk to be loaded
/// @return : the handle to the resource, or -1 if an error occured
int  ResourceManager_load( ResourceManager *pRM, ResourceType pType,  const char *pFile );

/// Returns a resource already loaded, by its file name
/// @return : the handle to the resource, or -1 if inexistant
int  ResourceManager_getResource( ResourceManager *pRM, const char *pFile );

/// Load all resources available in resource directories
/// @return : True if all resources were loaded correctly. false otherwise
bool ResourceManager_loadAllResources( ResourceManager *pRM );

#endif // BYTE_RESOURCE
