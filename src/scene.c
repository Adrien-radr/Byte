#include "scene.h"
#include "renderer.h"
#include "camera.h"
#include "device.h"
#include "context.h"
#include "world.h"

typedef struct s_Scene {
    u32             mEntityShader;      ///< Shader used to render entities
    EntityArray     *mEntities;         ///< Entities in the scene

    u32             mTextShader;        ///< Shader used to render texts
    TextArray       *mTexts;            ///< Texts in the scene

    WidgetArray  *mWidgets;         ///< Widgets in the scene. They use the same shader as the entities.
    u32 mMasterWidget;

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
        for( u32 i = 0; i < s->mWidgets->mMaxIndex; ++i ) {
            if( HandleManager_isUsed( s->mWidgets->mTextUsed, i ) )
                Text_setString( s->mWidgets->mTextMeshes[i], s->mWidgets->mFonts[i], s->mWidgets->mStrings[i] );
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


    s->mWidgets = WidgetArray_init(50);

    //mMasterWidget = Scene_addWidget(

    // init default text shader
    int ts = World_getResource( "textShader.json" );
    check( ts >= 0, "Text shader creation error!\n" );

    s->mTextShader = ts;

    //int ws = World_getResource( "defaultShader.json" );
    //check( ws >= 0, "bla bla bla" );


    //s->mWidgetShader = es;

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
        WidgetArray_destroy( pScene->mWidgets );
        Camera_destroy( pScene->mCamera );
        DEL_PTR( pScene );
    }
}

void Scene_update( Scene *pScene ) {
    Camera_update( pScene->mCamera );
}

void Scene_updateWidgets( Scene* pScene, vec2 pMouse ) {
    if( IsMouseUp( MB_Left ) ) {
        for( u32 i = 0; i < pScene->mWidgets->mMaxIndex; ++i ) {
            vec2 widgetPos;
            vec2 widgetExtents;
            if( HandleManager_getHandle( pScene->mWidgets->mTextUsed, i ) != 0 ) {
                widgetPos = pScene->mWidgets->mTextPositions[i];
                widgetExtents = vec2_add( &widgetPos, &pScene->mWidgets->mBounds[i] );
            }

            //  If the widget has an entity, we take its coordinates rather than the text's
            if( HandleManager_getHandle( pScene->mWidgets->mEntityUsed, i ) != 0 ) {
                widgetPos.x = pScene->mWidgets->mMatrices[i]->x[6];
                widgetPos.y = pScene->mWidgets->mMatrices[i]->x[7];
                widgetExtents = vec2_add( &widgetPos, &pScene->mWidgets->mBounds[i] );
            }

            if( !(pMouse.x < widgetPos.x || pMouse.y < widgetPos.y || pMouse.x > widgetExtents.x || pMouse.y > widgetExtents.y) ) {
                if( pScene->mWidgets->mCallbacks[i] )
                    pScene->mWidgets->mCallbacks[i];
            }
        }
    }
}


void Scene_render( Scene *pScene ) {
    if( pScene ) {
        // ##################################################
        //      RENDER ENTITIES
        Renderer_useShader( pScene->mEntityShader );

        for( u32 i = 0; i < pScene->mEntities->mMaxIndex; ++i ) {
            if( HandleManager_isUsed( pScene->mEntities->mUsed, i ) ) {
                Renderer_useTexture( pScene->mEntities->mTextures[i], 0 );
                mat3 projMat = Renderer_getProjectionMatrix( GameMatrix );
                Shader_sendMat3( "ProjectionMatrix",  &projMat );
                Shader_sendMat3( "ModelMatrix", pScene->mEntities->mMatrices[i] );
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


        // ##################################################
        //      RENDER WIDGETS
        Renderer_useShader( pScene->mEntityShader );
        mat3 projMat = Renderer_getProjectionMatrix( UIMatrix );
        Shader_sendMat3( "ProjectionMatrix", &projMat );

        for( u32 i = 0; i < pScene->mWidgets->mMaxIndex; ++i ){
            if( HandleManager_isUsed( pScene->mWidgets->mUsed, i ) ) {
                if( HandleManager_getHandle( pScene->mWidgets->mEntityUsed, i ) != 0) {
                    //  Rendering of the texture
                    Renderer_useTexture( pScene->mWidgets->mTextures[i], 0 );
                    Shader_sendMat3( "ModelMatrix", pScene->mWidgets->mMatrices[i] );
                    Shader_sendInt( "Depth", pScene->mWidgets->mDepths[i] );
                    Renderer_renderMesh( pScene->mWidgets->mTextureMeshes[i] );


                }
            }
        }

        Renderer_useShader( pScene->mTextShader );
        for( u32 i = 0; i < pScene->mWidgets->mMaxIndex; ++i ) {
            if( HandleManager_isUsed( pScene->mWidgets->mUsed, i ) ) {
                if( HandleManager_getHandle( pScene->mWidgets->mTextUsed, i ) != 0) {
                    Renderer_useTexture( pScene->mWidgets->mFonts[i]->mTexture, 0 );
                    Shader_sendColor( "Color", &pScene->mWidgets->mColors[i] );
                    Shader_sendVec2( "Position", &pScene->mWidgets->mTextPositions[i] );
                    Renderer_renderMesh( pScene->mWidgets->mTextMeshes[i] );
                }
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
            pScene->mEntities->mMatrices[handle] = pMM;
        }
    }
    return handle;
}

int  Scene_addEntityFromActor( Scene *pScene, Actor *pActor ) {
    int handle = -1;

    if( pScene && pActor ) {
        handle = EntityArray_add( pScene->mEntities );

        if( handle >= 0 ) {
            pScene->mEntities->mMeshes[handle] = pActor->mMesh_id;
            pScene->mEntities->mTextures[handle] = pActor->mTexture_id;
            pScene->mEntities->mMatrices[handle] = &pActor->mPosition;
        }
    }
    return handle;
}

void Scene_modifyEntity( Scene *pScene, u32 pHandle, EntityAttrib pAttrib, void *pData ) {
    if( pScene ) {
        if( HandleManager_isUsed( pScene->mEntities->mUsed, pHandle ) ) {
            switch( pAttrib ) {
                case EA_Matrix :
                    pScene->mEntities->mMatrices[pHandle] = (mat3*)pData;
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


//////////////////////////////

int Scene_addWidget( Scene *pScene, WidgetType pWT, void* pDataStruct, int pMother ) {
    int handle = -1;

    if( pScene ){
        handle = WidgetArray_add( pScene->mWidgets, pWT, pMother );
        switch( pWT ) {
            case WT_Master :
                if( handle >= 0 ) {
                    pScene->mWidgets->mBounds[handle] = Context_getSize();
                }
                break;
            case WT_Text :
                {
                    if( handle >= 0 ) {
                        WidgetTextAttributes wta = *(WidgetTextAttributes*)pDataStruct;
                        pScene->mWidgets->mBounds[handle] = wta.mBounds;
                        vec2 ws = Context_getSize();
                        f32 sx = 2.f / ws.x, sy = 2.f / ws.y;

                        wta.mPosition.x *= sx;
                        wta.mPosition.y *= -sy;

                        pScene->mWidgets->mTextPositions[handle] = wta.mPosition;
                        pScene->mWidgets->mFonts[handle] = wta.mFont;
                        pScene->mWidgets->mColors[handle] = wta.mColor;
                    }
                }
                break;
            case WT_Sprite :
                {
                    if( handle >= 0 ) {
                        WidgetSpriteAttributes wsa = *(WidgetSpriteAttributes*)pDataStruct;
                        pScene->mWidgets->mTextureMeshes[handle] = wsa.mMesh;
                        pScene->mWidgets->mTextures[handle] = wsa.mTexture;
                        pScene->mWidgets->mMatrices[handle] = wsa.mMM;
                        pScene->mWidgets->mBounds[handle] = wsa.mBounds;

                    }
                }
                break;
            case WT_Button :
                {
                    if( handle >= 0 ) {
                        WidgetButtonAttributes wba = *(WidgetButtonAttributes*)pDataStruct;
                        pScene->mWidgets->mFonts[handle] = wba.mFont;
                        pScene->mWidgets->mColors[handle] = wba.mColor;
                        pScene->mWidgets->mTextureMeshes[handle] = wba.mMesh;
                        pScene->mWidgets->mTextures[handle] = wba.mTexture;
                        pScene->mWidgets->mMatrices[handle] = wba.mMM;
                        pScene->mWidgets->mBounds[handle] = wba.mBounds;

                    }
                }
                break;
        }
    }
    pScene->mWidgets->mWidgetTypes[handle] = pWT;
    return handle;
}

void Scene_modifyWidget( Scene *pScene, u32 pHandle, WidgetAttrib pAttrib, void *pData ) {
    if( pScene ) {
        if( HandleManager_isUsed( pScene->mWidgets->mUsed, pHandle ) ) {
            if( HandleManager_getHandle( pScene->mWidgets->mEntityUsed, pHandle ) != 0) {
                switch( pAttrib ) {
                    case WA_Texture :
                        {
                            u32 new_texture = *((u32*)pData);
                            pScene->mWidgets->mTextures[pHandle] = new_texture;
                        }
                        break;
                    case WA_Matrix :
                        pScene->mWidgets->mMatrices[pHandle] = (mat3*)pData;
                        break;
                    case WA_Bounds :
                        {
                            vec2 new_bounds = *((vec2*)pData);
                            pScene->mWidgets->mBounds[pHandle] = new_bounds;
                        }
                        break;
                    case WA_Depth :
                        pScene->mEntities->mDepths[pHandle] = *((u32*)pData);
                        break;
                }
            }
            if( HandleManager_getHandle( pScene->mWidgets->mTextUsed, pHandle ) != 0) {
                switch( pAttrib ) {
                    case WA_Bounds :
                        {
                            vec2 new_bounds = *((vec2*)pData);
                            pScene->mWidgets->mBounds[pHandle] = new_bounds;
                        }
                        break;
                    case WA_Font :
                        {
                            const Font* new_font = ((const Font*)pData);
                            pScene->mWidgets->mFonts[pHandle] = new_font;
                        }
                        break;
                    case WA_Color :
                        pScene->mWidgets->mColors[pHandle] = *((Color*)pData);
                        break;
                    case WA_TextPosition :
                        {
                            vec2 new_pos = *((vec2*)pData);

                            //  Re-place the text on the entity so that a button will look like a button. ( //TODO: center the text )
                            if( pScene->mWidgets->mWidgetTypes[pHandle] == WT_Button ) {
                                new_pos.x = pScene->mWidgets->mMatrices[pHandle]->x[6];
                                new_pos.y = pScene->mWidgets->mMatrices[pHandle]->x[7];
                            }

                            vec2 ws = Context_getSize();
                            f32 sx = 2.f / ws.x, sy = 2.f / ws.y;

                            new_pos.x *= sx;
                            new_pos.y *= -sy;
                            pScene->mWidgets->mTextPositions[pHandle] = new_pos;
                        }
                        break;
                    case WA_String :
                        {
                            const char *s = (const char*)pData;

                            // recreate VBO
                            Text_setString( pScene->mWidgets->mTextMeshes[pHandle], pScene->mWidgets->mFonts[pHandle], s );

                            // copy string inside textarray to keep track of current string
                            DEL_PTR(pScene->mWidgets->mStrings[pHandle] );

                            pScene->mWidgets->mStrings[pHandle] = byte_alloc( strlen( s ) + 1 );
                            strcpy( pScene->mWidgets->mStrings[pHandle], s );
                        }
                        break;
                }
            }
        }
    }
}



void Scene_removeWidget( Scene *pScene, u32 pWidget ) {
    if( pScene ){
        WidgetArray_remove( pScene->mWidgets, pWidget );
    }
}

void Scene_clearWidgets( Scene *pScene) {
    if( pScene ){
        WidgetArray_clear( pScene->mWidgets );
    }
}
