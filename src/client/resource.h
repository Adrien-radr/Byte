#ifndef BYTE_RESOURCE_HPP
#define BYTE_RESOURCE_HPP

#include "common/common.h"


// Directories where the resource manager look for resources
extern const char ShaderDirectory[];
extern const char MeshDirectory[];
extern const char TextureDirectory[];
extern const char FontDirectory[];

/// Different types of resource that can be loaded
typedef enum {
    RT_Actor,
    RT_Shader,
    RT_Texture,
    RT_Mesh,
    RT_Font
} ResourceType;

/// Initialize the game resource manager
bool ResourceManager_init();

/// Destroy the ResourceManager
void ResourceManager_destroy();

/// Add a resource in the resource manager
void ResourceManager_add( const char *pName, u32 pResource );

/// Load a resource in the given resource manager
/// @param pType : Type of resource to be loaded (ResourceType)
/// @param pFile : file on disk to be loaded
/// @return : the handle to the resource, or -1 if an error occured
int  ResourceManager_load( ResourceType pType,  const char *pFile );

/// Returns a resource already loaded, by its file name
/// @return : the handle to the resource, or -1 if inexistant
int  ResourceManager_getResource( const char *pFile );

/// Load all resources available in resource directories
/// @return : True if all resources were loaded correctly. false otherwise
bool ResourceManager_loadAllResources();

#endif // BYTE_RESOURCE
