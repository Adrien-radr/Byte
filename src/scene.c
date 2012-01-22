#include "scene.h"
#include "renderer.h"


// ##########################################################################3
//      Entity Array 
// ##########################################################################3
    /// Array containing a handle manager of meshes, textures and model matrices
    /// In the scene, there is one EntityArray for each used shader
    typedef struct {
        HandleManager   *mMeshes;
        HandleManager   *mTextures;
        HandleManager   *mModelMatrices;

        u32             mCount;
        u32             mSize;
        u32             mMaxIndex;
    } EntityArray;

    EntityArray *EntityArray_init( u32 pArraySize ) {
        EntityArray *arr = byte_alloc( sizeof( EntityArray ) );
        check_mem( arr );
        arr->mMeshes = HandleManager_init( pArraySize );
        arr->mTextures = HandleManager_init( pArraySize );
        arr->mModelMatrices = HandleManager_init( pArraySize );

        arr->mSize = pArraySize;

    error:
        return arr;
    }

    void EntityArray_clear( EntityArray *pEA ) {
        pEA->mMaxIndex = 0;
        HandleManager_clear( pEA->mMeshes );
        HandleManager_clear( pEA->mTextures );
        HandleManager_clear( pEA->mModelMatrices );
    }

    u32 EntityArray_addEntity( EntityArray *pEA, u32 pMesh, u32 pTexture, mat3 *pMM ) {
        int handle = HandleManager_addHandle( pEA->mMeshes, pMesh );
        HandleManager_addHandle( pEA->mTextures, pTexture );
        HandleManager_addData( pEA->mModelMatrices, pMM );

        ++pEA->mMaxIndex;
        ++pEA->mCount;
        pEA->mSize = pEA->mMeshes->mSize;

        return handle;
    }

    void EntityArray_removeEntity( EntityArray *pEA, u32 pIndex ) {
       HandleManager_remove( pEA->mMeshes, pIndex ); 
       HandleManager_remove( pEA->mTextures, pIndex ); 
       HandleManager_remove( pEA->mModelMatrices, pIndex ); 

       --pEA->mCount;
    }

    void EntityArray_destroy( EntityArray *pEA ) {
        if( pEA ) {
            HandleManager_destroy( pEA->mMeshes );
            HandleManager_destroy( pEA->mTextures );
            HandleManager_destroy( pEA->mModelMatrices );
            DEL_PTR( pEA );
        }
    }




// ##########################################################################3
//      SCENE
// ##########################################################################3

/// Array of EntityArray*
HeapArray( EntityArray*, Entities, EntityArray_destroy );


/// Scene definition
typedef struct s_Scene {
    bool                mShaders[10];
    //HandleManager       *mShaders;
    EntitiesArray       mEntities;
} Scene;

Scene *Scene_new() {
    Scene *s = NULL;
    
    s = byte_alloc( sizeof( Scene ) );
    check_mem( s );

    // array of not-used shaders;
    //s->mShaders = HandleManager_init( 10 );
    memset( s->mShaders, false, 10 * sizeof( bool ) );

    // create empty entities array of the same size of shader array
    EntitiesArray_init( &s->mEntities, 10 );

        
error:
    return s;
}

void Scene_destroy( Scene *pScene ) {
    if( pScene ) {
        //HandleManager_destroy( pScene->mShaders );
        EntitiesArray_destroy( &pScene->mEntities );
        DEL_PTR( pScene );
    }   
}

void Scene_render( Scene *pScene ) {
    if( pScene ) {
        for( u32 i = 0; i < 10; ++i ) {
            if( pScene->mShaders[i] ) {
                // use this shader
                Renderer_useShader( i );

                // draw all activated entities using this shader
                for( u32 j = 0; j < pScene->mEntities.data[i]->mMaxIndex; ++j ) {
                    if( HandleManager_isUsed( pScene->mEntities.data[i]->mMeshes, j ) ) {
                        Renderer_useTexture( HandleManager_getHandle( pScene->mEntities.data[i]->mTextures, j ), 0 );
                        Shader_sendMat3( "ModelMatrix", HandleManager_getData( pScene->mEntities.data[i]->mModelMatrices, j ) );

                        Renderer_renderMesh( HandleManager_getHandle( pScene->mEntities.data[i]->mMeshes, j ) );
                    }

                }
            }
        }
    }
}

int  Scene_addEntity( Scene *pScene, u32 pShader, u32 pMesh, u32 pTexture, mat3 *pMM ) {
    int handle = -1;
    if( pScene && pMM ) {
        if( pShader < 10 ) {
            // allocate entity array of corresponding shader if not used yet
            if( !pScene->mShaders[pShader] ) {
                pScene->mShaders[pShader] = true;
                pScene->mEntities.data[pShader] = EntityArray_init( 50 );
                ++pScene->mEntities.cpt;
            }

            // insert entity
            handle = EntityArray_addEntity( pScene->mEntities.data[pShader], pMesh, pTexture, pMM );
        } else
            log_err( "Added an entity in scene with shader ID >= 10. D:\n" );
    }
    return handle;
}

void Scene_removeEntity( Scene *pScene, u32 pShader, u32 pIndex ) {
    if( pScene && pShader < 10) {
        EntityArray_removeEntity( pScene->mEntities.data[pShader], pIndex );
    }
}

void Scene_clearEntities( Scene *pScene ) {
    if( pScene ) {
        for( u32 i = 0; i < 10; ++i )
            EntityArray_clear( pScene->mEntities.data[i] );
        memset( pScene->mShaders, false, 10 * sizeof( bool ) );
    }
}

