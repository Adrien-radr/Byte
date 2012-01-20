#include "scene.h"
#include "renderer.h"

Scene *Scene_new() {
    Scene *s = NULL;
    
    s = byte_alloc( sizeof( Scene ) );
    check_mem( s );

    s->mMeshes = HandleManager_init( 50 );
    s->mShaders = HandleManager_init( 50 );
    s->mTextures = HandleManager_init( 50 );
    s->mModelMatrices = HandleManager_init( 50 );
        
error:
    return s;
}

void Scene_destroy( Scene *pScene ) {
    if( pScene ) {
        HandleManager_destroy( pScene->mMeshes );
        HandleManager_destroy( pScene->mShaders );
        HandleManager_destroy( pScene->mTextures );
        HandleManager_destroy( pScene->mModelMatrices );
        DEL_PTR( pScene );
    }   
}

void Scene_render( Scene *pScene ) {
    if( pScene ) {
        for( u32 i = 0; i < pScene->mMeshes->mMaxIndex; ++i ) {
            if( HandleManager_isUsed( pScene->mMeshes, i ) ) {
                Renderer_useShader( HandleManager_getHandle( pScene->mShaders, i ) );
                Renderer_useTexture( HandleManager_getHandle( pScene->mTextures, i ), 0 );

                Shader_sendMat3( "ModelMatrix", HandleManager_getData( pScene->mModelMatrices, i ) );
                Renderer_renderMesh( HandleManager_getHandle( pScene->mMeshes, i ) );
            }
        }
    }
}

int  Scene_addEntity( Scene *pScene, u32 pMesh, u32 pShader, u32 pTexture, mat3 *pMM ) {
    int handle = -1;
    if( pScene && pMM ) {
        handle = HandleManager_addHandle( pScene->mMeshes, pMesh );
        HandleManager_addHandle( pScene->mShaders, pShader );
        HandleManager_addHandle( pScene->mTextures, pTexture );
        HandleManager_addData( pScene->mModelMatrices, pMM );
    }
    return handle;
}

void Scene_removeEntity( Scene *pScene, u32 pIndex ) {
    if( pScene ) {
        HandleManager_remove( pScene->mMeshes, pIndex );
        HandleManager_remove( pScene->mShaders, pIndex );
        HandleManager_remove( pScene->mTextures, pIndex );
        HandleManager_remove( pScene->mModelMatrices, pIndex );
    }
}

void Scene_clearEntities( Scene *pScene ) {
    if( pScene ) {
        HandleManager_clear( pScene->mMeshes );
        HandleManager_clear( pScene->mShaders );
        HandleManager_clear( pScene->mTextures );
        HandleManager_clear( pScene->mModelMatrices );
    }
}

