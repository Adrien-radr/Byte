#ifndef BYTE_SCENE_HPP
#define BYTE_SCENE_HPP

#include "common.h"
#include "handlemanager.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"

// Forward declaration
typedef struct s_Scene Scene;


/// Create and returns a new scene instance
Scene *Scene_new();

/// Destroy and free the given scene
void Scene_destroy( Scene *pScene );

/// Render all entities in the scene
void Scene_render();

// ##########################################################################3
//      ENTITIES UTILS
// ##########################################################################3
    /// Add an entity to be rendered each frame in the scene
    /// @return : The handle to the given entity
    int  Scene_addEntity( Scene *pScene, u32 pShader, u32 pMesh, u32 pTexture, mat3 *pMM );

    /// Remove an entity from the scene rendered entities (by its handle in the given shader array)
    void Scene_removeEntity( Scene *pScene, u32 pShader, u32 pIndex );

    /// Clears the all entity array, to set a new scene
    void Scene_clearEntities( Scene *pScene );



#endif // BYTE_SCENE
