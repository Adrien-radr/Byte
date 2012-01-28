#include "scene.h"
#include "renderer.h"
#include "camera.h"
#include "device.h"



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
//    bool                mShaders[SCENE_SHADER_N];       ///< Shaders used by scene entities
//    EntitiesArray       mEntities;                      ///< Entities for each shader
    u32             mEntityShader;
    EntityArray     *mEntities;

    u32             mTextShader;                    ///< Shader used to render texts
    TextArray       *mTexts;                        ///< Texts in the scene

    Camera          *mCamera;                       ///< Camera of the scene
    World           *mWorld;                        ///< Pointer to the world
} Scene;




Scene *Scene_new( World *pWorld ) {
    Scene *s = NULL;
    
    s = byte_alloc( sizeof( Scene ) );
    check_mem( s );

    // set world
    s->mWorld = pWorld;

    // array of not-used shaders;
    //memset( s->mShaders, false, SCENE_SHADER_N * sizeof( bool ) );

    // create array of entities
    s->mEntities = EntityArray_init( SCENE_ENTITIES_N );
    //EntitiesArray_init( &s->mEntities, SCENE_SHADER_N );

    // init default entity shader
    int es = World_getResource( pWorld, "defaultShader.json" );
    check( es >= 0, "Entity shader creation error!\n" );

    // create array of texts
    s->mTexts = TextArray_init( SCENE_TEXTS_N );

    // init default text shader
    int ts = World_getResource( pWorld, "textShader.json" );
    check( ts >= 0, "Text shader creation error!\n" );

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
        //EntitiesArray_destroy( &pScene->mEntities );
        EntityArray_destroy( pScene->mEntities );
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
        Renderer_useShader( pScene->mEntityShader );

        for( u32 i = 0; i < pScene->mEntities->mMaxIndex; ++i ) {
            if( HandleManager_isUsed( pScene->mEntities->mUsed, i ) ) {
                Renderer_useTexture( pScene->mEntities->mTextures[i], 0 );
                Shader_sendMat3( "ModelMatrix", &pScene->mEntities->mModelMatrices[i] );
                Shader_sendInt( "Depth", pScene->mEntities->mDepths[i] );
                Renderer_renderMesh( pScene->mEntities->mMeshes[i] );
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

//  =======================

int  Scene_addEntity( Scene *pScene, u32 pMesh, u32 pTexture, mat3 pMM ) {
    int handle = -1;

    if( pScene ) {
        handle = EntityArray_add( pScene->mEntities );

        if( handle >= 0 ) {
            pScene->mEntities->mMeshes[handle] = pMesh;
            pScene->mEntities->mTextures[handle] = pTexture;
            pScene->mEntities->mModelMatrices[handle] = pMM;
        }
    }
    return handle;
}

void Scene_modifyEntity( Scene *pScene, u32 pHandle, EntityAttrib pAttrib, void *pData ) {
    if( pScene ) {
        if( HandleManager_isUsed( pScene->mEntities->mUsed, pHandle ) ) {
            switch( pAttrib ) {
                case EA_Texture :
                    pScene->mEntities->mTextures[pHandle] = *((u32*)pData);
                    break;
                case EA_Depth :
                    pScene->mEntities->mDepths[pHandle] = *((u32*)pData);
                    break;
            }
        }
    }   
}

void Scene_removeEntity( Scene *pScene, u32 pIndex ) {
    if( pScene ) 
        EntityArray_remove( pScene->mEntities, pIndex );
}

void Scene_clearEntities( Scene *pScene ) {
    if( pScene ) 
        EntityArray_clear( pScene->mEntities );
}

//  =======================

int Scene_addText( Scene *pScene, const Font *pFont, Color pColor ) {
    int handle = -1;

    if( pScene ) {
        handle = TextArray_add( pScene->mTexts );

        if( handle >= 0 ) {
            pScene->mTexts->mFonts[handle] = pFont;
            pScene->mTexts->mColors[handle] = pColor;
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
        TextArray_remove( pScene->mTexts, pIndex );
}

void Scene_clearTexts( Scene *pScene ) {
    if( pScene ) 
        TextArray_clear( pScene->mTexts );
}
