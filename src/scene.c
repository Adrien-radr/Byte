#include "scene.h"
#include "renderer.h"
#include "camera.h"
#include "device.h"
#include "context.h"
#include "world.h"

#include "GL/glew.h"

typedef struct s_Scene {
    u32             mEntityShader;      ///< Shader used to render entities
    EntityArray     *mEntities;         ///< Entities in the scene

    u32             mTextShader;        ///< Shader used to render texts
    TextArray       *mTexts;            ///< Texts in the scene

    Camera          *mCamera;           ///< Camera of the scene
} Scene;




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

// Event Listener for window resizing
    void sceneWindowResizing( const Event *pEvent, void *pData ) {
        Scene *s = (Scene*)pData;

        // update all texts with new window size
        for( u32 i = 0; i < s->mTexts->mMaxIndex; ++i ) {
            if( HandleManager_isUsed( s->mTexts->mUsed, i ) )
                Text_setString( s->mTexts->mMeshes[i], s->mTexts->mFonts[i], s->mTexts->mStrings[i] );
        }
    }


Scene *Scene_new() {
    Scene *s = NULL;
    
    s = byte_alloc( sizeof( Scene ) );
    check_mem( s );

    // create array of entities (initial size = 50)
    s->mEntities = EntityArray_init( 50 );

    // init default entity shader
    int es = World_getResource( "defaultShader.json" );
    check( es >= 0, "Entity shader creation error!\n" );

    // create array of texts (intial size = 50)
    s->mTexts = TextArray_init( 50 );

    // init default text shader
    int ts = World_getResource( "textShader.json" );
    check( ts >= 0, "Text shader creation error!\n" );

    s->mTextShader = ts;

    // camera
        s->mCamera = Camera_new();
        check_mem( s->mCamera );

        Camera_registerListener( s->mCamera, cameraMouseListener, LT_MouseListener );
        Camera_registerUpdateFunction( s->mCamera, cameraUpdate );

        Device_setCamera( s->mCamera );

    // Event listener
    EventManager_addListener( LT_ResizeListener, sceneWindowResizing, s );
        
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
                Shader_sendMat3( "ModelMatrix", &pScene->mEntities->mMatrices[i] );
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
                Shader_sendVec2( "Position", &pScene->mTexts->mPositions[i] );
                Renderer_renderMesh( pScene->mTexts->mMeshes[i] );
            }
        }
    }
}

//  =======================
 
int  Scene_addEntity( Scene *pScene, u32 pMesh, u32 pTexture, mat3 *pMM ) {
    int handle = -1;

    if( pScene ) {
        handle = EntityArray_add( pScene->mEntities );

        if( handle >= 0 ) {
            pScene->mEntities->mMeshes[handle] = pMesh;
            pScene->mEntities->mTextures[handle] = pTexture;
            memcpy( &pScene->mEntities->mMatrices[handle], pMM, 9 * sizeof( f32 ) ); 
        }
    }
    return handle;
}

int  Scene_addEntityFromActor( Scene *pScene, Actor *pActor ) { 
    int handle = -1;

    if( pScene && pActor ) {
        handle = EntityArray_add( pScene->mEntities );

        if( handle >= 0 ) {
            pActor->mUsedEntity = handle;
            pScene->mEntities->mMeshes[handle] = pActor->mMesh_id;
            pScene->mEntities->mTextures[handle] = pActor->mTexture_id;
            mat3 m;
            mat3_translationMatrixfv( &m, &pActor->mPosition );
            memcpy( pScene->mEntities->mMatrices[handle].x, m.x, 9 * sizeof( f32 ) ); 
        }
    }
    return handle;
}

void Scene_modifyEntity( Scene *pScene, u32 pHandle, EntityAttrib pAttrib, void *pData ) {
    if( pScene ) {
        if( HandleManager_isUsed( pScene->mEntities->mUsed, pHandle ) ) {
            switch( pAttrib ) {
                case EA_Matrix :
                    memcpy( &pScene->mEntities->mMatrices[pHandle], (mat3*)pData, 9 * sizeof( f32 ) ); 
                    break;
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

void Scene_transformEntity( Scene *pScene, u32 pHandle, mat3 *pTransform ) {
    if( pScene && pTransform ) {
        if( HandleManager_isUsed( pScene->mEntities->mUsed, pHandle ) ) {
            mat3_mul( &pScene->mEntities->mMatrices[pHandle], pTransform );
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
                case TA_Position :
                    {
                        vec2 new_pos = *((vec2*)pData);

                        vec2 ws = Context_getSize();
                        f32 sx = 2.f / ws.x, sy = 2.f / ws.y;

                        new_pos.x *= sx;
                        new_pos.y *= -sy;

                        pScene->mTexts->mPositions[pHandle] = new_pos;
                    }
                    break;
                case TA_String :
                    {
                        const char *s = (const char*)pData;

                        // recreate VBO
                        Text_setString( pScene->mTexts->mMeshes[pHandle], pScene->mTexts->mFonts[pHandle], s );

                        // copy string inside textarray to keep track of current string
                        DEL_PTR(pScene->mTexts->mStrings[pHandle] );

                        pScene->mTexts->mStrings[pHandle] = byte_alloc( strlen( s ) + 1 );
                        strcpy( pScene->mTexts->mStrings[pHandle], s );
                    }
                    break;
                case TA_Font :
                    pScene->mTexts->mFonts[pHandle] = (const Font*)pData;
                    break;
                case TA_Color:
                    pScene->mTexts->mColors[pHandle] = *((Color*)pData);
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
