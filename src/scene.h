#ifndef BYTE_SCENE_HPP
#define BYTE_SCENE_HPP

#include "common.h"
#include "handlemanager.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"

typedef struct {
    HandleManager   *mMeshes;
    HandleManager   *mShaders;
    HandleManager   *mTextures;
} Scene;


/// Create and returns a new scene instance
Scene *Scene_new();

/// Destroy and free the given scene
void Scene_destroy( Scene *pScene );

u32  Scene_addEntity( Mesh *pMesh, Shader *pShader, Texture *pTexture );
#endif // BYTE_SCENE
