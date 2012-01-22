#ifndef BYTE_SCENE_HPP
#define BYTE_SCENE_HPP

#include "common.h"
#include "handlemanager.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"


// ##########################################################################3
//      ENTITY
// ##########################################################################3
    /// An entity is a basic struct defining a rendered primitive. It consists of :
    ///     - a mesh to be rendered
    ///     - a shader used to render it
    ///     - the texture used on it
    ///     - the Model Matrix used to transform it
    typedef struct {
        u32     mMesh,
                mShader,
                mTexture;

        mat3    *mModelMatrix;
    } Entity;



// ##########################################################################3
//      SCENE
// ##########################################################################3
    /// Fixed number of shaders used to render scene entities
#   define     SCENE_SHADER_N              10
    /// Fixed number of begining size of each entity array (by shader)
#   define     SCENE_ENTITIES_N            50


    // Forward declaration
    typedef struct s_Scene Scene;


    /// Create and returns a new scene instance
    Scene *Scene_new();

    /// Destroy and free the given scene
    void Scene_destroy( Scene *pScene );

    /// Render all entities in the scene
    void Scene_render();

    // ##########################################################################3
    //      Entities Array
        /// Add an entity to be rendered each frame in the scene
        /// @return : The handle to the given entity
        int  Scene_addEntity( Scene *pScene, const Entity *pEntity );

        /// Remove an entity from the scene rendered entities (by its handle in the given shader array)
        void Scene_removeEntity( Scene *pScene, u32 pShader, u32 pIndex );

        /// Clears the all entity array, to set a new scene
        void Scene_clearEntities( Scene *pScene );



#endif // BYTE_SCENE
