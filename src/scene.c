#include "scene.h"
#include "renderer.h"
#include "camera.h"
#include "device.h"


// ##########################################################################3
//      Entity Array 
// ##########################################################################3
    /// Array containing a handle manager of meshes, textures and model matrices
    /// In the scene, there is one EntityArray for each used shader
    typedef struct {
        HandleManager   *mMeshes;
        HandleManager   *mTextures;
        HandleManager   *mModelMatrices;
        HandleManager   *mDepths;

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
        arr->mDepths = HandleManager_init( pArraySize );

        arr->mSize = pArraySize;

    error:
        return arr;
    }

    void EntityArray_clear( EntityArray *pEA ) {
        pEA->mMaxIndex = 0;
        HandleManager_clear( pEA->mMeshes );
        HandleManager_clear( pEA->mTextures );
        HandleManager_clear( pEA->mModelMatrices );
        HandleManager_clear( pEA->mDepths );
    }

    u32 EntityArray_addEntity( EntityArray *pEA, const Entity *pEntity ) {
        int handle = HandleManager_addHandle( pEA->mMeshes, pEntity->mMesh );
        HandleManager_addHandle( pEA->mTextures, pEntity->mTexture );
        HandleManager_addHandle( pEA->mDepths, pEntity->mDepth );
        HandleManager_addData( pEA->mModelMatrices, pEntity->mModelMatrix );

        ++pEA->mMaxIndex;
        ++pEA->mCount;
        pEA->mSize = pEA->mMeshes->mSize;

        return handle;
    }

    void EntityArray_removeEntity( EntityArray *pEA, u32 pIndex ) {
       HandleManager_remove( pEA->mMeshes, pIndex ); 
       HandleManager_remove( pEA->mTextures, pIndex ); 
       HandleManager_remove( pEA->mDepths, pIndex ); 
       HandleManager_remove( pEA->mModelMatrices, pIndex ); 

       --pEA->mCount;
    }

    void EntityArray_destroy( EntityArray *pEA ) {
        if( pEA ) {
            HandleManager_destroy( pEA->mMeshes );
            HandleManager_destroy( pEA->mTextures );
            HandleManager_destroy( pEA->mDepths );
            HandleManager_destroy( pEA->mModelMatrices );
            DEL_PTR( pEA );
        }
    }





// ##########################################################################3
//      SCENE
// ##########################################################################3

    /// Array of EntityArray*
    HeapArray( EntityArray*, Entities, EntityArray_destroy );

    // Camera listeners and update function
        void cameraMouseListener( const Event *pEvent, void *pCamera ) {
            Camera *cam = (Camera*)pCamera;
            // manage zoom event 
            if( pEvent->Type == E_MouseWheelMoved ) 
                Camera_zoom( cam, pEvent->i );
        }

        void cameraUpdate( Camera *pCamera ) {
            // manage position pan 
            vec2 move = { .x = 0.f, .y = 0.f };
            if( IsKeyDown( K_W ) )
                move.y -= 1.f;
            if( IsKeyDown( K_A ) )
                move.x -= 1.f;
            if( IsKeyDown( K_S ) )
                move.y += 1.f;
            if( IsKeyDown( K_D ) )
                move.x += 1.f;

            if( move.x || move.y )
                Camera_move( pCamera, &move );
        }

/// Scene definition
///     Bool shader array to know if the shader i is used(true) or not(false)
///     Entity array corresponding to a particular shader (SCENE_SHADER_N Entity arrays).
///     
///     The scene also possess the scene camera
typedef struct s_Scene {
    bool                mShaders[SCENE_SHADER_N];       ///< Shaders used by scene entities
    EntitiesArray       mEntities;                      ///< Entities for each shader

    u32                 mTextShader;                    ///< Shader used to render texts
    TextArray           *mTexts;                        ///< Texts in the scene

    Camera              *mCamera;                       ///< Camera of the scene
    World               *mWorld;                        ///< Pointer to the world
} Scene;

Scene *Scene_new( World *pWorld ) {
    Scene *s = NULL;
    
    s = byte_alloc( sizeof( Scene ) );
    check_mem( s );

    // set world
    s->mWorld = pWorld;

    // array of not-used shaders;
    memset( s->mShaders, false, SCENE_SHADER_N * sizeof( bool ) );

    // create empty entities array of the same size of shader array
    EntitiesArray_init( &s->mEntities, SCENE_SHADER_N );

    // create array of texts
    s->mTexts = TextArray_init( SCENE_TEXTS_N );

    // init default text shader
    int ts = World_getResource( pWorld, "textShader.json" );
    check( ts >= 0, "Text shader does not exist. (data/shaders/textShader.json)!\n" );

    s->mTextShader = ts;

    // camera
        s->mCamera = Camera_new();
        check_mem( s->mCamera );

        Camera_registerListener( s->mCamera, cameraMouseListener, LT_MouseListener );
        Camera_registerUpdateFunction( s->mCamera, cameraUpdate );

        Device_setCamera( s->mCamera );
        
    return s;
error:
    Scene_destroy( s );
    return NULL;
}

void Scene_destroy( Scene *pScene ) {
    if( pScene ) {
        EntitiesArray_destroy( &pScene->mEntities );
        TextArray_destroy( pScene->mTexts );
        Camera_destroy( pScene->mCamera );
        DEL_PTR( pScene );
    }   
}

void Scene_update( Scene *pScene ) {
    Camera_update( pScene->mCamera );
}

void Scene_render( Scene *pScene ) {
    if( pScene ) {
        // ##################################################
        //      RENDER ENTITIES
        for( u32 i = 0; i < SCENE_SHADER_N; ++i ) {
            if( pScene->mShaders[i] ) {
                // use this shader
                Renderer_useShader( i );

                // draw all activated entities using this shader
                for( u32 j = 0; j < pScene->mEntities.data[i]->mMaxIndex; ++j ) {
                    if( HandleManager_isUsed( pScene->mEntities.data[i]->mMeshes, j ) ) {
                        Renderer_useTexture( HandleManager_getHandle( pScene->mEntities.data[i]->mTextures, j ), 0 );
                        Shader_sendMat3( "ModelMatrix", HandleManager_getData( pScene->mEntities.data[i]->mModelMatrices, j ) );
                        Shader_sendInt( "Depth", HandleManager_getHandle( pScene->mEntities.data[i]->mDepths, j ) );

                        Renderer_renderMesh( HandleManager_getHandle( pScene->mEntities.data[i]->mMeshes, j ) );
                    }
                }
            }
        }


        // ##################################################
        //      RENDER TEXTS
        Renderer_useShader( pScene->mTextShader );
        for( u32 i = 0; i < pScene->mTexts->mMaxIndex; ++i ) {
            if( HandleManager_isUsed( pScene->mTexts->mUsed, i ) ) {
                Renderer_useTexture( pScene->mTexts->mFonts[i]->mTexture, 0 );
                Shader_sendColor( "Color", &pScene->mTexts->mColors[i] );
                Renderer_renderMesh( pScene->mTexts->mMeshes[i] );
            }
        }
    }
}

int  Scene_addEntity( Scene *pScene, const Entity *pEntity ) {
    int handle = -1;
    u32 shader = pEntity->mShader;

    if( pScene && pEntity ) {
        if( shader < SCENE_SHADER_N ) {
            // allocate entity array of corresponding shader if not used yet
            if( !pScene->mShaders[shader] ) {
                pScene->mShaders[shader] = true;
                pScene->mEntities.data[shader] = EntityArray_init( SCENE_ENTITIES_N );
                ++pScene->mEntities.cpt;
            }

            // insert entity
            handle = EntityArray_addEntity( pScene->mEntities.data[shader], pEntity );
        } else
            log_err( "Added an entity in scene with shader ID >= %d.\n", SCENE_SHADER_N );
    }
    return handle;
}

void Scene_removeEntity( Scene *pScene, u32 pShader, u32 pIndex ) {
    if( pScene && pShader < SCENE_SHADER_N ) {
        EntityArray_removeEntity( pScene->mEntities.data[pShader], pIndex );
    }
}

void Scene_clearEntities( Scene *pScene ) {
    if( pScene ) {
        for( u32 i = 0; i < SCENE_SHADER_N ; ++i )
            EntityArray_clear( pScene->mEntities.data[i] );
        memset( pScene->mShaders, false, SCENE_SHADER_N  * sizeof( bool ) );
    }
}

int Scene_addText( Scene *pScene, const Font *pFont, Color pColor ) {
    int handle = -1;

    if( pScene ) {
        handle = TextArray_addText( pScene->mTexts, pFont, pColor );

        if( handle >= 0 ) {
        }
    }

    return handle;
}

void Scene_modifyText( Scene *pScene, u32 pHandle, TextAttrib pAttrib, void *pData ) {
    if( pScene ) {
        // check if the given handle is a used text
        if( HandleManager_isUsed( pScene->mTexts->mUsed, pHandle ) ) {
            switch( pAttrib ) {
                case TA_Font :
                    pScene->mTexts->mFonts[pHandle] = (const Font*)pData;
                    break;
                case TA_Color:
                    pScene->mTexts->mColors[pHandle] = *((Color*)pData);
                    break;
                case TA_String:
                    Text_setString( pScene->mTexts->mMeshes[pHandle], pScene->mTexts->mFonts[pHandle], (const char*)pData );
                    break;
            }
        }
    }
}

void Scene_removeText( Scene *pScene, u32 pIndex ) {
    if( pScene ) 
        TextArray_removeText( pScene->mTexts, pIndex );
}

void Scene_clearTexts( Scene *pScene ) {
    if( pScene ) {
        TextArray_clear( pScene->mTexts );
    }
}
